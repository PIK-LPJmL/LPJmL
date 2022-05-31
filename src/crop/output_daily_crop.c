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

void output_daily_crop(Output* output, /**< daily output data */
                       const  Pft* pft, /**< pointer to PFT data */
                       Real gpp,         /**< GPP (gC/m2/day) */
                       Real rd,          /**< leaf respiration (gC/m2/day) */
                       const Config *config
                      )
{
  const Pftcrop *crop;
  crop=pft->data;
  if(config->crop_index==ALLSTAND)
    getoutput(output,D_GPP,config)+=gpp*pft->stand->frac;
  else
  {
    getoutput(output,D_LAI,config)=crop->lai;
    getoutput(output,D_LAIMAXAD,config)=crop->laimax_adjusted;
    getoutput(output,D_LAINPPDEF,config)=crop->lai_nppdeficit;
    getoutput(output,D_HUSUM,config)=crop->husum;
    getoutput(output,D_VDSUM,config)=crop->vdsum;
    getoutput(output,D_FPHU,config)=crop->fphu;
    getoutput(output,D_PHEN,config)=pft->phen;
    getoutput(output,D_GROWINGDAY,config)=crop->growingdays;
    getoutput(output,D_PVD,config)=crop->pvd;
    getoutput(output,D_PHU,config)=crop->phu;
    getoutput(output,D_GPP,config)=gpp;
    getoutput(output,D_RD,config)=rd;
  }
} /* of 'output_daily_crop' */
