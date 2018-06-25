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
#include "natural.h"
#include "agriculture.h"
#include "grassland.h"

struct landuse
{
  Bool intercrop;      /**< intercropping possible (TRUE/FALSE) */
  Bool allcrops;       /**< all crops establish (TRUE/FALSE) */
  int nbands;          /**< number of data items per cell */
  int nbands_sdate;    /**< number of data items per cell for sowing dates */
  Climatefile landuse; /**< file pointer */
  Climatefile sdate;   /**< file pointer to prescribed sdates */
};                     /**< definition of opaque datatype Landuse */

Landuse initlanduse(int ncft,            /**< number of crop PFTs */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return allocated landuse or NULL */
{
  Header header;
  String headername;
  int version;
  Landuse landuse;
  size_t offset;
  landuse=new(struct landuse);
  if(landuse==NULL)
  {
    printallocerr("landuse");
    return NULL;
  }
  landuse->allcrops=(config->withlanduse==ALL_CROPS);
  landuse->landuse.fmt=config->landuse_filename.fmt;
  if(landuse->allcrops)
    landuse->landuse.file=NULL;
  else
  {
    if(config->landuse_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->landuse,config->landuse_filename.name,config->landuse_filename.var,NULL,config))
      {
        free(landuse);
        return NULL;
      }
      landuse->nbands=landuse->landuse.var_len;
      if(landuse->landuse.var_len!=2*(ncft+NGRASS+NBIOMASSTYPE) && landuse->landuse.var_len!=4*(ncft+NGRASS+NBIOMASSTYPE))
      {
        if(landuse->landuse.var_len!=2*(ncft+NGRASS))
        {
          closeclimate_netcdf(&landuse->landuse,isroot(*config));
          if(isroot(*config))
             fprintf(stderr,
                    "ERROR147: Invalid number of bands=%d in landuse data file.\n",
                    (int)landuse->landuse.var_len);
          free(landuse);
          return NULL;
        }
        else
        {
          if(isroot(*config))
            fputs("WARNING022: No landuse for biomass defined.\n",stderr);
        }
      }
      if( landuse->landuse.var_len!=4*(ncft+NGRASS+NBIOMASSTYPE))
      {
        if(isroot(*config))
          fputs("WARNING024: Land-use input does not include irrigation systems, suboptimal country values are used.\n",stderr);
      }
    }
    else
    {
      if((landuse->landuse.file=openinputfile(&header,&landuse->landuse.swap,
                                              &config->landuse_filename,
                                              headername,
                                              &version,&offset,config))==NULL)
      {
        free(landuse);
        return NULL;
      }
      if(config->landuse_filename.fmt==RAW)
      {
        header.nbands=2*(ncft+NGRASS+NBIOMASSTYPE);
        landuse->landuse.datatype=LPJ_SHORT;
        landuse->landuse.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        if(header.nbands!=2*(ncft+NGRASS+NBIOMASSTYPE) && header.nbands!=4*(ncft+NGRASS+NBIOMASSTYPE))
        {
          if(header.nbands!=2*(ncft+NGRASS))
          {
            fclose(landuse->landuse.file);
            if(isroot(*config))
              fprintf(stderr,
                      "ERROR147: Invalid number of bands=%d in landuse data file.\n",
                      header.nbands);
            free(landuse);
            return NULL;
          }
          else
          {
            if(isroot(*config))
              fputs("WARNING022: No landuse for biomass defined.\n",stderr);
          }
        }
        landuse->landuse.datatype=header.datatype;
        if(header.nbands!=4*(ncft+NGRASS+NBIOMASSTYPE))
        {
          if(isroot(*config))
            fputs("WARNING024: Land-use input does not include irrigation systems, suboptimal country values are used.\n",stderr);
        }
        landuse->landuse.offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version)+offset;
      }
      landuse->landuse.firstyear=header.firstyear;
      landuse->landuse.nyear=header.nyear;
      landuse->landuse.size=header.ncell*header.nbands*typesizes[landuse->landuse.datatype];
      landuse->landuse.n=config->ngridcell*header.nbands;
      landuse->nbands=header.nbands;
      landuse->landuse.scalar=(version==1) ? 0.001 : header.scalar;
    }
  }

  if(config->sdate_option==PRESCRIBED_SDATE)
  {
    landuse->sdate.fmt=config->sdate_filename.fmt;
    if(config->sdate_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->sdate,config->sdate_filename.name,config->sdate_filename.var,NULL,config))
      {
        if(landuse->landuse.fmt==CDF)
          closeclimate_netcdf(&landuse->landuse,isroot(*config));
        else
          fclose(landuse->landuse.file);
        free(landuse);
        return NULL;
      }
      if(landuse->sdate.var_len!=2*ncft)
      {
        closeclimate_netcdf(&landuse->sdate,isroot(*config));
        if(isroot(*config))
           fprintf(stderr,
                  "ERROR147: Invalid number of bands=%d in sowing date data file.\n",
                  (int)landuse->sdate.var_len);
        if(landuse->landuse.fmt==CDF)
          closeclimate_netcdf(&landuse->landuse,isroot(*config));
        else
          fclose(landuse->landuse.file);
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->sdate.file=openinputfile(&header,&landuse->sdate.swap,
                                            &config->sdate_filename,headername,
                                            &version,&offset,config))==NULL)
      {
        fclose(landuse->landuse.file);
        free(landuse);
        return NULL;
      }
      if(config->sdate_filename.fmt==RAW)
      {
        header.nbands=2*ncft;
        landuse->sdate.datatype=LPJ_SHORT;
        landuse->sdate.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        if(header.nbands!=2*ncft)
        {
          if(landuse->landuse.fmt==CDF)
            closeclimate_netcdf(&landuse->landuse,isroot(*config));
          else
            fclose(landuse->landuse.file);
          fclose(landuse->sdate.file);
          if(isroot(*config))
            fprintf(stderr,
                    "ERROR147: Invalid number of bands=%d in sowing date file.\n",
                    header.nbands);
          free(landuse);
          return(NULL);
        }
        landuse->sdate.datatype=header.datatype;
        landuse->sdate.offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version)+offset;
      }
      landuse->sdate.size=header.ncell*header.nbands*typesizes[landuse->sdate.datatype];
      landuse->sdate.n=config->ngridcell*header.nbands;
      landuse->nbands_sdate=header.nbands;
      landuse->sdate.scalar=header.scalar;
    }
  }
  else
    landuse->sdate.file=NULL;
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
/**     written by Werner von Bloh, Sibyll Schaphoff                               \n**/
/**     Potsdam Institute for Climate Impact Research                              \n**/
/**     PO Box 60 12 03                                                            \n**/
/**     14412 Potsdam/Germany                                                      \n**/
/**                                                                                \n**/
/**     Last change: 15.10.2009                                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

static Real reducelanduse(Cell *cell,Real sum,int ncft)
{
  int i,j;
  if(cell->ml.landfrac[0].grass[1] > sum)
  {
    cell->ml.landfrac[0].grass[1]-=sum;
    return 0.0;
  }
  if(cell->ml.landfrac[1].grass[1] > sum)
  {
    cell->ml.landfrac[1].grass[1]-=sum;
    return 0.0;
  }
  for(j=0;j<2;j++)
  {
    for(i=0;i<ncft;i++)
      if(cell->ml.landfrac[j].crop[i] > sum)
      {
        cell->ml.landfrac[j].crop[i]-=sum;
        return 0;
      }
    for(i=0;i<NGRASS;i++)
      if(cell->ml.landfrac[j].grass[i] > sum)
      {
        cell->ml.landfrac[j].grass[i]-=sum;
        return 0;
      }
    if(cell->ml.landfrac[j].biomass_tree > sum)
    {
      cell->ml.landfrac[j].biomass_tree-=sum;
      return 0;
    }
    if(cell->ml.landfrac[j].biomass_grass > sum)
    {
      cell->ml.landfrac[j].biomass_grass-=sum;
      return 0;
    }
  }
  return sum;
} /* of 'reducelanduse' */

