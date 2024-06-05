/**************************************************************************************/
/**                                                                                \n**/
/**                           l  a  n  d  u  s  e  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes landuse datatype                                      \n**/
/**     - opens the landuse input file (see also cfts26_lu2clm.c)                  \n**/
/**     - sets the landuse variables (see also landuse.h)                          \n**/
/**     - order of landuse input data:                                             \n**/
/**        0-10   CFT                                                              \n**/
/**          11   OTHERS                                                           \n**/
/**          12   PASTURES                                                         \n**/
/**          13   BIOMASS_GRASS                                                    \n**/
/**          14   BIOMASS_TREE                                                     \n**/
/**       15-25   CFT_irr                                                          \n**/
/**          26   others_irr                                                       \n**/
/**          27   PASTURE_irr                                                      \n**/
/**          28   BIOMASS_GRASS IRR                                                \n**/
/**          29   BIOMASS_TREE IRR                                                 \n**/
/**     - called in iterate()                                                      \n**/
/**     - reads every year the fractions of the bands for all cells from           \n**/
/**       the input file                                                           \n**/
/**     - checks if sum of fraction is not greater 1                               \n**/
/**       -> if sum of fraction is greater 1: subtraction from fraction            \n**/
/**          of managed grass if possible                                          \n**/
/**       -> else fail incorrect input file                                        \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct landuse
{
  Bool intercrop;               /**< intercropping possible (TRUE/FALSE) */
  Climatefile landuse;          /**< file pointer */
  Climatefile fertilizer_nr;    /**< file pointer to nitrogen fertilizer file */
  Climatefile manure_nr;        /**< file pointer to manure fertilizer file */
  Climatefile with_tillage;     /**< file pointer to tillage file */
  Climatefile residue_on_field; /**< file pointer to residue extraction file */
  Climatefile sdate;            /**< file pointer to prescribed sdates */
  Climatefile crop_phu;         /**< file pointer to prescribed crop phus */
  Climatefile grassland_lsuha;  /**< file pointer to prescribed livestock density */
};                              /**< definition of opaque datatype Landuse */

static void checkyear(const char *name,const Climatefile *file,const Config *config)
{
  int lastyear;
  if(isroot(*config))
  {
    if(config->withlanduse==LANDUSE)
    {
      lastyear=(config->fix_landuse) ? config->fix_climate_year : config->lastyear;
      if(file->firstyear+file->nyear-1<lastyear)
        fprintf(stderr,"WARNING024: Last year in %s data file=%d is less than last simulation year %d, data from last year used.\n",
                name,file->firstyear+file->nyear-1,lastyear);
    }
    else if(file->firstyear+file->nyear-1<config->landuse_year_const)
      fprintf(stderr,"WARNING024: Last year in %s data file=%d is less than const. landuse year %d, data from last year used.\n",
              name,file->firstyear+file->nyear-1,config->landuse_year_const);

  }
} /* of 'checkyear' */

