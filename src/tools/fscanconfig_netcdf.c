/**************************************************************************************/
/**                                                                                \n**/
/**              f  s  c  a  n  c  o  n  f  i  g  _  n  e  t  c  d  f  .  c        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads missing values and axis from a LPJ file                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}

static Bool fscanaxis(LPJfile *file,      /**< pointer to LPJ file */
                      Axis *axis,         /**< axis definition */
                      const char *key,    /**< name of axis definition */
                      Verbosity verb      /**< verbosity level (NO_ERR,ERR,VERB) */
                     )                    /** \return TRUE on error */
{
  const char *s;
  LPJfile *d;
  d=fscanstruct(file,key,verb);
  if(d==NULL)
    return TRUE;
  s=fscanstring(d,NULL,"name",verb);
  if(s==NULL)
    return TRUE;
  axis->name=strdup(s);
  checkptr(axis->name);
  s=fscanstring(d,NULL,"dim",verb);
  if(s==NULL)
    return TRUE;
  axis->dim=strdup(s);
  checkptr(axis->dim);
  s=fscanstring(d,NULL,"standard_name",verb);
  if(s==NULL)
    return TRUE;
  axis->standard_name=strdup(s);
  checkptr(axis->standard_name);
  s=fscanstring(d,NULL,"long_name",verb);
  if(s==NULL)
    return TRUE;
  axis->long_name=strdup(s);
  checkptr(axis->long_name);
  s=fscanstring(d,NULL,"unit",verb);
  if(s==NULL)
    return TRUE;
  axis->unit=strdup(s);
  checkptr(axis->unit);
  if(fscandouble(d,&axis->scale,"scale",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscanaxis' */

static void freeaxis(Axis *axis)
{
  free(axis->name);
  free(axis->dim);
  free(axis->long_name);
  free(axis->standard_name);
  free(axis->unit);
} /* of 'freeaxis' */

Bool fscanconfig_netcdf(LPJfile *file,   /**< pointer to LPJ file */
                       Netcdf_config *config,
                       const char *key, /**< name of NetCDF configuration  or NULL*/
                       Verbosity verb   /**< verbosity level (NO_ERR,ERR,VERB) */
                      )                 /** \return TRUE on error */
{
  LPJfile *d,*m;
  const char *s;
  int i;
  if(key==NULL)
    d=file;
  else
    d=fscanstruct(file,key,verb);
  if(d==NULL)
    return TRUE;
  m=fscanstruct(d,"missing_value",verb);
  if(m==NULL)
    return TRUE;
  if(fscanfloat(m,&config->missing_value.f,"float",FALSE,verb))
    return TRUE;
  if(fscanint(m,&config->missing_value.i,"int",FALSE,verb))
    return TRUE;
  if(fscanint(m,&i,"short",FALSE,verb))
    return TRUE;
  config->missing_value.s=(short)i;
  if(fscanint(m,&i,"byte",FALSE,verb))
    return TRUE;
  config->missing_value.b=(Byte)i;
  if(fscanaxis(d,&config->lat,"latitude",verb))
    return TRUE;
  if(fscanaxis(d,&config->lat_bnds,"latitude_bnds",verb))
    return TRUE;
  if(fscanaxis(d,&config->lon,"longitude",verb))
    return TRUE;
  if(fscanaxis(d,&config->lon_bnds,"longitude_bnds",verb))
    return TRUE;
  if(fscanaxis(d,&config->time,"time",verb))
    return TRUE;
  if(fscanaxis(d,&config->time_bnds,"time_bnds",verb))
    return TRUE;
  if(fscanaxis(d,&config->depth,"depth",verb))
    return TRUE;
  if(fscanaxis(d,&config->depth_bnds,"depth_bnds",verb))
    return TRUE;
  if(fscanaxis(d,&config->pft,"pft",verb))
    return TRUE;
  if(fscanaxis(d,&config->pft_name,"pft_name",verb))
    return TRUE;
  s=fscanstring(d,NULL,"bnds_name",verb);
  if(s==NULL)
    return TRUE;
  config->bnds_name=strdup(s);
  checkptr(config->bnds_name);
  s=fscanstring(d,NULL,"calendar",verb);
  if(s==NULL)
    return TRUE;
  config->calendar=strdup(s);
  checkptr(config->calendar);
  s=fscanstring(d,NULL,"years",verb);
  if(s==NULL)
    return TRUE;
  config->years_name=strdup(s);
  checkptr(config->years_name);
  return FALSE;
} /* of 'fscanconfig_netcdf' */

void freeconfig_netcdf(Netcdf_config *config)
{
  freeaxis(&config->lat);
  freeaxis(&config->lat_bnds);
  freeaxis(&config->lon);
  freeaxis(&config->lon_bnds);
  freeaxis(&config->time);
  freeaxis(&config->time_bnds);
  freeaxis(&config->depth);
  freeaxis(&config->depth_bnds);
  freeaxis(&config->pft);
  freeaxis(&config->pft_name);
  free(config->bnds_name);
  free(config->calendar);
  free(config->years_name);
} /* of 'freenconfig_netcdf' */
