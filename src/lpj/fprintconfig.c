/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  c  o  n  f  i  g  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints LPJ configuration                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define notnull(s) (s==NULL) ? "" : s

typedef struct
{
  char *name;
  int index;
} Item;

static int compare(const Item *a,const Item *b)
{
  return strcmp(a->name,b->name);
} /* of 'compare' */

static int printsim(FILE *file,int len,int *count,const char *s)
{
  len=fputstring(file,len,(*count) ? ", " : "Simulation with ",78);
  (*count)++;
  len=fputstring(file,len,s,78);
  return len;
} /* of 'printsim' */

static size_t isnetcdfinput(const Config *config)
{
  size_t width;
  width=0;
  if(config->soil_filename.fmt==CDF)
    width=max(width,strlen(config->soil_filename.var));
  if(config->temp_filename.fmt==CDF)
    width=max(width,strlen(config->temp_filename.var));
  if(config->prec_filename.fmt==CDF)
    width=max(width,strlen(config->prec_filename.var));
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    if(config->temp_var_filename.fmt==CDF)
      width=max(width,strlen(config->temp_var_filename.var));
    if(config->prec_var_filename.fmt==CDF)
      width=max(width,strlen(config->prec_var_filename.var));
    if(config->prodpool_init_filename.fmt==CDF)
      width=max(width,strlen(config->prodpool_init_filename.var));
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
      if(config->lwnet_filename.fmt==CDF)
        width=max(width,strlen(config->lwnet_filename.var));
    if(config->swdown_filename.fmt==CDF)
      width=max(width,strlen(config->swdown_filename.var));
  }
  else if(config->cloud_filename.fmt==CDF)
    width=max(width,strlen(config->cloud_filename.var));
  if(config->with_nitrogen)
  {
    if(config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
    {
      if(config->no3deposition_filename.fmt==CDF)
        width=max(width,strlen(config->no3deposition_filename.var));
      if(config->nh4deposition_filename.fmt==CDF)
        width=max(width,strlen(config->nh4deposition_filename.var));
    }
    if(config->soilph_filename.fmt==CDF)
      width=max(width,strlen(config->soilph_filename.var));
  }
  if(config->landfrac_from_file)
  {
    if(config->landfrac_filename.fmt==CDF)
      width=max(width,strlen(config->landfrac_filename.var));
  }
  if(config->cropsheatfrost || config->fire==SPITFIRE_TMAX)
  {
    if(config->tmin_filename.fmt==CDF)
      width=max(width,strlen(config->tmin_filename.var));
    if(config->tmax_filename.fmt==CDF)
      width=max(width,strlen(config->tmax_filename.var));
  }
  if(config->fire==SPITFIRE && config->tamp_filename.fmt==CDF)
    width=max(width,strlen(config->tamp_filename.var));
  if((config->with_nitrogen || config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX) &&
     config->wind_filename.fmt==CDF)
    width=max(width,strlen(config->wind_filename.var));
  if(config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX && config->humid_filename.fmt==CDF)
      width=max(width,strlen(config->humid_filename.var));
    if(config->lightning_filename.fmt==CDF)
      width=max(width,strlen(config->lightning_filename.var));
    if(config->human_ignition_filename.fmt==CDF)
      width=max(width,strlen(config->human_ignition_filename.var));
  }
  if(config->ispopulation && config->popdens_filename.fmt==CDF)
    width=max(width,strlen(config->popdens_filename.var));
  if(config->grassharvest_filename.name!=NULL && config->grassharvest_filename.fmt==CDF)
    width=max(width,strlen(config->grassharvest_filename.var));
  if(config->withlanduse!=NO_LANDUSE)
  {
    if(config->countrycode_filename.fmt==CDF)
      width=max(width,strlen(config->countrycode_filename.var));
    if(config->landuse_filename.fmt==CDF)
      width=max(width,strlen(config->landuse_filename.var));
    if(config->iscotton)
    {
      if(config->sowing_cotton_rf_filename.fmt==CDF)
        width=max(width,strlen(config->sowing_cotton_rf_filename.var));
      if(config->harvest_cotton_rf_filename.fmt==CDF)
        width=max(width,strlen(config->harvest_cotton_rf_filename.var));
      if(config->sowing_cotton_ir_filename.fmt==CDF)
        width=max(width,strlen(config->sowing_cotton_ir_filename.var));
      if(config->harvest_cotton_ir_filename.fmt==CDF)
        width=max(width,strlen(config->harvest_cotton_ir_filename.var));
    }
    if(config->sdate_option==PRESCRIBED_SDATE && config->sdate_filename.fmt==CDF)
      width=max(width,strlen(config->sdate_filename.var));
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU && config->crop_phu_filename.fmt==CDF)
      width=max(width,strlen(config->crop_phu_filename.var));
    if (config->with_nitrogen && config->fertilizer_input==FERTILIZER && config->fertilizer_nr_filename.fmt == CDF)
      width = max(width, strlen(config->fertilizer_nr_filename.var));
    if (config->with_nitrogen && config->manure_input && config->manure_nr_filename.fmt == CDF)
      width = max(width, strlen(config->manure_nr_filename.var));
    if(config->residue_treatment==READ_RESIDUE_DATA && config->residue_data_filename.fmt==CDF)
      width=max(width, strlen(config->residue_data_filename.var));
    if(config->tillage_type==READ_TILLAGE && config->with_tillage_filename.fmt==CDF)
      width=max(width, strlen(config->with_tillage_filename.var));
    if(config->prescribe_lsuha && config->lsuha_filename.fmt==CDF)
      width=max(width,strlen(config->lsuha_filename.var));
  }
  if(config->reservoir)
  {
    if(config->elevation_filename.fmt==CDF)
      width=max(width,strlen(config->elevation_filename.var));
    if(config->reservoir_filename.fmt==CDF)
      width=max(width,strlen(config->reservoir_filename.var));
  }
  if(config->wet_filename.name!=NULL && config->wet_filename.fmt==CDF)
    width=max(width,strlen(config->wet_filename.var));
  if(config->with_lakes)
  {
    if(config->lakes_filename.fmt==CDF)
      width=max(width,strlen(config->lakes_filename.var));
  }
  if(config->river_routing)
  {
    if(config->drainage_filename.fmt==CDF)
      width=max(width,strlen(config->drainage_filename.var));
    if(config->withlanduse!=NO_LANDUSE && config->neighb_irrig_filename.fmt==CDF)
      width=max(width,strlen(config->neighb_irrig_filename.var));
  }
  if(config->prescribe_burntarea && config->burntarea_filename.fmt==CDF)
    width=max(width,strlen(config->burntarea_filename.var));
  if(config->prescribe_landcover && config->landcover_filename.fmt==CDF)
    width=max(width,strlen(config->landcover_filename.var));
  if(config->wateruse && config->wateruse_filename.fmt==CDF)
    width=max(width,strlen(config->wateruse_filename.var));
