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
    litter->item=newvec(Litteritem,litter->n);
    if(litter->item==NULL)
    {
      printallocerr("litter");
      return TRUE;
    }
    for(i=0;i<litter->n;i++)
    {
      if(fread(&b,sizeof(b),1,file)!=1)
      {
        free(litter->item);
        litter->n=0;
        litter->item=NULL;
        return TRUE;
      }
      litter->item[i].pft=pftpar+b;
      if(b>=ntotpft)
      {
        fprintf(stderr,"ERROR195: Invalid value %d for PFT index litter, must be in [0,%d].\n",
                (int)b,ntotpft-1);
        free(litter->item);
        litter->n=0;
        litter->item=NULL;
        return TRUE;
      }
      if(freadreal((Real *)&litter->item[i].ag,sizeof(Trait)/sizeof(Real),
                   swap,file)!=sizeof(Trait)/sizeof(Real))
        return TRUE;
      if(freadreal((Real *)&litter->item[i].agsub,sizeof(Trait)/sizeof(Real),
                   swap,file)!=sizeof(Trait)/sizeof(Real))
        return TRUE;
      freadreal((Real *)(&litter->item[i].bg),sizeof(Stocks)/sizeof(Real),swap,file);
    }
  }
  else
    litter->item=NULL;
  freadreal1(&litter->agtop_wcap,swap,file);
  freadreal1(&litter->agtop_moist,swap,file);
  freadreal1(&litter->agtop_cover,swap,file);
  freadreal1(&litter->agtop_temp,swap,file);
  return FALSE;
} /* of 'freadlitter' */
