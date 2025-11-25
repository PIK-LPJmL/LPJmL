/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  f  i  l  e  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints LPJ input/output files                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checklist(n) if((n)==0) fail(ALLOC_MEMORY_ERR,TRUE,FALSE,"Cannot allocate memory in %s()",__FUNCTION__)


static int cmpstringp(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
} /* of 'cmpstringp' */

static void fprintfilenames(FILE *file,const List *table)
{
  int i;
  qsort(table->data,getlistlen(table),sizeof(char *),cmpstringp);
  foreachlistitem(i,table)
    if(i)
    {
      if(strcmp(getlistitem(table,i-1),getlistitem(table,i)))
       fprintf(file,"%s\n",(char *)getlistitem(table,i));
    }
    else
      fprintf(file,"%s\n",(char *)getlistitem(table,i));
} /* of 'fprintfilenames' */

static void addfilename(List *table,              /**< pointer to table */
                        const Filename *filename, /**< filename */
                        Bool isyear               /**< input is year dependent */
                       )
{
  char *s,*name;
  int first,last,year,n;
  if(filename->fmt==FMS || filename->fmt==SOCK)
    return;
  if(filename->fmt==CDF && isyear)
  {
    s=strchr(filename->name,'[');
    if(s!=NULL && sscanf(s,"[%d-%d]",&first,&last)==2)
    {
      s=mkfilename(filename->name);
      if(s==NULL)
        fprintf(stderr,"ERROR225: Cannot parse filename '%s'.\n",filename->name);
      else
      {
        for(year=first;year<=last;year++)
        {
          name=getsprintf(s,year);
          check(name);
          n=addlistitem(table,name);
          checklist(n);
        }
        free(s);
      }
    }
    else
    {
      name=strdup(filename->name);
      check(name);
      n=addlistitem(table,name);
      checklist(n);
    }
  }
  else if(filename->fmt==META)
  {
    name=strdup(filename->name);
    check(name);
    n=addlistitem(table,name);
    checklist(n);
    s=getfilefrommeta(filename->name,TRUE);
    if(s!=NULL)
    {
      name=strdup(s);
      check(name);
      n=addlistitem(table,name);
      checklist(n);
      free(s);
    }
  }
  else
  {
    name=strdup(filename->name);
    check(name);
    n=addlistitem(table,name);
    checklist(n);
  }
} /* of 'addfilename' */

static void freetable(List *table)
{
  int i;
  foreachlistitem(i,table)
    free(getlistitem(table,i));
  freelist(table);
} /* of 'freetable' */