#ifdef IMAGE
  if(config->wateruse_wd_filename.name!=NULL && config->wateruse_wd_filename.fmt==CDF)
    width=max(width,strlen(config->wateruse_wd_filename.var));
  if(config->aquifer_irrig && config->aquifer_filename.fmt==CDF)
    width=max(width,strlen(config->aquifer_filename.var));
#endif
  if(width)
    width=max(width,strlen("Varname"));
  return width;
} /* of 'isnetcdfinput' */

static void printoutname(FILE *file,const Filename *filename,Bool isoneyear,
                         const Config *config)
{
  char *fmt;
  char *pos;
  if(filename->fmt!=SOCK)
  {
    if(isoneyear)
    {
      fmt=malloc(strlen(filename->name)+6);
      if(fmt!=NULL)
      {
        pos=strstr(filename->name,"%d");
        strncpy(fmt,filename->name,pos-filename->name);
        fmt[pos-filename->name]='\0';
        strcat(fmt,"[%d-%d]");
        strcat(fmt,pos+2);
        fprintf(file,fmt,config->firstyear,config->lastyear);
        free(fmt);
      }
    }
    else
      fputs(filename->name,file);
    if(filename->meta)
      fprintf(file," + %s",config->json_suffix);
  }
  if(iscoupled(*config) && filename->issocket)
  {
    if(filename->fmt!=SOCK)
      fputs(", ",file);
    fprintf(file,"%d -> %s:%d",filename->id,config->coupled_host,config->coupler_port);
  }
} /* of printoutname' */

