/**************************************************************************************/
/**                                                                                \n**/
/**                     f  r  e  a  d  t  o  p  h  e  a  d  e  r  .  c             \n**/
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "types.h"
#include "errmsg.h"
#include "swap.h"

Bool freadtopheader(FILE *file,     /**< file pointer of binary file */
                    Bool *swap,     /**< set to TRUE if data is in different order */
                    const char *headername, /**< string in header */
                    int *version,   /**< returns version of CLM file, has to be set
                                       to READ_VERSION or prescribed value  */
                    Bool isout      /**< write output on stdout (TRUE/FALSE) */
                   ) /** \return TRUE on error */
{
  char *buffer;
  int file_version,rc;
  buffer=newvec(char,strlen(headername)+1);
  if(buffer==NULL)
  {
    printallocerr("buffer");
    return TRUE;
  }
  rc=fread(buffer,1,strlen(headername),file);
  if(rc!=strlen(headername))
  {
    if(isout)
      fprintf(stderr,"ERROR239: Cannot read header id '%s', only %d bytes read.\n",
              headername,rc);
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
      fprintf(stderr,"ERROR239: Cannot read header version.\n");
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
  return FALSE;
} /* of 'freadtopheader' */
