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

static Bool freadtrait(Bstruct file,const char *name,Trait *trait)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(freadstocks(file,"leaf",&trait->leaf))
    return TRUE;
  if(freadstocksarray(file,"wood",trait->wood,NFUELCLASS))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of ' freadtrait' */

Bool freadlitter(Bstruct file, /**< File pointer to restrart file */
                 const char  *name, /**< name of object */
                 Litter *litter, /**< Litter pool to be read */
                 const Pftpar pftpar[], /**< PFT parameter array */
                 int ntotpft  /**< total number of PFTs */
              )            /** \return TRUE on error */
{
  int i,pft_id;
  if(bstruct_readrealarray(file,"avg_fbd",litter->avg_fbd,NFUELCLASS+1))
    return TRUE;
  if(bstruct_readarray(file,name,&litter->n))
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
      if(bstruct_readstruct(file,NULL))
        return TRUE;
      if(bstruct_readint(file,"pft_id",&pft_id))
      {
        free(litter->item);
        litter->n=0;
        litter->item=NULL;
        return TRUE;
      }
      litter->item[i].pft=pftpar+pft_id;
      if(pft_id<0 || pft_id>=ntotpft)
      {
        fprintf(stderr,"ERROR195: Invalid value %d for PFT index litter, must be in [0,%d].\n",
                pft_id,ntotpft-1);
        free(litter->item);
        litter->n=0;
        litter->item=NULL;
        return TRUE;
      }
      if(freadtrait(file,"agtop",&litter->item[i].agtop))
        return TRUE;
      if(freadtrait(file,"agsub",&litter->item[i].agsub))
        return TRUE;
      if(freadstocks(file,"bg",&litter->item[i].bg))
        return TRUE;
      if(bstruct_readendstruct(file,NULL))
        return TRUE;
    }
  }
  else
    litter->item=NULL;
  if(bstruct_readendarray(file,name))
    return TRUE;
  if(bstruct_readreal(file,"agtop_wcap",&litter->agtop_wcap))
    return TRUE;
  if(bstruct_readreal(file,"agtop_moist",&litter->agtop_moist))
    return TRUE;
  if(bstruct_readreal(file,"agtop_cover",&litter->agtop_cover))
    return TRUE;
  return bstruct_readreal(file,"agtop_temp",&litter->agtop_temp);
} /* of 'freadlitter' */
