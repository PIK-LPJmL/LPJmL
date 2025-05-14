/**************************************************************************************/
/**                                                                                \n**/
/**                   f  r  e  e  c  o  n  f  i  g  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates memory for LPJ configuration                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freefilename(Filename *filename)
{
  free(filename->name);
  free(filename->var);
  free(filename->map);
  free(filename->unit);
  free(filename->time);
} /* of 'freefilename' */

void freeconfig(Config *config /**< LPJmL configuration */
               )
{
  int i;
  free(config->cmd);
  free(config->json_filename);
  free(config->coupled_model);
  free(config->compress_cmd);
  free(config->compress_suffix);
  if(config->soil_filename.fmt!=CDF)
    freefilename(&config->coord_filename);
  if(config->landfrac_from_file)
    freefilename(&config->landfrac_filename);
  freefilename(&config->soil_filename);
  free(config->outputdir);
  free(config->inputdir);
  free(config->restartdir);
  free(config->arglist);
  free(config->sim_name);
  free(config->pft_index);
  free(config->layer_index);
  free(config->json_suffix);
  freeattrs(config->global_attrs,config->n_global);
  if(config->with_lakes)
    freefilename(&config->lakes_filename);
  if(config->river_routing)
  {
    freefilename(&config->drainage_filename);
    if(config->drainage_filename.fmt==CDF)
      freefilename(&config->river_filename);
    if(config->extflow)
      freefilename(&config->extflow_filename);
    if(config->withlanduse!=NO_LANDUSE)
    {
      freefilename(&config->neighb_irrig_filename);
      if(config->reservoir)
      {
        pnet_free(config->irrig_res);
        pnet_free(config->irrig_res_back);
        freefilename(&config->elevation_filename);
        freefilename(&config->reservoir_filename);
        if(config->reservoir_filename.fmt==CDF)
        {
          freefilename(&config->capacity_reservoir_filename);
          freefilename(&config->area_reservoir_filename);
          freefilename(&config->inst_cap_reservoir_filename);
          freefilename(&config->height_reservoir_filename);
          freefilename(&config->purpose_reservoir_filename);
        }
      }
      pnet_free(config->irrig_neighbour);
      pnet_free(config->irrig_back);
    }
    pnet_free(config->route);
  }
  if(config->wateruse)
    freefilename(&config->wateruse_filename);
#ifdef IMAGE
  if (config->wateruse_wd_filename.name != NULL)
    freefilename(&config->wateruse_wd_filename);
#endif
#if !defined IMAGE || !defined COUPLED
  free(config->coupled_host);
#endif
  freefilename(&config->temp_filename);
  freefilename(&config->prec_filename);
  if(config->with_radiation)
  {
    if(config->with_radiation!=RADIATION_SWONLY)
      freefilename(&config->lwnet_filename);
    freefilename(&config->swdown_filename);
  }
  else
    freefilename(&config->cloud_filename);
  freefilename(&config->co2_filename);
  if(config->wet_filename.name!=NULL)
    freefilename(&config->wet_filename);
  for(i=0;i<config->n_out;i++)
    freefilename(&config->outputvars[i].filename);
  free(config->outputvars);
  free(config->restart_filename);
  free(config->checkpoint_restart_filename);
  free(config->write_restart_filename);
  free(config->cult_types);
  free(config->pfttypes);
  freepftpar(config->pftpar,ivec_sum(config->npft,config->ntypes));
  freesoilpar(config->soilpar,config->nsoil);
  free(config->landcovermap);
  free(config->soilmap);
  free(config->npft);
  if(config->ispopulation)
    freefilename(&config->popdens_filename);
  if(config->grassharvest_filename.name!=NULL)
    freefilename(&config->grassharvest_filename);
  freefilename(&config->wind_filename);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
      freefilename(&config->humid_filename);
    freefilename(&config->lightning_filename);
    freefilename(&config->human_ignition_filename);
    if(config->prescribe_burntarea)
      freefilename(&config->burntarea_filename);
  }
  if(config->fire==SPITFIRE_TMAX)
  {
    freefilename(&config->tmin_filename);
    freefilename(&config->tmax_filename);
  }
  if(config->fire==SPITFIRE)
    freefilename(&config->tamp_filename);
  if(config->withlanduse!=NO_LANDUSE)
  {
    free(config->landusemap);
     if(config->fertilizer_input==FERTILIZER || config->residue_treatment==READ_RESIDUE_DATA || config->tillage_type==READ_TILLAGE)
      free(config->fertilizermap);
    free(config->mowingdays);
    if(config->sdate_option>=PRESCRIBED_SDATE || config->crop_phu_option>=PRESCRIBED_CROP_PHU)
      free(config->cftmap);
    if(config->tillage_type==READ_TILLAGE)
      freefilename(&config->with_tillage_filename);
    freecountrypar(config->countrypar,config->ncountries);
    freefilename(&config->landuse_filename);
    freefilename(&config->countrycode_filename);
    if(config->iscotton)
    {
      freefilename(&config->sowing_cotton_rf_filename);
      freefilename(&config->harvest_cotton_rf_filename);
      freefilename(&config->sowing_cotton_ir_filename);
      freefilename(&config->harvest_cotton_ir_filename);
    }
    if(config->sdate_option>=PRESCRIBED_SDATE)
      freefilename(&config->sdate_filename);
    if(config->prescribe_lsuha)
      freefilename(&config->lsuha_filename);
    if (config->residue_treatment == READ_RESIDUE_DATA)
      freefilename(&config->residue_data_filename);
    if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
      freefilename(&config->crop_phu_filename);
  }
  if(!config->unlim_nitrogen && !config->no_ndeposition)
  {
    freefilename(&config->no3deposition_filename);
    freefilename(&config->nh4deposition_filename);
  }
  freefilename(&config->soilph_filename);
  if(config->withlanduse!=NO_LANDUSE && config->fertilizer_input==FERTILIZER)
    freefilename(&config->fertilizer_nr_filename);
  if(config->withlanduse!=NO_LANDUSE&&config->manure_input)
    freefilename(&config->manure_nr_filename);
  if(config->prescribe_landcover != NO_LANDCOVER)
    freefilename(&config->landcover_filename);

  freeoutputvar(config->outnames,NOUT);
} /* of 'freeconfig' */
