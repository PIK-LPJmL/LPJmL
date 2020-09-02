/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  c  r  u  m  o  n  t  h  l  y  .  j  s                  \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 4.0.002                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "include/conf.h" /* include constant definitions */

"inpath" : "/p/projects/lpjml/input/coupling_image/coupling1970/ddm30",

"input" :
{
  "soil" :         { "fmt" : META, "name" : "soil.descr"},
  "coord" :        { "fmt" : CLM,  "name" : "grid_cru2000_image.clm"},
  "countrycode" :  { "fmt" : CLM,  "name" : "cow_coast_2006_image.bin"},
  "landuse" :      { "fmt" : CLM,  "name" : "landuse4spinup_IM32_C38.bin"},
  /* insert prescribed sdate file name here */
  "lakes" :        { "fmt" : META, "name" : "sealakefrac_image.descr"},
  "drainage" :     { "fmt" : CLM,  "name" : "drainage_image.bin"},
  "neighb_irrig" : { "fmt" : CLM,  "name" : "neighb_irrig_image2.bin"},
  "elevation" :    { "fmt" : CLM,  "name" : "elevation_image.bin"},
  "reservoir" :    { "fmt" : CLM,  "name" : "reservoir_grand_DDM_image.bin"},
  "temp" :         { "fmt" : CLM,  "name" : "temp_image_30ymeans.clm"},
  "prec" :         { "fmt" : CLM,  "name" : "prec_image_30ymeans.clm"},
  "cloud":         { "fmt" : CLM,  "name" : "cld_image_30ymeans.clm"},
  "co2" :          { "fmt" : TXT,  "name" : "co2_2003.dat"},
  "wetdays" :      { "fmt" : CLM,  "name" : "wet_image_30ymeans.clm"},
  "wateruse" :     { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/wateruse_1900_2000.bin" }, 
  "wateruse_wd" :     { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/wateruse_1900_2000.bin" } 
},
