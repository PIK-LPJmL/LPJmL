/**************************************************************************************/
/**                                                                                \n**/
/**                       n  e  w  g  r  i  d  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function newgrid allocates grid cells and reads soil code,                 \n**/
/**     country code and coordinates. If restart filename is set state             \n**/
/**     of grid cells is recovered from a restart file.                            \n**/
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

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return NULL; }

static Cell *newgrid2(Config *config,          /* Pointer to LPJ configuration */
                      int *count,
                      const Standtype standtype[], /* array of stand types */
                      int nstand,              /* number of stand types */
                      int npft,                /* number of natural PFTs */
                      int ncft                 /* number of crop PFTs */
                     ) /* returns allocated cell grid or NULL */
{
  Cell *grid;
  Header header;
  String headername;
  Stand *stand;
  int i,n,l,j,data;
  int version;
  int cft;
  Celldata celldata;
  Real lake_scalar;
  Bool swap_restart;
  Bool missing;
  Infile grassfix_file;
  Infile grassharvest_file;
  unsigned int soilcode;
  int soil_id;
#ifdef IMAGE
  Infile aquifers;
#endif
  Code code;
  FILE *file_restart;
  Infile lakes,countrycode,regioncode;

#if defined IMAGE && defined COUPLED
  Productinit *productinit;
  Product *productpool;
  if(config->sim_id==LPJML_IMAGE)
  {
    if((productpool=newvec(Product,config->ngridcell))==NULL)
    {
      printallocerr("productpool");
      free(productpool);
      return NULL;
    }
  }
#endif

  /* Open coordinate and soil file */
  celldata=opencelldata(config);
  if(celldata==NULL)
    return NULL;
  if(seekcelldata(celldata,config->startgrid))
  {
    closecelldata(celldata);
    return NULL;
  }
  if(config->countrypar!=NULL) /*does country file exist*/
  {
    if(config->countrycode_filename.fmt==CDF)
    {
      countrycode.cdf=openinput_netcdf(&config->countrycode_filename,NULL,0,config);
      if(countrycode.cdf==NULL)
      {
        closecelldata(celldata);
        return NULL;
      }
      regioncode.cdf=openinput_netcdf(&config->regioncode_filename,NULL,0,config);
      if(regioncode.cdf==NULL)
      {
        closeinput_netcdf(countrycode.cdf);
        closecelldata(celldata);
        return NULL;
      }
    }
    else
    {
      /* Open countrycode file */
      countrycode.bin.file=opencountrycode(&config->countrycode_filename,
                                           &countrycode.bin.swap,&countrycode.bin.type,&countrycode.bin.offset,isroot(*config));
      if(countrycode.bin.file==NULL)
      {
        closecelldata(celldata);
        return NULL;
      }
      if(seekcountrycode(countrycode.bin.file,config->startgrid,countrycode.bin.type,countrycode.bin.offset))
      {
        /* seeking to position of first grid cell failed */
        fprintf(stderr,
                "ERROR106: Cannot seek in countrycode file to position %d.\n",
                config->startgrid);
        closecelldata(celldata);
        fclose(countrycode.bin.file);
        return NULL;
      }
    }
    if(config->grassfix_filename.name!=NULL)
    {
      if(config->grassfix_filename.fmt==CDF)
      {
        grassfix_file.cdf=openinput_netcdf(&config->grassfix_filename,NULL,0,config);
        if(grassfix_file.cdf==NULL)
        {
          closecelldata(celldata);
          return NULL;
        }
      }
      else
      {
        /* Open grassfix file */
        grassfix_file.bin.file=openinputfile(&header,&grassfix_file.bin.swap,&config->grassfix_filename,
                                             headername,&version,&grassfix_file.bin.offset,FALSE,config);
        if(grassfix_file.bin.file==NULL)
        {
          closecelldata(celldata);
          if(config->countrypar!=NULL)
          {
            closeinput(countrycode,config->countrycode_filename.fmt);
            if(config->countrycode_filename.fmt==CDF)
              closeinput(regioncode,config->regioncode_filename.fmt);
          }
          return NULL;
        }
        grassfix_file.bin.type=(version<3) ? LPJ_BYTE : header.datatype;
        if(fseek(grassfix_file.bin.file,(config->startgrid-header.firstcell)*typesizes[grassfix_file.bin.type]+grassfix_file.bin.offset,SEEK_CUR))
        {
          /* seeking to position of first grid cell failed */
          fprintf(stderr,
                  "ERROR108: Cannot seek in grass fixed pft file to position %d.\n",
                  config->startgrid);
          closecelldata(celldata);
          fclose(grassfix_file.bin.file);
          if(config->countrypar!=NULL)
          {
            closeinput(countrycode,config->countrycode_filename.fmt);
            if(config->countrycode_filename.fmt==CDF)
              closeinput(regioncode,config->regioncode_filename.fmt);
          }
          return NULL;
        }
      }
    }
    // SR, grass management options: here choosing the grass harvest regime on the managed grassland
    if(config->grassharvest_filename.name!=NULL)
    {
      if(config->grassharvest_filename.fmt==CDF)
      {
        grassharvest_file.cdf=openinput_netcdf(&config->grassharvest_filename,NULL,0,config);
        if(grassharvest_file.cdf==NULL)
        {
          closecelldata(celldata);
          return NULL;
        }
      }
      else
      {
        /* Open grassharvest file */
        grassharvest_file.bin.file=openinputfile(&header,&grassharvest_file.bin.swap,&config->grassharvest_filename,
                                                 headername,&version,&grassharvest_file.bin.offset,FALSE,config);
        if(grassharvest_file.bin.file==NULL)
        {
          closecelldata(celldata);
          if(config->countrypar!=NULL)
          {
            closeinput(countrycode,config->countrycode_filename.fmt);
            if(config->countrycode_filename.fmt==CDF)
              closeinput(regioncode,config->regioncode_filename.fmt);
          }
          if(config->grassfix_filename.name!=NULL)
            closeinput(grassfix_file,config->grassfix_filename.fmt);
          return NULL;
        }
        grassharvest_file.bin.type=(version<3) ? LPJ_BYTE : header.datatype;
        if(fseek(grassharvest_file.bin.file,config->startgrid*typesizes[grassharvest_file.bin.type]+grassharvest_file.bin.offset,SEEK_CUR))
        {
          /* seeking to position of first grid cell failed */
          fprintf(stderr,
                  "ERROR108: Cannot seek in grass harvest options file to position %d.\n",
                  config->startgrid);
          closecelldata(celldata);
          fclose(grassharvest_file.bin.file);
          if(config->countrypar!=NULL)
          {
            closeinput(countrycode,config->countrycode_filename.fmt);
            if(config->countrycode_filename.fmt==CDF)
              closeinput(regioncode,config->regioncode_filename.fmt);
          }
          if(config->grassharvest_filename.name!=NULL)
            closeinput(grassharvest_file,config->grassharvest_filename.fmt);
          if(config->grassfix_filename.name!=NULL)
            closeinput(grassfix_file,config->grassfix_filename.fmt);
          return NULL;
        }
      }
    }
  }

  if(config->river_routing)
  {
    /* Open file for lake fraction */
    if(config->lakes_filename.fmt==CDF)
    {
      lakes.cdf=openinput_netcdf(&config->lakes_filename,"1",0,config);
      if(lakes.cdf==NULL)
      {
        closecelldata(celldata);
        if(config->countrypar!=NULL)
        {
          closeinput(countrycode,config->countrycode_filename.fmt);
          if(config->countrycode_filename.fmt==CDF)
            closeinput(regioncode,config->regioncode_filename.fmt);
        }
        if(config->grassfix_filename.name!=NULL)
          closeinput(grassfix_file,config->grassfix_filename.fmt);
        if(config->grassharvest_filename.name!=NULL)
          closeinput(grassharvest_file,config->grassharvest_filename.fmt);
        return NULL;
      }
    }
    else
    {
      lakes.bin.file=openinputfile(&header,&lakes.bin.swap,&config->lakes_filename,
                                   headername,&version,&lakes.bin.offset,FALSE,config);
      if(lakes.bin.file==NULL)
      {
        /* opening of lake fraction file failed */
        closecelldata(celldata);
        if(config->countrypar!=NULL)
        {
          closeinput(countrycode,config->countrycode_filename.fmt);
          if(config->countrycode_filename.fmt==CDF)
            closeinput(regioncode,config->regioncode_filename.fmt);
        }
        if(config->grassfix_filename.name!=NULL)
          closeinput(grassfix_file,config->grassfix_filename.fmt);
        if(config->grassharvest_filename.name!=NULL)
          closeinput(grassharvest_file,config->grassharvest_filename.fmt);
        return NULL;
      }
      lake_scalar=(version<=1) ? 0.01 : header.scalar;
      lakes.bin.type=(version<3) ? LPJ_BYTE : header.datatype;
      if(fseek(lakes.bin.file,(config->startgrid-header.firstcell)*typesizes[lakes.bin.type]+lakes.bin.offset,SEEK_CUR))
      {
        /* seeking to position of first grid cell failed */
        fprintf(stderr,
                "ERROR108: Cannot seek in lake fraction file to position %d.\n",
                config->startgrid);
        closecelldata(celldata);
        fclose(lakes.bin.file);
        if(config->countrypar!=NULL)
        {
          closeinput(countrycode,config->countrycode_filename.fmt);
          if(config->countrycode_filename.fmt==CDF)
            closeinput(regioncode,config->regioncode_filename.fmt);
        }
        if(config->grassfix_filename.name!=NULL)
          closeinput(grassfix_file,config->grassfix_filename.fmt);
        if(config->grassharvest_filename.name!=NULL)
          closeinput(grassharvest_file,config->grassharvest_filename.fmt);
        return NULL;
      }
    }
  }
#if defined IMAGE
  if(config->aquifer_irrig==AQUIFER_IRRIG)
  {
    /* Open file with aquifer locations */
    if(config->aquifer_filename.fmt==CDF)
    {
      aquifers.cdf=openinput_netcdf(&config->aquifer_filename,NULL,0,config);
      if(aquifers.cdf==NULL)
      {
        closecelldata(celldata);
        if(config->river_routing)
          closeinput(lakes,config->lakes_filename.fmt);
        if(config->countrypar!=NULL)
        {
          closeinput(countrycode,config->countrycode_filename.fmt);
          if(config->countrycode_filename.fmt==CDF)
            closeinput(regioncode,config->regioncode_filename.fmt);
        }
        if(config->grassfix_filename.name!=NULL)
          closeinput(grassfix_file,config->grassfix_filename.fmt);
        if(config->grassharvest_filename.name!=NULL)
          closeinput(grassharvest_file,config->grassharvest_filename.fmt);
        return NULL;
      }
    }
    else
    {
      aquifers.bin.file=openinputfile(&header,&aquifers.bin.swap,&config->aquifer_filename,
                                      headername,&version,&aquifers.bin.offset,FALSE,config);
      if(aquifers.bin.file==NULL)
      {
        /* opening of aquifer file failed */
        if(isroot(*config))
          printfopenerr(config->aquifer_filename.name);
        closecelldata(celldata);
        if(config->river_routing)
          closeinput(lakes,config->lakes_filename.fmt);
        if(config->countrypar!=NULL)
        {
          closeinput(countrycode,config->countrycode_filename.fmt);
          if(config->countrycode_filename.fmt==CDF)
            closeinput(regioncode,config->regioncode_filename.fmt);
        }
        if(config->grassfix_filename.name!=NULL)
          closeinput(grassfix_file,config->grassfix_filename.fmt);
        if(config->grassharvest_filename.name!=NULL)
          closeinput(grassharvest_file,config->grassharvest_filename.fmt);
        return NULL;
      }
      if(config->aquifer_filename.fmt==RAW  || version<3)
         aquifers.bin.type=LPJ_BYTE;
      else
         aquifers.bin.type=header.datatype;
      if(fseek(aquifers.bin.file,config->startgrid*typesizes[aquifers.bin.type],SEEK_CUR))
      {
        /* seeking to position of first grid cell failed */
        fprintf(stderr,
                "ERROR108: Cannot seek in aquifer file to position %d.\n",
                config->startgrid);
        closecelldata(celldata);
        fclose(aquifers.bin.file);
        if(config->river_routing)
          closeinput(lakes,config->lakes_filename.fmt);
        if(config->countrypar!=NULL)
        {
          closeinput(countrycode,config->countrycode_filename.fmt);
          if(config->countrycode_filename.fmt==CDF)
            closeinput(regioncode,config->regioncode_filename.fmt);
        }
        if(config->grassfix_filename.name!=NULL)
          closeinput(grassfix_file,config->grassfix_filename.fmt);
        if(config->grassharvest_filename.name!=NULL)
          closeinput(grassharvest_file,config->grassharvest_filename.fmt);
        return NULL;
      }
    }
  }
#endif

#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    if ((productinit = initproductinit(config)) == NULL)
    {
      if (isroot(*config))
        fprintf(stderr, "ERROR201: Cannot open file '%s'.\n",
                config->prodpool_init_filename.name);
      return NULL;
    }
    if(getproductpools(productinit,productpool,config->ngridcell))
    {
      fputs("ERROR202: Cannot read initial product pools.\n",stderr);
      return NULL;
    }
  }
