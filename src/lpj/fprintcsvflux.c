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
    fputs(",SoilC,Litc,VegC",file);
    if(config->with_nitrogen)
      fputs(",nuptake,ndemand,nlosses,ninflux",file);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%d,%g,%g",year,flux.nep*convert,flux.estab*convert);
  if(config->fire)
    fprintf(file,",%g",flux.fire*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%g",flux.harvest*convert);
  fprintf(file,",%g",cflux_total*convert);
  fprintf(file,",%g", flux.npp*convert);
  fprintf(file,",%g,%g,%g",
          flux.transp*convert,flux.evap*convert,flux.interc*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file,",%g",flux.wd*convert);
  if(config->river_routing)
    fprintf(file,",%g",flux.discharge*convert);
  fprintf(file,",%g", flux.soilc*convert);
  fprintf(file,",%g", flux.litc*convert);
  fprintf(file,",%g", flux.vegc*convert);
  if(config->with_nitrogen)
    fprintf(file,",%g,%g,%g,%g",flux.n_uptake*convert*1000,flux.n_demand*convert*1000,flux.n_outflux*convert*1000,
    flux.n_influx*convert);
  fputc('\n',file);
} /* of 'fprintcsvflux' */
