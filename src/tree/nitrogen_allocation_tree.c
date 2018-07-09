/**************************************************************************************/
/**                                                                                \n**/
/**       n i t r o g e n _ a l l o c a t i o n _ t r e e . c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function solves the following linear system for (a1,a2,a3):                \n**/
/**                                                                                \n**/
/**     s1.nitrogen+a1*n_inc      s2.nitrogen+a2*n_inc                             \n**/
/**     -------------------- = r1 --------------------                             \n**/
/**        s1.carbon                s2.carbon                                      \n**/
/**                                                                                \n**/
/**     s1.nitrogen+a1*n_inc      s3.nitrogen+a3*n_inc                             \n**/
/**     -------------------- = r2 --------------------                             \n**/
/**        s1.carbon                s3.carbon                                      \n**/
/**                                                                                \n**/
/**             a1 + a2 + a3 = 1                                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml/lpjml                             \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void nitrogen_allocation_tree(Real *a1,Real *a2,Real *a3,
                              Stocks s1,Stocks s2,Stocks s3,
                              Real r1,Real r2,Real n_inc)
{
  *a1=-(-s1.carbon*n_inc*r1*r2-s1.carbon*s2.nitrogen*r1*r2-s1.carbon*s3.nitrogen*r1*r2+s2.carbon*s1.nitrogen*r2+s3.carbon*s1.nitrogen*r1)/n_inc/(s1.carbon*r1*r2+s2.carbon*r2+s3.carbon*r1);
  *a2=-(s1.carbon*s2.nitrogen*r1*r2-s2.carbon*n_inc*r2-s2.carbon*s1.nitrogen*r2-s2.carbon*s3.nitrogen*r2+s3.carbon*s2.nitrogen*r1)/n_inc/(s1.carbon*r1*r2+s2.carbon*r2+s3.carbon*r1);
  *a3=-(s1.carbon*s3.nitrogen*r1*r2+s2.carbon*s3.nitrogen*r2-s3.carbon*n_inc*r1-s3.carbon*s1.nitrogen*r1-s3.carbon*s2.nitrogen*r1)/n_inc/(s1.carbon*r1*r2+s2.carbon*r2+s3.carbon*r1);
} /* of 'allocation_nitrogen_tree' */
