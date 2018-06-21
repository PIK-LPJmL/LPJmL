/**************************************************************************************/
/**                                                                                \n**/
/**              g  e  t  n  s  o  i  l  c  o  d  e  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets number of soil code values in file                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int getnsoilcode(const Filename *filename, /**< filename of soil code file */
                 unsigned int nsoil,       /**< number of soil types */
                 Bool isout                /**< error output? (TRUE/FALSE) */
                )                          /** \return number of soil codes */
{
  Coord_netcdf coord;
  int n, version;
  size_t offset;
  FILE *file;
  Bool swap;
  Header header;
  if(filename->fmt==CDF)
  {
    coord=opencoord_netcdf(filename->name,filename->var,isout);
    if(coord==NULL)
    {
      if(isout)
        fputs("ERROR165: Cannot get number of cells from soil code file.\n",stderr);
      return -1;
    }
    n=numcoord_netcdf(coord);
    if(n==-1)
    {
      if(isout)
        fputs("ERROR165: Cannot get number of cells from soil code file.\n",stderr);
      return -1;
    }
    closecoord_netcdf(coord);
    return n;
  }
  if(filename->fmt==RAW)
  {
    n=getfilesize(filename->name);
    if(n<0)
    {
      if(isout)
        fprintf(stderr,"ERROR165: Cannot get number of cells from soil code file '%s': %s.\n",
                filename->name,strerror(errno));
      return n;
    }
    if(nsoil<UCHAR_MAX)
      return n;
    else if(nsoil<USHRT_MAX)
      return n/sizeof(short);
    else
      return n/sizeof(int);
  }
  else if(filename->fmt==META)
  {
    header.ncell=-1;
    file=openmetafile(&header,&swap,&offset,filename->name,isout);
    if(file==NULL)
    {
      if(isout)
        fprintf(stderr,"ERROR224: Cannot read description file '%s'.\n",filename->name);
      return -1;
    }
    return header.ncell; 
  }
  else /* file is in CLM/CLM2 format */
  {
    file=fopen(filename->name,"rb"); /* open CLM file */
    if(file==NULL)
    {
      if(isout)
        printfopenerr(filename->name);
      return -1;
    }
    if(filename->fmt==CLM)
      version=READ_VERSION;
    else
      version=2;
    /* read header */
    if(freadheader(file,&header,&swap,LPJSOIL_HEADER,&version))
    {
      if(isout)
        fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",filename->name);
      fclose(file);
      return -1;
    }
    fclose(file);
    return header.ncell; /* return number of cells in soil code file */
  }
} /* of 'getnsoilcode' */
