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

#ifdef USE_TIMING
double tread=0; /* used for timing for reading restart file */
#endif

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
  Stand *stand;
  int i,n,l,j,data;
  int cft;
  Celldata celldata;
  Bool missing;
  Infile grassharvest_file;
  unsigned int soilcode;
  int soil_id;
  int miss,miss_total;
  int skipped,skipped_total;
  char *name;
  size_t offset;
  Bool isregion;
#ifdef IMAGE
  Infile aquifers;
#ifdef COUPLED
  Productinit *productinit=NULL;
  Product *productpool=NULL;
#endif
#endif
  int code;
  Bstruct file_restart;
  Infile countrycode;

  /* Open coordinate and soil file */
  celldata=opencelldata(config);
  if(celldata==NULL)
    return NULL;
#if defined IMAGE && defined COUPLED
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
  if(seekcelldata(celldata,config->startgrid))
  {
    closecelldata(celldata,config);
    return NULL;
  }
  if(config->countrypar!=NULL) /*does country file exist*/
  {
    countrycode.fmt=config->countrycode_filename.fmt;
    if(config->countrycode_filename.fmt==CDF)
    {
      countrycode.cdf=openinput_netcdf(&config->countrycode_filename,NULL,0,config);
      if(countrycode.cdf==NULL)
      {
        closecelldata(celldata,config);
        return NULL;
      }
    }
    else
    {
      /* Open countrycode file */
      countrycode.file=opencountrycode(&config->countrycode_filename,
                                       &countrycode.swap,&isregion,&countrycode.type,&offset,isroot(*config));
      if(countrycode.file==NULL)
      {
        closecelldata(celldata,config);
        return NULL;
      }
      if(seekcountrycode(countrycode.file,config->startgrid,(isregion) ? 2 : 1,countrycode.type,offset))
      {
        /* seeking to position of first grid cell failed */
        fprintf(stderr,
                "ERROR106: Cannot seek in countrycode file to position %d.\n",
                config->startgrid);
        closecelldata(celldata,config);
        fclose(countrycode.file);
        return NULL;
      }
    }
    if(config->grassharvest_filename.name!=NULL)
    {
      // SR, grass management options: here choosing the grass harvest regime on the managed grassland
      if(openinputdata(&grassharvest_file,&config->grassharvest_filename,"grass harvest",NULL,LPJ_BYTE,1.0,0,config))
      {
        closecelldata(celldata,config);
        if(config->countrypar!=NULL)
        {
          closeinput(&countrycode);
        }
        return NULL;
      }
    }
  }