static void printinputfile(FILE *file,const char *descr,const Filename *filename,
                           int width,const Config *config)
{
  if(width)
    fprintf(file,"%-12s %-4s %-*s ",descr,fmt[filename->fmt],
            width,notnull(filename->var));
  else
    fprintf(file,"%-12s %-4s ",descr,fmt[filename->fmt]);
  if(filename->fmt!=SOCK)
  {
    fprintf(file,"%s",notnull(filename->name));
    if(iscoupled(*config) && filename->issocket)
      fputs(", ",file);
  }
  if(iscoupled(*config) && filename->issocket)
    fprintf(file,"%d <- %s:%d",filename->id,config->coupled_host,config->coupler_port);
  fputc('\n',file);
} /* of 'printinputfile' */

static void fprintcultivations(FILE *file,const Pftpar *pftpar,int ntotpft)
{
  int p;
  int cult;
  cult=pftpar[0].cultivation_type;
  fprintf(file,"Cultivation types: %s",cultivation_type[cult]);
  for(p=1;p<ntotpft;p++)
  {
    if(cult!=pftpar[p].cultivation_type)
    {
      cult=pftpar[p].cultivation_type;
      fprintf(file,", %s",cultivation_type[cult]);
    }
  }
  fputc('\n',file);
} /* of 'fprintcultivations' */

void fprintconfig(FILE *file,          /**< File pointer to text output file */
                  int npft,            /**< Number of natural PFTs */
                  int ncft,            /**< Number of crop PFTs */
                  const Config *config /**< LPJmL configuration */
                 )
{
  char *fdi[]={"Nesterov index","water vapour pressure deficit index"};
  char *irrig[]={"no","limited","potential","all","irrigation on rainfed"};
  String s;
  Item *item;
  int len;
  int i,count=0,width,width_unit,index;
  Bool isnetcdf;
  fputs("==============================================================================\n",file);
  fprintattrs(file,config->global_attrs,config->n_global);
  fprintf(file,"Simulation \"%s\"",config->sim_name);
  if(config->ntask>1)
    fprintf(file," running on %d tasks\n",config->ntask);
  else
    putc('\n',file);
  len=0;
  if(config->landfrac_from_file)
    len=printsim(file,len,&count,"land fraction read from file");
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
    len=printsim(file,len,&count,"IMAGE coupling");
#endif
  if(config->wet_filename.name!=NULL)
    len=printsim(file,len,&count,"random precipitation");
  if(config->fire)
  {
    len=printsim(file,len,&count,(config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)  ? "spitfire" : "fire");
    if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->ispopulation)
      len=printsim(file,len,&count,"and population");
    if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->prescribe_burntarea)
      len=printsim(file,len,&count,"prescribe burntarea");
    if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
      len=printsim(file,len,&count,fdi[config->fdi]);
    if(config->fire_on_grassland)
      len=printsim(file,len,&count,"fire on grassland");
  }
  if(config->shuffle_spinup_climate)
    len=printsim(file,len,&count,"shuffle spinup climate");
  if(config->fix_climate)
  {
    snprintf(s,STRING_LEN,"fix climate after year %d %s years %d-%d",
             config->fix_climate_year,(config->fix_climate_shuffle) ? "shuffling" : "cycling",
             config->fix_climate_interval[0],config->fix_climate_interval[1]);
    len=printsim(file,len,&count,s);
  }
  if(config->withlanduse!=CONST_LANDUSE && config->withlanduse!=NO_LANDUSE && config->fix_landuse)
  {
    snprintf(s,STRING_LEN,"fix landuse after year %d",config->fix_landuse_year);
    len=printsim(file,len,&count,s);
  }
  if(config->fix_co2)
  {
    snprintf(s,STRING_LEN,"fix CO2 after year %d",config->fix_co2_year);
    len=printsim(file,len,&count,s);
  }
  if(config->fix_deposition)
  {
    if(config->fix_deposition_with_climate)
      len=printsim(file,len,&count,"fix deposition with climate");
    else
    {
      snprintf(s,STRING_LEN,"fix N deposition after year %d %s years %d-%d",
               config->fix_deposition_year,(config->fix_deposition_shuffle) ? "shuffling" : "cycling",
               config->fix_deposition_interval[0],config->fix_deposition_interval[1]);
      len=printsim(file,len,&count,s);
    }
  }
  if(config->no_ndeposition)
    len=printsim(file,len,&count,"no N deposition");
  if(config->river_routing)
    len=printsim(file,len,&count,"river routing");
  if(config->with_lakes)
    len=printsim(file,len,&count,"with lakes");
  if(config->extflow)
    len=printsim(file,len,&count,"external flow");
  if(config->equilsoil)
    len=printsim(file,len,&count,"equilsoil is called");
  if(config->with_nitrogen)
    len=printsim(file,len,&count,(config->with_nitrogen==UNLIM_NITROGEN) ? "unlimited nitrogen" : "nitrogen limitation");
  if(config->permafrost)
    len=printsim(file,len,&count,"permafrost");
