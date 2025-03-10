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

typedef struct
{
  char **names; /** array of filenames */
  int size;     /** size of array */
} Table;

static int cmpstringp(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
} /* of 'cmpstringp' */

static void fprintfilenames(FILE *file,const Table *table)
{
  int i;
  qsort(table->names,table->size,sizeof(char *),cmpstringp);
  for(i=0;i<table->size;i++)
    if(i)
    {
      if(strcmp(table->names[i-1],table->names[i]))
       fprintf(file,"%s\n",table->names[i]);
    }
    else
     fprintf(file,"%s\n",table->names[i]);
} /* of 'fprintfilenames' */

static void addfilename(Table *table,             /**< pointer to table */
                        const Filename *filename, /**< filename */
                        Bool isyear               /**< input is year dependent */
                       )
{
  char *s;
  int first,last,year;
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
        table->names=(char **)realloc(table->names,(table->size+last-first+1)*sizeof(char *));
        check(table->names);
        for(year=first;year<=last;year++)
        {
          table->names[table->size+year-first]=getsprintf(s,year);
          check(table->names[table->size+year-first]);
        }
        free(s);
        table->size+=last-first+1;
      }
    }
    else
    {
      table->names=(char **)realloc(table->names,(table->size+1)*sizeof(char *));
      check(table->names);
      table->names[table->size]=strdup(filename->name);
      check(table->names[table->size]);
      table->size++;
    }
  }
  else if(filename->fmt==META)
  {
    table->names=(char **)realloc(table->names,(table->size+1)*sizeof(char *));
    check(table->names);
    table->names[table->size]=strdup(filename->name);
    check(table->names[table->size]);
    table->size++;
    s=getfilefrommeta(filename->name,TRUE);
    if(s!=NULL)
    {
      table->names=(char **)realloc(table->names,(table->size+1)*sizeof(char *));
      check(table->names);
      table->names[table->size]=strdup(s);
      check(table->names[table->size]);
      table->size++;
      free(s);
    }
  }
  else
  {
    table->names=(char **)realloc(table->names,(table->size+1)*sizeof(char *));
    check(table->names);
    table->names[table->size]=strdup(filename->name);
    check(table->names[table->size]);
    table->size++;
  }
} /* of 'addfilename' */

static void freetable(Table *table)
{
  int i;
  for(i=0;i<table->size;i++)
    free(table->names[i]);
  free(table->names);
  table->names=NULL;
  table->size=0;
} /* of 'freetable' */

void fprintfiles(FILE *file,          /**< pointer to text output file */
                 Bool withinput,      /**< list input data files (TRUE/FALSE) */
                 Bool withoutput,     /**< list output data files (TRUE/FALSE) */
                 const Config *config /**< LPJmL configuration */
                )
{
  Table table={NULL,0};
  size_t len;
  int i,j;
  if(isreadrestart(config))
    fprintf(file,"%s\n",config->restart_filename);
  if(withinput)
  {
  if(config->soil_filename.fmt!=CDF)
    addfilename(&table,&config->coord_filename,FALSE);
  addfilename(&table,&config->soil_filename,FALSE);
  if(config->landfrac_from_file)
    addfilename(&table,&config->landfrac_filename,FALSE);
  addfilename(&table,&config->temp_filename,TRUE);
  addfilename(&table,&config->prec_filename,TRUE);
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    addfilename(&table,&config->temp_var_filename,TRUE);
    addfilename(&table,&config->prec_var_filename,TRUE);
    addfilename(&table,&config->prodpool_init_filename,FALSE);
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation!=RADIATION_SWONLY)
      addfilename(&table,&config->lwnet_filename,TRUE);
    addfilename(&table,&config->swdown_filename,TRUE);
  }
  else
    addfilename(&table,&config->cloud_filename,TRUE);
  fprintf(file,"%s\n",config->co2_filename.name);
  if(!config->unlim_nitrogen && !config->no_ndeposition)
  {
    addfilename(&table,&config->no3deposition_filename,TRUE);
    addfilename(&table,&config->nh4deposition_filename,TRUE);
  }
  addfilename(&table,&config->soilph_filename,FALSE);
  addfilename(&table,&config->wind_filename,TRUE);
  if(config->fire==SPITFIRE_TMAX)
  {
    addfilename(&table,&config->tmax_filename,TRUE);
    addfilename(&table,&config->tmin_filename,TRUE);
  }
  if(config->fire==SPITFIRE)
    addfilename(&table,&config->tamp_filename,TRUE);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
      addfilename(&table,&config->humid_filename,TRUE);
    if(config->prescribe_burntarea)
      addfilename(&table,&config->burntarea_filename,TRUE);
    addfilename(&table,&config->lightning_filename,FALSE);
    addfilename(&table,&config->human_ignition_filename,FALSE);
  }
  if(config->ispopulation)
    addfilename(&table,&config->popdens_filename,TRUE);
  if(config->grassharvest_filename.name!=NULL)
    addfilename(&table,&config->grassharvest_filename,FALSE);
  if(config->withlanduse!=NO_LANDUSE)
  {
    addfilename(&table,&config->countrycode_filename,FALSE);
    addfilename(&table,&config->landuse_filename,TRUE);
    if(config->sdate_option>=PRESCRIBED_SDATE)
      addfilename(&table,&config->sdate_filename,TRUE);
    if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
      addfilename(&table,&config->crop_phu_filename,TRUE);
    if(config->fertilizer_input)
      addfilename(&table,&config->fertilizer_nr_filename,TRUE);
    if (config->manure_input)
      addfilename(&table,&config->manure_nr_filename,TRUE);
    if(config->residue_treatment==READ_RESIDUE_DATA)
      addfilename(&table,&config->residue_data_filename,TRUE);
    if(config->tillage_type==READ_TILLAGE)
      addfilename(&table,&config->with_tillage_filename,TRUE);

    if(config->prescribe_lsuha)
      addfilename(&table,&config->lsuha_filename,FALSE);
  }
  if(config->reservoir)
  {
    addfilename(&table,&config->elevation_filename,FALSE);
    addfilename(&table,&config->reservoir_filename,FALSE);
    if(config->reservoir_filename.fmt==CDF)
    {
      addfilename(&table,&config->capacity_reservoir_filename,FALSE);
      addfilename(&table,&config->inst_cap_reservoir_filename,FALSE);
      addfilename(&table,&config->area_reservoir_filename,FALSE);
      addfilename(&table,&config->height_reservoir_filename,FALSE);
      addfilename(&table,&config->purpose_reservoir_filename,FALSE);
    }
  }
