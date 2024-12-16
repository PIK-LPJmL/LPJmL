/**************************************************************************************/
/**                                                                                \n**/
/**                  o  p  e  n  i  n  p  u  t  d  a  t  a  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens static input data file                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool openinputdata(Infile *file,             /**< pointer to file */
                   const Filename *filename, /**< filename */
                   const char *name,         /**< name of data */
                   const char *unit,         /**< unit or NULL */
                   Type datatype,            /**< datatype for version 2 files */
                   Real scalar,              /**< scalar for version 1 files */
                   int size,                 /**< size of input vector or 0 for scalar */
                   const Config *config      /**< LPJ configuration */
                  )                          /** \return TRUE on error */
{
  char *realname;
  Header header;
  String headername;
  int version;
  size_t offset,filesize;
  file->fmt=filename->fmt;
  if(file->fmt==CDF)
  {
    file->cdf=openinput_netcdf(filename,unit,size,config);
    if(file->cdf==NULL)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open %s data file.\n",name);
      return TRUE;
    }
  }
  else
  {
    if((file->file=openinputfile(&header,&file->swap,
                                 filename,headername,unit,datatype,
                                 &version,&offset,FALSE,config))==NULL)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open %s data file.\n",name);
      return TRUE;
    }
    if(file->fmt==RAW)
    {
      header.nbands=1;
      header.nyear=1;
      file->type=datatype;
      offset=config->startgrid*header.nbands*typesizes[datatype];
    }
    else
    {
      file->nbands=(size==0) ? 1 : size;
      file->type=header.datatype;
      if(isroot(*config) && filename->fmt!=META)
      {
        filesize=getfilesizep(file->file)-headersize(headername,version)-offset;
        if(filesize!=typesizes[file->type]*header.nyear*header.nbands*header.ncell*header.nstep)
          fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",filename->name);
      }
      offset+=(config->startgrid-header.firstcell)*header.nbands*typesizes[file->type]+headersize(headername,version);
    }
    file->scalar=(version<=1) ? scalar : header.scalar;
    if(header.nstep!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid number of steps=%d in %s data file '%s', must be 1.\n",
                header.nstep,name,filename->name);
      closeinput(file);
      return TRUE;
    }
    if(header.nyear!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"WARNING038: Number of years=%d in %s data file '%s' greater than 1, only first year read.\n",
                header.nyear,name,filename->name);
    }
    if(header.nbands!=file->nbands)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid number of bands=%d in %s data file '%s', must be %d.\n",
                header.nbands,name,filename->name,file->nbands);
      closeinput(file);
      return TRUE;
    }
    if(fseek(file->file,offset,SEEK_SET))
    {
      realname=getrealfilename(filename);
      fprintf(stderr,"ERROR108: Cannot seek in %s file '%s' to position %d.\n",
              name,realname,config->startgrid);
      free(realname);
      closeinput(file);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'openinputdata' */