#ifdef COUPLING_WITH_FMS
  if(!config->nitrogen_coupled)
    len=printsim(file,len,&count,"water and nitrogen limitations uncoupled");
#endif
  if(config->johansen)
    len=printsim(file,len,&count,"Johansen conductivity");
  if(config->prescribe_landcover)
    len=printsim(file,len,&count,(config->prescribe_landcover==LANDCOVEREST) ? "prescribed establishment":"prescribed maximum FPC");
  if(config->gsi_phenology)
    len=printsim(file,len,&count,"GSI phenology");
  if(config->transp_suction_fcn)
    len=printsim(file,len,&count,"transpiration suction function");
  if(config->soilpar_option==FIXED_SOILPAR)
  {
    len=fputstring(file,len,", ",78);
    count++;
    snprintf(s,STRING_LEN,"fixed soil parameter after %d",config->soilpar_fixyear);
    len=fputstring(file,len,s,78);
  }
  else if(config->soilpar_option==PRESCRIBED_SOILPAR)
  {
    len=fputstring(file,len,", ",78);
    count++;
    len=fputstring(file,len,"prescribed soil parameter",78);
  }
  if(config->npp_controlled_bnf)
      len=printsim(file,len,&count,"NPP controlled BNF");
  if(config->withlanduse)
  {
    switch(config->withlanduse)
    {
      case CONST_LANDUSE:
        snprintf(s,STRING_LEN," const landuse set to year %d, ",config->landuse_year_const);
        len=printsim(file,len,&count,s);
        break;
      case ALL_CROPS:
        len=printsim(file,len,&count,"all crops land use, ");
        break;
      case ONLY_CROPS:
        snprintf(s,STRING_LEN,"only crops land use for year %d, ",config->landuse_year_const);
        len=printsim(file,len,&count,s);
        break;
      default:
        len=printsim(file,len,&count,"land use, ");
    } /* of switch */
    if(config->rw_manage)
      len=printsim(file,len,&count,"rainwater management, ");
    len=fputstring(file,len,irrig[config->irrig_scenario],78);
    len=fputstring(file,len," irrigation",78);
    if(config->fix_fertilization)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"fixed fertilization",78);
    }
    if(config->intercrop)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"intercropping",78);
    }
    if (config->others_to_crop)
    {
      snprintf(s,STRING_LEN,"others to crop \"%s\" and \"%s\"",
               config->pftpar[config->cft_tropic+npft].name,
               config->pftpar[config->cft_temp+npft].name);
      len=printsim(file,len,&count,s);
    }
    if (config->cropsheatfrost)
    {
      len += fprintf(file, ", ");
      len = fputstring(file, len, "with crops heat frost", 78);
    }
    if (config->grassonly)
    {
      len += fprintf(file, ", ");
      len = fputstring(file, len, "grassonly", 78);
    }
    if(config->luc_timber)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"land-use change timber",78);
    }
    if(config->residue_treatment==FIXED_RESIDUE_REMOVE)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"fixed residue remove",78);
    }
    else if(config->residue_treatment==READ_RESIDUE_DATA)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"residue remove read from file",78);
    }
    if(config->tillage_type==TILLAGE)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"with tillage",78);
    }
    else if(config->tillage_type==READ_TILLAGE)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"with tillage read from file",78);
    }
    if (config->crop_resp_fix)
    {
      len += fprintf(file, ", ");
      len = fputstring(file, len, "C:N fixed crop respiration", 78);
    }
    if (config->fertilizer_input==AUTO_FERTILIZER)
    {
      len += fprintf(file, ", ");
      len = fputstring(file, len, "with auto fertilizer", 78);
    }
    if(config->residues_fire)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"fire in residuals",78);
    }
    if(config->laimax_manage==LAIMAX_CONST)
    {
      len+=fprintf(file,", ");
      snprintf(s,STRING_LEN,"const LAImax=%.1f",config->laimax);
      len=fputstring(file,len,s,78);
    }
    else if(config->laimax_manage==LAIMAX_PAR)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"pft.cjson LAImax",78);
    }
    if(config->sdate_option==FIXED_SDATE)
    {
      len=fputstring(file,len,", ",78);
      count++;
      snprintf(s,STRING_LEN,"fixed sowing date after %d",config->sdate_fixyear);
      len=fputstring(file,len,s,78);
    }
    else if(config->sdate_option==PRESCRIBED_SDATE)
    {
      len=fputstring(file,len,", ",78);
      count++;
      len=fputstring(file,len,"prescribed sowing date",78);
    }
    if(config->grassharvest_filename.name==NULL)
    {
      len=fputstring(file,len,", ",78);
      count++;
      snprintf(s,STRING_LEN,"%s grazing",grazing_type[config->grazing]);
      len=fputstring(file,len,s,78);
    }
    if(!config->others_to_crop)
    {
      len=fputstring(file,len,", ",78);
      snprintf(s,STRING_LEN,"%s others grazing",grazing_type[config->grazing_others]);
      count++;
      len=fputstring(file,len,s,78);
    }
    len=fputstring(file,len,", ",78);
    count++;
    snprintf(s,STRING_LEN,"%s crop PHU option",crop_phu_options[config->crop_phu_option]);
    len=fputstring(file,len,s,78);
  }
  if(config->separate_harvests)
    len=printsim(file,len,&count,"separate harvests");
  if(config->grassharvest_filename.name!=NULL)
    len=printsim(file,len,&count,"grassland harvest options");
  if(config->prescribe_lsuha)
    len=printsim(file,len,&count,"prescribed livestock density");
  if(config->reservoir)
    len=printsim(file,len,&count,"dam reservoirs");
