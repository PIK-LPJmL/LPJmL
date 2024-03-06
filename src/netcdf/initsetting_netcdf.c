/**************************************************************************************/
/**                                                                                \n**/
/**           i  n  i  t  s  e  t  t  i  n  g  _  n  e  t  c  d  f  .  c           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes axis names and missing values to default values       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define MISSING_VALUE_FLOAT -1e32
#define MISSING_VALUE_SHORT -9999
#define MISSING_VALUE_INT -999999
#define MISSING_VALUE_BYTE 99
#define LON_NAME "lon"
#define LON_STANDARD_NAME "longitude"
#define LON_LONG_NAME "Longitude"
#define LON_UNITS "degrees_east"
#define LAT_NAME "lat"
#define LAT_LONG_NAME "Latitude"
#define LAT_STANDARD_NAME "latitude"
#define LAT_UNITS "degrees_north"
#define YEARS_NAME "Years"
#define TIME_NAME "time"
#define TIME_STANDARD_NAME "time"
#define TIME_LONG_NAME "Time"
#define LON_DIM_NAME "lon"
#define LAT_DIM_NAME "lat"
#define TIME_DIM_NAME "time"
#define DEPTH_NAME "depth"
#define DEPTH_DIM_NAME "layer"
#define DEPTH_STANDARD_NAME "depth_below_surface"
#define DEPTH_LONG_NAME "Depth of Vertical Layer Center Below Surface"
#define BNDS_NAME "depth_bnds"
#define BNDS_DIM_NAME "depth_bnds"
#define BNDS_LONG_NAME "bnds=0 for the top of the layer, and bnds=1 for the bottom of the layer"
#define BNDS_STANDARD_NAME ""
#define PFT_NAME "NamePFT"
#define PFT_DIM_NAME "npft"
#define PFT_LONG_NAME "Name of plant functional types"
#define PFT_STANDARD_NAME ""
#define CALENDAR "noleap"

void initsetting_netcdf(Netcdf_config *config)
{
  config->missing_value.f=MISSING_VALUE_FLOAT;
  config->missing_value.s=MISSING_VALUE_SHORT;
  config->missing_value.i=MISSING_VALUE_INT;
  config->missing_value.b=MISSING_VALUE_BYTE;
  config->time.name=TIME_NAME;
  config->time.dim=TIME_DIM_NAME;
  config->time.standard_name=TIME_STANDARD_NAME;
  config->time.long_name=TIME_LONG_NAME;
  config->time.unit="";
  config->lon.name=LON_NAME;
  config->lon.dim=LON_DIM_NAME;
  config->lon.standard_name=LON_STANDARD_NAME;
  config->lon.long_name=LON_LONG_NAME;
  config->lon.unit=LON_UNITS;
  config->lat.name=LAT_NAME;
  config->lat.dim=LAT_DIM_NAME;
  config->lat.standard_name=LAT_STANDARD_NAME;
  config->lat.long_name=LAT_LONG_NAME;
  config->lat.unit=LAT_UNITS;
  config->depth.name=DEPTH_NAME;
  config->depth.dim=DEPTH_DIM_NAME;
  config->depth.standard_name=DEPTH_STANDARD_NAME;
  config->depth.long_name=DEPTH_LONG_NAME;
  config->depth.unit="m";
  config->depth.scale=1e-3;
  config->bnds.name=BNDS_NAME;
  config->bnds.dim=BNDS_DIM_NAME;
  config->bnds.standard_name=BNDS_STANDARD_NAME;
  config->bnds.long_name=BNDS_LONG_NAME;
  config->bnds.unit="m";
  config->bnds.scale=1e-3;
  config->pft.name=PFT_NAME;
  config->pft.dim=PFT_DIM_NAME;
  config->pft.standard_name=PFT_STANDARD_NAME;
  config->pft.long_name=PFT_LONG_NAME;
  config->pft.unit="";
  config->years_name=YEARS_NAME;
  config->calendar=CALENDAR;
} /* of 'initsetting_netcdf' */
