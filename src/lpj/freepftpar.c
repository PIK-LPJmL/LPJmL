/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  e  p  f  t  p  a  r  .  c                               \n**/
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

void freepftpar(Pftpar pftpar[], /**< PFT parameter array */
                int ntotpft      /**< total number of PFTs */
               )
{
  int i;
  for(i=0;i<ntotpft;i++)
  {
    free(pftpar[i].name);
    free(pftpar[i].data);
  }
  free(pftpar);
} /* of 'freepftpar' */
