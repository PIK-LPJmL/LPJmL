/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  c  o  n  f  i  g  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads configuration from an ascii file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanint2(file,var,name) if(fscanint(file,var,name,FALSE,verbose)) return TRUE;
#define fscanreal2(file,var,name) if(fscanreal(file,var,name,FALSE,verbose)) return TRUE;
#define fscanbool2(file,var,name) if(fscanbool(file,var,name,FALSE,verbose)) return TRUE;
#define fscanname(file,var,name) {              \
    if(fscanstring(file,var,name,FALSE,verbose)) {                 \
      if(verbose) readstringerr(name);  \
      return TRUE;                              \
    }                                              \
  }

#define scanfilename(file,var,path,what) {                              \
    if(readfilename2(file,var,what,path,verbose)) {                  \
      if(verbose) fprintf(stderr,"ERROR209: Cannot read filename for '%s' input.\n",what); \
      return TRUE;                                                      \
    }                                                                   \
    if(verbose>=VERB)                      \
      printf("%s %s\n", what, (var)->name);                             \
  }

#define scanclimatefilename(file,var,path,isfms,what) {                 \
    if(readclimatefilename(file,var,what,path,isfms,verbose)) {      \
      if(verbose) fprintf(stderr,"ERROR209: Cannot read filename for '%s' input.\n",what); \
      return TRUE;                                                      \
    }                                                                   \
    if(verbose>=VERB)                      \
      printf("%s %s\n", what, (var)->name);                             \
  }

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE; }

const char *crop_phu_options[]={"old","new","prescribed"};
const char *grazing_type[]={"default","mowing","ext","int","none"};

static Bool readfilename2(LPJfile *file,Filename *name,const char *key,const char *path,Verbosity verbose)
{
  if(readfilename(file,name,key,path,FALSE,verbose))
    return TRUE;
  if(name->fmt==CDF)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: NetCDF is not supported for input '%s' in this version of LPJmL.\n",name->name);
    return TRUE;
  }
  else if(name->fmt==TXT)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: text file is not supported for input '%s' in this version of LPJmL.\n",name->name);
    return TRUE;
  }
  return FALSE;
} /* of 'readfilename2' */

static Bool readclimatefilename(LPJfile *file,Filename *name,const char *key,const char *path,Bool isfms,Verbosity verbose)
{
  if(readfilename(file,name,key,path,TRUE,verbose))
    return TRUE;
  if(!isfms && name->fmt==FMS)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: FMS coupler not allowed for input '%s'.\n",key);
    return TRUE;
  }
  if(name->fmt==TXT)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: text file is not supported for input '%s' in this version of LPJmL.\n",name->name);
    return TRUE;
  }
  return FALSE;
} /* of 'readclimatefilename' */


static void divide(int *start, /**< index of first grid cell */
                   int *end,   /**< index of last grid cell */
                   int rank,   /**< my rank id */
                   int ntask   /**< total number of tasks */
                  )
{
/*
 * Function is used in the parallel MPI version to distribute the cell grid
 * equally on ntask tasks
 * On return start and end are set to the local boundaries of each task
 */
  int i,lo,hi,n;
  n=*end-*start+1;
  lo=*start;
  hi=*start+n/ntask-1;
  if(n % ntask)
    hi++;
  for(i=1;i<=rank;i++)
  {
    lo=hi+1;
    hi=lo+n/ntask-1;
    if(n % ntask>i)
      hi++;
  }
  *start=lo;
  *end=hi;
} /* of 'divide' */

Bool fscanconfig(Config *config,    /**< LPJ configuration */
                 LPJfile *file,     /**< File pointer to LPJ file */
                 Pfttype scanfcn[], /**< array of PFT-specific scan
                                         functions */
                 int ntypes,        /**< Number of PFT classes */
                 int nout_max       /**< maximum number of output files */
                )                   /** \return TRUE on error */
 {
  String name;
  LPJfile input;
  int restart,endgrid,israndom,grassfix,grassharvest;
  Verbosity verbose;
  const char *landuse[]={"no","yes","const","all_crops","only_crops"};
  const char *fertilizer[]={"no","yes","auto"};
  const char *irrigation[]={"no","lim","pot","all"};
  const char *radiation[]={"cloudiness","radiation","radiation_swonly","radiation_lwdown"};
  const char *fire[]={"no_fire","fire","spitfire","spitfire_tmax"};
  const char *sowing_data_option[]={"no_fixed_sdate","fixed_sdate","prescribed_sdate"};
  const char *soilpar_option[]={"no_fixed_soilpar","fixed_soilpar","prescribed_soilpar"};
  const char *wateruse[]={"no","yes","all"};
  const char *prescribe_landcover[]={"no_landcover","landcoverest","landcoverfpc"};
  const char *laimax_interpolate[]={"laimax_cft","laimax_interpolate","const_lai_max","laimax_par"};
  const char *fdi[]={"nesterov","wvpd"};
  const char *nitrogen[]={"no","lim","unlim"};
  const char *tillage[]={"no","all","read"};
  const char *residue_treatment[]={"no_residue_remove","fixed_residue_remove","read_residue_data"};
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;

  /*=================================================================*/
  /* I. Reading type section                                         */
  /*=================================================================*/

  if (verbose>=VERB) puts("// I. type section");
  fscanbool2(file,&israndom,"random_prec");
  config->seed_start=RANDOM_SEED;
  if(isstring(file,"random_seed"))
  {
    fscanstring(file,name,"random_seed",FALSE,verbose);
    if(!strcmp(name,"random_seed"))
      config->seed_start=RANDOM_SEED;
    else
    {
      if(verbose)
        fprintf(stderr,"ERROR233: Invalid string '%s' for random_seed, must be 'random_seed' or number.\n",name);
      return TRUE;
    }
  }
  else if(fscanint(file,&config->seed_start,"random_seed",TRUE,verbose))
    return TRUE;
  if(config->seed_start==RANDOM_SEED)
    config->seed_start=time(NULL);
  setseed(config->seed,config->seed_start);
  config->with_nitrogen=NO_NITROGEN;
  if(fscankeywords(file,&config->with_nitrogen,"with_nitrogen",nitrogen,3,TRUE,verbose))
    return TRUE;
  if(fscankeywords(file,&config->with_radiation,"radiation",radiation,4,FALSE,verbose))
    return TRUE;
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE && config->with_radiation)
  {
    if(verbose)
      fputs("ERROR208: Radiation data not supported by IMAGE coupler.\n",
            stderr);
    return TRUE;
  }