#endif

  config->count=0;

  /* allocate grid */
  if((grid=newvec(Cell,config->ngridcell))==NULL)
  {
    printallocerr("grid");
    closecelldata(celldata);
    if(config->river_routing)
      closeinput(lakes,config->lakes_filename.fmt);
#ifdef IMAGE
    if(config->aquifer_irrig==AQUIFER_IRRIG)
      closeinput(aquifers,config->aquifer_filename.fmt);
#endif
    if(config->countrypar!=NULL)
    {
      closeinput(countrycode,config->countrycode_filename.fmt);
      if(config->countrycode_filename.fmt==CDF)
        closeinput(regioncode,config->regioncode_filename.fmt);
    }
    if(config->grassfix_filename.name!=NULL)
      closeinput(grassfix_file,config->grassfix_filename.fmt);
    if(config->grassharvest_filename.name!=NULL)
      closeinput(grassharvest_file,config->grassharvest_filename.fmt);
    return NULL;
  }
  config->initsoiltemp=FALSE;
  /* If FROM_RESTART open restart file */
  config->ischeckpoint=ischeckpointrestart(config) && getfilesize(config->checkpoint_restart_filename)!=-1;
  config->landuse_restart=FALSE;
  if(config->restart_filename==NULL && !config->ischeckpoint)
  {
    file_restart=NULL;
    config->initsoiltemp=TRUE;
  }
  else
  {
    file_restart=openrestart((config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename,config,npft+ncft,&swap_restart);
    if(file_restart==NULL)
    {
      free(grid);
      closecelldata(celldata);
      if(config->river_routing)
        closeinput(lakes,config->lakes_filename.fmt);
      if(config->countrypar!=NULL)
      {
        closeinput(countrycode,config->countrycode_filename.fmt);
        if(config->countrycode_filename.fmt==CDF)
          closeinput(regioncode,config->regioncode_filename.fmt);
      }
      if(config->grassfix_filename.name!=NULL)
        closeinput(grassfix_file,config->grassfix_filename.fmt);
      if(config->grassharvest_filename.name!=NULL)
        closeinput(grassharvest_file,config->grassharvest_filename.fmt);
      return NULL;
    }
    if(config->new_seed)
      setseed(config->seed,config->seed_start);
  }
  *count=0;
  for(i=0;i<config->ngridcell;i++)
  {
    /* read cell coordinate and soil code from file */
    if(readcelldata(celldata,&grid[i].coord,&soilcode,&grid[i].soilph,i,config))
      return NULL;

    if(config->countrypar!=NULL)
    {
      if(config->countrycode_filename.fmt==CDF)
      {
        if(readintinput_netcdf(countrycode.cdf,&data,&grid[i].coord,&missing) || missing)
          code.country=-1;
        else
          code.country=(short)data;
        if(readintinput_netcdf(regioncode.cdf,&data,&grid[i].coord,&missing) || missing)
          code.region=-1;
        else
          code.region=(short)data;
      }
      else
      {
        if(readcountrycode(countrycode.bin.file,&code,countrycode.bin.type,countrycode.bin.swap))
        {
          fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                  config->countrycode_filename.name,i+config->startgrid);
          return NULL;
        }
      }
      if(config->soilmap[soilcode]>0)
      {
        if(code.country<0 || code.country>=config->ncountries ||
           code.region<0 || code.region>=config->nregions)
          fprintf(stderr,"WARNING009: Invalid countrycode=%d or regioncode=%d with valid soilcode in cell (not skipped)\n",code.country,code.region);
        else
        {
          if(initmanage(&grid[i].ml.manage,config->countrypar+code.country,
                        config->regionpar+code.region,config->pftpar,npft,config->nagtree,ncft,
                        config->laimax_interpolate,config->laimax))
            return NULL;
        }
      }

      if(config->grassfix_filename.name != NULL)
      {
        if(config->grassfix_filename.fmt==CDF)
        {
          if(readintinput_netcdf(grassfix_file.cdf,&grid[i].ml.fixed_grass_pft,&grid[i].coord,&missing) || missing)
          {
            fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                    config->grassfix_filename.name,i+config->startgrid);
            return NULL;
          }
        }
        else
        {
          if(readintvec(grassfix_file.bin.file,&grid[i].ml.fixed_grass_pft,1,grassfix_file.bin.swap,grassfix_file.bin.type))
          {
            fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                    config->grassharvest_filename.name,i+config->startgrid);
            return NULL;
          }
        }
      }
      else
        grid[i].ml.fixed_grass_pft= -1;
      if(config->grassharvest_filename.name != NULL)
      {
        if(config->grassharvest_filename.fmt==CDF)
        {
          if(readintinput_netcdf(grassharvest_file.cdf,&data,&grid[i].coord,&missing) || missing)
          {
            fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                    config->grassharvest_filename.name,i+config->startgrid);
            return NULL;
          }
          grid[i].ml.grass_scenario=(GrassScenarioType)data;
        }
        else
        {
          if(readintvec(grassharvest_file.bin.file,(int *)(&grid[i].ml.grass_scenario),1,grassharvest_file.bin.swap,grassharvest_file.bin.type))
          {
            fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                    config->grassharvest_filename.name,i+config->startgrid);
            return NULL;
          }
          //grid[i].ml.grass_scenario=GS_NONE;
        }
      }
      else
        grid[i].ml.grass_scenario=GS_DEFAULT;

    }
    grid[i].lakefrac=0.0;
    if(config->river_routing)
    {
      if(config->lakes_filename.fmt==CDF)
      {
        if(readinput_netcdf(lakes.cdf,&grid[i].lakefrac,&grid[i].coord))
        {
          fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                  config->lakes_filename.name,i+config->startgrid);
          return NULL;
        }
      }
      else
      {
        if(readrealvec(lakes.bin.file,&grid[i].lakefrac,0,lake_scalar,1,lakes.bin.swap,lakes.bin.type))
        {
          fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                  config->lakes_filename.name,i+config->startgrid);
          return NULL;
        }

      }
      if(grid[i].lakefrac>1-epsilon)
        grid[i].lakefrac=1;
    }
