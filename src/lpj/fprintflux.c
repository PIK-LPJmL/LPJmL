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
    else convert = 1e-9;
  if((year<config->firstyear &&
     (year-config->firstyear+config->nspinup) % LINES_PER_HEADER==0) || 
     (year>=config->firstyear && (year-config->firstyear) % LINES_PER_HEADER==0))
  {
    /* print header */
    tabs=(config->fire) ? 4 : 3;
    if(config->withlanduse!=NO_LANDUSE)
      tabs++;
    fputs("\n       ",file);
    frepeatch(file,' ',(tabs*8-16)/2-1);
    fputs("Carbon flux (GtC)",file);
    frepeatch(file,' ',(tabs*8-16)/2);
    if(config->river_routing)
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 16 : 20);
    else
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 8 : 12);
    fputs("Water (km3)",file);
    if(config->with_nitrogen)
    {
      if(config->river_routing)
        frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 4 : 8);
      fputs("                Nitrogen (TgN)",file);
    }
    fputs("\n       ",file);
    frepeatch(file,'-',tabs*8-1);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" ----------------------------------",file);
    else
      fputs(" --------------------------",file);
    if(config->river_routing)
      fputs("-----------",file);
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
      fputs(" harvest",file);
    fputs(" total  ",file);
    fputs(" transp     evap    interc ",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" wd     ",file);
    if(config->river_routing)
      fputs(" discharge ",file);
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
    if(config->with_nitrogen)
      fputs(" ------- -------- ------- -------",file);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%6d %7.3f %7.3f",year,flux.nep*convert,flux.estab*convert);
  if(config->fire)
    fprintf(file," %7.3f",flux.fire*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.3f",flux.harvest*convert);
  fprintf(file," %7.3f",cflux_total*convert);
  fprintf(file," %10.1f %7.1f %7.1f",
          flux.transp*convert*1000,flux.evap*convert*1000,flux.interc*convert*1000);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.1f",flux.wd*convert*1000);
  if(config->river_routing)
    fprintf(file," %10.1f",flux.discharge*convert*1000);
  if(config->with_nitrogen)
    fprintf(file," %7.1f %8.1f %7.1f %7.1f",flux.n_uptake*convert*1000,flux.n_demand*convert*1000,flux.n_outflux*convert*1000,
    flux.n_influx*convert*1000);
  fputc('\n',file);
} /* of 'fprintflux' */
