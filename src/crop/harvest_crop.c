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
#include "agriculture.h"
#include "crop.h"

/*
 *  Called in function daily_agriculture when crop
 *  is harvested
 *
 */
void harvest_crop(Output *output,        /**< Output data */
                  Stand *stand,          /**< pointer to crop stand */
                  Pft *pft,              /**< PFT variables */
                  int npft,              /**< number of natural PFTs */
                  int ncft,              /**< number of crop PFTs */
                  Bool remove_residuals, /**< remove residuals after harvest (TRUE/FALSE) */
                  Bool residues_fire,    /**< fire in residuals after harvest (TRUE/FALSE) */
                  Bool pft_output_scaled /**< pft-specific output scaled with
                                              stand->frac (TRUE/FALSE) */
                 )
{
  Pftcroppar *croppar;
  Harvest harvest;
  Pftcrop *crop;
  Irrigation *data;
  Real fuelratio,bifratio,factor;
  data=stand->data;
  crop=pft->data;
  stand->soil.litter.ag[pft->litter].trait.leaf+=(crop->ind.leaf+crop->ind.pool)*param.residues_in_soil;
  if(!residues_fire)
  {
    harvest.residuals_burnt=harvest.residuals_burntinfield=0;
    factor=(1-param.residues_in_soil);
  }
  else
  {
    fuelratio=stand->cell->ml.manage.regpar->fuelratio; /* burn outside of field */
    bifratio=stand->cell->ml.manage.regpar->bifratio; /* burn in field */
    if(bifratio+fuelratio>(1-param.residues_in_soil))
    {
      bifratio*=(1-param.residues_in_soil);
      fuelratio*=(1-param.residues_in_soil);
    }
    factor=1-param.residues_in_soil-fuelratio-bifratio;
    harvest.residuals_burnt=(crop->ind.leaf+crop->ind.pool)*fuelratio;
    harvest.residuals_burntinfield=(crop->ind.leaf+crop->ind.pool)*bifratio;
  }
  if(remove_residuals)
    harvest.residual=(crop->ind.leaf+crop->ind.pool)*factor;
  else
  {
    stand->soil.litter.ag[pft->litter].trait.leaf+=(crop->ind.leaf+crop->ind.pool)*factor;
    harvest.residual=0;
  }
  harvest.harvest=crop->ind.so;
  stand->soil.litter.bg[pft->litter]+=crop->ind.root;
#ifdef DOUBLE_HARVEST
  if(pft_output_scaled)
  {
    double_harvest(output->syear2[pft->par->id-npft+data->irrigation*ncft],
      &(output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest),
      &(output->pft_harvest2[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest),harvest.harvest*stand->frac);
    double_harvest(output->syear2[pft->par->id-npft+data->irrigation*ncft],
      &(output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual),
      &(output->pft_harvest2[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual),(harvest.residual+harvest.residuals_burnt+harvest.residuals_burntinfield)*stand->frac);
  }
  else
  {
    double_harvest(output->syear2[pft->par->id-npft+data->irrigation*ncft],
      &(output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest),
      &(output->pft_harvest2[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest),harvest.harvest);
    double_harvest(output->syear2[pft->par->id-npft+data->irrigation*ncft],
      &(output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual),
      &(output->pft_harvest2[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual),harvest.residual+harvest.residuals_burnt+harvest.residuals_burntinfield);
  }
  /* harvested area */
  double_harvest(output->syear2[pft->par->id-npft+data->irrigation*ncft],
    output->cftfrac+pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE),
    output->cftfrac2+pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE),stand->frac);
  if(output->syear2[pft->par->id-npft+data->irrigation*ncft]>0)
    output->sdate2[pft->par->id-npft+data->irrigation*ncft]=crop->sdate;
  else
    output->sdate[pft->par->id-npft+data->irrigation*ncft]=crop->sdate;
#else
  if(pft_output_scaled)
  {
    output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest*stand->frac;
    output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual+=(harvest.residual+harvest.residuals_burnt+harvest.residuals_burntinfield)*stand->frac;
  }
  else
  {
    output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest;
    output->pft_harvest[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual+=(harvest.residual+harvest.residuals_burnt+harvest.residuals_burntinfield);
  }
  /* harvested area */
  output->cftfrac[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]=stand->frac;
#endif


  output->flux_harvest+=(harvest.harvest+harvest.residual+harvest.residuals_burnt+harvest.residuals_burntinfield)*stand->frac;
  output->dcflux+=(harvest.harvest+harvest.residual+harvest.residuals_burnt+harvest.residuals_burntinfield)*stand->frac;
  output->flux_rharvest_burnt+=harvest.residuals_burnt*stand->frac;
  output->flux_rharvest_burnt_in_field+=harvest.residuals_burntinfield*stand->frac;
  croppar=pft->par->data;
  if(data->irrigation)
    stand->cell->ml.cropdates[pft->par->id-npft].fallow_irrig=croppar->fallow_days;
  else
    stand->cell->ml.cropdates[pft->par->id-npft].fallow=croppar->fallow_days;
} /* of 'harvest_crop' */