#ifdef IMAGE
    grid[i].discharge.aquifer=0;
    if(config->aquifer_irrig==AQUIFER_IRRIG)
    {
      if(config->aquifer_filename.fmt==CDF)
      {
        if (readintinput_netcdf(aquifers.cdf, &grid[i].discharge.aquifer, &grid[i].coord,&missing) || missing)
        {
          fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                  config->aquifer_filename.name,i+config->startgrid);
          return NULL;
        }
        grid[i].discharge.aquifer = (Bool)data;
      }
      else
      {
        if(readintvec(aquifers.bin.file,&grid[i].discharge.aquifer,1,aquifers.bin.swap,aquifers.bin.type))
        {
          fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                  config->aquifer_filename.name,i+config->startgrid);
          return NULL;
        }
      }
    }
#endif
    /* Init cells */
    grid[i].ml.dam=FALSE;
    grid[i].ml.seasonality_type=NO_SEASONALITY;
    grid[i].ml.cropfrac_rf=grid[i].ml.cropfrac_ir=grid[i].ml.reservoirfrac=0;
    grid[i].ml.product.fast.carbon=grid[i].ml.product.slow.carbon=grid[i].ml.product.fast.nitrogen=grid[i].ml.product.slow.nitrogen=0;
    grid[i].balance.totw=grid[i].balance.tot.carbon=grid[i].balance.tot.nitrogen=0.0;
    grid[i].balance.estab_storage_tree[0].carbon=grid[i].balance.estab_storage_tree[1].carbon=100.0;
    grid[i].balance.estab_storage_tree[0].nitrogen=grid[i].balance.estab_storage_tree[1].nitrogen=10.0;
    grid[i].balance.estab_storage_grass[0].carbon=grid[i].balance.estab_storage_grass[1].carbon=20.0;
    grid[i].balance.estab_storage_grass[0].nitrogen=grid[i].balance.estab_storage_grass[1].nitrogen=2.0;
    grid[i].balance.surface_storage_last=grid[i].balance.soil_storage_last=0.0;
    grid[i].discharge.waterdeficit=0.0;
    grid[i].discharge.wateruse=0.0;
