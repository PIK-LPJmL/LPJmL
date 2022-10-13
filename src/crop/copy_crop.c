/**************************************************************************************/
/**                                                                                \n**/
/**               c  o  p  y  -  c  r  o  p .  c                                   \n**/
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
#include "crop.h"

void copy_crop(Pft *dst,const Pft *src)
{
  Pftcrop *dst_crop;
  const Pftcrop *src_crop;

  dst_crop=dst->data;
  src_crop=src->data;
  *dst_crop=*src_crop;
  if(src_crop->dh!=NULL)
  {
    dst_crop->dh=new(Double_harvest);
    check(dst_crop->dh);
    *(dst_crop->dh)=*(src_crop->dh);
  }
} /* of 'copy_crop' */