#if defined IMAGE
  if(config->aquifer_irrig)
  {
    /* Open file with aquifer locations */
    if(openinputdata(&aquifers,&config->aquifer_filename,"aquifer",NULL,LPJ_BYTE,1.0,0,config))
    {
      closecelldata(celldata,config);
      if(config->countrypar!=NULL)
      {
        closeinput(&countrycode);
      }
      if(config->grassharvest_filename.name!=NULL)
         closeinput(&grassharvest_file);
      return NULL;
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
    closecelldata(celldata,config);
#ifdef IMAGE
    if(config->aquifer_irrig)
      closeinput(&aquifers);
#endif
    if(config->countrypar!=NULL)
    {
      closeinput(&countrycode);
    }
    if(config->grassharvest_filename.name!=NULL)
      closeinput(&grassharvest_file);
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
#ifdef USE_TIMING
    if(isroot(*config))
       tread=mrun();
#endif
    file_restart=openrestart((config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename,config,npft,ncft);
    if(file_restart==NULL)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR254: Cannot open %s file '%s'.\n",(config->ischeckpoint) ? "checkpoint" : "restart",
                (config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename);
      free(grid);
      closecelldata(celldata,config);
      if(config->countrypar!=NULL)
      {
        closeinput(&countrycode);
      }
      if(config->grassharvest_filename.name!=NULL)
        closeinput(&grassharvest_file);
      return NULL;
    }
    skipped=bstruct_getskipped(file_restart);
    if(!config->ischeckpoint && config->new_seed)
      setseed(config->seed,config->seed_start);
  }
  *count=0;
  for(i=0;i<config->ngridcell;i++)
  {
    /* read cell coordinate and soil code from file */
    if(readcelldata(celldata,grid+i,&soilcode,i,config))
      return NULL;

    if(config->countrypar!=NULL)
    {
      if(config->countrycode_filename.fmt==CDF)
      {
        if(readintinput_netcdf(countrycode.cdf,&data,&grid[i].coord,&missing) || missing)
          code=-1;
        else
          code=data;
      }
      else
      {
        if(readcountrycode(countrycode.file,&code,countrycode.type,isregion,countrycode.swap))
        {
          name=getrealfilename(&config->countrycode_filename);
          fprintf(stderr,"ERROR190: Cannot read country code from '%s' for cell %d.\n",
                  name,i+config->startgrid);
          free(name);
          return NULL;
        }
      }
      if(config->soilmap[soilcode]>0)
      {
        if(code<0 || code>=config->ncountries)
        {
          name=getrealfilename(&config->countrycode_filename);
          fprintf(stderr,"ERROR190: Invalid country code=%d read from '%s' in cell %d, must be in [0,%d].\n",
                  code,name,i+config->startgrid,config->ncountries-1);
          free(name);
          return NULL;
        }
        else
        {
          if(initmanage(&grid[i].ml.manage,code,npft,ncft,config))
            return NULL;
        }
      }

      if(config->grassharvest_filename.name != NULL)
      {
        if(readintinputdata(&grassharvest_file,(int *)&grid[i].ml.grass_scenario,NULL,&grid[i].coord,i+config->startgrid,&config->grassharvest_filename))
          return NULL;
      }
      else
        grid[i].ml.grass_scenario=(GrassScenarioType)config->grazing;

    }
#ifdef IMAGE
    grid[i].discharge.aquifer=0;
    if(config->aquifer_irrig)
    {
      if(readintinputdata(&aquifers,&grid[i].discharge.aquifer,NULL,&grid[i].coord,i+config->startgrid,&config->aquifer_filename))
        return NULL;
    }
#endif
    /* Init cells */
    grid[i].ml.grassland_lsuha=param.lsuha;
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
    grid[i].discharge.fin_ext=0.0;
    grid[i].discharge.afin_ext=0.0;
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
      grid[i].ml.fertilizer_nr=newlandfrac(ncft,config->nagtree);
      checkptr(grid[i].ml.fertilizer_nr);
      grid[i].ml.manure_nr=newlandfrac(ncft,config->nagtree);
      checkptr(grid[i].ml.manure_nr);
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
        grid[i].standlist=newlist(0);
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
          if(initsoil(stand,config->soilpar+soil_id,npft+ncft,config))
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
        if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
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
                   config))
      {
        fprintf(stderr,"ERROR190: Cannot read restart data from '%s' for cell %d.\n",
                (config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename,i+config->startgrid);
        return NULL;
      }
      if(!config->ischeckpoint && config->new_seed)
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
  {
    miss=bstruct_getmiss(file_restart);
#ifdef USE_MPI
    MPI_Allreduce(&miss,&miss_total,1,MPI_INT,MPI_SUM,config->comm);
#else
    miss_total=miss;
#endif
    if(isroot(*config) && miss_total)
      fprintf(stderr,"REMARK002: %d objects not in right order in restart file '%s'.\n",
              miss_total,(config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename);
    if(miss_total==0)
    {
      skipped=bstruct_getskipped(file_restart)-skipped;
#ifdef USE_MPI
      MPI_Reduce(&skipped,&skipped_total,1,MPI_INT,MPI_SUM,0,config->comm);
#else
      skipped_total=skipped;
#endif
      if(isroot(*config) && skipped_total)
        fprintf(stderr,"REMARK003: %d objects skipped in restart file '%s'.\n",
                skipped_total,(config->ischeckpoint) ? config->checkpoint_restart_filename : config->restart_filename);
    }
    bstruct_finish(file_restart);
  }
  closecelldata(celldata,config);
  if(config->grassharvest_filename.name!=NULL)
    closeinput(&grassharvest_file);
  if(config->countrypar!=NULL)
  {
    closeinput(&countrycode);
  }
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    free(productpool);
    freeproductinit(productinit);
  }
#endif
#ifdef IMAGE
  if(config->aquifer_irrig)
    closeinput(&aquifers);
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
  {
#ifdef USE_MPI
    free(counts);
#endif
    return NULL;
  }
#ifdef USE_TIMING
  if(isroot(*config))
    tread=mrun()-tread;
#endif
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
    {
      if(isroot(*config))
        fputs("ERROR207: Cannot initialize river network.\n",stderr);
      return NULL;
    }
  }
  if(config->reservoir)
  {
    if(initreservoir(grid,config))
    {
      if(isroot(*config))
        fputs("ERROR207: Cannot initialize reservoir data.\n",stderr);
      return NULL;
    }
  }
  if(config->withlanduse!=NO_LANDUSE && config->iscotton)
  {
    if(readcottondays(grid,config))
     return NULL;
  }
  return grid;
} /* of 'newgrid' */
