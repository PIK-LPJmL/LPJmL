/*************************************************************************************/
/**                                                                                \n**/
/**                     u  p  d  a  t  e  _  i  r  r  i  g  .  c                   \n**/
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

void update_irrig(Stand *stand,        /**< pointer to stand */
                  int index,           /**< index in output data */
                  int ncft,            /**< number of crop PFTs */
                  const Config *config /**< LPJmL configuration */
                 )
{
  int nirrig;
  Biomass_tree *biomass_tree;
  biomass_tree=stand->data;
  nirrig=getnirrig(ncft,config);
  cutpfts(stand,config);
  biomass_tree->age=biomass_tree->growing_time=0;
  if(biomass_tree->irrigation.irrigation)
  {
    stand->cell->discharge.dmass_lake+=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*stand->cell->coord.area*stand->frac;
    stand->cell->balance.awater_flux-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*stand->frac;
    /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
    stand->cell->discharge.dmass_lake+=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*biomass_tree->irrigation.conv_evap*stand->cell->coord.area*stand->frac;
    stand->cell->balance.awater_flux-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*biomass_tree->irrigation.conv_evap*stand->frac;
    getoutput(&stand->cell->output,STOR_RETURN,config)+=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*stand->frac;
    getoutput(&stand->cell->output,CONV_LOSS_EVAP,config)-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*biomass_tree->irrigation.conv_evap*stand->frac;
    getoutput(&stand->cell->output,CONV_LOSS_DRAIN,config)-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*(1-biomass_tree->irrigation.conv_evap)*stand->frac;
    stand->cell->balance.aconv_loss_evap-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*biomass_tree->irrigation.conv_evap*stand->frac;
    stand->cell->balance.aconv_loss_drain-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*(1-biomass_tree->irrigation.conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
    if(stand->cell->ml.image_data!=NULL)
    {
      stand->cell->ml.image_data->mirrwatdem[NMONTH-1]-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*stand->frac;
      stand->cell->ml.image_data->mevapotr[NMONTH-1]-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*stand->frac;
    }
#endif

    if(config->pft_output_scaled)
    {
      getoutputindex(&stand->cell->output,CFT_CONV_LOSS_EVAP,index+biomass_tree->irrigation.irrigation*nirrig,config)-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*biomass_tree->irrigation.conv_evap*stand->cell->ml.landfrac[1].biomass_tree;
      getoutputindex(&stand->cell->output,CFT_CONV_LOSS_DRAIN,index+biomass_tree->irrigation.irrigation*nirrig,config)-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*(1-biomass_tree->irrigation.conv_evap)*stand->cell->ml.landfrac[1].biomass_tree;
    }
    else
    {
      getoutputindex(&stand->cell->output,CFT_CONV_LOSS_EVAP,index+biomass_tree->irrigation.irrigation*nirrig,config)-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*biomass_tree->irrigation.conv_evap;
      getoutputindex(&stand->cell->output,CFT_CONV_LOSS_DRAIN,index+biomass_tree->irrigation.irrigation*nirrig,config)-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*(1/biomass_tree->irrigation.ec-1)*(1-biomass_tree->irrigation.conv_evap);
    }

    biomass_tree->irrigation.irrig_stor=0;
    biomass_tree->irrigation.irrig_amount=0;
  }
} /* of 'update_irrig' */
