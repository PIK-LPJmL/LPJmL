/**************************************************************************************/
/**                                                                                \n**/
/**                     f  r  e  a  d  h  e  a  d  e  r  .  c                      \n**/
/**                                                                                \n**/
/**     Reading file header for LPJ related files. Detects                         \n**/
/**     whether byte order has to be changed                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadheader(FILE *file,     /**< file pointer of binary file */
                 Header *header, /**< file header to be read */
                 Bool *swap,     /**< set to TRUE if data is in different order */
                 const char *headername, /**< string in header */
                 int *version,   /**< returns version of CLM file, has to be set
                                    to READ_VERSION or prescribed value  */
                 Bool isout      /**< write output on stdout (TRUE/FALSE) */
                ) /** \return TRUE on error */
{
  char *buffer;
  int file_version,*ptr;
  buffer=newvec(char,strlen(headername)+1);
  if(buffer==NULL)
    return TRUE;
  if(fread(buffer,strlen(headername),1,file)!=1)
  {
    if(isout)
      fprintf(stderr,"ERROR239: Cannot read header id: %s.\n",strerror(errno));
    free(buffer);
    return TRUE;
  }
  /* set ending '\0' in string */
  buffer[strlen(headername)]='\0';
  if(strcmp(buffer,headername))
  {
    if(isout)
    {
      fputs("ERROR239: Header id '",stderr);
      fputprintable(stderr,buffer);
      fprintf(stderr,"' does not match '%s'.\n",headername);
    }
    free(buffer);
    return TRUE;
  }
  free(buffer);
  if(fread(&file_version,sizeof(file_version),1,file)!=1)
  {
    if(isout)
      fprintf(stderr,"ERROR239: Cannot read header version: %s.\n",strerror(errno));
    return TRUE;
  }
  if((file_version & 0xff)==0)
  {
    /* byte order has to be changed in file */
    *swap=TRUE;
    file_version=swapint(file_version);
  }
  else
    *swap=FALSE;
  if(*version==READ_VERSION)
    *version=file_version;
  switch(*version)
  {
    case 1: /* old header? */
      if(fread(header,sizeof(Header_old),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
      /* set default values for cell size, scalar and datatype */
      header->cellsize_lon=0.5;
      header->scalar=1;
      header->datatype=LPJ_SHORT;
      header->nstep=1;
      header->timestep=1;
      break;
    case 2:
      if(fread(header,sizeof(Header2),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
      header->datatype=LPJ_SHORT;
      header->nstep=1;
      header->timestep=1;
      break;
    case 4:
      if(fread(header,sizeof(Header),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
      break;
    default:
      if(fread(header,sizeof(Header3),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
      header->nstep=1;
      header->timestep=1;
  } /* of switch */
  if(*swap)  /* is data in different byte order? */
  {
    /* yes, swap bytes */
    header->order=swapint(header->order);
    header->firstyear=swapint(header->firstyear);
    header->nyear=swapint(header->nyear);
    header->firstcell=swapint(header->firstcell);
    header->ncell=swapint(header->ncell);
    header->nbands=swapint(header->nbands);
    if(*version>1)
    {
      ptr=(int *)&header->cellsize_lon;
      header->cellsize_lon=swapfloat(*ptr);
      ptr=(int *)&header->scalar;
      header->scalar=swapfloat(*ptr);
    }
    if(*version>2)
    {
      ptr=(int *)&header->cellsize_lat;
      header->cellsize_lat=swapfloat(*ptr);
      header->datatype=(Type)swapint(header->datatype);
    }
    if(*version==4)
    {
      header->nstep=swapint(header->nstep);
      header->timestep=swapint(header->timestep);
    }
 }
 if(*version<3)
   header->cellsize_lat=header->cellsize_lon;
 if(header->datatype<0 || header->datatype>LPJ_DOUBLE)
 {
   if(isout)
     fprintf(stderr,"ERROR240: Invalid value %d for datatype.\n",(int)header->datatype);
   return TRUE;
 }
 return FALSE;
} /* of 'freadheader' */