#ifdef IMAGE
  if(config->groundwater_irrig)
    len=printsim(file,len,&count,"groundwater irrigation");
  if(config->aquifer_irrig)
    len=printsim(file,len,&count,"aquifer irrigation");
#endif
  if(config->wateruse)
   len=printsim(file,len,&count,(config->wateruse==ALL_WATERUSE) ? "always water use" : "water use");
#ifdef IMAGE
  if(config->wateruse_wd_filename.name!=NULL)
    len = printsim(file, len, &count, "water use wd");
#endif
  if(count)
    fputs(".\n",file);
  if(config->withlanduse && config->grassharvest_filename.name!=NULL)
  {
    fprintf(file,"Mowing days for grassland:");
    for(i=0;i<config->mowingdays_size;i++)
      fprintf(file," %d",config->mowingdays[i]);
    fputc('\n',file);
  }
  fprintcultivations(file,config->pftpar,npft+ncft);
  fprintf(file,"Working directory: %s\n",getdir());
  if(isreadrestart(config))
    fprintf(file,"Starting from restart file '%s'.\n",config->restart_filename);
  else
    fputs("Starting from scratch.\n",file);
  width=(int)isnetcdfinput(config);
  fputs("Input files:\n",file);
  if(width)
  {
    fprintf(file,"Variable     Fmt  %-*s Filename\n"
          "------------ ---- ",width,"Varname");
    frepeatch(file,'-',width);
    fputc(' ',file);
    frepeatch(file,'-',79-20-width);
    fputc('\n',file);
  }
  else
    fputs("Variable     Fmt  Filename\n"
          "------------ ---- -------------------------------------------------------------\n",file);
  printinputfile(file,"soil",&config->soil_filename,width,config);
  if(config->soil_filename.fmt!=CDF)
    printinputfile(file,"coord",&config->coord_filename,width,config);
  if(config->landfrac_from_file)
    printinputfile(file,"landfrac",&config->landfrac_filename,width,config);
  printinputfile(file,"temp",&config->temp_filename,width,config);
  printinputfile(file,"prec",&config->prec_filename,width,config);
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    printinputfile(file,"temp var",&config->temp_var_filename,width,config);
    printinputfile(file,"prec var",&config->prec_var_filename,width,config);
    printinputfile(file,"prod pool",&config->prodpool_init_filename,width,config);
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
      printinputfile(file,(config->with_radiation==RADIATION) ? "lwnet" : "lwdown",
                     &config->lwnet_filename,width,config);
    printinputfile(file,"swdown",&config->swdown_filename,width,config);
  }
  else
    printinputfile(file,"cloud",&config->cloud_filename,width,config);
  if(config->with_nitrogen)
  {
    if(config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
    {
      printinputfile(file,"no3_depo",&config->no3deposition_filename,width,config);
      printinputfile(file,"nh4_depo",&config->nh4deposition_filename,width,config);
    }
    printinputfile(file,"soilpH",&config->soilph_filename,width,config);
  }
  printinputfile(file,"co2",&config->co2_filename,width,config);
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    printinputfile(file,"windspeed",&config->wind_filename,width,config);
  if(config->cropsheatfrost || config->fire==SPITFIRE_TMAX)
  {
    printinputfile(file,"tmin",&config->tmin_filename,width,config);
    printinputfile(file,"tmax",&config->tmax_filename,width,config);
  }
  if(config->fire==SPITFIRE)
    printinputfile(file,"temp ampl",&config->tamp_filename,width,config);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
      printinputfile(file,"humid",&config->humid_filename,width,config);
    printinputfile(file,"lightning",&config->lightning_filename,width,config);
    printinputfile(file,"human ign",&config->human_ignition_filename,width,config);
  }
  if(config->ispopulation)
    printinputfile(file,"pop. dens",&config->popdens_filename,width,config);
  if(config->prescribe_burntarea)
    printinputfile(file,"burntarea",&config->burntarea_filename,width,config);
  if(config->prescribe_landcover)
    printinputfile(file,"landcover",&config->landcover_filename,width,config);
  if(config->grassharvest_filename.name!=NULL)
    printinputfile(file,"Grassharvest",&config->grassharvest_filename,width,config);
  if(config->withlanduse!=NO_LANDUSE)
  {
    printinputfile(file,"countries",&config->countrycode_filename,width,config);
    if(config->countrycode_filename.fmt==CDF)
      printinputfile(file,"regions",&config->regioncode_filename,width,config);
    printinputfile(file,"landuse",&config->landuse_filename,width,config);
    if(config->iscotton)
    {
      printinputfile(file,"sowing_rf",&config->sowing_cotton_rf_filename,width,config);
      printinputfile(file,"harvest_rf",&config->harvest_cotton_rf_filename,width,config);
      printinputfile(file,"sowing_ir",&config->sowing_cotton_ir_filename,width,config);
      printinputfile(file,"harvest_ir",&config->harvest_cotton_ir_filename,width,config);
    }
    if(config->sdate_option==PRESCRIBED_SDATE)
      printinputfile(file,"sdates",&config->sdate_filename,width,config);
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
      printinputfile(file,"crop_phu",&config->crop_phu_filename,width,config);
    if(config->with_nitrogen&&config->fertilizer_input==FERTILIZER)
      printinputfile(file,"fertilizer",&config->fertilizer_nr_filename, width,config);
    if(config->with_nitrogen&&config->manure_input)
      printinputfile(file,"manure_nr",&config->manure_nr_filename, width,config);
    if(config->residue_treatment==READ_RESIDUE_DATA)
      printinputfile(file,"residue",&config->residue_data_filename,width,config);
    if(config->tillage_type==READ_TILLAGE)
      printinputfile(file,"tillage",&config->with_tillage_filename,width,config);
    if(config->prescribe_lsuha)
      printinputfile(file,"livestock",&config->lsuha_filename,width,config);
  }
  if(config->reservoir)
  {
    printinputfile(file,"elevation",&config->elevation_filename,width,config);
    printinputfile(file,"reservoir",&config->reservoir_filename,width,config);
  }
