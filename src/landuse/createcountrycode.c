/**************************************************************************************/
/**                                                                                \n**/
/**       c  r  e  a  t  e  c  o  u  n  t  r  y  c  o  d  e  .  c                  \n**/
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

FILE *createcountrycode(const char *filename, /**< filename */
                        int year,             /**< year (AD) */
                        int ncell             /**< number of LPJ cells */
                       )                      /** \return pointer to open file or NULL */
{
  FILE *file;
  Header header;
  file=fopen(filename,"wb");
  if(file==NULL)
  {
    printfcreateerr(filename);
    return NULL;
  }
  header.firstcell=0;
  header.ncell=ncell;
  header.order=CELLYEAR;
  header.nbands=2;
  header.firstyear=year;
  header.nyear=1;
  if(fwriteheader(file,&header,LPJ_COUNTRY_HEADER,LPJ_COUNTRY_VERSION))
  {
    fprintf(stderr,"ERROR152: Cannot write header in '%s'.\n",filename);
    fclose(file);                                                       
    return NULL;
  }
  return file;
} /* of 'createcountrycode' */
