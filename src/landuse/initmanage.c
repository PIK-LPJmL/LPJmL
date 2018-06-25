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

void initmanage(Manage *manage, /**< pointer to management data */
                const Countrypar *countrypar, /**< pointer to country param */
                const Regionpar *regionpar, /**< pointer to region param */
                int npft, /**< number of natural PFTs */
                int ncft, /**< number of crop PFts */
                Bool isconstlai, /**< using const. LAImax? */
                Real laimax     /**< maximum LAI */
               )
{
  int cft;
  manage->par=countrypar;    
  manage->regpar=regionpar;
  if(manage->par->laimax_cft==NULL)
    manage->laimax=newvec2(Real,npft,npft+ncft-1);  /* allocate memory for country-specific laimax*/
  else
    manage->laimax=manage->par->laimax_cft-npft;  /* set pointer to country specific laimax */
  if(isconstlai)
    for(cft=0;cft<ncft;cft++)
      manage->laimax[npft+cft]=laimax;
} /* of 'initmanage' */
