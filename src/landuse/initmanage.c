/**************************************************************************************/
/**                                                                                \n**/
/**     i n i t m a n a g e .  c                                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJ                                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

Bool initmanage(Manage *manage,               /**< pointer to management data */
                const Countrypar *countrypar, /**< pointer to country param */
                const Regionpar *regionpar,   /**< pointer to region param */
                const Pftpar *pftpar,         /**< PFT parameter array */
                int npft,                     /**< number of natural PFTs */
                int nagtree,
                int ncft,                     /**< number of crop PFts */
                int laimax_opt,               /**< Option for maximm LAI setting */
                Real laimax                   /**< maximum LAI */
               )                              /** \return TRUE on error */
{
  const Pftcroppar *croppar;
  int cft;
  manage->par=countrypar;    
  manage->regpar=regionpar;
  if(manage->par->laimax_cft==NULL)
  {
    manage->laimax=newvec2(Real,npft,npft+ncft-1);  /* allocate memory for country-specific laimax*/
    if(manage->laimax+npft==NULL)
    {
      printallocerr("laimax");
      return TRUE;
    }
  }
  else
    manage->laimax=manage->par->laimax_cft-npft;  /* set pointer to country specific laimax */
  manage->k_est = manage->par->k_est - npft + nagtree; /* set pointer to country specific k_est */

  if(laimax_opt==CONST_LAI_MAX)
    for(cft=0;cft<ncft;cft++)
      manage->laimax[npft+cft]=laimax;
  else if(laimax_opt==LAIMAX_PAR)
    for(cft=0;cft<ncft;cft++)
    {
      croppar=pftpar[npft+cft].data;
      manage->laimax[npft+cft]=croppar->laimax;
    }
  return FALSE;
} /* of 'initmanage' */
