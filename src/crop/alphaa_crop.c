/**************************************************************************************/
/**                                                                                \n**/
/**                 a  l  p  h  a  a  _  c  r  o  p  .  c                          \n**/
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

Real alphaa_crop(const Pft *pft,            /**< pointer to crop PFT */
                 int UNUSED(with_nitrogen), /**< nitrogen cycle enabled */
                 int lai_opt                /**< LAImax option */
                )                           /** \return alpha_a (0..1) */
{
  Real laimax;
  if(lai_opt==LAIMAX_PAR)
    return pft->par->alphaa;
  if(pft->stand->cell->ml.manage.laimax==NULL)
    laimax=0;
  else
    laimax=pft->stand->cell->ml.manage.laimax[pft->par->id];
  laimax= (laimax<=7) ? laimax : 7;
  /* learning from AgMIP, MAIZE reaches highest intensity level at LAImax=5*/
  if(pft->par->id==MAIZE)
    return min(1,pft->par->alphaa-(0.15*(5-laimax)));
  else
    return pft->par->alphaa-(0.1*(7-laimax));
} /* of 'alphaa_crop' */
