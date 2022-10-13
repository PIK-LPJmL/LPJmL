/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  f  l  u  x  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints global carbon and water fluxes.                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define LINES_PER_HEADER 25

void fprintflux(FILE *file,          /**< Output file pointer */
                Flux flux,           /**< Carbon and water fluxes */
                Real cflux_total,    /**< Total carbon flux  (gC) */
                int year,            /**< Simulation year (AD) */
                const Config *config /**< LPJ configuration */
               )
{
  int i,tabs;
  Real convert;
  if(config->ngridcell > 2) convert = 1e-15;
    else convert = 1e-3;
  if((year<config->firstyear &&
     (year-config->firstyear+config->nspinup) % LINES_PER_HEADER==0) ||
     (year>=config->firstyear && (year-config->firstyear) % LINES_PER_HEADER==0))
  {
    /* print header */
    tabs=(config->fire) ? 5 : 4;
    if(config->withlanduse!=NO_LANDUSE)
      tabs+=2;
    fputs("\n       ",file);
    frepeatch(file,' ',(tabs*8-16)/2-1);
    fprintf(file,"Carbon flux (%ctC)",(convert==1e-15)  ? 'G' : 'k');
    frepeatch(file,' ',(tabs*8-16)/2);
    if(config->river_routing)
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 16 : 20);
    else
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 8 : 12);
    fprintf(file,"Water (%sm3)",(convert==1e-15)  ? "k" : "da");
    fprintf(file,"          CH4 fluxes (TgCH4)");
      fprintf(file,"              Carbon (%ctC)    ",(convert==1e-15)  ? 'T' : 'k');
    if(config->with_nitrogen)
    {
      if(config->river_routing)
        frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 4 : 8);
      fprintf(file,"                Nitrogen (%cgN)",(convert==1e-15)  ? 'T' : 'M');
    }
    fputs("\n       ",file);
    frepeatch(file,'-',tabs*8-1);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" ----------------------------------",file);
    else
      fputs(" --------------------------",file);
    if(config->river_routing)
      fputs("-----------",file);
    //fputs("--------", file);
    fputs(" -------------------------------- ------------------------------",file);
    if(config->with_nitrogen)
      fputs(" --------------------------------",file);
    fputc('\n',file);
    if(year<config->firstyear)
      fputs("Spinup ",file);
    else
      fputs("Year   ",file);
    fputs("NEP     estab  ",file);
    if(config->fire)
      fputs(" fire   ",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" harvest product",file);
    fputs(" NBP    ",file);
    fputs(" NPP    ",file);
    fputs(" transp     evap    interc ",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" wd     ",file);
    if(config->river_routing)
      fputs(" discharge ",file);
    fputs(" CH4 emiss. CH4 sink   CH4 fire   SoilC     slowSoilC   VegC     Soil CH4  Soil NO3  Soil NH4  ",file);
    if(config->with_nitrogen)
      fputs(" nuptake ndemand  nlosses ninflux",file);
    fputc('\n',file);
    fputs("------",file);
    for(i=0;i<tabs;i++)
      fputs(" -------",file);
    fputs(" ---------- ------- -------",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" -------",file);
    if(config->river_routing)
      fputs(" ----------",file);
    fputs(" ---------- ---------- ---------- --------- ---------- --------- --------- ---------- ---------", file);
    if(config->with_nitrogen)
      fputs(" ------- -------- ------- -------",file);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%6d %7.3f %7.3f",year,(flux.npp-flux.rh)*convert,flux.estab*convert);
  if(config->fire)
    fprintf(file," %7.3f",flux.fire*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.3f %7.3f",flux.harvest*convert,flux.product_turnover*convert);
  fprintf(file," %7.3f",cflux_total*convert);
  fprintf(file," %7.3f",flux.npp*convert);
  fprintf(file," %10.1f %7.1f %7.1f",
          flux.transp*convert*1000,flux.evap*convert*1000,flux.interc*convert*1000);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.1f",flux.wd*convert*1000);
  if(config->river_routing)
    fprintf(file," %10.1f ",flux.discharge*convert);
  fprintf(file, " %9.3f ", flux.aCH4_emissions*convert*1000);
  fprintf(file, " %9.3f ", flux.aCH4_sink*convert*1000);
  fprintf(file, " %9.3f ", flux.aCH4_fire*convert*1000);
  fprintf(file, " %9.3f ", flux.soilc*convert);
  fprintf(file, " %9.3f ", flux.soilc_slow*convert);
  fprintf(file, " %9.3f", flux.vegc*convert);
  fprintf(file, " %9.3f", flux.soil_CH4*convert);
  fprintf(file, " %9.3f", flux.soil_NO3*convert);
  fprintf(file, " %9.3f", flux.soil_NH4*convert);
  if(config->with_nitrogen)
    fprintf(file," %7.1f %8.1f %7.1f %7.1f",flux.n_uptake*convert*1000,flux.n_demand*convert*1000,flux.n_outflux*convert*1000,
    flux.n_influx*convert*1000);
  fputc('\n',file);
} /* of 'fprintflux' */
