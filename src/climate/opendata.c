/**************************************************************************************/
/**                                                                                \n**/
/**                           o  p  e  n  d  a  t  a  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens data file                                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool opendata(Climatefile *file,        /**< pointer to file */
              const Filename *filename, /**< filename */
              const char *name,         /**< name of data */
              const char *unit,         /**< unit or NULL */
              Type datatype,            /**< datatype for version 2 files */
              Real scalar,              /**< scalar for version 1 files */
              int nbands,               /**< number of bands */
              Bool ischeck,             /**< check number of bands (TRUE/FALSE) */
              const Config *config      /**< LPJ configuration */
             )                          /** \return TRUE on error */
{
  int nbands_socket;
  file->fmt=filename->fmt;
  if(file->fmt==SOCK)
  {
    if(openinput_coupler(filename->id,datatype,config->nall,&nbands_socket,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Cannot initialize %s data socket stream.\n",name);
      return TRUE;
    }
    file->id=filename->id;
    file->var_len=nbands_socket;
    file->datatype=datatype;
    if(ischeck && file->var_len!=nbands)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid number of bands=%zu in %s socket stream, must be %d.\n",
                file->var_len,name,nbands);
      return TRUE;
    }
    return FALSE;
  }
  if(file->fmt==CDF)
  {
    if(opendata_netcdf(file,filename,unit,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open %s data file.\n",name);
      return TRUE;
    }
  }
  else
  {
    if(openclmdata(file,filename,name,unit,datatype,scalar,nbands,config))
      return TRUE;
  }
  if(ischeck && file->var_len!=nbands)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR147: Invalid number of bands=%zu in %s data file '%s', must be %d.\n",
             file->var_len,name,filename->name,nbands);
    closeclimatefile(file,isroot(*config));
    return TRUE;
  }
  return FALSE;
} /* of 'opendata' */