Bool getlanduse(Landuse landuse,     /**< Pointer to landuse data */
                Cell grid[],         /**< LPJ cell array */
                int year,            /**< year (AD) */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error */
{
  int i,j,p,count,cell;
  Real sum,*data;
  int *dates;
  /* so far, read prescribed sdates only once at the beginning of each simulation */
  if(config->sdate_option==PRESCRIBED_SDATE)
  {
    if(landuse->sdate.fmt==CDF)
    {
      dates=newvec(int,config->ngridcell*landuse->sdate.var_len);
      if(dates==NULL)
      {
        printallocerr("dates");
        return TRUE;
      }
      if(readintdata_netcdf(&landuse->sdate,dates,grid,0,config))
      {
        fprintf(stderr,
                "ERROR149: Cannot read sowing dates of year %d in getlanduse().\n",
                year+landuse->sdate.firstyear);
        fflush(stderr);
        return TRUE;
      }
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          for(j=0;j<2*ncft;j++)
            grid[cell].ml.sdate_fixed[j]=dates[count++];
        else
          count+=2*ncft;
      free(dates);
    }
    else
    {
      if(fseek(landuse->sdate.file,landuse->sdate.offset,SEEK_SET))
      {
        fprintf(stderr,
                "ERROR148: Cannot seek sowing dates to year %d in getlanduse().\n",
                year);
        return TRUE;
      }
      dates=newvec(int,landuse->sdate.n);
      if(dates==NULL)
      {
        printallocerr("dates");
        return TRUE;
      }
      if(readintvec(landuse->sdate.file,dates,landuse->sdate.n,landuse->sdate.swap,landuse->sdate.datatype))
      {
        fprintf(stderr,
                "ERROR149: Cannot read sowing dates of year %d in getlanduse().\n",
                year);
        free(dates);
        return TRUE;
      }
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          for(j=0;j<2*ncft;j++)
            grid[cell].ml.sdate_fixed[j]=dates[count++];
        else
          count+=2*ncft;
      free(dates);
    }
  }

  if(!landuse->allcrops)
  {
    year-=landuse->landuse.firstyear;
    if(year>=landuse->landuse.nyear)
      year=landuse->landuse.nyear-1;
    else if(year<0)
      year=0;
    if(landuse->landuse.fmt==CDF)
    {
      data=newvec(Real,config->ngridcell*landuse->landuse.var_len);
      if(data==NULL)
      {
        printallocerr("data");
        return TRUE;
      }
      if(readdata_netcdf(&landuse->landuse,data,grid,year,config))
      {
        fprintf(stderr,
                "ERROR149: Cannot read landuse of year %d in getlanduse().\n",
                year+landuse->landuse.firstyear);
        fflush(stderr);
        return TRUE;
      }
    }
    else
    {
      if(fseek(landuse->landuse.file,(long long)year*landuse->landuse.size+landuse->landuse.offset,SEEK_SET))
      {
        fprintf(stderr,
                "ERROR148: Cannot seek landuse to year %d in getlanduse().\n",
                year+landuse->landuse.firstyear);
        fflush(stderr);
        return TRUE;
      }
      data=newvec(Real,landuse->landuse.n);
      if(data==NULL)
      {
        printallocerr("data");
        return TRUE;
      }
      if(readrealvec(landuse->landuse.file,data,0,landuse->landuse.scalar,landuse->landuse.n,landuse->landuse.swap,landuse->landuse.datatype))
      {
        fprintf(stderr,
                "ERROR149: Cannot read landuse of year %d in getlanduse().\n",
                year+landuse->landuse.firstyear);
        fflush(stderr);
        free(data);
        return TRUE;
      }
    }
    count=0;
  }

  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(landuse->allcrops)
    {
      if(!grid[cell].skip)
      {
        sum=0;
        for(j=0;j<ncft;j++)
        {
          grid[cell].ml.landfrac[1].crop[j]=0.01;
          grid[cell].ml.landfrac[0].crop[j]=0.01;
          sum += 0.02;
          grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
        }
        for(j=0;j<NGRASS;j++)
        {
          grid[cell].ml.landfrac[0].grass[j]=0.01;
          grid[cell].ml.landfrac[1].grass[j]=0.01;
          sum += 0.02;
          grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
        }
        grid[cell].ml.landfrac[1].biomass_tree=0.01;
        grid[cell].ml.landfrac[0].biomass_tree=0.01;
        grid[cell].ml.landfrac[1].biomass_grass=0.01;
        grid[cell].ml.landfrac[0].biomass_grass=0.01;
        sum += 0.04;
        grid[cell].ml.landfrac[0].grass[NGRASS-1]=1.0-sum+0.01; /* I set landfrac[0], trunk has landfrac[1], which is irrigated -> much more irrigated grass in allcrops run */

        grid[cell].ml.irrig_system->biomass_tree=grid[cell].ml.manage.par->default_irrig_system;
        grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
      }
    }
    else
    {
      for(i=0;i<WIRRIG;i++)
      {
        /* read cropfrac from 32 bands or rain-fed cropfrac from 64 bands input */
        if(landuse->nbands!=4*(ncft+NGRASS+NBIOMASSTYPE) || i<1)
        {
          for(j=0;j<ncft;j++)
          {
            grid[cell].ml.landfrac[i].crop[j]=data[count++];
            if(i>0 && !grid[cell].skip)
              grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
          }
          for(j=0;j<NGRASS;j++)
          {
            grid[cell].ml.landfrac[i].grass[j]=data[count++];
            if(i>0 && !grid[cell].skip)
              grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
          }
          if(landuse->nbands!=2*(ncft+NGRASS))
          {
            grid[cell].ml.landfrac[i].biomass_grass=data[count++];
            if(i>0 && !grid[cell].skip)
              grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
            grid[cell].ml.landfrac[i].biomass_tree=data[count++];
            if(i>0 && !grid[cell].skip)
              grid[cell].ml.irrig_system->biomass_tree=grid[cell].ml.manage.par->default_irrig_system;
          }
          else
            grid[cell].ml.landfrac[i].biomass_grass=grid[cell].ml.landfrac[i].biomass_tree=0;
        }
        else /* read irrigated cropfrac and irrigation systems from 64 bands input */
        {
          for(j=0;j<ncft;j++) /* initialization needed */
            grid[cell].ml.landfrac[i].crop[j]=0;
          for(j=0;j<NGRASS;j++)
            grid[cell].ml.landfrac[i].grass[j]=0;
          grid[cell].ml.landfrac[i].biomass_grass=0;
          grid[cell].ml.landfrac[i].biomass_tree=0;

          for(p=1;p<4;p++) /* irrigation system loop; 1: surface, 2: sprinkler, 3: drip */
          {
            for(j=0;j<ncft;j++)
            {
              if(data[count]>0)
              {
                grid[cell].ml.landfrac[i].crop[j]=data[count++];
                grid[cell].ml.irrig_system->crop[j]=p;
              }
              else
                count++;
            }
            for(j=0;j<NGRASS;j++)
            {
              if(data[count]>0)
              {
                grid[cell].ml.landfrac[i].grass[j]=data[count++];
                grid[cell].ml.irrig_system->grass[j]=p;
              }
              else
                count++;
            }
            if(data[count]>0)
            {
              grid[cell].ml.landfrac[i].biomass_grass=data[count++];
              grid[cell].ml.irrig_system->biomass_grass=p;
            }
            else
              count++;
            if(data[count]>0)
            {
              grid[cell].ml.landfrac[i].biomass_tree=data[count++];
              grid[cell].ml.irrig_system->biomass_tree=p;
            }
            else
              count++;
          }
        }
      }
      switch(config->irrig_scenario)
      {
        case NO_IRRIGATION:
          for(j=0;j<ncft;j++)
          {
            grid[cell].ml.landfrac[0].crop[j]+=grid[cell].ml.landfrac[1].crop[j];
            grid[cell].ml.landfrac[1].crop[j]=0;
            grid[cell].ml.irrig_system->crop[j]=NOIRRIG;
          }
          for(j=0;j<NGRASS;j++)
          {
            grid[cell].ml.landfrac[0].grass[j]+=grid[cell].ml.landfrac[1].grass[j];
            grid[cell].ml.landfrac[1].grass[j]=0;
            grid[cell].ml.irrig_system->grass[j]=NOIRRIG;
          }
          grid[cell].ml.landfrac[0].biomass_grass+=grid[cell].ml.landfrac[1].biomass_grass;
          grid[cell].ml.landfrac[1].biomass_grass=0;
          grid[cell].ml.irrig_system->biomass_grass=NOIRRIG;
          grid[cell].ml.landfrac[0].biomass_tree+=grid[cell].ml.landfrac[1].biomass_tree;
          grid[cell].ml.landfrac[1].biomass_tree=0;
          grid[cell].ml.irrig_system->biomass_tree=NOIRRIG;
          break;
        case ALL_IRRIGATION:
          for(j=0;j<ncft;j++)
          {
            grid[cell].ml.landfrac[1].crop[j]+=grid[cell].ml.landfrac[0].crop[j];
            grid[cell].ml.landfrac[0].crop[j]=0;
            if(!grid[cell].skip)
              grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
          }
          for(j=0;j<NGRASS;j++)
          {
            grid[cell].ml.landfrac[1].grass[j]+=grid[cell].ml.landfrac[0].grass[j];
            grid[cell].ml.landfrac[0].grass[j]=0;
            if(!grid[cell].skip)
              grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
          }
          grid[cell].ml.landfrac[1].biomass_grass+=grid[cell].ml.landfrac[0].biomass_grass;
          grid[cell].ml.landfrac[0].biomass_grass=0;
          if(!grid[cell].skip)
            grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.landfrac[1].biomass_tree+=grid[cell].ml.landfrac[0].biomass_tree;
          grid[cell].ml.landfrac[0].biomass_tree=0;
          grid[cell].ml.irrig_system->biomass_tree=grid[cell].ml.manage.par->default_irrig_system;
          break;
      } /* of switch(...) */

      /* DEBUG: here you can set land-use fractions manually, it overwrites the land-use input, in all cells */
      /* the irrigation system is set to the default country value, but you can set a number from 1-3 manually */
      /* 1: surface, 2: sprinkler, 3: drip irrigation */


/*      sum=landfrac_sum(grid[cell].ml.landfrac,ncft,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,TRUE);

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

      grid[cell].ml.landfrac[1].biomass_grass=0.25;
      grid[cell].ml.landfrac[0].biomass_grass=0.25;
      grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
      grid[cell].ml.landfrac[1].biomass_tree=0.25;
      grid[cell].ml.landfrac[0].biomass_tree=0.25;
      grid[cell].ml.irrig_system->biomass_tree=grid[cell].ml.manage.par->default_irrig_system;

      grid[cell].ml.landfrac[1].grass[1]=0.0;
      grid[cell].ml.landfrac[0].grass[0]=0.0;
      //if (sum>1.00001) grid[cell].ml.landfrac[0].grass[0]=1.0;
*/
      /* END DEBUG */

      sum=landfrac_sum(grid[cell].ml.landfrac,ncft,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,TRUE);
    }

    if(sum>1.00001)
    {
      if(year>0)
      {
        fprintf(stderr,"WARNING013: in cell %d at year %d: sum of crop fractions greater 1: %f\n",
                cell+config->startgrid,year+landuse->landuse.firstyear,sum);
        fflush(stderr);
      }
      sum=reducelanduse(grid+cell,sum-1,ncft);
      if(sum > 0.00001)
        fail(CROP_FRACTION_ERR,FALSE,
             "crop fraction greater 1: %f cell: %d, managed grass is 0",
             sum+1,cell+config->startgrid);
    }
  } /* for(cell=0;...) */

  if(!landuse->allcrops)
    free(data);
  return FALSE;
} /* of 'getlanduse' */

Bool getintercrop(const Landuse landuse /**< pointer to landuse data */
                 )                      /** \return intercropping enabled? (TRUE/FALSE) */
{
  return (landuse==NULL) ? FALSE : landuse->intercrop;
} /* of 'getintercrop' */

void freelanduse(Landuse landuse, /**< pointer to landuse data */
                 Bool isroot      /**< task is root task */
                )
{
  if(landuse!=NULL)
  {
    if(!landuse->allcrops)
    {
      if(landuse->landuse.fmt==CDF)
        closeclimate_netcdf(&landuse->landuse,isroot);
      else
        fclose(landuse->landuse.file);
    }
    if(landuse->sdate.file!=NULL)
    {
      if(landuse->sdate.fmt==CDF)
        closeclimate_netcdf(&landuse->sdate,isroot);
      else
        fclose(landuse->sdate.file);
    }
    free(landuse);
  }
} /* of 'freelanduse' */
