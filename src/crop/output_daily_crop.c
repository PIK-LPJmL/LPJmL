/**************************************************************************************/
/**                                                                                \n**/
/**       o  u  t  p  u  t  _  d  a  i  l  y  _  c  r  o  p  .  c                  \n**/
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

void output_daily_crop(Daily_outputs* output, /**< daily output data */
                       const  Pft* pft, /**< pointer to PFT data */
                       Real gpp,         /**< GPP (gC/m2/day) */
                       Real rd           /**< leaf respiration (gC/m2/day) */
                      )
{
  const Pftcrop *crop;
  crop=pft->data;
  output->lai=crop->lai;
  output->laimaxad=crop->laimax_adjusted;
  output->lainppdeficit=crop->lai_nppdeficit;
  output->husum=crop->husum;
  output->vdsum=crop->vdsum;
  output->fphu=crop->fphu;
  output->phen=pft->phen;
  output->growingday=crop->growingdays;
  output->pvd=crop->pvd;
  output->phu=crop->phu;
  output->gpp=gpp;
  output->rd=rd;
} /* of 'output_daily_crop' */
