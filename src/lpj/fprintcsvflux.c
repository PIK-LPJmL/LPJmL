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
  if(config->nall<80)
    scale=1/flux.area;
  else
    scale=1e-15;
  if(year==config->firstyear-config->nspinup)
  {
    fprintf(file,"Year%cNEP%cGPP%cNPP%cRH%cestabc%cnegc_fluxes",d,d,d,d,d,d);
    if(config->fire)
      fprintf(file,"%cfirec",d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%carea%charvestc%cprod_turnoverc",d,d,d);
    fprintf(file,"%cNBP%cNBP_nat%ctransp%cevap%cinterc",d,d,d,d,d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cwd",d);
    if(config->river_routing)
      fprintf(file,"%cdischarge",d);
    if(config->with_methane)
    {
      fprintf(file,"%cCH4 emiss.%cCH4 oxid%cCH4 sink%cCH4 fire",d,d,d,d);
      if(config->withlanduse!=NO_LANDUSE)
        fprintf(file,"%cCH4 rice%cCH4 grass%cCH4 agr",d,d,d);
    }
    fprintf(file,"%cprec%cSoilC%cSoilC_slow%cLitC%cVegC",d,d,d,d,d);
    if(config->with_methane)
      fprintf(file,"%cSoil_CH4",d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cProductC",d);
    fprintf(file,"%cnuptake%cndemand%cnlosses%cninflux%cestabn%cnegn_fluxes",d,d,d,d,d,d);
    if(config->fire)
      fprintf(file,"%cfiren",d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%charvestN%cprod_turnovern",d,d);
    fprintf(file,"%cSoilN%cSoilN_slow%cLitN%cVegN%cSoilNH4%cSoilNO3",d,d,d,d,d,d);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%cProductN",d);
    if(config->withlanduse!=NO_LANDUSE)
    {
      fprintf(file,"%cestab_storageC",d);
      fprintf(file,"%cestab_storageN",d);
    }
    fprintf(file,"\n(AD)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)%c(%g gC/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->fire)
      fprintf(file,"%c(%g gC/yr)",d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(m2)%c(%g gC/yr)%c(%g gC/yr)",d,d,1/scale,d,1/scale);
    fprintf(file,"%c(%g gC/yr)%c(%g gC/yr)%c(%g dm3/yr)%c(%g dm3/yr)%c(%g dm3/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    if(config->river_routing)
      fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    if(config->with_methane)
    {
      fprintf(file,"%c(%g gCH4/yr)%c(%g gCH4/yr)%c(%g gCH4/yr)%c(%g gCH4/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale);
      if(config->withlanduse!=NO_LANDUSE)
        fprintf(file,"%c(%g gCH4/yr)%c(%g gCH4/yr)%c(%g gCH4/yr)",d,1/scale,d,1/scale,d,1/scale);
    }
    fprintf(file,"%c(%g dm3/yr)",d,1/scale);
    fprintf(file,"%c(%g gC)%c(%g gC)%c(%g gC)%c(%g gC)",d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->with_methane)
      fprintf(file,"%c(%g gCH4)",d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g gC)",d,1/scale);
    fprintf(file,"%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)%c(%g gN/yr)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->fire)
      fprintf(file,"%c(%g gN/yr)",d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g gN/yr)%c(%g gN/yr)",d,1/scale,d,1/scale);
    fprintf(file,"%c(%g gN)%c(%g gN)%c(%g gN)%c(%g gN)%c(%g gN)%c(%g gN)",d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale,d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c(%g gN)",d,1/scale);
    if(config->withlanduse!=NO_LANDUSE)
    {
      fprintf(file,"%c(%g gC)",d,1/scale);
      fprintf(file,"%c(%g gN)",d,1/scale);
    }
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%d%c%g%c%g%c%g%c%g%c%g%c%g",year,d,(flux.npp-flux.rh)*scale,d,flux.gpp*scale,d,flux.npp*scale,d,flux.rh*scale,d,flux.estab.carbon*scale,d,flux.neg_fluxes.carbon*scale);
  if(config->fire)
    fprintf(file,"%c%g",d,flux.fire.carbon*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g%c%g%c%g",d,flux.area_agr,d,flux.harvest.carbon*scale,d,flux.product_turnover.carbon*scale);
  fprintf(file,"%c%g%c%g",d,cflux_total*scale,d,flux.nat_nbpflux*scale);
  fprintf(file,"%c%g%c%g%c%g",
          d,flux.transp*scale,d,flux.evap*scale,d,flux.interc*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.wd*scale);
  if(config->river_routing)
    fprintf(file,"%c%g",d,flux.discharge*scale);
  if(config->with_methane)
  {
    fprintf(file,"%c%g",d,flux.CH4_emissions*scale);
    fprintf(file,"%c%g",d,flux.CH4_oxidation*scale);
    fprintf(file,"%c%g",d,flux.CH4_sink*scale);
    fprintf(file,"%c%g",d,flux.CH4_fire*scale);
    if(config->withlanduse!=NO_LANDUSE)
      fprintf(file,"%c%g%c%g%c%g",d,flux.CH4_rice*scale,d,flux.CH4_grassland*scale,d,flux.CH4_agr*scale);
  }
  fprintf(file,"%c%g",d,flux.prec*scale);
  fprintf(file,"%c%g",d,flux.soil.carbon*scale);
  fprintf(file,"%c%g",d,flux.soil_slow.carbon*scale);
  fprintf(file,"%c%g",d,flux.lit.carbon*scale);
  fprintf(file,"%c%g",d,flux.veg.carbon*scale);
  if(config->with_methane)
    fprintf(file,"%c%g",d,flux.soil_CH4*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.product.carbon*scale);
  fprintf(file,"%c%g%c%g%c%g%c%g%c%g%c%g",d,flux.n_uptake*scale,d,flux.n_demand*scale,d,flux.n_outflux*scale,
      d,flux.influx.nitrogen*scale,d,flux.estab.nitrogen*scale,d,flux.neg_fluxes.nitrogen*scale);
  if(config->fire)
    fprintf(file,"%c%g",d,flux.fire.nitrogen*scale);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g%c%g",d,flux.harvest.nitrogen*scale,d,flux.product_turnover.nitrogen*scale);
  fprintf(file,"%c%g",d,flux.soil.nitrogen*scale);
  fprintf(file,"%c%g",d,flux.soil_slow.nitrogen*scale);
  fprintf(file,"%c%g",d,flux.lit.nitrogen*scale);
  fprintf(file,"%c%g",d,flux.veg.nitrogen*scale);
  fprintf(file,"%c%g",d,flux.soil_NH4*scale);
  fprintf(file,"%c%g",d,flux.soil_NO3*scale);

  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,"%c%g",d,flux.product.nitrogen*scale);
  if(config->withlanduse!=NO_LANDUSE)
  {
    fprintf(file,"%c%g",d,flux.estab_storage.carbon*scale);
    fprintf(file,"%c%g",d,flux.estab_storage.nitrogen*scale);
  }
  fputc('\n',file);
  fflush(file);
} /* of 'fprintcsvflux' */
