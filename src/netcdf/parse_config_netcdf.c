/**************************************************************************************/
/**                                                                                \n**/
/**     p  a  r  s  e  _  c  o  n  f  i  g  _  n  e  t  c  d  f  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function parses missing values and axis names from JSON file               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool parse_config_netcdf(Netcdf_config *nc_config, /**< NetCDF settings */
                         const char *filename      /**< filename of JSON file */
                        )                          /** \return TRUE on error */
{
  FILE *file;
  LPJfile *lpjfile;
  Bool rc;
  /* open JSON file */
  if((file=fopen(filename,"r"))==NULL)
  {
    printfopenerr(filename);
    return TRUE;
  }
  initscan(filename);
  lpjfile=parse_json(file,ERR);
  if(lpjfile==NULL)
  {
    fclose(file);
    return TRUE;
  }
  rc=fscanconfig_netcdf(lpjfile,nc_config,NULL,ERR);
  closeconfig(lpjfile);
  fclose(file);
  return rc;
} /* of 'parse_config_netcdf' */