void fprintfiles(FILE *file,          /**< pointer to text output file */
                 Bool withinput,      /**< list input data files (TRUE/FALSE) */
                 Bool withoutput,     /**< list output data files (TRUE/FALSE) */
                 const Config *config /**< LPJmL configuration */
                )
{
  List *table;
  char *name;
  int i,j,n;
  table=newlist(0);
  if(isreadrestart(config))
    fprintf(file,"%s\n",config->restart_filename);
  if(withinput)
  {
  if(config->soil_filename.fmt!=CDF)
    addfilename(table,&config->coord_filename,FALSE);
  addfilename(table,&config->soil_filename,FALSE);
  addfilename(table,&config->kbf_filename,FALSE);
  addfilename(table,&config->slope_filename,FALSE);
  addfilename(table,&config->slope_min_filename,FALSE);
  addfilename(table,&config->slope_max_filename,FALSE);
  if(config->landfrac_from_file)
    addfilename(table,&config->landfrac_filename,FALSE);
  addfilename(table,&config->temp_filename,TRUE);
  addfilename(table,&config->prec_filename,TRUE);
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    addfilename(table,&config->temp_var_filename,TRUE);
    addfilename(table,&config->prec_var_filename,TRUE);
    addfilename(table,&config->prodpool_init_filename,FALSE);
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation!=RADIATION_SWONLY)
      addfilename(table,&config->lwnet_filename,TRUE);
    if(config->isanomaly)
    {
      addfilename(table,&config->delta_lwnet_filename,TRUE);
      addfilename(table,&config->delta_swdown_filename,TRUE);
    }
    addfilename(table,&config->swdown_filename,TRUE);
  }
  else
    addfilename(table,&config->cloud_filename,TRUE);
  addfilename(table,&config->hydrotopes_filename,FALSE);
  addfilename(table,&config->co2_filename,FALSE);
  if (config->with_methane && config->with_dynamic_ch4==PRESCRIBED_CH4)
    addfilename(table,&config->ch4_filename,FALSE);
  if(!config->unlim_nitrogen && !config->no_ndeposition)
  {
    addfilename(table,&config->no3deposition_filename,TRUE);
    addfilename(table,&config->nh4deposition_filename,TRUE);
  }
  addfilename(table,&config->soilph_filename,FALSE);
  addfilename(table,&config->wind_filename,TRUE);
  if(config->fire==SPITFIRE_TMAX)
  {
    addfilename(table,&config->tmax_filename,TRUE);
    addfilename(table,&config->tmin_filename,TRUE);
  }
  if(config->fire==SPITFIRE)
    addfilename(table,&config->tamp_filename,TRUE);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
      addfilename(table,&config->humid_filename,TRUE);
    if(config->prescribe_burntarea)
      addfilename(table,&config->burntarea_filename,TRUE);
    addfilename(table,&config->lightning_filename,FALSE);
    addfilename(table,&config->human_ignition_filename,FALSE);
  }
  if(config->ispopulation)
    addfilename(table,&config->popdens_filename,TRUE);
  if(config->grassharvest_filename.name!=NULL)
    addfilename(table,&config->grassharvest_filename,FALSE);
  if(config->withlanduse!=NO_LANDUSE)
  {
    addfilename(table,&config->countrycode_filename,FALSE);
    addfilename(table,&config->landuse_filename,TRUE);
    if(config->sdate_option>=PRESCRIBED_SDATE)
      addfilename(table,&config->sdate_filename,TRUE);
    if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
      addfilename(table,&config->crop_phu_filename,TRUE);
    if(config->fertilizer_input)
      addfilename(table,&config->fertilizer_nr_filename,TRUE);
    if (config->manure_input)
      addfilename(table,&config->manure_nr_filename,TRUE);
    if(config->residue_treatment==READ_RESIDUE_DATA)
      addfilename(table,&config->residue_data_filename,TRUE);
    if(config->tillage_type==READ_TILLAGE)
      addfilename(table,&config->with_tillage_filename,TRUE);

    if(config->prescribe_lsuha)
      addfilename(table,&config->lsuha_filename,FALSE);
  }
  if(config->reservoir)
  {
    addfilename(table,&config->elevation_filename,FALSE);
    addfilename(table,&config->reservoir_filename,FALSE);
    if(config->reservoir_filename.fmt==CDF)
    {
      addfilename(table,&config->capacity_reservoir_filename,FALSE);
      addfilename(table,&config->inst_cap_reservoir_filename,FALSE);
      addfilename(table,&config->area_reservoir_filename,FALSE);
      addfilename(table,&config->height_reservoir_filename,FALSE);
      addfilename(table,&config->purpose_reservoir_filename,FALSE);
    }
  }
#ifdef IMAGE
  if(config->aquifer_irrig)
    addfilename(table,&config->aquifer_filename,FALSE);
#endif
  if(config->wet_filename.name!=NULL)
    addfilename(table,&config->wet_filename,TRUE);
  if(config->with_lakes)
    addfilename(table,&config->lakes_filename,FALSE);
  if(config->river_routing)
  {
    addfilename(table,&config->drainage_filename,FALSE);
    if(config->withlanduse!=NO_LANDUSE)
      addfilename(table,&config->neighb_irrig_filename,FALSE);
  }
  if(config->wateruse)
    addfilename(table,&config->wateruse_filename,TRUE);
#ifdef IMAGE
  if (config->wateruse_wd_filename.name != NULL)
    addfilename(table,&config->wateruse_wd_filename,TRUE);
#endif
  fprintfilenames(file,table);
  freetable(table);
  }
  if(withoutput)
  {
    table=newlist(0);
    if(config->json_filename!=NULL)
      fprintf(file,"%s\n",config->json_filename);
    if(iswriterestart(config))
      fprintf(file,"%s\n",config->write_restart_filename);
    for(i=0;i<config->n_out;i++)
      if(config->outputvars[i].filename.fmt!=SOCK)
      {
        if(config->outputvars[i].oneyear)
        {
          for(j=config->outputyear;j<=config->lastyear;j++)
          {
            name=getsprintf(config->outputvars[i].filename.name,j);
            check(name);
            n=addlistitem(table,name);
            checklist(n);
            if(config->outputvars[i].filename.meta)
            {
              name=malloc(strlen(getlistitem(table,getlistlen(table)-1))+1+strlen(config->json_suffix));
              check(name);
              strcat(strcpy(name,getlistitem(table,getlistlen(table)-1)),config->json_suffix);
              n=addlistitem(table,name);
              checklist(n);
            }
          }
        }
        else
        {
          name=strdup(config->outputvars[i].filename.name);
          check(name);
          n=addlistitem(table,name);
          checklist(n);
          if(config->outputvars[i].filename.meta)
          {
            name=malloc(strlen(getlistitem(table,n-1))+1+strlen(config->json_suffix));
            check(name);
            strcat(strcpy(name,getlistitem(table,n-1)),config->json_suffix);
            n=addlistitem(table,name);
            checklist(n);
          }
        }
    }
    fprintfilenames(file,table);
    freetable(table);
  }
} /* of 'fprintfiles' */
