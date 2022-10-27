/**************************************************************************************/
/**                                                                                \n**/
/**           s  e  t  o  t  h  e  r  s  t  o  c  r  o  p  .  c                    \n**/
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
#include "grassland.h"
#include "agriculture.h"

void setotherstocrop(void)
{
  others_stand.newstand=agriculture_stand.newstand;
  others_stand.fwrite=agriculture_stand.fwrite;
  others_stand.fread=agriculture_stand.fread;
  others_stand.fprint=agriculture_stand.fprint;
  others_stand.annual=agriculture_stand.annual;
  others_stand.daily=agriculture_stand.daily;
} /* of 'setotherstocrop' */