#ifdef IMAGE
  if(config->aquifer_irrig)
    printinputfile(file,"aquifer",&config->aquifer_filename,width,config);
#endif
  if(config->wet_filename.name!=NULL)
    printinputfile(file,"wetdays",&config->wet_filename,width,config);
  if(config->with_lakes)
    printinputfile(file,"lakes",&config->lakes_filename,width,config);
  if(config->river_routing)
  {
    printinputfile(file,"drainage",&config->drainage_filename,width,config);
    if(config->drainage_filename.fmt==CDF)
      printinputfile(file,"river",&config->river_filename,width,config);
    if(config->extflow)
      printinputfile(file,"extflow",&config->extflow_filename,width,config);
    if(config->withlanduse!=NO_LANDUSE)
      printinputfile(file,"neighbour",&config->neighb_irrig_filename,width,config);
  }
  if(config->wateruse)
    printinputfile(file,"wateruse",&config->wateruse_filename,width,config);
#ifdef IMAGE
  if (config->wateruse_wd_filename.name != NULL)
    printinputfile(file,"wateruse wd", &config->wateruse_wd_filename,width,config);
#endif
  if(width)
  {
    fputs("------------ ---- ",file);
    frepeatch(file,'-',width);
    fputc(' ',file);
    frepeatch(file,'-',79-20-width);
    fputc('\n',file);
  }
  else
    fputs("------------ ---- -------------------------------------------------------------\n",file);
  if(config->param_out)
    fprintparam(file,npft,ncft,config);
  if(iswriterestart(config))
    fprintf(file,"Writing restart file '%s' after year %d.\n",
            config->write_restart_filename,config->restartyear);
  if(ischeckpointrestart(config))
    fprintf(file,"Checkpoint restart file: '%s'.\n",
            config->checkpoint_restart_filename);

