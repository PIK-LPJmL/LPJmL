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
  char d;
  d=config->csv_delimit;
  if(year==config->firstyear-config->nspinup)
  {
    fprintf(file,"Year%cNEP%cGPP%cNPP%cRH%cestab",d,d,d,d,d);
    if(config->fire)
      fprintf(file,"%cfire",d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%carea%charvest",d,d);
    fprintf(file,"%ctotal%ctransp%cevap%cinterc",d,d,d,d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cwd",d);
    if(config->river_routing)
      fprintf(file,"%cdischarge",d);
    fprintf(file,"%cprec%cSoilC%cSoilC_slow%cLitc%cVegC",d,d,d,d,d);
    if(config->with_nitrogen)
      fprintf(file,"%cnuptake%cndemand%cnlosses%cninflux",d,d,d,d);
    fprintf(file,"\n(AD)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->fire)
      fprintf(file,"%c(%g gC/yr)",d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(m2)%c(%g gC/yr)",d,d,1/scale);
    fprintf(file,"%c(%g gC/yr)%c(%g dm3/yr)%c(%g dm3/yr)%c(%g dm3/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    if(config->river_routing)
      fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    fprintf(file,"%c(%g dm3/yr)%c(%g gC)%c(%g gC)%c(%g gC)%c(%g gC)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->with_nitrogen)
      fprintf(file,"%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%d%c%g%c%g%c%g%c%g%c%g",year,d,(flux.npp-flux.rh)*scale,d,flux.gpp*scale,d,flux.npp*scale,d,flux.rh*scale,d,flux.estab*scale);
  if(config->fire)
    fprintf(file,"%c%g",d,flux.fire*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g%c%g",d,flux.area_agr,d,flux.harvest*scale);
  fprintf(file,"%c%g",d,cflux_total*scale);
  fprintf(file,"%c%g%c%g%c%g",
          d,flux.transp*scale,d,flux.evap*scale,d,flux.interc*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.wd*scale);
  if(config->river_routing)
    fprintf(file,"%c%g",d,flux.discharge*scale);
  fprintf(file,"%c%g",d,flux.prec*scale);
  fprintf(file,"%c%g",d,flux.soilc*scale);
  fprintf(file,"%c%g",d,flux.soilc_slow*scale);
  fprintf(file,"%c%g",d,flux.litc*scale);
  fprintf(file,"%c%g",d,flux.vegc*scale);
  if(config->with_nitrogen)
    fprintf(file,"%c%g%c%g%c%g%c%g",d,flux.n_uptake*scale,d,flux.n_demand*scale,d,flux.n_outflux*scale,
            d,flux.n_influx*scale);
  fputc('\n',file);
  fflush(file);
} /* of 'fprintcsvflux' */