#ifdef IMAGE
    grid[i].discharge.wateruse_wd=0.0;
    grid[i].discharge.wateruse_fraction = 0.0;
#endif
    grid[i].balance.excess_water=0;
    grid[i].discharge.dmass_lake_max=grid[i].lakefrac*H*grid[i].coord.area*1000;
    grid[i].discharge.dmass_lake=grid[i].discharge.dmass_river=0.0;
    grid[i].discharge.dfout=grid[i].discharge.fout=0.0;
    grid[i].discharge.gir=grid[i].discharge.irrig_unmet=0.0;
    grid[i].discharge.act_irrig_amount_from_reservoir=0.0;
    grid[i].discharge.withdrawal=grid[i].discharge.wd_demand=0.0;
#ifdef IMAGE
    grid[i].discharge.dmass_gw=0.0;
    grid[i].discharge.withdrawal_gw=0.0;
#endif
    grid[i].discharge.wd_neighbour=grid[i].discharge.wd_deficit=0.0;
    grid[i].discharge.mfout=grid[i].discharge.mfin=0.0;
    grid[i].discharge.dmass_sum=0.0;
    grid[i].discharge.queue=NULL;
    grid[i].ignition.nesterov_accum=0;
    grid[i].ignition.nesterov_max=0;
    grid[i].ignition.nesterov_day=0;
    grid[i].landcover=NULL;
    grid[i].output.data=NULL;
