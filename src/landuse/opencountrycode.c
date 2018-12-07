/**************************************************************************************/
/**                                                                                \n**/
/**             o  p  e  n  c  o  u  n  t  r  y  c  o  d  e  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

FILE *opencountrycode(const Filename *filename, /**< filename */
                      Bool *swap, /**< byte order has to be swapped (TRUE/FALS) */
                      Type *type,  /**< LPJ datatype */
                      long *offset, /**< offset in binary file */
                      Bool isout    /**< enable error output (TRUE/FALSE) */
                     )              /** \return pointer to open file or NULL */
{
  FILE *file;
  Header header;
  String headername;
  int version;
  size_t h_offset;
  if(filename->fmt==META)
  {
    header.scalar=1;
    header.datatype=LPJ_SHORT;
    header.nbands=2;
    header.firstcell=0;
    header.ncell=0;
    header.cellsize_lon=header.cellsize_lat=0.5;
    file=openmetafile(&header,swap,&h_offset,filename->name,isout);
    if(file==NULL)
      return NULL;
    if(header.nbands!=2)
    {
      if(isout)
        fprintf(stderr,"ERROR218: Number of bands=%d in description file '%s' is not 2.\n",
                header.nbands,filename->name);
      fclose(file);
      return NULL;
    }
    *type=header.datatype;
    *offset=h_offset-typesizes[header.datatype]*2*header.firstcell;
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
    *offset=0;
    *type=LPJ_SHORT;
  }
  else
  {
    version=(filename->fmt==CLM) ? READ_VERSION : 2;
    if(freadanyheader(file,&header,swap,headername,&version))
    {
      if(isout)
        fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",filename->name);
      fclose(file);                                                       
      return NULL;
    }
    if(header.nbands!=2)
    {
      if(isout)
        fprintf(stderr,"ERROR218: Number of bands=%d in countrycode file '%s' is not 2.\n",
                header.nbands,filename->name);
      fclose(file);                                                       
      return NULL;
    }
    *type=header.datatype;
    *offset=headersize(headername,version)-typesizes[header.datatype]*2*header.firstcell;
  }
  return file;
} /* of 'opencountrycode' */
