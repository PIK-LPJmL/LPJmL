/**************************************************************************************/
/**                                                                                \n**/
/**                i  r  r  i  g  _  a  m  o  u  n  t  .  c                        \n**/
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

void irrig_amount(Stand *stand, /**< pointer to non-natural stand */
                  Irrigation *data, /**< Irrigation data */
                  Bool pft_output_scaled, /**< output is PFT scaled (TRUE/FALSE) */
                  int npft,     /**< number of natural PFTs */
                  int ncft      /**< number of crop PFTs */
                 )
{
  int p;
  Pft *pft;
  Real conv_loss,irrig_stand,irrig_threshold;
  Real wr;
#ifdef DOUBLE_HARVEST
  Pftcrop *crop;
#endif
  irrig_threshold=0.0;

  /* determine if today irrigation dependent on threshold */
  data->irrig_event=FALSE;
  data->irrig_amount=0;

  if(data->irrigation)
  {
    foreachpft(pft,p,&stand->pftlist)
    {
      wr=getwr(&stand->soil,pft->par->rootdist);

      if(pft->par->id==RICE)
        irrig_threshold=param.irrig_threshold_rice;
      else
      {
        if(pft->par->path==C3)
        {
          if(stand->cell->climbuf.aprec<param.aprec_lim)
            irrig_threshold=param.irrig_threshold_c3_dry;
          else
            irrig_threshold=param.irrig_threshold_c3_humid;
        }
        else
          irrig_threshold=param.irrig_threshold_c4;
      }

      if(wr>irrig_threshold)
        data->irrig_event=TRUE; /* if one of possibly two (grass) pfts requests irrigation, both get irrigated */
    } /*for each pft*/

    irrig_stand=max(data->net_irrig_amount+data->dist_irrig_amount-data->irrig_stor,0);

    /* conveyance losses */
    conv_loss=irrig_stand*(1/data->ec-1);

    if(data->irrig_event)
    {

      /* net irrigation requirement */
      pft=getpft(&stand->pftlist,0);
      switch(stand->type->landusetype)
      {
        case AGRICULTURE:
#ifdef DOUBLE_HARVEST
          crop=pft->data;
          crop->nirsum+=data->net_irrig_amount;
#else
          if(pft_output_scaled)
            stand->cell->output.cft_nir[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->frac;
          else
            stand->cell->output.cft_nir[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
#endif
          break;
        case BIOMASS_GRASS:
          if(pft_output_scaled)
            stand->cell->output.cft_nir[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].biomass_grass;
          else
            stand->cell->output.cft_nir[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
          break;
        case GRASSLAND:
          if(pft_output_scaled)
          {
            stand->cell->output.cft_nir[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].grass[0];
            stand->cell->output.cft_nir[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].grass[1];
          }
          else
          {
            stand->cell->output.cft_nir[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
            stand->cell->output.cft_nir[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
          }
          break;
        default:
          if(pft_output_scaled)
            stand->cell->output.cft_nir[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].biomass_tree;
          else
            stand->cell->output.cft_nir[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
      } /* of switch */

      data->irrig_amount=data->irrig_stor+irrig_stand;
      data->irrig_stor=0.0;

      if(data->irrig_amount>(data->net_irrig_amount+data->dist_irrig_amount))
      {
        data->irrig_stor=data->irrig_amount-(data->net_irrig_amount+data->dist_irrig_amount);
        data->irrig_amount=data->net_irrig_amount+data->dist_irrig_amount;
      }
    }
    else
    {
      data->irrig_stor+=irrig_stand;
      data->irrig_amount=0.0;
    }

    data->net_irrig_amount=data->dist_irrig_amount=0.0;
    stand->cell->output.mconv_loss_drain+=conv_loss*(1-data->conv_evap)*stand->frac;
    stand->cell->output.mconv_loss_evap+=conv_loss*data->conv_evap*stand->frac;

    /* write cft-specific conveyance losses, ATTENTION: full conv losses incl. evaporation and drainage */
    switch(stand->type->landusetype)
    {
      case AGRICULTURE:
        pft=getpft(&stand->pftlist,0);
        if(pft_output_scaled)
        {
          stand->cell->output.cft_conv_loss_evap[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->frac;
          stand->cell->output.cft_conv_loss_drain[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          stand->cell->output.cft_conv_loss_evap[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
          stand->cell->output.cft_conv_loss_drain[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
        }
        break;
      case GRASSLAND:
        if(pft_output_scaled)
        {
          stand->cell->output.cft_conv_loss_evap[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].grass[0];
          stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].grass[1];
          stand->cell->output.cft_conv_loss_drain[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].grass[0];
          stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].grass[1];
        }
        else
        {
          stand->cell->output.cft_conv_loss_evap[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
          stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
          stand->cell->output.cft_conv_loss_drain[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
          stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
        }
        break;
      case BIOMASS_GRASS:
        if(pft_output_scaled)
        {
          stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].biomass_grass;
          stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_grass;
        }
        else
        {
          stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
          stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
        }
        break;
      default:
        if(pft_output_scaled)
        {
          stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].biomass_tree;
          stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_tree;
        }
        else
        {
          stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
          stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
        }
    } /* of switch */
  } /* if data->irrigation */

}   /* of 'irrig_amount' */