#endif
  if(fscankeywords(file,&config->fire,"fire",fire,4,FALSE,verbose))
    return TRUE;
  if(config->sim_id==LPJ)
    config->firewood=NO_FIREWOOD;
  else
  {
    fscanbool2(file,&config->firewood,"firewood");
  }
  if(config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)
  {
    if(fscankeywords(file,&config->fdi,"fdi",fdi,2,FALSE,verbose))
      return TRUE;
    if(config->fdi==WVPD_INDEX && verbose)
      fputs("WARNING029: VPD index only calibrated for South America.\n",stderr);
  }
  fscanbool2(file,&config->ispopulation,"population");
  config->prescribe_burntarea=FALSE;
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(fscanbool(file,&config->prescribe_burntarea,"prescribe_burntarea",TRUE,verbose))
      return TRUE;
  }
  config->prescribe_landcover=NO_LANDCOVER;
  if(fscankeywords(file,&config->prescribe_landcover,"prescribe_landcover",prescribe_landcover,3,TRUE,verbose))
    return TRUE;
  fscanbool2(file,&config->new_phenology,"new_phenology");
  config->new_trf=FALSE;
  if(fscanbool(file,&config->new_trf,"new_trf",TRUE,verbose))
    return TRUE;
  fscanbool2(file,&config->river_routing,"river_routing");
  config->extflow=FALSE;
  if(config->river_routing)
  {
    if(fscanbool(file,&config->extflow,"extflow",TRUE,verbose))
      return TRUE;
  }
  config->reservoir=FALSE;
#ifdef IMAGE
  config->groundwater_irrig = NO_GROUNDWATER_IRRIG;
  config->aquifer_irrig = NO_AQUIFER_IRRIG;
