/**************************************************************************************/
/**                                                                                \n**/
/**              f  o  p  e  n  s  o  i  l  c  o  d  e  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens soil code file                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

FILE *fopensoilcode(const Filename *filename, /**< filename  of soil code file */
                    Map **map,               /**< soilmap array or NULL */
                    Bool *swap,           /**< byte order has to be changed */
                    size_t *offset,       /**< offset in binary file */
                    Type *type,           /**< data type in soilcode file */
                    unsigned int nsoil,   /**< number of soil types */
                    Bool isout            /**< enable output of error messages */
                   )                      /** \return file pointer or NULL */
{
  FILE *file;
  Header header;
  int version;
  if(filename->fmt==META)
  {
    header.scalar=1;
    header.order=CELLYEAR;
    header.nbands=1;
    header.nstep=1;
    header.datatype=LPJ_BYTE;
    header.firstcell=0;
    header.ncell=0;
    header.nyear=1;
    header.cellsize_lon=header.cellsize_lat=0.5;
    file=openmetafile(&header,map,filename->map,swap,offset,filename->name,isout);
    if(file==NULL)
      return file;
    *type=header.datatype;
    if(header.nbands!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR127: Invalid number of bands %d in '%s', must be 1.\n",
                header.nbands,filename->name);
      fclose(file);
      return NULL;
    }
    if(header.nstep!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR127: Invalid number of steps %d in '%s', must be 1.\n",
                header.nstep,filename->name);
      fclose(file);
      return NULL;
    }

    return file;
  }
  if(map!=NULL)
    *map=NULL;
  file=fopen(filename->name,"rb");
  if(file==NULL)
  {
    if(isout)
      printfopenerr(filename->name);
    return NULL;
  }
  if(filename->fmt==RAW)
  {
    *swap=FALSE;
    if(nsoil<UCHAR_MAX)
      *type=LPJ_BYTE;
    else if(nsoil<USHRT_MAX)
      *type=LPJ_SHORT;
    else
      *type=LPJ_INT;
    *offset=0;
  }
  else
  {
    if(filename->fmt==CLM)
      version=READ_VERSION;
    else
      version=2;
    if(freadheader(file,&header,swap,LPJSOIL_HEADER,&version,isout))
    {
      if(isout)
        fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",filename->name);
      fclose(file);
      return NULL;
    }
    if(version>2)
      *type=header.datatype;
    else if(nsoil<UCHAR_MAX)
      *type=LPJ_BYTE;
    else if(nsoil<USHRT_MAX)
      *type=LPJ_SHORT;
    else
      *type=LPJ_INT;
    if(header.nbands!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR127: Invalid number of bands %d in '%s', must be 1.\n",
                header.nbands,filename->name);
      fclose(file);
      return NULL;
    }
    if(header.nstep!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR127: Invalid number of steps %d in '%s', must be 1.\n",
                header.nstep,filename->name);
      fclose(file);
      return NULL;
    }

    *offset=headersize(LPJSOIL_HEADER,version);
    if(isout && getfilesizep(file)!=typesizes[*type]*header.ncell+*offset)
      fprintf(stderr,"WARNING032: File size of '%s' does not match header.\n",filename->name);
  }
  return file;
} /* of 'fopensoilcode' */
