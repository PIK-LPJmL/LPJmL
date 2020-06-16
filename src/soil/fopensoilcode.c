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
    header.nbands=1;
    header.datatype=LPJ_BYTE;
    header.firstcell=0;
    header.ncell=0;
    header.cellsize_lon=header.cellsize_lat=0.5;
    file=openmetafile(&header,swap,offset,filename->name,isout);
    *type=header.datatype;
    if(file==NULL && isout)
      fprintf(stderr,"ERROR224: Cannot read description file '%s'.\n",filename->name);
    return file;
  }
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
    if(freadheader(file,&header,swap,LPJSOIL_HEADER,&version))
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
    *offset=headersize(LPJSOIL_HEADER,version);
  }
  return file;
} /* of 'fopensoilcode' */