#endif
  fscanbool2(file,&config->permafrost,"permafrost");
  config->johansen = TRUE;
  if(fscanbool(file,&config->johansen,"johansen",TRUE,verbose))
    return TRUE;
  config->sdate_option=NO_FIXED_SDATE;
  config->crop_phu_option=NEW_CROP_PHU;
  config->rw_manage=FALSE;
  config->const_climate=FALSE;
  config->tillage_type=NO_TILLAGE;
  config->residue_treatment=NO_RESIDUE_REMOVE;
  config->no_ndeposition=FALSE;
  config->cropsheatfrost=FALSE;
  config->black_fallow=FALSE;
  config->double_harvest=FALSE;
  config->others_to_crop = FALSE;
  config->soilpar_option=NO_FIXED_SOILPAR;
  if(fscankeywords(file,&config->soilpar_option,"soilpar_option",soilpar_option,3,TRUE,verbose))
    return TRUE;
  if(config->soilpar_option==FIXED_SOILPAR)
  {
    fscanint2(file,&config->soilpar_fixyear,"soilpar_fixyear");
  }
  if(fscanbool(file,&config->const_climate,"const_climate",TRUE,verbose))
    return TRUE;
  config->storeclimate=TRUE;;
  if(fscanbool(file,&config->storeclimate,"store_climate",TRUE,verbose))
    return TRUE;
  config->shuffle_climate=FALSE;
  if(fscanbool(file,&config->shuffle_climate,"shuffle_climate",TRUE,verbose))
    return TRUE;
  config->fix_climate=FALSE;
  if(fscanbool(file,&config->fix_climate,"fix_climate",TRUE,verbose))
    return TRUE;
  if(config->fix_climate)
  {
    fscanint2(file,&config->fix_climate_year,"fix_climate_year");
    fscanint2(file,&config->fix_climate_cycle,"fix_climate_cycle");
  }
  config->const_deposition=FALSE;
  if(config->with_nitrogen==LIM_NITROGEN)
  {
    if(fscanbool(file,&config->const_deposition,"const_deposition",TRUE,verbose))
      return TRUE;
    if(fscanbool(file,&config->no_ndeposition,"no_ndeposition",TRUE,verbose))
      return TRUE;
  }
  config->fertilizer_input=NO_FERTILIZER;
  config->fire_on_grassland=FALSE;
  if(config->sim_id!=LPJ)
  {
    if(fscankeywords(file,&config->withlanduse,"landuse",landuse,5,FALSE,verbose))
      return TRUE;
    if(config->withlanduse!=NO_LANDUSE)
    {
      if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && !config->prescribe_burntarea)
      {
        if(fscanbool(file,&config->fire_on_grassland,"fire_on_grassland",TRUE,verbose))
          return TRUE;
      }
      if(fscanbool(file,&config->double_harvest,"double_harvest",TRUE,verbose))
        return TRUE;
      if(config->withlanduse==CONST_LANDUSE || config->withlanduse==ALL_CROPS || config->withlanduse==ONLY_CROPS)
        fscanint2(file,&config->landuse_year_const,"landuse_year_const");
      config->fix_landuse=FALSE;
      if(config->withlanduse!=CONST_LANDUSE)
      {
        if(fscanbool(file,&config->fix_landuse,"fix_landuse",TRUE,verbose))
          return TRUE;
        if(config->fix_landuse)
        {
          fscanint2(file,&config->fix_landuse_year,"fix_landuse_year");
        }
      }
      if(fscankeywords(file,&config->sdate_option,"sowing_date_option",sowing_data_option,3,FALSE,verbose))
        return TRUE;
      if(config->sdate_option==FIXED_SDATE || config->sdate_option==PRESCRIBED_SDATE)
        fscanint2(file,&config->sdate_fixyear,"sdate_fixyear");
      if(fscankeywords(file,&config->irrig_scenario,"irrigation",irrigation,4,FALSE,verbose))
        return TRUE;
      if(fscankeywords(file,&config->crop_phu_option,"crop_phu_option",crop_phu_options,3,TRUE,verbose))
        return TRUE;
      fscanbool2(file,&config->intercrop,"intercrop");
      config->crop_resp_fix=FALSE;
      config->manure_input=FALSE;
      config->fix_fertilization=FALSE;
      if(config->with_nitrogen)
      {
        if(fscanbool(file,&config->crop_resp_fix,"crop_resp_fix",TRUE,verbose))
          return TRUE;
      }
      if(config->with_nitrogen==LIM_NITROGEN)
      {
        if(fscanbool(file,&config->fix_fertilization,"fix_fertilization",TRUE,verbose))
          return TRUE;
        if(!config->fix_fertilization)
        {
          config->fertilizer_input=FERTILIZER;
          if(fscankeywords(file,&config->fertilizer_input,"fertilizer_input",fertilizer,3,TRUE,verbose))
            return TRUE;
          if(config->fertilizer_input!=AUTO_FERTILIZER)
          {
            if (fscanbool(file,&config->manure_input,"manure_input",TRUE,verbose))
              return TRUE;
          }
        }
      }
      if (fscanbool(file,&config->cropsheatfrost,"cropsheatfrost",TRUE, verbose))
        return TRUE;
      if(config->cropsheatfrost && config->fire==SPITFIRE)
        config->fire=SPITFIRE_TMAX;
      config->grassonly = FALSE;
      if (fscanbool(file, &config->grassonly, "grassonly", TRUE, verbose))
        return TRUE;
      config->istimber=FALSE;
      if(fscanbool(file,&config->istimber,"istimber",TRUE,verbose))
        return TRUE;
      config->residues_fire=FALSE;
      if(fscanbool(file,&config->residues_fire,"residues_fire",TRUE,verbose))
        return TRUE;
      if(fscanbool(file,&config->rw_manage,"rw_manage",TRUE,verbose))
        return TRUE;
      if(fscankeywords(file,&config->laimax_interpolate,"laimax_interpolate",laimax_interpolate,4,FALSE,verbose))
        return TRUE;
      if(config->laimax_interpolate==CONST_LAI_MAX)
        fscanreal2(file,&config->laimax,"laimax");
      if (fscanbool(file, &config->others_to_crop, "others_to_crop", TRUE, verbose))
        return TRUE;
      if(config->river_routing)
      {
        fscanbool2(file,&config->reservoir,"reservoir");
#ifdef IMAGE
        fscanbool(file,&config->groundwater_irrig,"groundwater_irrigation", TRUE,verbose);
        fscanbool(file,&config->aquifer_irrig,"aquifer_irrigation",TRUE,verbose);
#endif
      }
      grassfix=FALSE;
      if(fscanbool(file,&grassfix,"grassland_fixed_pft",TRUE,verbose))
        return TRUE;
      grassharvest=FALSE;
      if(fscanbool(file,&grassharvest,"grass_harvest_options", TRUE, verbose))
        return TRUE;
      if(!grassharvest)
      {
        config->grazing=GS_DEFAULT;
        if(fscankeywords(file,&config->grazing,"grazing",grazing_type,5,TRUE,verbose))
          return TRUE;
      }
      if(fscanmowingdays(file,config))
        return TRUE;
      if(fscankeywords(file,&config->tillage_type,"tillage_type",tillage,3,TRUE,verbose))
        return TRUE;
      if(config->tillage_type)
      {
        fscanint2(file,&config->till_startyear,"till_startyear");
      }
      if(fscankeywords(file,&config->residue_treatment,"residue_treatment",residue_treatment,3,TRUE,verbose))
        return TRUE;
    }
    if(fscanbool(file,&config->black_fallow,"black_fallow",TRUE,verbose))
      return TRUE;
    if(config->black_fallow)
    {
      fscanbool2(file,&config->till_fallow,"till_fallow");
      fscanbool2(file,&config->prescribe_residues,"prescribe_residues");
    }
    if(isboolean(file,"wateruse"))
    {
      if(isroot(*config))
        fputs("WARNING028: Type of 'wateruse' is boolean, converted to int.\n",stderr);
      fscanbool2(file,&config->wateruse,"wateruse");
    }
    else
    {
      if(fscankeywords(file,&config->wateruse,"wateruse",wateruse,3,FALSE,verbose))
        return TRUE;
    }
    if(config->wateruse && config->withlanduse==NO_LANDUSE)
    {
      if(verbose)
        fputs("ERROR224: Wateruse without landuse set.\n",stderr);
      return TRUE;
    }
  }
  else
  {
    config->withlanduse=NO_LANDUSE;
    config->wateruse=NO_WATERUSE;
  }
  /*=================================================================*/
  /* II. Reading input parameter section                             */
  /*=================================================================*/

  if (verbose>=VERB) puts("// II. input parameter section");

  /* ntypes is set to natural vegetation must be considered
   * in light and establishment
   * crops must have last id-number */
  /* Read PFT parameter array */
  if(fscanparam(file,config))
  {
    if(verbose)
      fputs("ERROR230: Cannot read LPJ parameter 'param'.\n",stderr);
    return TRUE;
  }
  if((config->nsoil=fscansoilpar(file,&config->soilpar,config->soilpar_option,config->with_nitrogen,verbose))==0)
  {
    if(verbose)
      fputs("ERROR230: Cannot read soil parameter 'soilpar'.\n",stderr);
    return TRUE;
  }
  config->soilmap=fscansoilmap(file,&config->soilmap_size,config);
  if(config->soilmap==NULL)
    return TRUE;
  config->ntypes=ntypes;
  if(fscanpftpar(file,scanfcn,config))
  {
    if(verbose)
      fputs("ERROR230: Cannot read PFT parameter 'pftpar'.\n",stderr);
    return TRUE;
  }
  config->nbiomass=getnculttype(config->pftpar,config->npft[GRASS]+config->npft[TREE],BIOMASS);
  config->nagtree=getnculttype(config->pftpar,config->npft[GRASS]+config->npft[TREE],ANNUAL_TREE);
  config->nwft=getnculttype(config->pftpar, config->npft[GRASS] + config->npft[TREE],WP);
  config->nwptype=(config->nwft) ? NWPTYPE : 0;
  config->ngrass=getngrassnat(config->pftpar,config->npft[GRASS]+config->npft[TREE]);
  config->iscotton=findpftname("cotton",config->pftpar+config->npft[GRASS]+config->npft[TREE]-config->nagtree,config->nagtree)!=NOT_FOUND;
  if(config->others_to_crop)
  {
    if(fscanstring(file,name,"cft_temp",FALSE,verbose))
      return TRUE;
    config->cft_temp=findpftname(name,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
    if(config->cft_temp==NOT_FOUND)
    {
      if(verbose)
      {
        fprintf(stderr,"ERROR230: Invalid CFT '%s' for 'cft_temp', must be ",name);
        fprintpftnames(stderr,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
        fputs(".\n",stderr);
      }
      return TRUE;
    }
    if(fscanstring(file,name,"cft_tropic",FALSE,verbose))
      return TRUE;
    config->cft_tropic=findpftname(name,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
    if(config->cft_tropic==NOT_FOUND)
    {
      if(verbose)
      {
        fprintf(stderr,"ERROR230: Invalid CFT '%s' for 'cft_tropic', must be ",name);
        fprintpftnames(stderr,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
        fputs(".\n",stderr);
      }
      return TRUE;
    }
  }
  if(config->black_fallow && config->prescribe_residues)
  {
    if(fscanstring(file,name,"residue_pft",FALSE,verbose))
      return TRUE;
    config->pft_residue=findpftname(name,config->pftpar,config->npft[GRASS]+config->npft[TREE]+config->npft[CROP]);
    if(config->pft_residue==NOT_FOUND)
    {
      if(verbose)
      {
        fprintf(stderr,"ERROR230: Invalid PFT '%s' for black fallow, must be ",name);
        fprintpftnames(stderr,config->pftpar,config->npft[GRASS]+config->npft[TREE]+config->npft[CROP]);
        fputs(".\n",stderr);
      }
      return TRUE;
    }
  }
  /* Read soil paramater array */
  if(config->withlanduse!=NO_LANDUSE)
  {
    /* landuse enabled */
    if((config->ncountries=fscancountrypar(file,&config->countrypar,config->npft[GRASS]+config->npft[TREE],(config->laimax_interpolate==LAIMAX_CFT) ? config->npft[CROP] : 0,verbose,config))==0)
    {
      if(verbose)
        fputs("ERROR230: Cannot read country parameter 'countrypar'.\n",stderr);
      return TRUE;
    }
    if((config->nregions=fscanregionpar(file,&config->regionpar,verbose))==0)
    {
      if(verbose)
        fputs("ERROR230: Cannot read region parameter 'regionpar'.\n",stderr);
      return TRUE;
    }
    if(config->nagtree)
    {
      if (fscantreedens(file,config->countrypar,config->ncountries,config->npft[GRASS]+config->npft[TREE],verbose,config)==0)
      {
        if(verbose)
          fputs("ERROR230: Cannot read tree density (k_est) parameter 'treedens'.\n",stderr);
        return TRUE;
      }
    }
  }
  else
  {
    config->countrypar=NULL;
    config->regionpar=NULL;
  }
  config->compress=0;
  if(fscanint(file,&config->compress,"compress",TRUE,verbose))
    return TRUE;
#ifdef USE_NETCDF
  if(config->compress && verbose)
    fputs("WARNING403: Compression of NetCDF files is not supported in this version of NetCDF.\n",stderr);
#endif
  config->missing_value=MISSING_VALUE_FLOAT;
  if(fscanfloat(file,&config->missing_value,"missing_value",TRUE,verbose))
    return TRUE;
  fscanname(file,name,"pft_index");
  config->pft_index=strdup(name);
  checkptr(config->pft_index);
  fscanname(file,name,"layer_index");
  config->layer_index=strdup(name);
  checkptr(config->layer_index);
  config->outnames=fscanoutputvar(file,NOUT,verbose);
  if(config->outnames==NULL)
  {
    if(verbose)
      fputs("ERROR230: Cannot read output description 'outputvar'.\n",stderr);
    return TRUE;
  }
  /*=================================================================*/
  /* III. Reading input data section                                 */
  /*=================================================================*/

  if (verbose>=VERB) puts("// III. input data section");
  config->check_climate=FALSE;
  if(iskeydefined(file,"check_climate"))
  {
    fscanbool2(file,&config->check_climate,"check_climate");
  }
  if(iskeydefined(file,"inpath"))
  {
    if(fscanstring(file,name,"inpath",FALSE,verbose))
      return TRUE;
    free(config->inputdir);
    config->inputdir=strdup(name);
    checkptr(config->inputdir);
  }
  if(fscanstruct(file,&input,"input",verbose))
    return TRUE;
  scanclimatefilename(&input,&config->soil_filename,config->inputdir,FALSE,"soil");
  if(config->soil_filename.fmt!=CDF)
  {
    scanfilename(&input,&config->coord_filename,config->inputdir,"coord");
  }
  if(config->withlanduse!=NO_LANDUSE)
  {
    config->landusemap=scancftmap(file,&config->landusemap_size,"landusemap",FALSE,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
    if(config->landusemap==NULL)
      return TRUE;
    if(config->withlanduse!=ALL_CROPS && !findcftmap("cotton",config->pftpar,config->landusemap,config->landusemap_size))
      config->iscotton=FALSE;
    if(config->fertilizer_input==FERTILIZER || config->manure_input || config->residue_treatment==READ_RESIDUE_DATA || config->tillage_type==READ_TILLAGE)
    {
      config->fertilizermap=scancftmap(file,&config->fertilizermap_size,"fertilizermap",FALSE,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
      if(config->fertilizermap==NULL)
        return TRUE;
    }
    if(config->sdate_option==PRESCRIBED_SDATE || config->crop_phu_option==PRESCRIBED_CROP_PHU)
    {
      config->cftmap=scancftmap(file,&config->cftmap_size,"cftmap",TRUE,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
      if(config->cftmap==NULL)
        return TRUE;
    }
    scanclimatefilename(&input,&config->countrycode_filename,config->inputdir,FALSE,"countrycode");
    if(config->countrycode_filename.fmt==CDF)
    {
      scanclimatefilename(&input,&config->regioncode_filename,config->inputdir,FALSE,"regioncode");
    }
    scanclimatefilename(&input,&config->landuse_filename,config->inputdir,FALSE,"landuse");
    if(config->iscotton)
    {
      scanclimatefilename(&input,&config->sowing_cotton_rf_filename,config->inputdir,FALSE,"sowing_ag_tree_rf");
      scanclimatefilename(&input,&config->harvest_cotton_rf_filename,config->inputdir,FALSE,"harvest_ag_tree_rf");
      scanclimatefilename(&input,&config->sowing_cotton_ir_filename,config->inputdir,FALSE,"sowing_ag_tree_ir");
      scanclimatefilename(&input,&config->harvest_cotton_ir_filename,config->inputdir,FALSE,"harvest_ag_tree_ir");
    }
    if(config->sdate_option==PRESCRIBED_SDATE)
    {
      scanclimatefilename(&input,&config->sdate_filename,config->inputdir,FALSE,"sdate");
    }
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
    {
      scanclimatefilename(&input,&config->crop_phu_filename,config->inputdir,FALSE,"crop_phu");
    }
    if(config->with_nitrogen && config->fertilizer_input==FERTILIZER)
      scanclimatefilename(&input,&config->fertilizer_nr_filename,config->inputdir,FALSE,"fertilizer_nr");
    if (config->with_nitrogen && config->manure_input)
      scanclimatefilename(&input,&config->manure_nr_filename,config->inputdir,FALSE,"manure_nr");
    if (config->tillage_type==READ_TILLAGE)
      scanclimatefilename(&input,&config->with_tillage_filename,config->inputdir,FALSE,"with_tillage");
    if (config->residue_treatment == READ_RESIDUE_DATA)
      scanclimatefilename(&input,&config->residue_data_filename,config->inputdir,FALSE,"residue_on_field");
    if(grassfix == GRASS_FIXED_PFT)
    {
      scanclimatefilename(&input,&config->grassfix_filename,config->inputdir,FALSE,"grassland_fixed_pft");
    }
    else
      config->grassfix_filename.name = NULL;
    if(grassharvest)
    {
      scanclimatefilename(&input,&config->grassharvest_filename,config->inputdir,FALSE,"grass_harvest_options");
    }
    else
      config->grassharvest_filename.name = NULL;
  }
  else
  {
    config->grassfix_filename.name = NULL;
    config->grassharvest_filename.name = NULL;
  }
  if(config->river_routing)
  {
    scanclimatefilename(&input,&config->lakes_filename,config->inputdir,FALSE,"lakes");
    scanclimatefilename(&input,&config->drainage_filename,config->inputdir,FALSE,"drainage");
    if(config->drainage_filename.fmt==CDF)
    {
      scanclimatefilename(&input,&config->river_filename,config->inputdir,FALSE,"river");
    }
    if(config->extflow)
    {
      scanclimatefilename(&input,&config->extflow_filename,config->inputdir,FALSE,"extflow");
      if(config->extflow_filename.fmt!=META && config->extflow_filename.fmt!=CLM && config->extflow_filename.fmt!=CLM2)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR197: Only clm format is supported for extflow input in this version of LPJmL, %s not allowed.\n",
                  fmt[config->extflow_filename.fmt]);
        return TRUE;
      }
    }
    if(config->withlanduse!=NO_LANDUSE)
    {
      scanclimatefilename(&input,&config->neighb_irrig_filename,config->inputdir,FALSE,"neighb_irrig");
      if(config->reservoir)
      {
        scanclimatefilename(&input,&config->elevation_filename,config->inputdir,FALSE,"elevation");
        scanfilename(&input,&config->reservoir_filename,config->inputdir,"reservoir");
      }
#ifdef IMAGE
      if(config->aquifer_irrig==AQUIFER_IRRIG)
        scanclimatefilename(file,&config->aquifer_filename,config->inputdir,FALSE,"aquifer");
#endif
    }
  }
  scanclimatefilename(&input,&config->temp_filename,config->inputdir,config->sim_id==LPJML_FMS,"temp");
  scanclimatefilename(&input,&config->prec_filename,config->inputdir,config->sim_id==LPJML_FMS,"prec");
  switch(config->with_radiation)
  {
    case RADIATION:
      scanclimatefilename(&input,&config->lwnet_filename,config->inputdir,config->sim_id==LPJML_FMS,"lwnet");
      scanclimatefilename(&input,&config->swdown_filename,config->inputdir,config->sim_id==LPJML_FMS,"swdown");
      break;
    case RADIATION_LWDOWN:
      scanclimatefilename(&input,&config->lwnet_filename,config->inputdir,config->sim_id==LPJML_FMS,"lwdown");
      scanclimatefilename(&input,&config->swdown_filename,config->inputdir,config->sim_id==LPJML_FMS,"swdown");
      break;
    case CLOUDINESS:
      scanclimatefilename(&input,&config->cloud_filename,config->inputdir,config->sim_id==LPJML_FMS,"cloud");
      break;
    case RADIATION_SWONLY:
      scanclimatefilename(&input,&config->swdown_filename,config->inputdir,config->sim_id==LPJML_FMS,"swdown");
      break;
    default:
      if(verbose)
        fprintf(stderr,"ERROR213: Invalid setting %d for radiation.\n",config->with_radiation);
      return TRUE;
  }
  if(config->with_nitrogen)
  {
    if(config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
    {
      scanclimatefilename(&input,&config->no3deposition_filename,config->inputdir,config->sim_id==LPJML_FMS,"no3deposition");
      scanclimatefilename(&input,&config->nh4deposition_filename,config->inputdir,config->sim_id==LPJML_FMS,"nh4deposition");
    }
    else
      config->no3deposition_filename.name=config->nh4deposition_filename.name=NULL;
    scanclimatefilename(&input,&config->soilph_filename,config->inputdir,config->sim_id==LPJML_FMS,"soilpH");
  }
  else
    config->no3deposition_filename.name=config->nh4deposition_filename.name=config->soilph_filename.name=NULL;
  if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->fdi==WVPD_INDEX)
  {
    scanclimatefilename(&input,&config->humid_filename,config->inputdir,config->sim_id==LPJML_FMS,"humid");
  }
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    scanclimatefilename(&input,&config->wind_filename,config->inputdir,config->sim_id==LPJML_FMS,"wind");
  }
  if(config->fire==SPITFIRE_TMAX || config->cropsheatfrost)
  {
    scanclimatefilename(&input,&config->tmin_filename,config->inputdir,config->sim_id==LPJML_FMS,"tmin");
    scanclimatefilename(&input,&config->tmax_filename,config->inputdir,config->sim_id==LPJML_FMS,"tmax");
  }
  else
    config->tmax_filename.name=config->tmin_filename.name=NULL;
  if(config->fire==SPITFIRE)
  {
    scanclimatefilename(&input,&config->tamp_filename,config->inputdir,config->sim_id==LPJML_FMS,"tamp");
  }
  else
    config->tamp_filename.name=NULL;
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    scanclimatefilename(&input,&config->lightning_filename,config->inputdir,FALSE,"lightning");
    scanclimatefilename(&input,&config->human_ignition_filename,
                        config->inputdir,FALSE,"human_ignition");
  }
  if(config->ispopulation)
  {
    scanclimatefilename(&input,&config->popdens_filename,config->inputdir,FALSE,"popdens");
  }
  if(config->prescribe_burntarea)
  {
    scanclimatefilename(&input,&config->burntarea_filename,config->inputdir,FALSE,"burntarea");
  }
  if(config->prescribe_landcover!=NO_LANDCOVER)
  {
    scanclimatefilename(&input,&config->landcover_filename,config->inputdir,FALSE,"landcover");
  }
  if(readfilename(&input,&config->co2_filename,"co2",config->inputdir,FALSE,verbose))
  {
    if(verbose)
      fputs("ERROR209: Cannot read filename for 'co2' input.\n",stderr);
    return TRUE;
  }
  if(config->co2_filename.fmt!=TXT &&  (config->sim_id!=LPJML_FMS || config->co2_filename.fmt!=FMS))
  {
    if(verbose)
      fprintf(stderr,"ERROR197: Only txt format is supported for CO2 input in this version of LPJmL, %s not allowed.\n",
              fmt[config->co2_filename.fmt]);
    return TRUE;
  }

  if(israndom==RANDOM_PREC)
  {
    scanclimatefilename(&input,&config->wet_filename,config->inputdir,config->sim_id==LPJML_FMS,"wetdays");
  }
#if defined IMAGE && defined COUPLED
  else if(config->sim_id==LPJML_IMAGE)
  {
    if(verbose)
      fputs("ERROR175: Number of wet days must be supplied for IMAGE coupler.\n",stderr);
    return TRUE;
  }
#endif
  else
    config->wet_filename.name=NULL;
  if(config->wateruse)
  {
    scanclimatefilename(&input,&config->wateruse_filename,config->inputdir,FALSE,"wateruse");
#ifdef IMAGE
    scanclimatefilename(&input,&config->wateruse_wd_filename,config->inputdir,FALSE,"wateruse_wd");
#endif
  }
  else
  {
    config->wateruse_filename.name=NULL;
#ifdef IMAGE
    config->wateruse_wd_filename.name=NULL;
#endif
  }
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    /* reading IMAGE-coupling specific information */
    scanclimatefilename(&input,&config->temp_var_filename,config->inputdir,FALSE,"temp_var");
    scanclimatefilename(&input,&config->prec_var_filename,config->inputdir,FALSE,"prec_var");
    scanclimatefilename(&input,&config->prodpool_init_filename,config->inputdir,FALSE,"prodpool_init");
  }
#endif

  /*=================================================================*/
  /* IV. Reading output data section                                 */
  /*=================================================================*/

  if (verbose>=VERB) puts("// IV. output data section");
  if(fscanoutput(file,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config,nout_max))
  {
    if(verbose)
      fputs("ERROR230: Cannot read output data 'output'.\n",stderr);
    return TRUE;
  }

  /*=================================================================*/
  /* V. Reading run settings section                                 */
  /*=================================================================*/

  if (verbose>=VERB) puts("// V. run settings");
  if(iskeydefined(file,"restartpath"))
  {
    if(fscanstring(file,name,"restartpath",FALSE,verbose))
      return TRUE;
    free(config->restartdir);
    config->restartdir=strdup(name);
    checkptr(config->restartdir);
  }
  config->startgrid=ALL; /* set default value */
  if(isstring(file,"startgrid"))
  {
    fscanstring(file,name,"startgrid",FALSE,verbose);
    if(!strcmp(name,"all"))
      config->startgrid=ALL;
    else
    {
      if(verbose)
        fprintf(stderr,"ERROR233: Invalid string '%s' for startgrid, must be 'all' or number.\n",name);
      return TRUE;
    }
  }
  else if(fscanint(file,&config->startgrid,"startgrid",TRUE,verbose))
    return TRUE;
  if(config->startgrid==ALL)
  {
    config->startgrid=0;
    endgrid=getnsoilcode(&config->soil_filename,config->nsoil,isroot(*config));
    if(endgrid==0)
    {
      if(verbose)
        fputs("ERROR135: No soil code found.\n",stderr);
      return TRUE;
    }
    else if(endgrid>0)
      endgrid--;
  }
  else
  {
    endgrid=config->startgrid;
    if(fscanint(file,&endgrid,"endgrid",TRUE,verbose))
      return TRUE;
    if(endgrid<config->startgrid)
    {
      if(verbose)
        fprintf(stderr,"ERROR136: Endgrid=%d less than startgrid=%d.\n",
                endgrid,config->startgrid);
      return TRUE;
    }
  }
  if(endgrid==-1)
  {
   /* no soilcode file found */
   config->nall=-1;
   config->firstgrid=0;
  }
  else
  {
    config->nall=endgrid-config->startgrid+1;
    config->firstgrid=config->startgrid;
    if(config->nall<config->ntask)
    {
      if(verbose)
        fprintf(stderr,"ERROR198: Number of cells %d less than number of tasks %d.\n",
                config->nall,config->ntask);
      return TRUE;
    }
    if(config->ntask>1) /* parallel mode? */
      divide(&config->startgrid,&endgrid,config->rank,
             config->ntask);
    config->ngridcell=endgrid-config->startgrid+1;
  }
  fscanint2(file,&config->nspinup,"nspinup");
  config->isfirstspinupyear=FALSE;
  if(config->nspinup)
  {
    fscanint2(file,&config->nspinyear,"nspinyear");
    if(iskeydefined(file,"firstspinupyear"))
    {
      fscanint2(file,&config->firstspinupyear,"firstspinupyear");
      config->isfirstspinupyear=TRUE;
    }
  }
  fscanint2(file,&config->firstyear,"firstyear");
  fscanint2(file,&config->lastyear,"lastyear");
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    fscanint2(file,&config->start_imagecoupling,"start_imagecoupling");
  }
  else
    config->start_imagecoupling=INT_MAX;
#endif
  if(config->firstyear-config->nspinup>config->lastyear)
  {
    if(verbose)
      fprintf(stderr,"ERROR105: First simulation year=%d greater than last simulation year=%d.\n",
              config->firstyear-config->nspinup,config->lastyear);
    return TRUE;
  }
  if(config->n_out && iskeydefined(file,"outputyear"))
  {
    fscanint2(file,&config->outputyear,"outputyear");
    if(config->outputyear>config->lastyear)
    {
      if(verbose)
        fprintf(stderr,"ERROR230: First year output is written=%d greater than last simulation year=%d.\n",
                config->outputyear,config->lastyear);
      return TRUE;
    }
    else if(config->outputyear<config->firstyear-config->nspinup)
    {
      if(verbose)
        fprintf(stderr,"ERROR230: First year output is written=%d less than first simulation year=%d, set to %d.\n",
                config->outputyear,config->firstyear-config->nspinup,config->firstyear-config->nspinup);
      config->outputyear=config->firstyear-config->nspinup;
    }
  }
  else
    config->outputyear=config->firstyear;
  fscanbool2(file,&config->from_restart,"restart");
  config->new_seed=FALSE;
  config->equilsoil=FALSE;
  if(config->from_restart)
  {
    fscanname(file,name,"restart_filename");
    config->restart_filename=addpath(name,config->restartdir);
    checkptr(config->restart_filename);
    if(fscanbool(file,&config->new_seed,"new_seed",TRUE,verbose))
      return TRUE;
  }
  else
  {
    config->restart_filename=NULL;
  }
  fscanbool2(file,&config->equilsoil,"equilsoil");
  if(iskeydefined(file,"checkpoint_filename"))
  {
    fscanname(file,name,"checkpoint_filename");
    config->checkpoint_restart_filename=addpath(name,config->restartdir);
    checkptr(config->checkpoint_restart_filename);
  }
  else
    config->checkpoint_restart_filename=NULL;
  fscanbool2(file,&restart,"write_restart");
  if(restart)
  {
    fscanname(file,name,"write_restart_filename");
    config->write_restart_filename=addpath(name,config->restartdir);
    checkptr(config->write_restart_filename);
    fscanint2(file,&config->restartyear,"restart_year");
    if(config->restartyear>config->lastyear || config->restartyear<config->firstyear-config->nspinup)
    {
      if(verbose)
        fprintf(stderr,"WARNING014: Restart year=%d not in simulation years, no restart file written.\n",config->restartyear);
      free(config->write_restart_filename);
      config->write_restart_filename=NULL;
    }
  }
  else
    config->write_restart_filename=NULL;
  if(config->equilsoil && verbose && config->nspinup<soil_equil_year)
    fprintf(stderr,"WARNING031: Number of spinup years less than %d necessary for soil equilibration.\n",soil_equil_year);
  return FALSE;
} /* of 'fscanconfig' */
