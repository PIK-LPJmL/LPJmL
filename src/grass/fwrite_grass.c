/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  _  g  r  a  s  s  .  c                       \n**/
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

Bool fwrite_grass(FILE *file,const Pft *pft)
{
  const Pftgrass *grass;
  grass=pft->data;
  return fwrite(grass,sizeof(Pftgrass),1,file)!=1;
} /* of 'fwrite_grass' */
