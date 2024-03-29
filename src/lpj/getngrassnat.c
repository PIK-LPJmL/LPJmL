/**************************************************************************************/
/**                                                                                \n**/
/**                   g  e  t  n  g  r  a  s  s  n  a  t  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function counts number of natural grass PFTs                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int getngrassnat(const Pftpar pftpar[], /**< PFT parameter array */
                int npft               /**< number of natural PFTs */
               )                       /** \return number of natural grass PFTs */
{
  int p,ngrassnat;
  ngrassnat=0;
  for(p=0;p<npft;p++)
    if(pftpar[p].cultivation_type==NONE && pftpar[p].type==GRASS)
      ngrassnat++;
  return ngrassnat;
} /* of 'getngrassnat' */
