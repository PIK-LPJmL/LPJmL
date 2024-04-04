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
    if((var=fscanstring(file,NULL,name,verbose))==NULL) {                 \
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

#define scanclimatefilename(file,var,isfms,iscoupled,what) {                 \
    if(readclimatefilename(file,var,what,def,FALSE,isfms,iscoupled,config)) {      \
      if(verbose) fprintf(stderr,"ERROR209: Cannot read filename for '%s' input.\n",what); \
      return TRUE;                                                      \
    }                                                                   \
    if(verbose>=VERB)                      \
      printf("%s %s\n", what, (var)->name);                             \
  }

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE; }

const char *crop_phu_options[]={"old","new","prescribed"};
const char *grazing_type[]={"default","mowing","ext","int","livestock","none"};

static Bool readfilename2(LPJfile *file,Filename *name,const char *key,const char *path,Verbosity verbose)
{
  if(readfilename(file,name,key,path,TRUE,FALSE,FALSE,verbose))
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

static Bool readclimatefilename(LPJfile *file,Filename *name,const char *key,Bool def[N_IN],Bool istxt,Bool isfms,Bool iscoupled,Config *config)
{
  Verbosity verbose;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(readfilename(file,name,key,config->inputdir,TRUE,TRUE,TRUE,verbose))
    return TRUE;
  if(!(isfms && config->sim_id==LPJML_FMS) && name->fmt==FMS)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: FMS coupler not allowed for input '%s'.\n",key);
    return TRUE;
  }
  if(!iscoupled && name->issocket)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: File format 'sock' not allowed for input '%s'.\n",key);
    return TRUE;
  }
  if(name->issocket)
  {
    if(iscoupled(*config))
    {
      config->coupler_in++;
      if(name->id<0 || name->id>=N_IN)
      {
        if(verbose)
          fprintf(stderr,"ERROR197: Invalid index %d for %s input, must be in [0,%d].\n",name->id,config->coupled_model,N_IN-1);
        return TRUE;
      }
      if(def[name->id])
      {
        if(verbose)
        fprintf(stderr,"ERROR197: Index %d already defined for %s input.\n",name->id,config->coupled_model);
          return TRUE;
      }
      def[name->id]=TRUE;
    }
    else
    {
      name->issocket=FALSE;
      if(name->fmt==SOCK)
      {
        if(verbose)
          fprintf(stderr,"ERROR197: Socket %s input not allowed without coupled model.\n",key);
        return TRUE;
      }
    }
  }
  if(istxt && name->fmt!=TXT && name->fmt!=FMS && name->fmt!=SOCK)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: Only txt format is supported for input '%s' in this version of LPJmL, %s not allowed.\n",
              name->name,fmt[name->fmt]);
    return TRUE;
  }
  if(!istxt && name->fmt==TXT)
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

static Bool checktimestep(const Config *config)
{
  Bool rc;
  int i;
  rc=FALSE;
  for(i=0;i<config->n_out;i++)
  {
    if(config->outputvars[i].filename.timestep>1 && (config->lastyear-config->outputyear)/config->outputvars[i].filename.timestep==0)
    {
      rc=TRUE;
      if(isroot(*config))
      {
        fprintf(stderr,"WARNING040: Time step of %d yrs for output '%s' longer than output interval [%d,%d], no output written.\n",
                config->outputvars[i].filename.timestep,
                config->outnames[config->outputvars[i].id].name,
                config->outputyear,config->lastyear);
      }
    }
  }
  return rc;
} /* of 'checktimestep' */

