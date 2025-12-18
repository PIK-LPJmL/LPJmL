/**************************************************************************************/
/**                                                                                \n**/
/**               c  o  p  y  -  g  r  a  s  s .  c                                \n**/
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
#include "grass.h"

void copy_grass(Pft *dst,const Pft *src)
{
  Pftgrass *dst_grass;
  const Pftgrass *src_grass;
  dst_grass=dst->data;
  src_grass=src->data;
  *dst_grass=*src_grass;
} /* of 'copy_grass' */
