/**************************************************************************************/
/**                                                                                \n**/
/**               i  n  i  t  f  w  i  .  c                                        \n**/
/**                                                                                \n**/
/**     Function initializes Canadian fire weather index                           \n**/
/**     Derived from:                                                              \n**/
/**     Van Wagner, C.E.; Pickett, T.L. 1985. Equations and FORTRAN program for    \n**/
/**     the Canadian Forest Fire Weather Index System. Canadian Forestry Service,  \n**/
/**     Petawawa National Forestry Institute, Chalk River, Ontario.                \n**/
/**     Forestry Technical Report 33. 18 p.                                        \n**/
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
  /* Initial values from Van Wagner et al. (1985) */
  fwi->ffmc=85;
  fwi->dmc=6;
  fwi->dc=15;
} /* of 'initfwi' */