Bool fscanconfig(Config *config,    /**< LPJ configuration */
                 LPJfile *file,     /**< File pointer to LPJ file */
                 Pfttype scanfcn[], /**< array of PFT-specific scan
                                         functions */
                 int ntypes,        /**< Number of PFT classes */
                 int nout_max       /**< maximum number of output files */
                )                   /** \return TRUE on error */
 {
  const char *name;
  LPJfile *input;
  Bool israndom;
  int i,restart,endgrid,grassharvest;
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
  const char *laimax_manage[]={"laimax_cft","laimax_const","laimax_par"};
  const char *fdi[]={"nesterov","wvpd"};
  const char *nitrogen[]={"no","lim","unlim"};
  const char *tillage[]={"no","all","read"};
  const char *residue_treatment[]={"no_residue_remove","fixed_residue_remove","read_residue_data"};
  Bool def[N_IN];
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;

  /*=================================================================*/
  /* I. Reading type section                                         */
  /*=================================================================*/
  config->coupler_in=0;
  config->socket=NULL;
  if (verbose>=VERB) puts("// I. type section");
  config->coupled_model=NULL;
  if(iskeydefined(file,"coupled_model"))
  {
    if(!isnull(file,"coupled_model"))
    {
      fscanname(file,name,"coupled_model");
      if(config->sim_id==LPJML_IMAGE)
      {
        if(verbose)
          fprintf(stderr,"ERROR123: Coupling to '%s' model not allowed with image coupling enabled.\n",
                  name);
        return TRUE;
      }
      config->coupled_model=strdup(name);
      checkptr(name);
    }
  }
  else
  {
    if(verbose)
      fprintf(stderr,"WARNING027: Name 'coupled_model' for string not found, set to null.\n");
    if(config->pedantic)
      return TRUE;
  }
  config->landfrac_from_file=FALSE;
  if(fscanbool(file,&config->landfrac_from_file,"landfrac_from_file",!config->pedantic,verbose))
    return TRUE;
  fscanbool2(file,&israndom,"random_prec");
  config->seed_start=RANDOM_SEED;
  if(isstring(file,"random_seed"))
  {
    name=fscanstring(file,NULL,"random_seed",verbose);
    if(!strcmp(name,"random_seed"))
      config->seed_start=RANDOM_SEED;
    else
    {
      if(verbose)
        fprintf(stderr,"ERROR233: Invalid string '%s' for random_seed, must be 'random_seed' or number.\n",name);
      return TRUE;
    }
  }
  else if(fscanint(file,&config->seed_start,"random_seed",!config->pedantic,verbose))
    return TRUE;
  if(config->seed_start==RANDOM_SEED)
    config->seed_start=time(NULL);
  setseed(config->seed,config->seed_start);
  config->with_nitrogen=NO_NITROGEN;
#ifdef COUPLING_WITH_FMS
  config->nitrogen_coupled=FALSE;
#endif
  if(fscankeywords(file,&config->with_nitrogen,"with_nitrogen",nitrogen,3,!config->pedantic,verbose))
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
    if(fscanbool(file,&config->prescribe_burntarea,"prescribe_burntarea",!config->pedantic,verbose))
      return TRUE;
  }
  config->prescribe_landcover=NO_LANDCOVER;
  if(fscankeywords(file,&config->prescribe_landcover,"prescribe_landcover",prescribe_landcover,3,!config->pedantic,verbose))
    return TRUE;
  fscanbool2(file,&config->gsi_phenology,"gsi_phenology");
  config->transp_suction_fcn=FALSE;
  if(fscanbool(file,&config->transp_suction_fcn,"transp_suction_fcn",!config->pedantic,verbose))
    return TRUE;
  fscanbool2(file,&config->river_routing,"river_routing");
  config->with_lakes=config->river_routing;
  if(fscanbool(file,&config->with_lakes,"with_lakes",!config->pedantic,verbose))
    return TRUE;
  config->extflow=FALSE;
  if(config->river_routing)
  {
    if(fscanbool(file,&config->extflow,"extflow",!config->pedantic,verbose))
      return TRUE;
  }
  config->reservoir=FALSE;
#ifdef IMAGE
  config->groundwater_irrig = FALSE;
  config->aquifer_irrig = FALSE;
