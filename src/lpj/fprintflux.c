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
                const Config *config /**< LPJmL configuration */
               )
{
  int i,tabs;
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
    fputs("\n       ",file);
    frepeatch(file,'-',tabs*8-1);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" ----------------------------------",file);
    else
      fputs(" --------------------------",file);
    if(config->river_routing)
      fputs("-----------",file);
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
    fputs(" transp     evap    interc",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs("  wd",file);
    if(config->river_routing)
      fputs((config->withlanduse!=NO_LANDUSE) ? "      discharge" : "   discharge",file);
    fputc('\n',file);
    fputs("------",file);
    for(i=0;i<tabs;i++)
      fputs(" -------",file);
    fputs(" ---------- ------- -------",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" -------",file);
    if(config->river_routing)
      fputs(" ----------",file);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%6d %7.3f %7.3f",year,flux.nep*1e-15,flux.estab*1e-15);
  if(config->fire)
    fprintf(file," %7.3f",flux.fire*1e-15);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.3f",flux.harvest*1e-15); 
  fprintf(file," %7.3f",cflux_total*1e-15);
  fprintf(file," %10.1f %7.1f %7.1f",
          flux.transp*1e-12,flux.evap*1e-12,flux.interc*1e-12);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.1f",flux.wd*1e-12);
  if(config->river_routing)
    fprintf(file," %10.1f",flux.discharge*1e-12);
  fputc('\n',file);
} /* of 'fprintflux' */
