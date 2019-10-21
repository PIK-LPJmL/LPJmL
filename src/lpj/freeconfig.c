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

static void freefilename(Filename filename)
{
  free(filename.name);
  free(filename.var);
} /* of 'freefilename' */

void freeconfig(Config *config /**< LPJmL configuration */
               )
{
  int i;
  if(config->soil_filename.fmt!=CDF)
    freefilename(config->coord_filename);
  freefilename(config->soil_filename);
  free(config->outputdir);
  free(config->inputdir);
  free(config->restartdir);
  free(config->arglist);
  free(config->sim_name);
  if(config->river_routing)
  {
    freefilename(config->drainage_filename);
    if(config->drainage_filename.fmt==CDF)
      freefilename(config->river_filename);
    freefilename(config->lakes_filename);
    if(config->withlanduse!=NO_LANDUSE)
    {
      freefilename(config->neighb_irrig_filename);
      if(config->reservoir)
      {
        pnet_free(config->irrig_res);
        pnet_free(config->irrig_res_back);
        freefilename(config->elevation_filename);
        freefilename(config->reservoir_filename);
      }
      pnet_free(config->irrig_neighbour);
      pnet_free(config->irrig_back);
    }
    pnet_free(config->route);
  }
  if(config->wateruse)
    freefilename(config->wateruse_filename);
  freefilename(config->temp_filename);
  freefilename(config->prec_filename);
  if(config->with_radiation)
  {
    if(config->with_radiation!=RADIATION_SWONLY)
      freefilename(config->lwnet_filename);
    freefilename(config->swdown_filename);
  }
  else
    freefilename(config->cloud_filename);
  freefilename(config->co2_filename);
  if(config->wet_filename.name!=NULL)
    freefilename(config->wet_filename);
  for(i=0;i<config->n_out;i++)
    free(config->outputvars[i].filename.name);
  free(config->outputvars);
  free(config->restart_filename);
  free(config->checkpoint_restart_filename);
  free(config->write_restart_filename);
  freepftpar(config->pftpar,ivec_sum(config->npft,config->ntypes));
  freesoilpar(config->soilpar,config->nsoil);
  free(config->npft);
  if(config->ispopulation)
    freefilename(config->popdens_filename);
  if(config->grassfix_filename.name!=NULL)
    freefilename(config->grassfix_filename);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
      freefilename(config->humid_filename);
    freefilename(config->wind_filename);
    freefilename(config->tamp_filename);
    if(config->tamp_filename.fmt==CDF && config->tmax_filename.name!=NULL)
      freefilename(config->tmax_filename);
    freefilename(config->lightning_filename);
    freefilename(config->human_ignition_filename);
    if(config->prescribe_burntarea)
      freefilename(config->burntarea_filename);
  }
  if(config->withlanduse!=NO_LANDUSE)
  {
    freecountrypar(config->countrypar,config->ncountries);
    freeregionpar(config->regionpar,config->nregions);
    freefilename(config->landuse_filename);
    freefilename(config->countrycode_filename);
    if(config->countrycode_filename.fmt==CDF)
      freefilename(config->regioncode_filename);
  }
  
  if(config->prescribe_landcover != NO_LANDCOVER)
    freefilename(config->landcover_filename);
    
  freeoutputvar(config->outnames,NOUT);
} /* of 'freeconfig' */
