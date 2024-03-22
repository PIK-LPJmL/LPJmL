/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  h  e  a  d  e  r  i  d  .  c                       \n**/
/**                                                                                \n**/
/**     Functions reads header id from CLM file                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadheaderid(FILE *file,        /**< file pointer of binary file */
                   String headername, /**< returns header string */
                   Bool isout         /**< write output on stdout (TRUE/FALSE) */
                  )                   /** \return TRUE on error */
{
  int count;
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
  headername[count-1]='\0';
  return FALSE;
} /* of 'freadheaderid' */