#ifdef COUPLING_WITH_FMS
    grid[i].laketemp=0;
#endif
    if(config->withlanduse!=NO_LANDUSE)
    {
      grid[i].ml.landfrac=newlandfrac(ncft,config->nagtree);
      checkptr(grid[i].ml.landfrac);
      if(config->with_nitrogen)
      {
        grid[i].ml.fertilizer_nr=newlandfrac(ncft,config->nagtree);
        checkptr(grid[i].ml.fertilizer_nr);
        grid[i].ml.manure_nr=newlandfrac(ncft,config->nagtree);
        checkptr(grid[i].ml.manure_nr);
      }
      else
      {
        grid[i].ml.fertilizer_nr = NULL;
        grid[i].ml.manure_nr = NULL;
      }
      grid[i].ml.irrig_system=new(Irrig_system);
      checkptr(grid[i].ml.irrig_system);
      grid[i].ml.residue_on_field=newlandfrac(ncft,config->nagtree);
      checkptr(grid[i].ml.residue_on_field);
      grid[i].ml.irrig_system->crop=newvec(IrrigationType,ncft);
      grid[i].ml.irrig_system->ag_tree=newvec(IrrigationType,config->nagtree);
      checkptr(grid[i].ml.irrig_system->crop);

      for(j=0;j<ncft;j++)
        grid[i].ml.irrig_system->crop[j]=NOIRRIG;
      for(j=0;j<NGRASS;j++)
        grid[i].ml.irrig_system->grass[j]=NOIRRIG;
      for(j=0;j<config->nagtree;j++)
        grid[i].ml.irrig_system->ag_tree[j]=NOIRRIG;
      grid[i].ml.irrig_system->biomass_grass=grid[i].ml.irrig_system->biomass_tree=NOIRRIG;
      grid[i].ml.irrig_system->woodplantation = NOIRRIG;
    }
    else
    {
      grid[i].ml.landfrac=NULL;
      grid[i].ml.fertilizer_nr=NULL;
      grid[i].ml.manure_nr = NULL;
      grid[i].ml.residue_on_field = NULL;
      grid[i].ml.irrig_system=NULL;
    }
    soil_id=config->soilmap[soilcode]-1;
    if(file_restart==NULL)
    {
      if(config->soilmap[soilcode]==0)
      {
        (*count)++;
        fprintf(stderr,"Invalid soilcode=%u, cell %d skipped\n",soilcode,i+config->startgrid);
        grid[i].skip=TRUE;
      }
      else
      {
        setseed(grid[i].seed,config->seed_start+(i+config->startgrid)*36363);
        grid[i].skip=FALSE;
        grid[i].standlist=newlist();
        checkptr(grid[i].standlist);
        grid[i].gdd=newgdd(npft);
        checkptr(grid[i].gdd);
        grid[i].ml.sowing_month=newvec(int,2*ncft);
        checkptr(grid[i].ml.sowing_month);
        grid[i].ml.gs=newvec(int,2*ncft);
        checkptr(grid[i].ml.gs);
        for(l=0;l<2*ncft;l++)
        {
          grid[i].ml.sowing_month[l]=0;
          grid[i].ml.gs[l]=0;
        }
        if(grid[i].lakefrac<1)
        {
          n=addstand(&natural_stand,grid+i);
          stand=getstand(grid[i].standlist,n-1);
          stand->frac=1-grid[i].lakefrac;
          if(initsoil(stand,config->soilpar+soil_id,npft+ncft,config->with_nitrogen))
            return NULL;
          for(l=0;l<FRACGLAYER;l++)
            stand->frac_g[l]=1.0;
        }
        if(new_climbuf(&grid[i].climbuf,ncft))
        {
          printallocerr("climbuf");
          return NULL;
        }
        grid[i].ml.cropdates=init_cropdates(config->pftpar+npft,ncft,grid[i].coord.lat);
        checkptr(grid[i].ml.cropdates);
        if(config->sdate_option>NO_FIXED_SDATE)
        {
          grid[i].ml.sdate_fixed=newvec(int,2*ncft);
          checkptr(grid[i].ml.sdate_fixed);
          for(cft=0;cft<2*ncft;cft++)
            grid[i].ml.sdate_fixed[cft]=0;
        }
        else
          grid[i].ml.sdate_fixed=NULL;
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
        {
          grid[i].ml.crop_phu_fixed=newvec(Real,2*ncft);
          checkptr(grid[i].ml.crop_phu_fixed);
          for(cft=0;cft<2*ncft;cft++)
            grid[i].ml.crop_phu_fixed[cft]=0;
        }
        else
          grid[i].ml.crop_phu_fixed=NULL;
      }
    }
    else /* read cell data from restart file */
    {
      if(freadcell(file_restart,grid+i,npft,ncft,
                   config->soilpar+soil_id,standtype,nstand,
                   swap_restart,config))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                (config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename,i+config->startgrid);
        return NULL;
      }
      if(config->new_seed)
        setseed(grid[i].seed,config->seed_start+(i+config->startgrid)*36363);
      if(!grid[i].skip)
        check_stand_fracs(grid+i,
                          grid[i].lakefrac+grid[i].ml.reservoirfrac);
      else
        (*count)++;
    }
    if(!grid[i].skip)
    {
      config->count++;
#if defined IMAGE && defined COUPLED
      if(config->sim_id==LPJML_IMAGE)
      {
        if(new_image(grid+i,productpool+i))
          return NULL;
        if(i%1000==0) printf("initialized product pools in pix %d to %g %g\n",i,
          grid[i].ml.product.fast.carbon,grid[i].ml.product.slow.carbon);
        /* data sent to image */
      }
      else
        grid[i].ml.image_data=NULL;
    }
    else /* skipped cells don't need memory allocation */
    {
      grid[i].ml.image_data=NULL;
#endif
    }
  } /* of for(i=0;...) */
  if(file_restart!=NULL)
    fclose(file_restart);
  closecelldata(celldata);
  if(config->river_routing)
    closeinput(lakes,config->lakes_filename.fmt);
  if(config->grassfix_filename.name!=NULL)
      closeinput(grassfix_file,config->grassfix_filename.fmt);
  if(config->grassharvest_filename.name!=NULL)
    closeinput(grassharvest_file,config->grassharvest_filename.fmt);
  if(config->countrypar!=NULL)
  {
    closeinput(countrycode,config->countrycode_filename.fmt);
    if(config->countrycode_filename.fmt==CDF)
      closeinput(regioncode,config->regioncode_filename.fmt);
  }
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    free(productpool);
    freeproductinit(productinit);
  }