#ifdef IMAGE
  if(config->aquifer_irrig)
    addfilename(&table,&config->aquifer_filename,FALSE);
#endif
  if(config->wet_filename.name!=NULL)
    addfilename(&table,&config->wet_filename,TRUE);
  if(config->with_lakes)
    addfilename(&table,&config->lakes_filename,FALSE);
  if(config->river_routing)
  {
    addfilename(&table,&config->drainage_filename,FALSE);
    if(config->withlanduse!=NO_LANDUSE)
      addfilename(&table,&config->neighb_irrig_filename,FALSE);
  }
  if(config->wateruse)
    addfilename(&table,&config->wateruse_filename,TRUE);
#ifdef IMAGE
  if (config->wateruse_wd_filename.name != NULL)
    addfilename(&table,&config->wateruse_wd_filename,TRUE);
#endif
  fprintfilenames(file,&table);
  freetable(&table);
  }
  if(withoutput)
  {
    if(config->json_filename!=NULL)
      fprintf(file,"%s\n",config->json_filename);
    if(iswriterestart(config))
      fprintf(file,"%s\n",config->write_restart_filename);
    for(i=0;i<config->n_out;i++)
      if(config->outputvars[i].filename.fmt!=SOCK)
      {
        if(config->outputvars[i].oneyear)
        {
          if(config->outputvars[i].filename.meta)
            table.names=(char **)realloc(table.names,(table.size+(config->lastyear-config->outputyear+1)*2)*sizeof(char *));
          else
            table.names=(char **)realloc(table.names,(table.size+config->lastyear-config->outputyear+1)*sizeof(char *));
          check(table.names);
          for(j=config->outputyear;j<=config->lastyear;j++)
          {
            len=snprintf(NULL,0,config->outputvars[i].filename.name,j);
            table.names[table.size]=malloc(len+1);
            table.names[table.size]=getsprintf(config->outputvars[i].filename.name,j);
            check(table.names[table.size]);
            table.size++;
            if(config->outputvars[i].filename.meta)
            {
              table.names[table.size]=malloc(strlen(table.names[table.size-1])+1+strlen(config->json_suffix));
              strcat(strcpy(table.names[table.size],table.names[table.size-1]),config->json_suffix);
              table.size++;
            }
          }
        }
        else
        {
          table.names=(char **)realloc(table.names,(table.size+1)*sizeof(char *));
          check(table.names);
          table.names[table.size]=strdup(config->outputvars[i].filename.name);
          check(table.names[table.size]);
          table.size++;
          if(config->outputvars[i].filename.meta)
          {
            table.names=(char **)realloc(table.names,(table.size+1)*sizeof(char *));
            check(table.names);
            table.names[table.size]=malloc(strlen(table.names[table.size-1])+1+strlen(config->json_suffix));
            check(table.names[table.size]);
            strcat(strcpy(table.names[table.size],table.names[table.size-1]),config->json_suffix);
            table.size++;
          }
        }
    }
    fprintfilenames(file,&table);
    freetable(&table);
  }
} /* of 'fprintfiles' */
