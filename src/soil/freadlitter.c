/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  l  i  t  t  e  r  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads litter data from file                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadlitter(FILE *file, /**< File pointer to binary file */
                 Litter *litter, /**< Litter pool to be read */
                 const Pftpar pftpar[], /**< PFT parameter array */
                 int ntotpft, /**< total number of PFTs */
                 Bool swap /**< Byte order has to be changed (TRUE/FALSE) */
              )            /** \return TRUE on error */
{
  Byte b;
  int i;
  freadreal(litter->avg_fbd,NFUELCLASS+1,swap,file);
  if(fread(&b,sizeof(b),1,file)!=1)
    return TRUE;
  litter->n=b;
  if(litter->n)
  {
    litter->ag=newvec(Litteritem,litter->n);
    litter->bg=newvec(Real,litter->n);
    if(litter->ag==NULL || litter->bg==NULL)
    {
      printallocerr("litter");
      return TRUE;
    }
    for(i=0;i<litter->n;i++)
    {
      if(fread(&b,sizeof(b),1,file)!=1)
      {
        free(litter->ag);
        free(litter->bg);
        litter->n=0;
        litter->ag=NULL;
        litter->bg=NULL;
        return TRUE;
      }
      litter->ag[i].pft=pftpar+b;
      if(b>=ntotpft)
      {
        fprintf(stderr,"ERROR195: Invalid value %d for PFT index litter.\n",(int)b);
        free(litter->ag);
        free(litter->bg);
        litter->n=0;
        litter->ag=NULL;
        litter->bg=NULL;
        return TRUE;
      }
      if(freadreal((Real *)&litter->ag[i].trait,sizeof(Trait)/sizeof(Real),
                   swap,file)!=sizeof(Trait)/sizeof(Real))
        return TRUE;
      freadreal1(&litter->bg[i],swap,file);
    }
  }
  else
  {
    litter->ag=NULL;
    litter->bg=NULL;
  }
  return FALSE;
} /* of 'freadlitter' */