#endif
  fscanbool2(file,&config->permafrost,"permafrost");
  config->johansen = TRUE;
  if(fscanbool(file,&config->johansen,"johansen",!config->pedantic,verbose))
    return TRUE;
  config->sdate_option=NO_FIXED_SDATE;
  config->crop_phu_option=NEW_CROP_PHU;
  config->rw_manage=FALSE;
  config->tillage_type=NO_TILLAGE;
  config->residue_treatment=NO_RESIDUE_REMOVE;
  config->no_ndeposition=FALSE;
  config->separate_harvests=FALSE;
  config->others_to_crop = FALSE;
  config->npp_controlled_bnf = FALSE;
  config->prescribe_lsuha=FALSE;
  if(config->with_nitrogen)
  {
    if(fscanbool(file,&config->npp_controlled_bnf,"npp_controlled_bnf",!config->pedantic,verbose))
      return TRUE;
#ifdef COUPLING_WITH_FMS
    config->nitrogen_coupled=TRUE;
    if(fscanbool(file,&config->nitrogen_coupled,"nitrogen_coupled",!config->pedantic,verbose))
      return TRUE;
#endif
  }
  config->soilpar_option=NO_FIXED_SOILPAR;
  if(fscankeywords(file,&config->soilpar_option,"soilpar_option",soilpar_option,3,!config->pedantic,verbose))
    return TRUE;
  if(config->soilpar_option==FIXED_SOILPAR)
  {
    fscanint2(file,&config->soilpar_fixyear,"soilpar_fixyear");
  }
  config->storeclimate=TRUE;
  if(fscanbool(file,&config->storeclimate,"store_climate",!config->pedantic,verbose))
    return TRUE;
  config->fix_climate=FALSE;
  if(fscanbool(file,&config->fix_climate,"fix_climate",!config->pedantic,verbose))
    return TRUE;
  if(config->fix_climate)
  {
    fscanint2(file,&config->fix_climate_year,"fix_climate_year");
    if(fscanintarray(file,config->fix_climate_interval,2,"fix_climate_interval",verbose))
      return TRUE;
    if(config->fix_climate_interval[1]<config->fix_climate_interval[0])
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR255: Upper limit for fix climate interval=%d must be higher than lower limit=%d.\n",
                config->fix_climate_interval[1],config->fix_climate_interval[0]);
      return TRUE;
    }
    fscanbool2(file,&config->fix_climate_shuffle,"fix_climate_shuffle");
  }
  config->fix_deposition=FALSE;
  config->fix_deposition_with_climate=FALSE;
  if(config->with_nitrogen==LIM_NITROGEN)
  {
    if(fscanbool(file,&config->no_ndeposition,"no_ndeposition",!config->pedantic,verbose))
      return TRUE;
    if(!config->no_ndeposition)
    {
      if(config->fix_climate)
      {
        fscanbool2(file,&config->fix_deposition_with_climate,"fix_deposition_with_climate");
      }
      if(config->fix_deposition_with_climate)
      {
        config->fix_deposition=TRUE;
        config->fix_deposition_year=config->fix_climate_year;
        config->fix_deposition_interval[0]=config->fix_climate_interval[0];
        config->fix_deposition_interval[1]=config->fix_climate_interval[1];
        config->fix_deposition_shuffle=config->fix_climate_shuffle;
      }
      else
      {
        if(fscanbool(file,&config->fix_deposition,"fix_deposition",!config->pedantic,verbose))
          return TRUE;
        if(config->fix_deposition)
        {
          fscanint2(file,&config->fix_deposition_year,"fix_deposition_year");
          if(fscanintarray(file,config->fix_deposition_interval,2,"fix_deposition_interval",verbose))
            return TRUE;
          if(config->fix_deposition_interval[1]<config->fix_deposition_interval[0])
          {
            if(isroot(*config))
              fprintf(stderr,"ERROR255: Upper limit for fix N deposition interval=%d must be higher than lower limit=%d.\n",
                      config->fix_deposition_interval[1],config->fix_deposition_interval[0]);
            return TRUE;
          }
          fscanbool2(file,&config->fix_deposition_shuffle,"fix_deposition_shuffle");
        }
      }
    }
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
        if(fscanbool(file,&config->fire_on_grassland,"fire_on_grassland",!config->pedantic,verbose))
          return TRUE;
      }
      if(fscanbool(file,&config->separate_harvests,"separate_harvests",!config->pedantic,verbose))
        return TRUE;
      if(config->withlanduse==CONST_LANDUSE || config->withlanduse==ALL_CROPS || config->withlanduse==ONLY_CROPS)
        fscanint2(file,&config->landuse_year_const,"landuse_year_const");
      config->fix_landuse=FALSE;
      if(config->withlanduse!=CONST_LANDUSE)
      {
        if(fscanbool(file,&config->fix_landuse,"fix_landuse",!config->pedantic,verbose))
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
      if(fscankeywords(file,&config->crop_phu_option,"crop_phu_option",crop_phu_options,3,!config->pedantic,verbose))
        return TRUE;
      fscanbool2(file,&config->intercrop,"intercrop");
      config->manure_input=FALSE;
      config->fix_fertilization=FALSE;
      if(config->with_nitrogen)
      {
        config->crop_resp_fix=FALSE;
        if(fscanbool(file,&config->crop_resp_fix,"crop_resp_fix",!config->pedantic,verbose))
          return TRUE;
      }
      else
        config->crop_resp_fix=TRUE;
      if(config->with_nitrogen==LIM_NITROGEN)
      {
        if(fscanbool(file,&config->fix_fertilization,"fix_fertilization",!config->pedantic,verbose))
          return TRUE;
        if(!config->fix_fertilization)
        {
          config->fertilizer_input=FERTILIZER;
          if(fscankeywords(file,&config->fertilizer_input,"fertilizer_input",fertilizer,3,!config->pedantic,verbose))
            return TRUE;
          if(config->fertilizer_input!=AUTO_FERTILIZER)
          {
            if (fscanbool(file,&config->manure_input,"manure_input",!config->pedantic,verbose))
              return TRUE;
          }
        }
      }
      config->grassonly = FALSE;
      if (fscanbool(file, &config->grassonly, "grassonly", !config->pedantic, verbose))
        return TRUE;
      config->luc_timber=FALSE;
      if(fscanbool(file,&config->luc_timber,"luc_timber",!config->pedantic,verbose))
        return TRUE;
      config->residues_fire=FALSE;
      if(fscanbool(file,&config->residues_fire,"residues_fire",!config->pedantic,verbose))
        return TRUE;
      if(fscanbool(file,&config->rw_manage,"rw_manage",!config->pedantic,verbose))
        return TRUE;
      if(fscankeywords(file,&config->laimax_manage,"laimax_manage",laimax_manage,3,FALSE,verbose))
        return TRUE;
      if(config->laimax_manage==LAIMAX_CONST)
        fscanreal2(file,&config->laimax,"laimax");
      if (fscanbool(file, &config->others_to_crop, "others_to_crop", !config->pedantic, verbose))
        return TRUE;
      if(config->river_routing)
      {
        fscanbool2(file,&config->reservoir,"reservoir");
#ifdef IMAGE
        fscanbool(file,&config->groundwater_irrig,"groundwater_irrigation", !config->pedantic,verbose);
        fscanbool(file,&config->aquifer_irrig,"aquifer_irrigation",!config->pedantic,verbose);
#endif
      }
      grassharvest=FALSE;
      if(fscanbool(file,&grassharvest,"grass_harvest_options", !config->pedantic, verbose))
        return TRUE;
      if(!grassharvest)
      {
        config->grazing=GS_DEFAULT;
        if(fscankeywords(file,&config->grazing,"grazing",grazing_type,6,!config->pedantic,verbose))
          return TRUE;
      }
      config->grazing_others=GS_DEFAULT;
      if(!config->others_to_crop)
      {
        if(fscankeywords(file,&config->grazing_others,"grazing_others",grazing_type,6,!config->pedantic,verbose))
          return TRUE;
      }
      if(fscanmowingdays(file,config))
        return TRUE;
      if(fscanbool(file,&config->prescribe_lsuha,"prescribe_lsuha", !config->pedantic, verbose))
        return TRUE;
      if(fscankeywords(file,&config->tillage_type,"tillage_type",tillage,3,!config->pedantic,verbose))
        return TRUE;
      if(fscankeywords(file,&config->residue_treatment,"residue_treatment",residue_treatment,3,TRUE,verbose))
        return TRUE;
    }
    if(isboolean(file,"wateruse"))
    {
      if(isroot(*config))
        fputs("WARNING028: Type of 'wateruse' is boolean, converted to int.\n",stderr);
      if(config->pedantic)
        return TRUE;
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
  if(fscansoilpar(file,config))
  {
    if(verbose)
      fputs("ERROR230: Cannot read soil parameter 'soilpar'.\n",stderr);
    return TRUE;
  }
  if(iskeydefined(file,"soilmap"))
  {
    config->soilmap=fscansoilmap(file,&config->soilmap_size,config);
    if(config->soilmap==NULL)
      return TRUE;
  }
  else
  {
    config->soilmap=NULL;
    config->soilmap_size=0;
  }
  config->ntypes=ntypes;
  if(fscancultivationtypes(file,"cultivation_types",&config->cult_types,&config->ncult_types,verbose))
    return TRUE;
  if(fscanpftpar(file,scanfcn,config))
  {
    if(verbose)
      fputs("ERROR230: Cannot read PFT parameter 'pftpar'.\n",stderr);
    return TRUE;
  }
  if(config->npft[CROP]==0 && config->withlanduse)
  {
    if(verbose)
      fputs("ERROR230: No crop PFTs defined in 'pftpar' and land use enabled.\n",stderr);
    return TRUE;
  }
  if(fscanparamcft(file,config))
  {
    if(verbose)
      fputs("ERROR230: Cannot read LPJ parameter 'param'.\n",stderr);
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
    name=fscanstring(file,NULL,"cft_temp",verbose);
    if(name==NULL)
      return TRUE;
    setotherstocrop();
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
    name=fscanstring(file,NULL,"cft_tropic",verbose);
    if(name==NULL)
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
  /* Read soil paramater array */
  if(config->withlanduse!=NO_LANDUSE)
  {
    /* landuse enabled */
    if((config->ncountries=fscancountrypar(file,&config->countrypar,config->npft[GRASS]+config->npft[TREE],(config->laimax_manage==LAIMAX_CFT) ? config->npft[CROP] : 0,verbose,config))==0)
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
      if (fscantreedens(file,config->countrypar,config->npft[GRASS]+config->npft[TREE],verbose,config)==0)
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
  config->global_attrs=NULL;
  config->n_global=0;
  if(iskeydefined(file,"global_attrs"))
  {
    if(fscanattrs(file,&config->global_attrs,&config->n_global,"global_attrs",verbose))
      return TRUE;
  }
  config->compress=0;
  if(fscanint(file,&config->compress,"compress",!config->pedantic,verbose))
    return TRUE;
#ifdef USE_NETCDF
  if(config->compress)
  {
    if(verbose)
      fputs("WARNING403: Compression of NetCDF files is not supported in this version of NetCDF.\n",stderr);
    if(config->pedantic)
      return TRUE;
  }
#endif
  config->missing_value=MISSING_VALUE_FLOAT;
  if(fscanfloat(file,&config->missing_value,"missing_value",!config->pedantic,verbose))
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
    name=fscanstring(file,NULL,"inpath",verbose);
    if(name==NULL)
      return TRUE;
    free(config->inputdir);
    config->inputdir=strdup(name);
    checkptr(config->inputdir);
  }
  input=fscanstruct(file,"input",verbose);
  if(input==NULL)
    return TRUE;
  for(i=0;i<N_IN;i++)
    def[i]=FALSE;
  scanclimatefilename(input,&config->soil_filename,FALSE,FALSE,"soil");
  if(config->soil_filename.fmt!=CDF)
  {
    scanfilename(input,&config->coord_filename,config->inputdir,"coord");
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
    scanclimatefilename(input,&config->countrycode_filename,FALSE,FALSE,"countrycode");
    if(config->countrycode_filename.fmt==CDF)
    {
      scanclimatefilename(input,&config->regioncode_filename,FALSE,FALSE,"regioncode");
    }
    scanclimatefilename(input,&config->landuse_filename,FALSE,TRUE,"landuse");
    if(config->iscotton)
    {
      scanclimatefilename(input,&config->sowing_cotton_rf_filename,FALSE,FALSE,"sowing_ag_tree_rf");
      scanclimatefilename(input,&config->harvest_cotton_rf_filename,FALSE,FALSE,"harvest_ag_tree_rf");
      scanclimatefilename(input,&config->sowing_cotton_ir_filename,FALSE,FALSE,"sowing_ag_tree_ir");
      scanclimatefilename(input,&config->harvest_cotton_ir_filename,FALSE,FALSE,"harvest_ag_tree_ir");
    }
    if(config->sdate_option==PRESCRIBED_SDATE)
    {
      scanclimatefilename(input,&config->sdate_filename,FALSE,TRUE,"sdate");
    }
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
    {
      scanclimatefilename(input,&config->crop_phu_filename,FALSE,TRUE,"crop_phu");
    }
    if(config->with_nitrogen && config->fertilizer_input==FERTILIZER)
      scanclimatefilename(input,&config->fertilizer_nr_filename,FALSE,TRUE,"fertilizer_nr");
    if (config->with_nitrogen && config->manure_input)
      scanclimatefilename(input,&config->manure_nr_filename,FALSE,TRUE,"manure_nr");
    if (config->tillage_type==READ_TILLAGE)
      scanclimatefilename(input,&config->with_tillage_filename,FALSE,TRUE,"with_tillage");
    if (config->residue_treatment == READ_RESIDUE_DATA)
      scanclimatefilename(input,&config->residue_data_filename,FALSE,TRUE,"residue_on_field");
    if(config->prescribe_lsuha)
      scanclimatefilename(input,&config->lsuha_filename,FALSE,TRUE,"grassland_lsuha");
    if(grassharvest)
    {
      scanclimatefilename(input,&config->grassharvest_filename,FALSE,FALSE,"grass_harvest_options");
    }
    else
      config->grassharvest_filename.name = NULL;
  }
  else
  {
    config->grassharvest_filename.name = NULL;
    config->lsuha_filename.name = NULL;
  }
  if(config->landfrac_from_file)
  {
    scanclimatefilename(input,&config->landfrac_filename,FALSE,FALSE,"landfrac");
  }
  if(config->with_lakes)
  {
    scanclimatefilename(input,&config->lakes_filename,FALSE,FALSE,"lakes");
  }
  if(config->river_routing)
  {
    scanclimatefilename(input,&config->drainage_filename,FALSE,FALSE,"drainage");
    if(config->drainage_filename.fmt==CDF)
    {
      scanclimatefilename(input,&config->river_filename,FALSE,FALSE,"river");
    }
    if(config->extflow)
    {
      scanclimatefilename(input,&config->extflow_filename,FALSE,FALSE,"extflow");
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
      scanclimatefilename(input,&config->neighb_irrig_filename,FALSE,FALSE,"neighb_irrig");
      if(config->reservoir)
      {
        scanclimatefilename(input,&config->elevation_filename,FALSE,FALSE,"elevation");
        scanfilename(input,&config->reservoir_filename,config->inputdir,"reservoir");
      }
#ifdef IMAGE
      if(config->aquifer_irrig)
        scanclimatefilename(input,&config->aquifer_filename,FALSE,FALSE,"aquifer");
#endif
    }
  }
  scanclimatefilename(input,&config->temp_filename,TRUE,TRUE,"temp");
  scanclimatefilename(input,&config->prec_filename,TRUE,TRUE,"prec");
  switch(config->with_radiation)
  {
    case RADIATION:
      scanclimatefilename(input,&config->lwnet_filename,TRUE,TRUE,"lwnet");
      scanclimatefilename(input,&config->swdown_filename,TRUE,TRUE,"swdown");
      break;
    case RADIATION_LWDOWN:
      scanclimatefilename(input,&config->lwnet_filename,TRUE,TRUE,"lwdown");
      scanclimatefilename(input,&config->swdown_filename,TRUE,TRUE,"swdown");
      break;
    case CLOUDINESS:
      scanclimatefilename(input,&config->cloud_filename,TRUE,TRUE,"cloud");
      break;
    case RADIATION_SWONLY:
      scanclimatefilename(input,&config->swdown_filename,TRUE,TRUE,"swdown");
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
      scanclimatefilename(input,&config->no3deposition_filename,TRUE,TRUE,"no3deposition");
      scanclimatefilename(input,&config->nh4deposition_filename,TRUE,TRUE,"nh4deposition");
    }
    else
      config->no3deposition_filename.name=config->nh4deposition_filename.name=NULL;
    scanclimatefilename(input,&config->soilph_filename,FALSE,FALSE,"soilpH");
  }
  else
    config->no3deposition_filename.name=config->nh4deposition_filename.name=config->soilph_filename.name=NULL;
  if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->fdi==WVPD_INDEX)
  {
    scanclimatefilename(input,&config->humid_filename,TRUE,TRUE,"humid");
  }
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    scanclimatefilename(input,&config->wind_filename,TRUE,TRUE,"wind");
  }
  if(config->fire==SPITFIRE_TMAX)
  {
    scanclimatefilename(input,&config->tmin_filename,TRUE,TRUE,"tmin");
    scanclimatefilename(input,&config->tmax_filename,TRUE,TRUE,"tmax");
  }
  else
    config->tmax_filename.name=config->tmin_filename.name=NULL;
  if(config->fire==SPITFIRE)
  {
    scanclimatefilename(input,&config->tamp_filename,TRUE,TRUE,"tamp");
  }
  else
    config->tamp_filename.name=NULL;
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    scanclimatefilename(input,&config->lightning_filename,FALSE,TRUE,"lightning");
    scanclimatefilename(input,&config->human_ignition_filename,
                        FALSE,TRUE,"human_ignition");
  }
  if(config->ispopulation)
  {
    scanclimatefilename(input,&config->popdens_filename,FALSE,TRUE,"popdens");
  }
  if(config->prescribe_burntarea)
  {
    scanclimatefilename(input,&config->burntarea_filename,FALSE,FALSE,"burntarea");
  }
  if(config->prescribe_landcover!=NO_LANDCOVER)
  {
    config->landcovermap=fscanlandcovermap(file,&config->landcovermap_size,"landcovermap",config->npft[GRASS]+config->npft[TREE],config);
    if(config->landcovermap==NULL)
      return TRUE;
    scanclimatefilename(input,&config->landcover_filename,FALSE,FALSE,"landcover");
  }
  else
    config->landcovermap=NULL;
  config->fix_co2=FALSE;
  if(fscanbool(file,&config->fix_co2,"fix_co2",!config->pedantic,verbose))
    return TRUE;
  if(config->fix_co2)
  {
    fscanint2(file,&config->fix_co2_year,"fix_co2_year");
  }
  if(readclimatefilename(input,&config->co2_filename,"co2",def,TRUE,TRUE,TRUE,config))
  {
    if(verbose)
      fprintf(stderr,"ERROR209: Cannot read filename for CO2 input.\n");
    return TRUE;
  }
  if(israndom)
  {
    scanclimatefilename(input,&config->wet_filename,TRUE,TRUE,"wetdays");
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
    scanclimatefilename(input,&config->wateruse_filename,FALSE,TRUE,"wateruse");
#ifdef IMAGE
    scanclimatefilename(input,&config->wateruse_wd_filename,FALSE,FALSE,"wateruse_wd");
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
    scanclimatefilename(input,&config->temp_var_filename,FALSE,FALSE,"temp_var");
    scanclimatefilename(input,&config->prec_var_filename,FALSE,FALSE,"prec_var");
    scanclimatefilename(input,&config->prodpool_init_filename,FALSE,FALSE,"prodpool_init");
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
    name=fscanstring(file,NULL,"restartpath",verbose);
    if(name==NULL)
      return TRUE;
    free(config->restartdir);
    config->restartdir=strdup(name);
    checkptr(config->restartdir);
  }
  config->startgrid=ALL; /* set default value */
  if(isstring(file,"startgrid"))
  {
    name=fscanstring(file,NULL,"startgrid",verbose);
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
  config->shuffle_spinup_climate=FALSE;
  if(config->nspinup)
  {
    if(config->nspinup<0)
    {
      if(verbose)
        fprintf(stderr,"ERROR262: Number of spinup years=%d must be greater than or equal to zero.\n",
               config->nspinup);
      return TRUE;
    }
    if(fscanbool(file,&config->shuffle_spinup_climate,"shuffle_spinup_climate",TRUE,verbose))
      return TRUE;
    fscanint2(file,&config->nspinyear,"nspinyear");
    if(config->nspinyear<1)
    {
      if(verbose)
        fprintf(stderr,"ERROR263: Cycle length of spinup (\"nspinyear\")=%d must be greater than zero.\n",
               config->nspinyear);
      return TRUE;
    }
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
    fscanint2(file,&config->start_coupling,"start_coupling");
  }
  else
    config->start_coupling=INT_MAX;
#else
  if(iscoupled(*config))
  {
    config->start_coupling=config->firstyear-config->nspinup;
    if(!isnull(file,"start_coupling"))
    {
      if(fscanint(file,&config->start_coupling,"start_coupling",TRUE,verbose))
        return TRUE;
    }
  }
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
      if(config->pedantic)
        return TRUE;
      config->outputyear=config->firstyear-config->nspinup;
    }
  }
  else
    config->outputyear=config->firstyear;
  if(checktimestep(config) && config->pedantic)
    return TRUE;
  config->baseyear=config->outputyear;
  if(config->n_out && iskeydefined(file,"baseyear"))
  {
    fscanint2(file,&config->baseyear,"baseyear");
  }
  fscanbool2(file,&config->from_restart,"restart");
  config->new_seed=FALSE;
  config->equilsoil=FALSE;
  if(config->from_restart)
  {
    fscanname(file,name,"restart_filename");
    config->restart_filename=addpath(name,config->restartdir);
    checkptr(config->restart_filename);
    if(fscanbool(file,&config->new_seed,"new_seed",!config->pedantic,verbose))
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
      if(config->pedantic)
        return TRUE;
      free(config->write_restart_filename);
      config->write_restart_filename=NULL;
    }
  }
  else
    config->write_restart_filename=NULL;
  if(config->equilsoil && config->nspinup<(param.veg_equil_year+param.nequilsoil*param.equisoil_interval+param.equisoil_fadeout))
  {
    fprintf(stderr,"ERROR230: Number of spinup years=%d insuffficient for selected spinup settings, must be at least %d.\n",
            config->nspinup,param.veg_equil_year+param.nequilsoil*param.equisoil_interval+param.equisoil_fadeout); 
    return TRUE;
  }
  return FALSE;
} /* of 'fscanconfig' */
