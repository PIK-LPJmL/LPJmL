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
  scale=1e-15;
  if(year==config->firstyear-config->nspinup)
  {
    fprintf(file,"Year%cNEP%cGPP%cNPP%cRH%cestab%cnegc_fluxes",d,d,d,d,d,d);
    if(config->fire)
      fprintf(file,"%cfire",d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%carea%charvest%cprod_turnover",d,d,d);
    fprintf(file,"%cNBP%ctransp%cevap%cinterc",d,d,d,d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cwd",d);
    if(config->river_routing)
      fprintf(file,"%cdischarge",d);
    fprintf(file,"%cCH4 emiss.%cCH4 sink%cCH4 fire",d,d,d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cCH4 rice",d);
    fprintf(file,"%cprec%cSoilC%cSoilC_slow%cLitC%cVegC%cSoil_CH4%cSoil_NO3%cSoil_NH4",d,d,d,d,d,d,d,d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cProductC",d);
    if(config->with_nitrogen)
      fprintf(file,"%cnuptake%cndemand%cnlosses%cninflux",d,d,d,d);
    fprintf(file,"\n(AD)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->fire)
      fprintf(file,"%c(%g gC/yr)",d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(m2)%c(%g gC/yr)%c(%g gC/yr)",d,d,1/scale,d,1/scale);
    fprintf(file,"%c(%g gC/yr)%c(%g dm3/yr)%c(%g dm3/yr)%c(%g dm3/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    if(config->river_routing)
      fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    fprintf(file,"%c(%g gCH4/yr)%c(%g gCH4/yr)%c(%g gCH4/yr)",d,1/scale,d,1/scale,d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g gC/yr)",d,1/scale);
    fprintf(file,"%c(%g dm3/yr)%c(%g gC)%c(%g gC)%c(%g gC)%c(%g gC)%c(%g gC)%c(%g gN)%c(%g gN)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g gC/yr)",d,1/scale);
    if(config->with_nitrogen)
      fprintf(file,"%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%d%c%g%c%g%c%g%c%g%c%g%c%g",year,d,(flux.npp-flux.rh)*scale,d,flux.gpp*scale,d,flux.npp*scale,d,flux.rh*scale,d,flux.estab*scale,d,flux.neg_fluxes*scale);
  if(config->fire)
    fprintf(file,"%c%g",d,flux.fire*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g%c%g%c%g",d,flux.area_agr,d,flux.harvest*scale,d,flux.product_turnover*scale);
  fprintf(file,"%c%g",d,cflux_total*scale);
  fprintf(file,"%c%g%c%g%c%g",
          d,flux.transp*scale,d,flux.evap*scale,d,flux.interc*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.wd*scale);
  if(config->river_routing)
    fprintf(file,"%c%g",d,flux.discharge*scale);
  fprintf(file,"%c%g",d,flux.aCH4_emissions*scale);
  fprintf(file,"%c%g",d,flux.aCH4_sink*scale);
  fprintf(file,"%c%g",d,flux.aCH4_fire*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.aCH4_rice*scale);
  fprintf(file,"%c%g",d,flux.prec*scale);
  fprintf(file,"%c%g",d,flux.soilc*scale);
  fprintf(file,"%c%g",d,flux.soilc_slow*scale);
  fprintf(file,"%c%g",d,flux.litc*scale);
  fprintf(file,"%c%g",d,flux.vegc*scale);
  fprintf(file,"%c%g",d,flux.soil_CH4*scale);
  fprintf(file,"%c%g",d,flux.soil_NO3*scale);
  fprintf(file,"%c%g",d,flux.soil_NH4*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.productc*scale);
  if(config->with_nitrogen)
    fprintf(file,"%c%g%c%g%c%g%c%g",d,flux.n_uptake*scale,d,flux.n_demand*scale,d,flux.n_outflux*scale,
            d,flux.n_influx*scale);
  fputc('\n',file);
  fflush(file);
} /* of 'fprintcsvflux' */
