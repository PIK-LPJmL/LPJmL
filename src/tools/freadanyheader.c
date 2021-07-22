/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  a  n  y  h  e  a  d  e  r  .  c                    \n**/
/**                                                                                \n**/
/**     Functions reads header from file with any header name and version          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadanyheader(FILE *file,        /**< file pointer of binary file */
                    Header *header,    /**< file header to be read */
                    Bool *swap,        /**< set to TRUE if data is in different order */
                    String headername, /**< returns header string */
                    int *version,      /**< returns version of CLM file, has to
                                          be set to READ_VERSION or prescribed
                                          value */
                    Bool isout         /**< write output on stdout (TRUE/FALSE) */
                   )                   /** \return TRUE on error */
{
  int count,*ptr,file_version;
  char b;
  count=0;
  /* read header string, must start with prefix LPJ */
  do
  {
    if(count==STRING_LEN) /* string too long? */
    {
      if(isout)
        fprintf(stderr,"ERROR239: Header id too long.\n");
      return TRUE;
    }
    else if(count==3 && strncmp("LPJ",headername,3))
    {
      if(isout)
        fprintf(stderr,"ERROR239: Header id does not start with 'LPJ'.\n");
      return TRUE;
    }
    if(fread(headername+count,1,1,file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR239: Unexpected end of file reading header id.\n");
      return TRUE;
    }
  }while(headername[count++]>=' ');
  if(count<3)
  {
    if(isout)
      fprintf(stderr,"ERROR239: Header id shorter than 3 characters.\n");
    return TRUE;
  }
  if(fread(&b,1,1,file)!=1)
  {
    if(isout)
      fprintf(stderr,"ERROR239: Unexpected end of file reading header.\n");
    return TRUE;
  }
  if(headername[count-1] && b)
  {
    if(isout)
      fprintf(stderr,"ERROR239: Invalid version in header.\n");
    return TRUE;
  }
  headername[count-1]='\0';
  fseek(file,-2,SEEK_CUR);
  if(fread(&file_version,sizeof(int),1,file)!=1)
    return TRUE;
  if((file_version & 0xff)==0)
  {
    /* file is in different byte order */
    *swap=TRUE;
    file_version=swapint(file_version);
  }
  else
    *swap=FALSE;
  if(*version==READ_VERSION)
    *version=file_version;
  switch(*version)
  {
    case 1: /* old version header? */
      if(fread(header,sizeof(Header_old),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
      header->cellsize_lon=0.5;   /* set default cell size */
      header->scalar=1;           /* and scaling factor */
      header->datatype=LPJ_SHORT; /* and datatype */
      break;
    case 2:
      if(fread(header,sizeof(Header2),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
      header->datatype=LPJ_SHORT; /* set default datatype */
      break;
    default:
      if(fread(header,sizeof(Header),1,file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR239: Cannot read header data: %s.\n",strerror(errno));
        return TRUE;
      }
  } /* of switch */
  if(*swap) /* is data in different byte order? */
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
} /* of 'freadanyheader' */
