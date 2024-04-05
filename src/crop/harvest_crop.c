/**************************************************************************************/
/**                                                                                \n**/
/**         h  a  r  v  e  s  t  _  c  r  o  p  .  c                               \n**/
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

/*
 *  Called in function daily_agriculture when crop
 *  is harvested
 *
 */
void harvest_crop(Output *output,      /**< Output data */
                  Stand *stand,        /**< pointer to crop stand */
                  Pft *pft,            /**< PFT variables */
                  int npft,            /**< number of natural PFTs */
                  int ncft,            /**< number of crop PFTs */
                  int year,            /**< year of harvest */
                  const Config *config /**< LPJmL configuration */
                 )
{
  Pftcroppar *croppar;
  Harvest harvest;
  Pftcrop *crop;
  Irrigation *data;
  Real fuelratio,bifratio;
  Real res_onfield, res_remove;
  int nnat,nirrig,index;
  data=stand->data;
  crop=pft->data;

  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);
  if(stand->type->landusetype==OTHERS)
    index=data->irrigation*nirrig+rothers(ncft);
  else
    index=pft->par->id-npft+data->irrigation*nirrig;
  if(config->residue_treatment<READ_RESIDUE_DATA)
    res_onfield = config->residue_treatment==FIXED_RESIDUE_REMOVE ? param.residues_in_soil : 1 ;
  else
    res_onfield=stand->cell->ml.residue_on_field[data->irrigation].crop[pft->par->id-npft];
  res_remove = (1-res_onfield);
  stand->soil.litter.item[pft->litter].agtop.leaf.carbon += (crop->ind.leaf.carbon + crop->ind.pool.carbon)*res_onfield;
  getoutput(output,LITFALLC,config)+=(crop->ind.leaf.carbon + crop->ind.pool.carbon)*res_onfield*stand->frac;
  stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen += (crop->ind.leaf.nitrogen + crop->ind.pool.nitrogen)*res_onfield;
  getoutput(output,LITFALLN,config)+=(crop->ind.leaf.nitrogen + crop->ind.pool.nitrogen)*res_onfield*stand->frac;
  getoutput(output,LITFALLN_AGR,config)+=(crop->ind.leaf.nitrogen + crop->ind.pool.nitrogen)*res_onfield*stand->frac;
  if (!config->residues_fire)
  {
    harvest.residuals_burnt.carbon = harvest.residuals_burntinfield.carbon =
    harvest.residuals_burnt.nitrogen = harvest.residuals_burntinfield.nitrogen = 0;
  }
  else
  {
    fuelratio = param.fuelratio; /* burn outside of field */
    bifratio = param.bifratio; /* burn in field */
    if (bifratio + fuelratio > res_remove)
    {
      bifratio *= res_remove;
      fuelratio *= res_remove;
    }
    res_remove -= fuelratio + bifratio;
    harvest.residuals_burnt.carbon = (crop->ind.leaf.carbon + crop->ind.pool.carbon)*fuelratio;
    harvest.residuals_burntinfield.carbon = (crop->ind.leaf.carbon + crop->ind.pool.carbon)*bifratio;
    harvest.residuals_burnt.nitrogen = (crop->ind.leaf.nitrogen + crop->ind.pool.nitrogen)*fuelratio;
    harvest.residuals_burntinfield.nitrogen = (crop->ind.leaf.nitrogen + crop->ind.pool.nitrogen)*bifratio;
  }
  harvest.residual.carbon = (crop->ind.leaf.carbon + crop->ind.pool.carbon)*res_remove;
  harvest.residual.nitrogen = (crop->ind.leaf.nitrogen + crop->ind.pool.nitrogen)*res_remove;
  harvest.harvest=crop->ind.so;
  stand->soil.litter.item[pft->litter].bg.carbon+=crop->ind.root.carbon;
  getoutput(output,LITFALLC,config)+=crop->ind.root.carbon*stand->frac;
  stand->soil.litter.item[pft->litter].bg.nitrogen+=crop->ind.root.nitrogen;
  getoutput(output,LITFALLN,config)+=crop->ind.root.nitrogen*stand->frac;
  getoutput(output,LITFALLN_AGR,config)+=crop->ind.root.nitrogen*stand->frac;
  if(crop->sh!=NULL)
  {
    if(config->pft_output_scaled)
    {
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_HARVESTC,index,config)),
                     &(getoutputindex(output,PFT_HARVESTC2,index,config)),harvest.harvest.carbon*stand->frac);
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_HARVESTN,index,config)),
                     &(getoutputindex(output,PFT_HARVESTN2,index,config)),harvest.harvest.nitrogen*stand->frac);
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_RHARVESTC,index,config)),
                     &(getoutputindex(output,PFT_RHARVESTC2,index,config)),(harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon)*stand->frac);
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_RHARVESTN,index,config)),
                     &(getoutputindex(output,PFT_RHARVESTN2,index,config)),(harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen)*stand->frac);
    }
    else
    {
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_HARVESTC,index,config)),
                     &(getoutputindex(output,PFT_HARVESTC2,index,config)),harvest.harvest.carbon);
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_HARVESTN,index,config)),
                     &(getoutputindex(output,PFT_HARVESTN2,index,config)),harvest.harvest.nitrogen);
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_RHARVESTC,index,config)),
                     &(getoutputindex(output,PFT_RHARVESTC2,index,config)),harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon);
      separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                     &(getoutputindex(output,PFT_RHARVESTN,index,config)),
                     &(getoutputindex(output,PFT_RHARVESTN2,index,config)),harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen);
    }
    /* harvested area */
    separate_harvests(output->syear2[pft->par->id-npft+data->irrigation*ncft],
                   &(getoutputindex(output,CFTFRAC,index,config)),
                   &(getoutputindex(output,CFTFRAC2,index,config)),stand->frac);
    if(output->syear2[pft->par->id-npft+data->irrigation*ncft]>0)
      getoutputindex(output,SDATE2,pft->par->id-npft+data->irrigation*ncft,config)=crop->sh->sdate;
    else
      getoutputindex(output,SDATE,pft->par->id-npft+data->irrigation*ncft,config)=crop->sh->sdate;
  }
  else
  {
    if(config->pft_output_scaled)
    {
#if defined IMAGE && defined COUPLED
      stand->cell->pft_harvest[index]+=harvest.harvest.carbon*stand->frac;
#endif
      getoutputindex(output,PFT_HARVESTC,index,config)+=harvest.harvest.carbon*stand->frac;
      getoutputindex(output,PFT_HARVESTN,index,config)+=harvest.harvest.nitrogen*stand->frac;
      getoutputindex(output,PFT_RHARVESTC,index,config)+=(harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon)*stand->frac;
      getoutputindex(output,PFT_RHARVESTN,index,config)+=(harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen)*stand->frac;
    }
    else
    {
#if defined IMAGE && defined COUPLED
      stand->cell->pft_harvest[index]+=harvest.harvest.carbon;
#endif
      getoutputindex(output,PFT_HARVESTC,index,config)+=harvest.harvest.carbon;
      getoutputindex(output,PFT_HARVESTN,index,config)+=harvest.harvest.nitrogen;
      getoutputindex(output,PFT_RHARVESTC,index,config)+=(harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon);
      getoutputindex(output,PFT_RHARVESTN,index,config)+=(harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen);
    }
    /* harvested area */
    getoutputindex(output,CFTFRAC,index,config)=stand->frac;
    getoutputindex(output,CFT_NHARVEST,index,config)+=1.0;
  }
  if(isannual(PFT_NLEAF,config))
    getoutputindex(output,PFT_NLEAF,nnat+index,config)+=crop->ind.leaf.nitrogen;
  if(isannual(PFT_NLIMIT,config))
    getoutputindex(output,PFT_NLIMIT,nnat+index,config)+=pft->nlimit;
  if(isannual(PFT_CLEAF,config))
    getoutputindex(output,PFT_CLEAF,nnat+index,config)+=crop->ind.leaf.carbon;
  if(isannual(PFT_NROOT,config))
    getoutputindex(output,PFT_NROOT,nnat+index,config)+=crop->ind.root.nitrogen;
  if(isannual(PFT_CROOT,config))
    getoutputindex(output,PFT_CROOT,nnat+index,config)+=crop->ind.root.carbon;
  if(isannual(PFT_VEGN,config))
    getoutputindex(output,PFT_VEGN,nnat+index,config)+=vegn_sum(pft);
  if(isannual(PFT_VEGC,config))
    getoutputindex(output,PFT_VEGC,nnat+index,config)+=vegc_sum(pft);

  getoutput(output,HARVESTC,config)+=(harvest.harvest.carbon+harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon)*stand->frac;
  getoutput(output,HARVESTC_AGR,config)+=(harvest.harvest.carbon+harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon)*stand->frac;
  getoutput(output,HARVESTN,config)+=(harvest.harvest.nitrogen+harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen)*stand->frac;
  stand->cell->balance.flux_harvest.carbon+=(harvest.harvest.carbon+harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon)*stand->frac;
  stand->cell->balance.flux_harvest.nitrogen+=(harvest.harvest.nitrogen+harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen)*stand->frac;
  getoutput(output,HARVESTN_AGR,config)+=(harvest.harvest.nitrogen+harvest.residual.nitrogen+harvest.residuals_burnt.nitrogen+harvest.residuals_burntinfield.nitrogen)*stand->frac;
  output->dcflux+=(harvest.harvest.carbon+harvest.residual.carbon+harvest.residuals_burnt.carbon+harvest.residuals_burntinfield.carbon)*stand->frac;
  getoutput(output,RHARVEST_BURNTC,config)+=harvest.residuals_burnt.carbon*stand->frac;
  getoutput(output,RHARVEST_BURNTN,config)+=harvest.residuals_burnt.nitrogen*stand->frac;
  getoutput(output,RHARVEST_BURNT_IN_FIELDC,config)+=harvest.residuals_burntinfield.carbon*stand->frac;
  getoutput(output,RHARVEST_BURNT_IN_FIELDN,config)+=harvest.residuals_burntinfield.nitrogen*stand->frac;
  croppar=pft->par->data;
  stand->cell->ml.cropdates[pft->par->id-npft].fallow[data->irrigation]=croppar->fallow_days;
} /* of 'harvest_crop' */
