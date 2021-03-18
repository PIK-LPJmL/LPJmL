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
                   int year,            /**< Simulation year (AD) */
                   const Config *config /**< LPJ configuration */
                  )
{
  Real convert;
  if(config->ngridcell > 2) convert = 1e-15;
     else convert = 1e-9;
  if(year==config->firstyear-config->nspinup)
  {
    fputs("Year,NEP,estab",file);
    if(config->fire)
      fputs(",fire",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(",harvest",file);
    fputs(",total,NPP,transp,evap,interc",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(",wd",file);
    if(config->river_routing)
      fputs(",discharge",file);
    fputs(",CH4 emiss.,CH4 sink,CH4 fire,SoilC,slowSoilC,VegC",file);
    if(config->with_nitrogen)
      fputs(",nuptake,ndemand,nlosses,ninflux",file);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%6d,%7.3f,%7.3f",year,flux.nep*convert,flux.estab*convert);
  if(config->fire)
    fprintf(file,",%7.3f",flux.fire*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%7.3f",flux.harvest*convert);
  fprintf(file,",%7.3f",cflux_total*convert);
  fprintf(file,",%7.3f", flux.anpp*convert);
  fprintf(file,",%10.1f,%7.1f,%7.1f",
          flux.transp*convert,flux.evap*convert,flux.interc*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%7.1f",flux.wd*convert);
  if(config->river_routing)
    fprintf(file,",%10.1f",flux.discharge*convert);
  fprintf(file, ",%9.3f", flux.aCH4_emissions*convert*1000);
  fprintf(file, ",%9.3f", flux.aCH4_sink*convert*1000);
  fprintf(file, ",%9.3f", flux.aCH4_fire*convert*1000);
  fprintf(file, ",%9.3f", flux.soilc*convert);
  fprintf(file, ",%9.3f", flux.soilc_slow*convert);
  fprintf(file, ",%9.3f", flux.vegc*convert);
  if(config->with_nitrogen)
    fprintf(file,",%7.1f,%8.1f,%7.1f,%7.1f",flux.n_uptake*convert*1000,flux.n_demand*convert*1000,flux.n_outflux*convert*1000,
    flux.n_influx*convert);
  fputc('\n',file);
} /* of 'fprintcsvflux' */
