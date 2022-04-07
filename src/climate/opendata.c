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
  Header header;
  String headername;
  int version;
  size_t offset,filesize;
  file->fmt=filename->fmt;
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
    if((file->file=openinputfile(&header,&file->swap,
                                 filename,headername,
                                 &version,&offset,TRUE,config))==NULL)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open %s data file.\n",name);
      return TRUE;
    }
    file->isopen=TRUE;
    if(file->fmt==RAW)
    {
      header.nbands=nbands;
      file->datatype=datatype;
      file->offset=config->startgrid*header.nbands*typesizes[datatype];
    }
    else
    {
      file->datatype=(version<=2) ? datatype : header.datatype;
      file->offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[file->datatype]+headersize(headername,version)+offset;
      if(isroot(*config) && filename->fmt!=META)
      {
        filesize=getfilesizep(file->file)-headersize(headername,version)-offset;
        if(filesize!=typesizes[file->datatype]*header.nyear*header.nbands*header.ncell*header.nstep)
          fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",filename->name);
      }
    }
    file->firstyear=header.firstyear;
    file->nyear=header.nyear;
    file->size=header.ncell*header.nbands*typesizes[file->datatype];
    file->n=config->ngridcell*header.nbands;
    file->var_len=header.nbands;
    file->scalar=(version<=1) ? scalar : header.scalar;
    if(header.nstep!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid number of steps=%d in %s data file '%s', must be 1.\n",
                header.nstep,name,filename->name);
      closeclimatefile(file,isroot(*config));
      return TRUE;
    }
    if(header.timestep!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid time step=%d in %s data file '%s', must be 1.\n",
                header.timestep,name,filename->name);
      closeclimatefile(file,isroot(*config));
      return TRUE;
    }
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