#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
    fprintf(file,
            "Coupled to IMAGE model running on host %s using port %d and %d.\n"
            "Time to wait for connection: %6d sec\n",
            config->image_host,config->image_inport,
            config->image_outport,config->wait);

#else
  if(iscoupled(*config))
  {
    fprintf(file,"Coupled to %s model running on host %s using port %d.\n",
            config->coupled_model,config->coupled_host,config->coupler_port);
    if(config->wait)
      fprintf(file,"Time to wait for connection: %5d sec\n",config->wait);
    fprintf(file,"Number of inputs from %s: %5d\n"
            "Number of outputs to %s:  %5d\n",
            config->coupled_model,config->coupler_in,config->coupled_model,config->coupler_out);

  }
#endif

#ifndef PERMUTE
  if(config->wet_filename.name!=NULL)
#endif
  {
    if(isreadrestart(config) && !config->new_seed)
      fputs("Reading random seeds from restart file.\n",file);
    else
      fprintf(file,"Random seed: %d\n",config->seed_start);
  }
  if(config->n_out)
  {
    /* sort output alphabetically by name */
    item=newvec(Item,config->n_out);
    if(item!=NULL)
    {
      for(i=0;i<config->n_out;i++)
      {
        item[i].index=i;
        item[i].name=config->outnames[config->outputvars[i].id].name;
      }
      qsort(item,config->n_out,sizeof(Item),(int (*)(const void *,const void *))compare);
    }
    width=strlen("Variable");
    width_unit=strlen("Unit");
    for(i=0;i<config->n_out;i++)
    {
      if(config->outnames[config->outputvars[i].id].name!=NULL &&
         width<(int)strlen(config->outnames[config->outputvars[i].id].name))
        width=strlen(config->outnames[config->outputvars[i].id].name);
      width_unit=max(width_unit,(int)strlen(config->outnames[config->outputvars[i].id].unit));
    }
    fprintf(file,"Number of output files:       %d\n"
                 "Output written in year:       %d\n"
                 "Byte order in output files:   %s\n",
            config->n_out,config->outputyear,
            bigendian() ? "big endian" : "little endian");
    fputc('\n',file);
    isnetcdf=FALSE;
    for(i=0;i<config->n_out;i++)
      if(hassuffix(config->outputvars[i].filename.name,config->compress_suffix))
      {
        fprintf(file,"Output with %s suffix is compressed with '%s'.\n",
                config->compress_suffix,config->compress_cmd);
        break;
      }
    for(i=0;i<config->n_out;i++)
      if(config->outputvars[i].filename.fmt==CDF)
      {
        isnetcdf=TRUE;
        break;
      }
    if(isnetcdf)
    {
      if(config->compress)
        fprintf(file,"Compression level for NetCDF: %d\n",config->compress);
      fprintf(file,"Missing value in NetCDF:      %g\n"
                   "Base year in NetCDF:          %d\n"
                   "NetCDF grid:                  %s\n",
              config->missing_value,config->baseyear,
              config->global_netcdf ? "global" : "local");
    }
    fprintf(file,"%*s Fmt  %*s Type   tstep nbd Filename\n",-width,"Variable",-width_unit,"Unit");
    frepeatch(file,'-',width);
    fputs(" ---- ",file);
    frepeatch(file,'-',width_unit);
    fputs(" ------ ----- --- ",file);
    frepeatch(file,'-',76-width-4-width_unit-7-3-7);
    putc('\n',file);
    for(i=0;i<config->n_out;i++)
    {
      index=(item==NULL) ? i : item[i].index;
      if(config->outnames[config->outputvars[index].id].name==NULL)
        fprintf(file,"%*d",-width,config->outputvars[index].id);
      else
        fprintf(file,"%*s",-width,config->outnames[config->outputvars[index].id].name);
      fprintf(file," %-4s %*s %-6s %-5s %3d ",fmt[config->outputvars[index].filename.fmt],
              -width_unit,strlen(config->outnames[config->outputvars[index].id].unit)==0 ? "-" : config->outnames[config->outputvars[index].id].unit,
              typenames[getoutputtype(config->outputvars[index].id,config->grid_type)],
              sprinttimestep(s,config->outnames[config->outputvars[index].id].timestep),outputsize(config->outputvars[index].id,npft,ncft,config));
      printoutname(file,&config->outputvars[index].filename,config->outputvars[index].oneyear,config);
      putc('\n',file);
    }
    free(item);
    frepeatch(file,'-',width);
    fputs(" ---- ",file);
    frepeatch(file,'-',width_unit);
    fputs(" ------ ----- --- ",file);
    frepeatch(file,'-',76-width-4-width_unit-7-3-7);
    putc('\n',file);
    if(config->pft_output_scaled)
      fputs("PFT-specific output is grid scaled.\n",file);
  }
  else
    fputs("No output files written.\n",file);
  if(config->nspinup)
  {
    if(config->isfirstspinupyear)
      fprintf(file,"First spinup year:           %6d\n",config->firstspinupyear);
    fprintf(file,"Spinup years:                %6d\n"
            "Cycle length during spinup:  %6d\n",
             config->nspinup,config->nspinyear);
  }
  else
    fputs("No spinup years.\n",file);
  fprintf(file,"First year:                  %6d\n"
          "Last year:                   %6d\n",
          config->firstyear,config->lastyear);
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
    fprintf(file,"Start IMAGE coupling:        %6d\n",
            config->start_coupling);
#else
  if(iscoupled(*config))
    fprintf(file,"Start coupling:              %6d\n",
            config->start_coupling);
#endif
  if(config->nall==-1)
    fprintf(file,"Number of grid cells:       N/A\n");
  else
  {
    if(config->firstgrid)
      fprintf(file,"Index of first cell:        %7d\n",config->firstgrid);
    fprintf(file,"Number of grid cells:       %7d\n",config->nall);
  }
  fputs("==============================================================================\n",file);
  fflush(file);
} /* of 'fprintconfig' */
