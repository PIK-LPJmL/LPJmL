/**************************************************************************************/
/**                                                                                \n**/
/**                     e  q  u  i  l  s  o  m  . c                                \n**/
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

/*
 *  DESCRIPTION
 *
 *  after vegetation equilibrium reset decomposition parameter
 *
 *
 */

void equilveg(Cell *cell /**< pointer to cell */
             )
{
  int s,l;
  Stand *stand;
  
  foreachstand(stand,s,cell->standlist)
  {
    forrootsoillayer(l)
    {
       stand->soil.k_mean[l].fast=0.0;
       stand->soil.k_mean[l].slow=0.0;
    }
    stand->soil.decomp_litter_mean=stand->soil.count=0;
  }
} /* of 'equilveg' */
