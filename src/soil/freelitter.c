/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  e  l  i  t  t  e  r  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates memory for litter pools                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freelitter(Litter *litter /**< pointer to litter data */
               )
{
  if(litter->n)
  {
    free(litter->ag);
    free(litter->bg); 
  }
  litter->n=0;
  litter->ag=NULL;
  litter->bg=NULL;
} /* of 'freelitter' */
