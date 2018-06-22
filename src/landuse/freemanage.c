/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  e  m  a  n  a  g  e  .  c                               \n**/
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

void freemanage(Manage *manage, /**< pointer to management data */
                int npft        /**< number of natural PFTs */
               )
{
  if(manage->par!=NULL && manage->par->laimax_cft==NULL)
    free(manage->laimax+npft);
} /* of 'freemanage' */