Landuse initlanduse(const Config *config /**< LPJ configuration */
                   )                     /** \return allocated landuse or NULL */
{
  Landuse landuse;
  landuse=new(struct landuse);
  if(landuse==NULL)
  {
    printallocerr("landuse");
    return NULL;
  }
  landuse->landuse.isopen=landuse->fertilizer_nr.isopen=landuse->manure_nr.isopen=landuse->with_tillage.isopen=
  landuse->residue_on_field.isopen=landuse->sdate.isopen=landuse->crop_phu.isopen=landuse->grassland_lsuha.isopen=FALSE;
  /* open landuse input data */
  if(opendata(&landuse->landuse,&config->landuse_filename,"landuse","1",LPJ_FLOAT,LPJ_SHORT,0.001,2*config->landusemap_size,FALSE,config))
  {
    freelanduse(landuse,config);
    return NULL;
  }
  if(landuse->landuse.var_len!=2*config->landusemap_size && landuse->landuse.var_len!=4*config->landusemap_size)
  {
    if(isroot(*config))
      fprintf(stderr,
              "ERROR147: Invalid number of bands=%zu in landuse data file, must be %d or %d.\n",
              landuse->landuse.var_len,2*config->landusemap_size,4*config->landusemap_size);
    freelanduse(landuse,config);
    return NULL;
  }
  if(isroot(*config) && landuse->landuse.var_len!=4*config->landusemap_size)
    fputs("WARNING024: Land-use input does not include irrigation systems, suboptimal country values are used.\n",stderr);
  if(config->landuse_filename.fmt!=SOCK)
    checkyear("land-use",&landuse->landuse,config);
  if(config->sdate_option==PRESCRIBED_SDATE)
  {
    /* open sdate input data */
    if(opendata(&landuse->sdate,&config->sdate_filename,"sowing",NULL,LPJ_INT,LPJ_SHORT,1.0,2*config->cftmap_size,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    checkyear("sowing date",&landuse->sdate,config);
  } /* End sdate */

  /* Multiple-years PRESCRIBED_CROP_PHU */
  if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
  {
    /* open sdate input data */
    if(opendata(&landuse->crop_phu,&config->crop_phu_filename,"crop phu",NULL,LPJ_FLOAT,LPJ_SHORT,1.0,2*config->cftmap_size,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    checkyear("crop phu",&landuse->crop_phu,config);
  } /* End crop_phu */

  if(config->fertilizer_input==FERTILIZER)
  {
    /* open fertilizer data */
    if(opendata(&landuse->fertilizer_nr,&config->fertilizer_nr_filename,"fertilizer","g/m2",LPJ_FLOAT,LPJ_SHORT,1.0,2*config->fertilizermap_size,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    if(config->fertilizer_nr_filename.fmt!=SOCK)
      checkyear("fertilizer",&landuse->fertilizer_nr,config);
  }

  if(config->manure_input)
  {
    /* open manure fertilizer data */
    if(opendata(&landuse->manure_nr,&config->manure_nr_filename,"manure","g/m2",LPJ_FLOAT,LPJ_SHORT,1.0,2*config->fertilizermap_size,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    if(config->manure_nr_filename.fmt!=SOCK)
      checkyear("manure",&landuse->manure_nr,config);
  }

  if(config->tillage_type==READ_TILLAGE)
  {
    if(opendata(&landuse->with_tillage,&config->with_tillage_filename,"tillage",NULL, LPJ_INT,LPJ_SHORT,1.0,1,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    if(config->with_tillage_filename.fmt!=SOCK)
      checkyear("tillage",&landuse->with_tillage,config);
  }

  if(config->residue_treatment==READ_RESIDUE_DATA)
  {
    /* open residue data */
    if(opendata(&landuse->residue_on_field,&config->residue_data_filename,"residue extraction",NULL,LPJ_FLOAT,LPJ_SHORT,1.0,config->fertilizermap_size,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    if(config->residue_data_filename.fmt!=SOCK)
      checkyear("residue extraction",&landuse->residue_on_field,config);
  }

  if(config->prescribe_lsuha)
  {
    if(opendata(&landuse->grassland_lsuha,&config->lsuha_filename,"livestock density","LSU/ha",LPJ_FLOAT,LPJ_SHORT,0.001,1,TRUE,config))
    {
      freelanduse(landuse,config);
      return NULL;
    }
    checkyear("livestock density",&landuse->grassland_lsuha,config);
  }

  landuse->intercrop=config->intercrop;
  return landuse;
} /* of 'initlanduse' */


/**************************************************************************************/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads landuse data for a specific year                            \n**/
/**                                                                                \n**/
/**     - order of landuse input data:                                             \n**/
/**        0-10   CFT                                                              \n**/
/**          11   OTHERS                                                           \n**/
/**          12   PASTURES                                                         \n**/
/**          13   BIOMASS_GRASS                                                    \n**/
/**          14   BIOMASS_TREE                                                     \n**/
/**       15-25   CFT_irr                                                          \n**/
/**          26   others_irr                                                       \n**/
/**          27   PASTURE_irr                                                      \n**/
/**          28   BIOMASS_GRASS IRR                                                \n**/
/**          29   BIOMASS_TREE IRR                                                 \n**/
/**     - called in iterate()                                                      \n**/
/**     - reads every year the fractions of the bands for all cells from           \n**/
/**       the input file                                                           \n**/
/**     - checks if sum of fraction is not greater 1                               \n**/
/**       -> if sum of fraction is greater 1: subtraction from fraction            \n**/
/**          of managed grass if possible                                          \n**/
/**       -> else fail incorrect input file                                        \n**/
/**                                                                                \n**/
/**************************************************************************************/

static Real reducelanduse(Cell *cell,Real sum,int ncft,int nagtree)
{
  int i,j;
  if(cell->ml.landfrac[0].grass[1]>sum)
  {
    cell->ml.landfrac[0].grass[1]-=sum;
    return 0.0;
  }
  if(cell->ml.landfrac[1].grass[1]>sum)
  {
    cell->ml.landfrac[1].grass[1]-=sum;
    return 0.0;
  }
  for(j=0; j<2; j++)
  {
    for(i=0; i<ncft; i++)
      if(cell->ml.landfrac[j].crop[i]>sum)
      {
        cell->ml.landfrac[j].crop[i]-=sum;
        return 0;
      }
    for(i=0; i<nagtree; i++)
      if(cell->ml.landfrac[j].ag_tree[i]>sum)
      {
        cell->ml.landfrac[j].ag_tree[i]-=sum;
        return 0;
      }
    for(i=0; i<NGRASS; i++)
      if(cell->ml.landfrac[j].grass[i]>sum)
      {
        cell->ml.landfrac[j].grass[i]-=sum;
        return 0;
      }
    if(cell->ml.landfrac[j].biomass_tree>sum)
    {
      cell->ml.landfrac[j].biomass_tree-=sum;
      return 0;
    }
    if(cell->ml.landfrac[j].biomass_grass>sum)
    {
      cell->ml.landfrac[j].biomass_grass-=sum;
      return 0;
    }
    if(cell->ml.landfrac[j].woodplantation>sum)
    {
      cell->ml.landfrac[j].woodplantation-=sum;
      return 0;
    }
  }
  return sum;
} /* of 'reducelanduse' */

Bool getlanduse(Landuse landuse,     /**< Pointer to landuse data */
                Cell grid[],         /**< LPJ cell array */
                int year,            /**< year (AD) */
                int actual_year,     /**< year (AD) but not the static in case of CONST_LANDUSE */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error */
{
  int i,j,count,cell;
  int start;
  IrrigationType p;
  Real sum,*data;
  int *dates;
  int yearsdate=actual_year;     /*sdate year*/
  int yearphu=actual_year;       /*crop phu year*/
  int yearf=year;
  int yearm=year;
  int yeart=year;
  int yearr=year;
  /* for testing soil type to avoid all crops on ROCK and ICE cells */
  Stand *stand;
  int soiltype=-1;
  int yearl=year;
  String line;

  /* Initialize yearly prescribed sdate */
  if(config->sdate_option==PRESCRIBED_SDATE)
  {
    dates=readintdata(&landuse->sdate,grid,"sowing dates",yearsdate,config);
    if(dates==NULL)
      return TRUE;
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        for(j=0;j<2*ncft;j++)
          grid[cell].ml.sdate_fixed[j]=0;
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.sdate_fixed[config->cftmap[j]]=dates[count++];
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.sdate_fixed[config->cftmap[j]+ncft]=dates[count++];
      }
      else
        count+=2*ncft;
    free(dates);
  }
  if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
  {
    /* assigning crop phus data */
    data=readdata(&landuse->crop_phu,NULL,grid,"crop phus",yearphu,config);
    if(data==NULL)
      return TRUE;
    count=0;
    for(cell=0; cell<config->ngridcell; cell++)
      if(!grid[cell].skip)
      {
        for(j=0; j<2*ncft; j++)
          grid[cell].ml.crop_phu_fixed[j]=0;
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.crop_phu_fixed[config->cftmap[j]]=data[count++];
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.crop_phu_fixed[config->cftmap[j]+ncft]=data[count++];
      }
      else
        count+=2*ncft;
    free(data);
  } /* end crop_phu*/
  /* read landuse data */
  data=readdata(&landuse->landuse,NULL,grid,"landuse",yearl,config);
  if(data==NULL)
    return TRUE;
  if(config->landfrac_from_file)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
    {
      sum = 0;
      start = count;
      for(i=0;i<landuse->landuse.var_len;i++)
        if(grid[cell].landfrac==0)
          data[count++]=0;
        else
        {
          data[count]/=grid[cell].landfrac;
          sum+=data[count++];
        }
      if(sum > 1)
      {
        if(sum*grid[cell].landfrac>1+epsilon)
        {
          fprintf(stderr,"WARNING013: Sum of land-use fractions in cell %d at year %d greater 1: %f even before scaling with landfrac\n",
                  cell+config->startgrid,yearl,sum*grid[cell].landfrac);
        }
        else if(sum>1+epsilon)
          fprintf(stderr,"WARNING013: Sum of land-use fractions in cell %d at year %d greater 1: %f after scaling with landfrac of %f\n",
                  cell+config->startgrid,yearl,sum,grid[cell].landfrac);
        fflush(stderr);
        for(i=0;i<landuse->landuse.var_len;i++)
          data[start++]/=sum;
      }
    }
  }
  count=0;

  for(cell=0;cell<config->ngridcell;cell++)
  {
    /* get soiltype of first stand (not stored in cell structure) */
    if(!grid[cell].skip && grid[cell].standlist->n>0)
    {
      stand=getstand(grid[cell].standlist,0);
      soiltype=stand->soil.par->type;
    }
    else
      soiltype=-1;
    for(i=0;i<WIRRIG;i++)
    {
      initlandfracitem(grid[cell].ml.landfrac+i,ncft,config->nagtree);
      /* read cropfrac from 32 bands or rain-fed cropfrac from 64 bands input */
      if(landuse->landuse.var_len!=4*config->landusemap_size || i<1)
      {
        if(i>0 && !grid[cell].skip)
        {
          for(j=0;j<ncft;j++)
            grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
          for(j=0;j<NGRASS;j++)
            grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
          for(j=0;j<config->nagtree;j++)
            grid[cell].ml.irrig_system->ag_tree[j]=grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.irrig_system->woodplantation = grid[cell].ml.manage.par->default_irrig_system;
        }
        if(readlandfracmap(grid[cell].ml.landfrac+i,config->landusemap,
                        config->landusemap_size,data,&count,ncft,config->nwptype))
        {
          fprintf(stderr,"ERROR149: Land-use input=%g less than zero for cell %d (%s) in year %d.\n",
                  data[count],cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearl);
          return TRUE;
        }
      }
      else /* read irrigated cropfrac and irrigation systems from 64 bands input */
      {
        for(p=SURF;p<=DRIP;p++) /* irrigation system loop; 1: surface, 2: sprinkler, 3: drip */
        {
          for(j=0;j<config->landusemap_size;j++)
          {
            if(data[count]>0)
            {
              if(config->landusemap[j]==NOT_FOUND)
                count++; /* ignore data */
              else if(config->landusemap[j]<ncft)
              {
                grid[cell].ml.landfrac[i].crop[config->landusemap[j]]+=data[count++];
                grid[cell].ml.irrig_system->crop[config->landusemap[j]]=p;
                grid[cell].ml.irrig_system->crop[RICE]=SURF;                               //TODO this should be a flag
              }
              else if(config->landusemap[j]<ncft+NGRASS)
              {
                grid[cell].ml.landfrac[i].grass[config->landusemap[j]-ncft]+=data[count++];
                grid[cell].ml.irrig_system->grass[config->landusemap[j]-ncft]=p;
              }
              else if(config->landusemap[j]==ncft+NGRASS)
              {
                grid[cell].ml.landfrac[i].biomass_grass+=data[count++];
                grid[cell].ml.irrig_system->biomass_grass=p;
              }
              else if(config->landusemap[j]==ncft+NGRASS+1)
              {
                grid[cell].ml.landfrac[i].biomass_tree+=data[count++];
                grid[cell].ml.irrig_system->biomass_tree=p;
              }
              else if(config->nwptype && config->landusemap[j]==ncft+NGRASS+NBIOMASSTYPE)
              {
                grid[cell].ml.landfrac[i].woodplantation+=data[count++];
                grid[cell].ml.irrig_system->woodplantation=p;
              }
              else
              {
                grid[cell].ml.landfrac[i].ag_tree[config->landusemap[j]-ncft-NGRASS-NBIOMASSTYPE-config->nwptype]+=data[count++];
                grid[cell].ml.irrig_system[i].ag_tree[config->landusemap[j]-ncft-NGRASS-NBIOMASSTYPE-config->nwptype]=p;
              }
            }
            else
              count++;
          }
        } /* of for(p=SURF;p<=DRIP;p++) */
      }
    } /* of  for(i=0;i<WIRRIG;i++) */
    switch (config->irrig_scenario)
    {
      case NO_IRRIGATION:
        for (j = 0; j < ncft; j++)
        {
          grid[cell].ml.landfrac[0].crop[j] += grid[cell].ml.landfrac[1].crop[j];
          grid[cell].ml.landfrac[1].crop[j] = 0;
          grid[cell].ml.irrig_system->crop[j] = NOIRRIG;
        }
        for (j = 0; j < NGRASS; j++)
        {
          grid[cell].ml.landfrac[0].grass[j] += grid[cell].ml.landfrac[1].grass[j];
          grid[cell].ml.landfrac[1].grass[j] = 0;
          grid[cell].ml.irrig_system->grass[j] = NOIRRIG;
        }
        grid[cell].ml.landfrac[0].biomass_grass += grid[cell].ml.landfrac[1].biomass_grass;
        grid[cell].ml.landfrac[1].biomass_grass = 0;
        grid[cell].ml.irrig_system->biomass_grass = NOIRRIG;
        grid[cell].ml.landfrac[0].biomass_tree += grid[cell].ml.landfrac[1].biomass_tree;
        grid[cell].ml.landfrac[1].biomass_tree = 0;
        grid[cell].ml.irrig_system->biomass_tree = NOIRRIG;
        for (j = 0; j < config->nagtree; j++)
        {
          grid[cell].ml.landfrac[0].ag_tree[j] += grid[cell].ml.landfrac[1].ag_tree[j];
          grid[cell].ml.landfrac[1].ag_tree[j] = 0;
          grid[cell].ml.irrig_system->ag_tree[j] = NOIRRIG;
        }
        grid[cell].ml.landfrac[0].woodplantation += grid[cell].ml.landfrac[1].woodplantation;
        grid[cell].ml.landfrac[1].woodplantation=0;
        grid[cell].ml.irrig_system->woodplantation = NOIRRIG;
        break;
      case ALL_IRRIGATION:
        for (j = 0; j < ncft; j++)
        {
          grid[cell].ml.landfrac[1].crop[j] += grid[cell].ml.landfrac[0].crop[j];
          grid[cell].ml.landfrac[0].crop[j] = 0;
          if (!grid[cell].skip)
            grid[cell].ml.irrig_system->crop[j] = grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
        }
        for (j = 0; j < NGRASS; j++)
        {
          grid[cell].ml.landfrac[1].grass[j] += grid[cell].ml.landfrac[0].grass[j];
          grid[cell].ml.landfrac[0].grass[j] = 0;
          if (!grid[cell].skip)
            grid[cell].ml.irrig_system->grass[j] = grid[cell].ml.manage.par->default_irrig_system;
        }
        grid[cell].ml.landfrac[1].biomass_grass += grid[cell].ml.landfrac[0].biomass_grass;
        grid[cell].ml.landfrac[0].biomass_grass = 0;
        if (!grid[cell].skip)
          grid[cell].ml.irrig_system->biomass_grass = grid[cell].ml.manage.par->default_irrig_system;
        grid[cell].ml.landfrac[1].biomass_tree += grid[cell].ml.landfrac[0].biomass_tree;
        grid[cell].ml.landfrac[0].biomass_tree = 0;
        grid[cell].ml.irrig_system->biomass_tree = grid[cell].ml.manage.par->default_irrig_system;
        for (j = 0; j < config->nagtree; j++)
        {
          grid[cell].ml.landfrac[1].ag_tree[j] += grid[cell].ml.landfrac[0].ag_tree[j];
          grid[cell].ml.landfrac[0].ag_tree[j] = 0;
          if (!grid[cell].skip)
            grid[cell].ml.irrig_system->ag_tree[j] = grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
        }
        grid[cell].ml.landfrac[1].woodplantation += grid[cell].ml.landfrac[0].woodplantation;
        grid[cell].ml.landfrac[0].woodplantation = 0;
        if (!grid[cell].skip)
          grid[cell].ml.irrig_system->woodplantation = grid[cell].ml.manage.par->default_irrig_system;
        break;
    } /* of switch(...) */

    /* DEBUG: here you can set land-use fractions manually, it overwrites the land-use input, in all cells */
    /* the irrigation system is set to the default country value, but you can set a number from 1-3 manually */
    /* 1: surface, 2: sprinkler, 3: drip irrigation */

/*
      sum=landfrac_sum(grid[cell].ml.landfrac,ncft,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,TRUE);

      for(j=0;j<ncft;j++)
      {
        grid[cell].ml.landfrac[1].crop[j]=0.0;
        grid[cell].ml.landfrac[0].crop[j]=0.0;
        grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system;
      }
      for(j=0;j<NGRASS;j++)
      {
        grid[cell].ml.landfrac[1].grass[j]=0.0;
        grid[cell].ml.landfrac[0].grass[j]=0.0;
        grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
      }

      grid[cell].ml.landfrac[1].biomass_grass=0.0;
      grid[cell].ml.landfrac[0].biomass_grass=0.0;
      grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
      grid[cell].ml.landfrac[1].biomass_tree=0.0;
      grid[cell].ml.landfrac[0].biomass_tree=0.0;
      grid[cell].ml.irrig_system->biomass_tree=grid[cell].ml.manage.par->default_irrig_system;

      grid[cell].ml.landfrac[1].grass[1]=0.0;
      grid[cell].ml.landfrac[0].grass[0]=1.0;
      //if (sum>1.00001) grid[cell].ml.landfrac[0].grass[0]=1.0;
*/
/* END DEBUG */

    if(config->grassonly)
    {
      for(j=0; j<ncft; j++)
        grid[cell].ml.landfrac[0].crop[j]=grid[cell].ml.landfrac[1].crop[j]=0;
      for(j=0; j<config->nagtree; j++)
        grid[cell].ml.landfrac[0].ag_tree[j]=grid[cell].ml.landfrac[1].ag_tree[j]=0;
      grid[cell].ml.landfrac[0].grass[0]=grid[cell].ml.landfrac[1].grass[0]=0;
      grid[cell].ml.landfrac[0].biomass_grass=grid[cell].ml.landfrac[1].biomass_grass=
      grid[cell].ml.landfrac[0].biomass_tree=grid[cell].ml.landfrac[1].biomass_tree=0;
      grid[cell].ml.landfrac[0].woodplantation=grid[cell].ml.landfrac[1].woodplantation=0;
    }

    /* force tinyfrac for all crops only on pixels with valid soil */
    if (config->withlanduse==ALL_CROPS  && !grid[cell].skip && soiltype!=ROCK && soiltype!=ICE && soiltype >= 0)
    {
      for(j=0; j<ncft; j++)
      {
        if (grid[cell].ml.landfrac[1].crop[j] < param.tinyfrac)
        {
          grid[cell].ml.irrig_system->crop[j] = grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.landfrac[1].crop[j] = param.tinyfrac;
        }
        if (grid[cell].ml.landfrac[0].crop[j] < param.tinyfrac) grid[cell].ml.landfrac[0].crop[j] = param.tinyfrac;
      }
      for(j=0; j<config->nagtree; j++)
      {
        if (grid[cell].ml.landfrac[1].ag_tree[j] < param.tinyfrac)
        {
          grid[cell].ml.irrig_system->ag_tree[j] = grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.landfrac[1].ag_tree[j] = param.tinyfrac;
        }
        if (grid[cell].ml.landfrac[0].ag_tree[j] < param.tinyfrac) grid[cell].ml.landfrac[0].ag_tree[j] = param.tinyfrac;
      }
      for(j=0; j<NGRASS; j++)
      {
        if (grid[cell].ml.landfrac[0].grass[j] < param.tinyfrac) grid[cell].ml.landfrac[0].grass[j] = param.tinyfrac;
        if (grid[cell].ml.landfrac[1].grass[j] < param.tinyfrac)
        {
          grid[cell].ml.landfrac[1].grass[j] = param.tinyfrac;
          grid[cell].ml.irrig_system->grass[j] = grid[cell].ml.manage.par->default_irrig_system;
        }
      }
      if (grid[cell].ml.landfrac[1].biomass_tree < param.tinyfrac)
      {
        grid[cell].ml.landfrac[1].biomass_tree = param.tinyfrac;
        grid[cell].ml.irrig_system->biomass_tree = grid[cell].ml.manage.par->default_irrig_system;
      }
      if (grid[cell].ml.landfrac[0].biomass_tree < param.tinyfrac) grid[cell].ml.landfrac[0].biomass_tree = param.tinyfrac;
      if (grid[cell].ml.landfrac[1].biomass_grass < param.tinyfrac)
      {
        grid[cell].ml.landfrac[1].biomass_grass = param.tinyfrac;
        grid[cell].ml.irrig_system->biomass_grass = grid[cell].ml.manage.par->default_irrig_system;
      }
      if (grid[cell].ml.landfrac[0].biomass_grass < param.tinyfrac) grid[cell].ml.landfrac[0].biomass_grass = param.tinyfrac;
      if(config->nwptype)
      {
        if (grid[cell].ml.landfrac[1].woodplantation < param.tinyfrac)
        {
          grid[cell].ml.landfrac[1].woodplantation = param.tinyfrac;
          grid[cell].ml.irrig_system->woodplantation = grid[cell].ml.manage.par->default_irrig_system;
        }
        if (grid[cell].ml.landfrac[0].woodplantation < param.tinyfrac) grid[cell].ml.landfrac[0].woodplantation = param.tinyfrac;
      }

    }
    sum=landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,TRUE);

    /* set landuse to zero if no valid soil */
    if ((grid[cell].skip || soiltype==ROCK || soiltype==ICE || soiltype < 0) && sum>0)
    {
      for(j=0; j<ncft; j++)
      {
        grid[cell].ml.landfrac[0].crop[j]=0;
        grid[cell].ml.landfrac[1].crop[j]=0;
      }
      for(j=0; j<config->nagtree; j++)
      {
        grid[cell].ml.landfrac[0].ag_tree[j]=0;
        grid[cell].ml.landfrac[1].ag_tree[j]=0;
      }
      for(j=0; j<NGRASS; j++)
      {
        grid[cell].ml.landfrac[0].grass[j]=0;
        grid[cell].ml.landfrac[1].grass[j]=0;
      }
      grid[cell].ml.landfrac[0].biomass_grass=0;
      grid[cell].ml.landfrac[1].biomass_grass=0;
      grid[cell].ml.landfrac[0].biomass_tree=0;
      grid[cell].ml.landfrac[1].biomass_tree=0;
      grid[cell].ml.landfrac[0].woodplantation=0;
      grid[cell].ml.landfrac[1].woodplantation=0;
    }
    /* recommpute sum after scaling fraction */
    sum=landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,TRUE);

    if(sum>1.00001)
    {
      if(yearl>=landuse->landuse.firstyear && sum>1.01)
      {
        fprintf(stderr,"WARNING013: Sum of crop fractions in cell %d at year %d greater 1: %f\n",
                cell+config->startgrid,yearl,sum);
        fflush(stderr);
      }
      sum=reducelanduse(grid+cell,sum-1,ncft,config->nagtree);
      if(sum>0.00001)
      {
        fprintf(stderr,"ERROR016: Crop fraction=%g greater than 1 in cell %d (%s), managed grass is 0.\n",
                sum+1,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        return TRUE;
      }
    }
    if (config->withlanduse==ONLY_CROPS)
    {
      sum = 0;
      for (j = 0; j < ncft; j++)
      {
        sum += grid[cell].ml.landfrac[0].crop[j];
        sum += grid[cell].ml.landfrac[1].crop[j];
      }
      if (sum < 1 && sum > epsilon)
      {
        for (j = 0; j < ncft; j++)
        {
          grid[cell].ml.landfrac[0].crop[j] /= sum;
          grid[cell].ml.landfrac[1].crop[j] /= sum;
        }
        for (j = 0; j < NGRASS; j++)
        {
          grid[cell].ml.landfrac[0].grass[j] = 0;
          grid[cell].ml.landfrac[1].grass[j] = 0;
        }
        for (j = 0; j < config->nagtree; j++)
        {
          grid[cell].ml.landfrac[0].ag_tree[j] = 0;
          grid[cell].ml.landfrac[1].ag_tree[j] = 0;
        }
        grid[cell].ml.landfrac[0].biomass_grass = 0;
        grid[cell].ml.landfrac[1].biomass_grass = 0;
        grid[cell].ml.landfrac[0].biomass_tree = 0;
        grid[cell].ml.landfrac[1].biomass_tree = 0;
        grid[cell].ml.landfrac[0].woodplantation=0;
        grid[cell].ml.landfrac[1].woodplantation=0;
      }
    }
/** temporary set everything to irrigated maize */
/*        for(j=0; j<ncft; j++)
        {
          grid[cell].ml.landfrac[0].crop[j]=0;
          grid[cell].ml.landfrac[1].crop[j]=0;
        }
        for(j=0; j<NGRASS; j++)
        {
          grid[cell].ml.landfrac[0].grass[j]=0;
          grid[cell].ml.landfrac[1].grass[j]=0;
        }
        grid[cell].ml.landfrac[0].biomass_grass=0;
        grid[cell].ml.landfrac[1].biomass_grass=0;
        grid[cell].ml.landfrac[0].biomass_tree=0;
        grid[cell].ml.landfrac[1].biomass_tree=0;
      grid[cell].ml.landfrac[1].crop[0]=0.5;
      grid[cell].ml.landfrac[1].crop[2]=0.5;
      sum=1;*/
  } /* for(cell=0;...) */
  free(data);
  if(config->with_nitrogen)
  {
    for(cell=0; cell<config->ngridcell; cell++)
    {
      initlandfrac(grid[cell].ml.fertilizer_nr,ncft,config->nagtree);
      initlandfrac(grid[cell].ml.manure_nr,ncft,config->nagtree);
    }

    if(config->fertilizer_input==FERTILIZER)
    {
      /* assigning fertilizer Nr data */
      data=readdata(&landuse->fertilizer_nr,NULL,grid,"fertilizer",yearf,config);
      if(data==NULL)
        return TRUE;
      count=0;

      /* do changes here for the fertilization*/
      for(cell=0; cell<config->ngridcell; cell++)
      {
        for(i=0; i<WIRRIG; i++)
        {
          if(readlandfracmap(grid[cell].ml.fertilizer_nr+i,config->fertilizermap,
                             config->fertilizermap_size,data,&count,ncft,config->nwptype))
          {
            fprintf(stderr,"ERROR149: Fertilizer input=%g for band %d less than zero for cell %d (%s) in year %d.\n",
                    data[count],count % config->fertilizermap_size+i*config->fertilizermap_size,
                    cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearf);
            return TRUE;
          }
        }
      } /* for(cell=0;...) */
      free(data);
    }

    if(config->manure_input)
    {
      /* assigning manure fertilizer nr data */
      data=readdata(&landuse->manure_nr,NULL,grid,"manure",yearm,config);
      if(data==NULL)
        return TRUE;
      count=0;

      /* do changes here for the manure*/
      for(cell=0; cell<config->ngridcell; cell++)
      {
        for(i=0; i<WIRRIG; i++)
        {
          if(readlandfracmap(grid[cell].ml.manure_nr+i,config->fertilizermap,
                             config->fertilizermap_size,data,&count,ncft,config->nwptype))
          {
            fprintf(stderr,"ERROR149: Manure input=%g for band %d less than zero for cell %d (%s) in year %d.\n",
                    data[count],count % config->fertilizermap_size+i*config->fertilizermap_size,
                    cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearm);
            return TRUE;
          }
        }
      } /* for(cell=0;...) */
      free(data);
    }

    if(config->fix_fertilization)
    {
      for(cell=0; cell<config->ngridcell; cell++)
      {
        for(i=0; i<WIRRIG; i++)
        {
          for(j=0; j<ncft; j++)
         {
            grid[cell].ml.fertilizer_nr[i].crop[j]=param.fertilizer_rate;
            grid[cell].ml.manure_nr[i].crop[j]=param.manure_rate;
          }
          for(j=0; j<NGRASS; j++)
          {
            grid[cell].ml.fertilizer_nr[i].grass[j]=param.fertilizer_rate;
            grid[cell].ml.manure_nr[i].grass[j]=param.manure_rate;
          }
          for(j=0; j<config->nagtree; j++)
          {
            grid[cell].ml.fertilizer_nr[i].ag_tree[j]=param.fertilizer_rate;
            grid[cell].ml.manure_nr[i].ag_tree[j]=param.manure_rate;
          }
          grid[cell].ml.fertilizer_nr[i].biomass_grass=param.fertilizer_rate;
          grid[cell].ml.fertilizer_nr[i].biomass_tree=param.fertilizer_rate;
          grid[cell].ml.manure_nr[i].biomass_grass=param.manure_rate;
          grid[cell].ml.manure_nr[i].biomass_tree=param.manure_rate;
          grid[cell].ml.fertilizer_nr[i].woodplantation=param.fertilizer_rate;
          grid[cell].ml.manure_nr[i].woodplantation=param.manure_rate;
        }
      }
    }
  } /* of if(config->with_nitrogen) */

  if(config->tillage_type==READ_TILLAGE)
  {
    /* read in tillage data */
    dates=readintdata(&landuse->with_tillage,grid,"tillage types",yeart,config);
    if(dates==NULL)
      return TRUE;
    count=0;
    for(cell=0; cell<config->ngridcell; cell++)
      if(!grid[cell].skip)
        grid[cell].ml.with_tillage=dates[count++];
      else
        count++;
    free(dates);
  }
  else
  {
    for(cell=0; cell<config->ngridcell; cell++)
      grid[cell].ml.with_tillage=config->tillage_type!=NO_TILLAGE;
  }

  if(config->residue_treatment==READ_RESIDUE_DATA)
  {
    /* assigning residue extraction data */
    data=readdata(&landuse->residue_on_field,NULL,grid,"residue extraction",yearr,config);
    if(data==NULL)
      return TRUE;
    count=0;

    /* do changes for residue rate left on field*/
    for(cell=0; cell<config->ngridcell; cell++)
    {
      initlandfrac(grid[cell].ml.residue_on_field,ncft,config->nagtree);
      if(readlandfracmap(grid[cell].ml.residue_on_field,config->fertilizermap,
                         config->fertilizermap_size,data,&count,ncft,config->nwptype))
      {
        fprintf(stderr,"ERROR149: Residue rate input=%g for band %d less than zero for cell %d (%s) in year %d.\n",
                data[count],count % config->fertilizermap_size,
                cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearr);
        return TRUE;
      }
      count-=config->fertilizermap_size;
      readlandfracmap(grid[cell].ml.residue_on_field+1,config->fertilizermap,
                      config->fertilizermap_size,data,&count,ncft,config->nwptype);
    }
    free(data);
  }

  if(config->prescribe_lsuha)
  {
    data=readdata(&landuse->grassland_lsuha,NULL,grid,"livestock density",year,config);
    if(data==NULL)
      return TRUE;
    count=0;
    for(cell=0; cell<config->ngridcell; cell++)
      if(!grid[cell].skip)
        grid[cell].ml.grassland_lsuha=data[count++];
      else
        count++;
    free(data);
  }
  else
  {
    for(cell=0; cell<config->ngridcell; cell++)
      grid[cell].ml.grassland_lsuha=param.lsuha;
  }
  return FALSE;
} /* of 'getlanduse' */

Bool getintercrop(const Landuse landuse /**< pointer to landuse data */
                 )                      /** \return intercropping enabled? (TRUE/FALSE) */
{
  return (landuse==NULL) ? FALSE : landuse->intercrop;
} /* of 'getintercrop' */

void freelanduse(Landuse landuse,     /**< pointer to landuse data */
                 const Config *config /**< LPJmL configuration */
                )
{
  if(landuse!=NULL)
  {
    closeclimatefile(&landuse->landuse,isroot(*config));
    closeclimatefile(&landuse->sdate,isroot(*config));
    closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
    closeclimatefile(&landuse->crop_phu,isroot(*config));
    closeclimatefile(&landuse->manure_nr,isroot(*config));
    closeclimatefile(&landuse->with_tillage,isroot(*config));
    closeclimatefile(&landuse->residue_on_field,isroot(*config));
    closeclimatefile(&landuse->grassland_lsuha,isroot(*config));
    free(landuse);
  }
} /* of 'freelanduse' */
