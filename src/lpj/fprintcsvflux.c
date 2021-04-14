/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  c  s  v  f  l  u  x  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints comma separated global carbon and water fluxes.            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintcsvflux(FILE *file,          /**< Output file pointer */
                   Flux flux,           /**< Carbon and water fluxes */
                   Real cflux_total,    /**< Total carbon flux  (gC) */
                   Real scale,          /**< scaling factor */
                   int year,            /**< Simulation year (AD) */
                   const Config *config /**< LPJ configuration */
                  )
{
  if(year==config->firstyear-config->nspinup)
  {
    fprintf(file,"Year,NEP(%ggC/yr),GPP(%gC/yr),NPP(%gC/yr),RH(%gC/yr),estab(%ggC/yr)",1/scale,1/scale,1/scale,1/scale,1/scale);
    if(config->fire)
      fprintf(file,",fire(%ggC/yr)",1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,",harvest(%ggC/yr)",1/scale);
    fprintf(file,",total(%ggC/yr),transp(%gdm3/yr),evap(%gdm3/yr),interc(%gdm3/yr)",1/scale,1/scale,1/scale,1/scale,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,",wd(%gdm3/yr)",1/scale);
    if(config->river_routing)
      fprintf(file,",discharge(%gdm3/yr)",1/scale);
    fprintf(file,",prec(%gdm3/yr),SoilC(%ggC),Litc(%ggC),VegC(%ggC)",1/scale,1/scale,1/scale,1/scale);
    if(config->with_nitrogen)
      fprintf(file,",nuptake(%ggN/yr),ndemand(%ggN/yr),nlosses(%ggN/yr),ninflux(%ggN/yr)",1/scale,1/scale,1/scale,1/scale);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%d,%g,%g,%g,%g,%g",year,(flux.npp-flux.rh)*scale,flux.gpp*scale,flux.npp*scale,flux.rh*scale,flux.estab*scale);
  if(config->fire)
    fprintf(file,",%g",flux.fire*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%g",flux.harvest*scale);
  fprintf(file,",%g",cflux_total*scale);
  fprintf(file,",%g,%g,%g",
          flux.transp*scale,flux.evap*scale,flux.interc*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%g",flux.wd*scale);
  if(config->river_routing)
    fprintf(file,",%g",flux.discharge*scale);
  fprintf(file,",%g",flux.prec*scale);
  fprintf(file,",%g", flux.soilc*scale);
  fprintf(file,",%g", flux.litc*scale);
  fprintf(file,",%g", flux.vegc*scale);
  if(config->with_nitrogen)
    fprintf(file,",%g,%g,%g,%g",flux.n_uptake*scale,flux.n_demand*scale,flux.n_outflux*scale,
            flux.n_influx*scale);
  fputc('\n',file);
  fflush(file);
} /* of 'fprintcsvflux' */
