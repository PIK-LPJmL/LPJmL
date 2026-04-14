/**************************************************************************************/
/**                                                                                \n**/
/**               i  n  i  t  f  w  i  .  c                                        \n**/
/**                                                                                \n**/
/**     Function initializes Canadian fire weather index                           \n**/
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

void initfwi(FWIdata *fwi /**< pointer to FWI data initialized */
            )
{
  fwi->ffmc=85;
  fwi->dmc=6;
  fwi->dc=15;
} /* of 'initfwi' */
