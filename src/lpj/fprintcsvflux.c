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
    fprintf(file,"Year,NEP,GPP,NPP,RH,estab");
    if(config->fire)
      fprintf(file,",fire");
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,",area,harvest");
    fprintf(file,",total,transp,evap,interc");
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,",wd");
    if(config->river_routing)
      fprintf(file,",discharge");
    fprintf(file,",prec,SoilC,Litc,VegC");
    fprintf(file,"\n(AD),(%g gC/yr),(%g gC/yr),(%g gC/yr),(%g gC/yr),(%g gC/yr)",1/scale,1/scale,1/scale,1/scale,1/scale);
    if(config->fire)
      fprintf(file,",(%g gC/yr)",1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,",(m2),(%g gC/yr)",1/scale);
    fprintf(file,",(%g gC/yr),(%g dm3/yr),(%g dm3/yr),(%g dm3/yr)",1/scale,1/scale,1/scale,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,",(%g dm3/yr)",1/scale);
    if(config->river_routing)
      fprintf(file,",(%g dm3/yr)",1/scale);
    fprintf(file,",(%g dm3/yr),(%g gC),(%g gC),(%g gC)",1/scale,1/scale,1/scale,1/scale);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%d,%g,%g,%g,%g,%g",year,(flux.npp-flux.rh)*scale,flux.gpp*scale,flux.npp*scale,flux.rh*scale,flux.estab*scale);
  if(config->fire)
    fprintf(file,",%g",flux.fire*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%g,%g",flux.area_agr,flux.harvest*scale);
  fprintf(file,",%g",cflux_total*scale);
  fprintf(file,",%g,%g,%g",
          flux.transp*scale,flux.evap*scale,flux.interc*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%g",flux.wd*scale);
  if(config->river_routing)
    fprintf(file,",%g",flux.discharge*scale);
  fprintf(file,",%g",flux.prec*scale);
  fprintf(file,",%g",flux.soilc*scale);
  fprintf(file,",%g",flux.litc*scale);
  fprintf(file,",%g",flux.vegc*scale);
  fputc('\n',file);
  fflush(file);
} /* of 'fprintcsvflux' */
