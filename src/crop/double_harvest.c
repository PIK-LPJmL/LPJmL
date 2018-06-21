/**************************************************************************************/
/**                                                                                \n**/
/**       d  o  u  b  l  e  _  h  a  r  v  e  s  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function to distribute 2 harvests in one calendar year to                  \n**/
/**     separate output files                                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void double_harvest(int test,
                    Real *out,
                    Real *out2,
                    Real in
                   )
{
  if(test>0)
    *out2=in;
  else
    *out=in;
} /* of 'double_harvest' */ 
