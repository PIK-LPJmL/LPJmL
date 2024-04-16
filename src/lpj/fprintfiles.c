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

static void fprintfilename(FILE *file,               /**< pointer to text file */
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
        for(year=first;year<=last;year++)
        {
          fprintf(file,s,year);
          fputc('\n',file);
        }
        free(s);
      }
    }
    else
      fprintf(file,"%s\n",filename->name);
  }
  else if(filename->fmt==META)
  {
    fprintf(file,"%s\n",filename->name);
    s=getfilefrommeta(filename->name,TRUE);
    if(s!=NULL)
      fprintf(file,"%s\n",s);
    free(s);
  }
  else
    fprintf(file,"%s\n",filename->name);
} /* of 'fprintfilename' */

void fprintfiles(FILE *file,          /**< pointer to text output file */
                 Bool withinput,      /**< list input data files (TRUE/FALSE) */
                 Bool withoutput,     /**< list output data files (TRUE/FALSE) */
                 const Config *config /**< LPJmL configuration */
                )
{
  int i,j;
  if(isreadrestart(config))
    fprintf(file,"%s\n",config->restart_filename);
  if(withinput)
  {
  if(config->soil_filename.fmt!=CDF)
    fprintfilename(file,&config->coord_filename,FALSE);
  fprintfilename(file,&config->soil_filename,FALSE);
  fprintfilename(file,&config->kbf_filename,FALSE);
  fprintfilename(file,&config->slope_filename,FALSE);
  fprintfilename(file,&config->slope_min_filename,FALSE);
  fprintfilename(file,&config->slope_max_filename,FALSE);
  if(config->landfrac_from_file)
    fprintfilename(file,&config->landfrac_filename,FALSE);
  fprintfilename(file,&config->temp_filename,TRUE);
  fprintfilename(file,&config->prec_filename,TRUE);
  if(config->isanomaly)
  {
    fprintfilename(file,&config->icefrac_filename,TRUE);
    fprintfilename(file,&config->delta_temp_filename,TRUE);
    fprintfilename(file,&config->delta_prec_filename,TRUE);
  }
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    fprintfilename(file,&config->temp_var_filename,TRUE);
    fprintfilename(file,&config->prec_var_filename,TRUE);
    fprintfilename(file,&config->prodpool_init_filename,FALSE);
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation!=RADIATION_SWONLY)
      fprintfilename(file,&config->lwnet_filename,TRUE);
    fprintfilename(file,&config->swdown_filename,TRUE);
    if(config->isanomaly)
    {
      fprintfilename(file,&config->delta_lwnet_filename,TRUE);
      fprintfilename(file,&config->delta_swdown_filename,TRUE);
    }
  }
  else
    fprintfilename(file,&config->cloud_filename,TRUE);
  fprintfilename(file,&config->hydrotopes_filename,FALSE);
  fprintf(file,"%s\n",config->co2_filename.name);
  if (config->with_dynamic_ch4)
    fprintf(file,"%s\n",config->ch4_filename.name);
  if(config->with_nitrogen)
  {
    if(config->with_nitrogen!=UNLIM_NITROGEN)
    {
      fprintfilename(file,&config->no3deposition_filename,TRUE);
      fprintfilename(file,&config->nh4deposition_filename,TRUE);
    }
    fprintfilename(file,&config->soilph_filename,FALSE);
  }
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    fprintfilename(file,&config->wind_filename,TRUE);
  if(config->fire==SPITFIRE_TMAX||config->cropsheatfrost)
  {
    fprintfilename(file,&config->tmax_filename,TRUE);
    fprintfilename(file,&config->tmin_filename,TRUE);
  }
  if(config->fire==SPITFIRE)
    fprintfilename(file,&config->tamp_filename,TRUE);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
      fprintfilename(file,&config->humid_filename,TRUE);
    if(config->prescribe_burntarea)
      fprintfilename(file,&config->burntarea_filename,TRUE);
    fprintfilename(file,&config->lightning_filename,FALSE);
    fprintfilename(file,&config->human_ignition_filename,FALSE);
  }
  if(config->ispopulation)
    fprintfilename(file,&config->popdens_filename,TRUE);
  if(config->grassfix_filename.name!=NULL)
    fprintfilename(file,&config->grassfix_filename,FALSE);
  if(config->grassharvest_filename.name!=NULL)
    fprintfilename(file,&config->grassharvest_filename,FALSE);
  if(config->withlanduse!=NO_LANDUSE)
  {
    fprintfilename(file,&config->countrycode_filename,FALSE);
    fprintfilename(file,&config->landuse_filename,TRUE);
    if(config->sdate_option==PRESCRIBED_SDATE)
      fprintfilename(file,&config->sdate_filename,TRUE);
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
      fprintfilename(file,&config->crop_phu_filename,TRUE);
    if(config->with_nitrogen && config->fertilizer_input)
      fprintfilename(file,&config->fertilizer_nr_filename,TRUE);
    if (config->with_nitrogen && config->manure_input)
      fprintfilename(file,&config->manure_nr_filename,TRUE);
    if(config->residue_treatment==READ_RESIDUE_DATA)
      fprintfilename(file,&config->residue_data_filename,TRUE);
    if(config->tillage_type==READ_TILLAGE)
      fprintfilename(file,&config->with_tillage_filename,TRUE);

    if(config->prescribe_lsuha)
      fprintfilename(file,&config->lsuha_filename,FALSE);
  }
  if(config->reservoir)
  {
    fprintfilename(file,&config->elevation_filename,FALSE);
    fprintfilename(file,&config->reservoir_filename,FALSE);
  }
#ifdef IMAGE
  if(config->aquifer_irrig==AQUIFER_IRRIG)
    fprintfilename(file,&config->aquifer_filename,FALSE);
#endif
  if(config->wet_filename.name!=NULL)
    fprintfilename(file,&config->wet_filename,TRUE);
  if(config->with_lakes)
    fprintfilename(file,&config->lakes_filename,FALSE);
  if(config->river_routing)
  {
    fprintfilename(file,&config->drainage_filename,FALSE);
    if(config->withlanduse!=NO_LANDUSE)
      fprintfilename(file,&config->neighb_irrig_filename,FALSE);
  }
  if(config->wateruse)
    fprintfilename(file,&config->wateruse_filename,TRUE);
#ifdef IMAGE
  if (config->wateruse_wd_filename.name != NULL)
    fprintfilename(file,&config->wateruse_wd_filename,TRUE);
#endif
  }
  if(withoutput)
  {
    if(iswriterestart(config))
      fprintf(file,"%s\n",config->write_restart_filename);
    for(i=0;i<config->n_out;i++)
      if(config->outputvars[i].filename.fmt!=SOCK)
      {
        if(config->outputvars[i].oneyear)
          for(j=config->firstyear;j<=config->lastyear;j++)
          {
            fprintf(file,config->outputvars[i].filename.name,j);
            fputc('\n',file);
            if(config->outputvars[i].filename.meta)
            {
              fprintf(file,config->outputvars[i].filename.name,j);
              fprintf(file,"%s\n",config->json_suffix);
            }
          }
        else
        {
          fprintf(file,"%s\n",config->outputvars[i].filename.name);
          if(config->outputvars[i].filename.meta)
            fprintf(file,"%s%s\n",config->outputvars[i].filename.name,config->json_suffix);
        }
     }
  }
} /* of 'fprintfiles' */
