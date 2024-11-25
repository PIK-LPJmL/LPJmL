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
    else convert = 1/flux.area;
  if((year<config->firstyear &&
     (year-config->firstyear+config->nspinup) % LINES_PER_HEADER==0) ||
     (year>=config->firstyear && (year-config->firstyear) % LINES_PER_HEADER==0))
  {
    /* print header */
    tabs=(config->fire) ? 5 : 4;
    if(config->withlanduse!=NO_LANDUSE)
      tabs+=2;
    fputs("\n       ",file);
    frepeatch(file,' ',(tabs*8-17)/2-1);
    fprintf(file,"Carbon flux (%cgC/yr)",(convert==1e-15)  ? 'P' : 'G');
    frepeatch(file,' ',(tabs*8-17)/2);
    if(config->river_routing)
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 13 : 17);
    else
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 8 : 12);
    fprintf(file,"  Water (%sm3)",(convert==1e-15)  ? "k" : "da");
    fprintf(file,"       CH4 fluxes (%cgCH4)",(convert==1e-15)  ? 'T' : 'M');
    fprintf(file,"               Carbon (%cgC)    ",(convert==1e-15)  ? 'P' : 'G');
    frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 5 : 9);
    if(config->river_routing)
      frepeatch(file,' ',(config->withlanduse==NO_LANDUSE) ? 1 : 5);
    fprintf(file,"         Nitrogen (%cgN)            Nitrogen fluxes  %cgN/yr)",(convert==1e-15)  ? 'P' : 'M',(convert==1e-15)  ? 'T' : 'M');
    fputs("\n       ",file);
    frepeatch(file,'-',tabs*8-1);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" -------------------------------",file);
    else
      fputs(" -----------------------",file);
    if(config->river_routing)
      fputs("---------------",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" ---------------------------------------",file);
    else
      fputs(" -----------------------",file);
    fputs(" ----------------------------- -------",file);
    fputs(" -------------------------------------------------------",file);
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
    fputs("transp   evap    interc  ",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs("wd      ",file);
    if(config->river_routing)
      fputs("discharge  ",file);
    fputs("CH4em   CH4sink CH4fire ",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs("CH4rice CH4seta ",file);
    fputs("SoilC     slowSoilC VegC      SoilCH4 ",file);
    fputs("SoilNO3 SoilNH4 SoilN   nuptake ndefici nlosses ninflux ",file);
    fputc('\n',file);
    fputs("------",file);
    for(i=0;i<tabs;i++)
      fputs(" -------",file);
    fputs(" ------- ------- -------",file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" -------",file);
    if(config->river_routing)
      fputs(" ----------",file);
    fputs(" ------- ------- -------", file);
    if(config->withlanduse!=NO_LANDUSE)
      fputs(" ------- -------", file);
    fputs(" --------- --------- --------- -------", file);
    fputs(" ------- ------- ------- ------- ------- ------- -------",file);
    fputc('\n',file);
  }
  /* print data */
  fprintf(file,"%6d %7.2f %7.2f",year,(flux.npp-flux.rh)*convert,flux.estab.carbon*convert);
  if(config->fire)
    fprintf(file," %7.2f",flux.fire.carbon*convert);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.2f %7.2f",flux.harvest.carbon*convert,flux.product_turnover.carbon*convert);
  fprintf(file," %7.2f",cflux_total*convert);
  fprintf(file," %7.2f",flux.npp*convert);
  fprintf(file," %7.1f %7.1f %7.1f",
          flux.transp*convert*1000,flux.evap*convert*1000,flux.interc*convert*1000);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file," %7.1f",flux.wd*convert*1000);
  if(config->river_routing)
    fprintf(file," %10.1f",flux.discharge*convert);
  fprintf(file, " %7.1f", flux.CH4_emissions*convert*1000);
  fprintf(file, " %7.1f", flux.CH4_sink*convert*1000);
  fprintf(file, " %7.1f", flux.CH4_fire*convert*1000);
  if(config->withlanduse!=NO_LANDUSE)
    fprintf(file, " %7.1f %7.1f", flux.CH4_rice*convert*1000,flux.CH4_setaside*convert*1000);

  fprintf(file, " %9.1f", flux.soil.carbon*convert);
  fprintf(file, " %9.1f", flux.soil_slow.carbon*convert);
  fprintf(file, " %9.1f", flux.veg.carbon*convert);
  fprintf(file, " %7.2f", flux.soil_CH4*convert*1000);
  fprintf(file, " %7.2f", flux.soil_NO3*convert);
  fprintf(file, " %7.2f", flux.soil_NH4*convert);
  fprintf(file, " %7.1f", flux.soil.nitrogen*convert);
  fprintf(file," %7.1f %7.1f %7.1f %7.1f",flux.n_uptake*convert*1000,flux.n_demand*convert*1000,flux.n_outflux*convert*1000,
          flux.influx.nitrogen*convert*1000);
  fputc('\n',file);
} /* of 'fprintflux' */