#endif
#ifdef IMAGE
  if(config->aquifer_irrig==AQUIFER_IRRIG)
    closeinput(aquifers,config->aquifer_filename.fmt);
#endif
  return grid;
} /* of 'newgrid2' */

Cell *newgrid(Config *config,          /**< Pointer to LPJ configuration */
              const Standtype standtype[], /**< array of stand types */
              int nstand,              /**< number of stand types */
              int npft,                /**< number of natural PFTs */
              int ncft                 /**< number of crop PFTs */
             ) /** \return allocated cell grid or NULL */
{
#ifdef USE_MPI
  int *counts,i;
#endif
  int count,count_total;
  Bool iserr;
  Cell *grid;
  grid=newgrid2(config,&count,standtype,nstand,npft,ncft);
  iserr=(grid==NULL);
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  if(counts==NULL)
    iserr=TRUE;
#endif
  if(iserror(iserr,config))
    return NULL;
#ifdef USE_MPI
  MPI_Allgather(&config->count,1,MPI_INT,counts,1,MPI_INT,
                config->comm);
  config->offset=0;
  for(i=0;i<config->rank;i++)
    config->offset+=counts[i];
  MPI_Allreduce(&config->count,&config->total,1,MPI_INT,MPI_SUM,
                config->comm);
  free(counts);
  MPI_Reduce(&count,&count_total,1,MPI_INT,MPI_SUM,0,config->comm);
#else
  count_total=count;
  config->total=config->count;
#endif
  if(config->total==0)
  {
    if(isroot(*config))
      fputs("ERROR207: No cell with valid soil code found.\n",stderr);
    return NULL;
  }
  if(isroot(*config) && count_total)
    printf("Invalid soil code in %d cells.\n",count_total);
  if(config->river_routing)
  {
    /* initialize river-routing network */
    if(initdrain(grid,config))
      return NULL;
  }
  if(config->reservoir)
  {
    if(initreservoir(grid,config))
      return NULL;
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(initignition(grid,config))
      return NULL;
  }
  if(config->withlanduse!=NO_LANDUSE && config->nagtree)
  {
    if(readcottondays(grid,config))
     return NULL;
  }
  return grid;
} /* of 'newgrid' */
