/**************************************************************************************/
/**                                                                                \n**/
/**                n  e  w  _  s  o  i  l  .  c                                    \n**/
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

void newsoil(Soil *soil /**< pointer to soil data */)
{
  int l;
  soil->litter.n=0;
  soil->litter.ag=NULL;
  soil->litter.bg=NULL;
  forrootsoillayer(l)
    soil->cpool[l].fast=soil->cpool[l].slow=soil->YEDOMA=0.0;
  soil->snowheight=soil->snowfraction=0;
} /* of 'newsoil' */
