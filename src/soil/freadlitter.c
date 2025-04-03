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

static Bool freadtrait(FILE *file,const char *name,Trait *trait,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readstocks(file,"leaf",&trait->leaf,swap))
    return TRUE;
  if(readstocksarray(file,"wood",trait->wood,NFUELCLASS,swap))
    return TRUE;
  return readendstruct(file);
} /* of ' freadtrait' */

Bool freadlitter(FILE *file, /**< File pointer to restrart file */
                 const char  *name, /**< name of object */
                 Litter *litter, /**< Litter pool to be read */
                 const Pftpar pftpar[], /**< PFT parameter array */
                 int ntotpft, /**< total number of PFTs */
                 Bool swap /**< Byte order has to be changed (TRUE/FALSE) */
              )            /** \return TRUE on error */
{
  int i,pft_id;
  if(readrealarray(file,"avg_fbd",litter->avg_fbd,NFUELCLASS+1,swap))
    return TRUE;
  if(readarray(file,name,&litter->n,swap)) 
    return TRUE;
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
      if(readstruct(file,NULL,swap))
        return TRUE;
      if(readint(file,"pft_id",&pft_id,swap))
      {
        free(litter->item);
        litter->n=0;
        litter->item=NULL;
        return TRUE;
      }
      litter->item[i].pft=pftpar+pft_id;
      if(pft_id>=ntotpft)
      {
        fprintf(stderr,"ERROR195: Invalid value %d for PFT index litter, must be in [0,%d].\n",
                pft_id,ntotpft-1);
        free(litter->item);
        litter->n=0;
        litter->item=NULL;
        return TRUE;
      }
      if(freadtrait(file,"agtop",&litter->item[i].agtop,swap))
        return TRUE;
      if(freadtrait(file,"agsub",&litter->item[i].agsub,swap))
        return TRUE;
      if(readstocks(file,"bg",&litter->item[i].bg,swap))
        return TRUE;
      if(readendstruct(file))
        return TRUE;
    }
  }
  else
    litter->item=NULL;
  if(readendarray(file))
    return TRUE;
  if(readreal(file,"agtop_wcap",&litter->agtop_wcap,swap))
    return TRUE;
  if(readreal(file,"agtop_moist",&litter->agtop_moist,swap))
    return TRUE;
  if(readreal(file,"agtop_cover",&litter->agtop_cover,swap))
    return TRUE;
  return readreal(file,"agtop_temp",&litter->agtop_temp,swap);
} /* of 'freadlitter' */
