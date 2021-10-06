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

void irrig_amount(Stand *stand,        /**< pointer to non-natural stand */
                  Irrigation *data,    /**< Irrigation data */
                  int npft,            /**< number of natural PFTs */
                  int ncft,            /**< number of crop PFTs */
                  int month,           /**< month (0..11) */
                  const Config *config /**< LPJmL configuration */
                 )
{
  int p,nirrig;
  Pft *pft;
  Real conv_loss,irrig_stand,irrig_threshold;
  Real wr;
  Pftcrop *crop;
  irrig_threshold=0.0;

  /* determine if today irrigation dependent on threshold */
  data->irrig_event=FALSE;
  data->irrig_amount=0;
  nirrig=getnirrig(ncft,config);

  if(data->irrigation)
  {
    foreachpft(pft,p,&stand->pftlist)
    {
      wr=getwr(&stand->soil,pft->par->rootdist);
      if(!strcmp(pft->par->name,"rice"))
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
          crop=pft->data;
          if(crop->dh!=NULL)
            crop->dh->nirsum+=data->net_irrig_amount;
          else
          {
            if(config->pft_output_scaled)
              getoutputindex(&stand->cell->output,CFT_NIR,pft->par->id-npft+nirrig,config)+=data->net_irrig_amount*stand->frac;
            else
              getoutputindex(&stand->cell->output,CFT_NIR,pft->par->id-npft+nirrig,config)+=data->net_irrig_amount;
          }
          break;
        case AGRICULTURE_GRASS : case AGRICULTURE_TREE:
          if(config->pft_output_scaled)
            getoutputindex(&stand->cell->output,CFT_NIR,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+nirrig,config)+=data->net_irrig_amount*stand->frac;
          else
            getoutputindex(&stand->cell->output,CFT_NIR,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+nirrig,config)+=data->net_irrig_amount;
          break;
        case BIOMASS_GRASS:
          if(config->pft_output_scaled)
            getoutputindex(&stand->cell->output,CFT_NIR,rbgrass(ncft)+nirrig,config)+=data->net_irrig_amount*stand->cell->ml.landfrac[1].biomass_grass;
          else
            getoutputindex(&stand->cell->output,CFT_NIR,rbgrass(ncft)+nirrig,config)+=data->net_irrig_amount;
          break;
        case GRASSLAND:
          if(config->pft_output_scaled)
          {
            getoutputindex(&stand->cell->output,CFT_NIR,rothers(ncft)+nirrig,config)+=data->net_irrig_amount*stand->cell->ml.landfrac[1].grass[0];
            getoutputindex(&stand->cell->output,CFT_NIR,rmgrass(ncft)+nirrig,config)+=data->net_irrig_amount*stand->cell->ml.landfrac[1].grass[1];
          }
          else
          {
            getoutputindex(&stand->cell->output,CFT_NIR,rothers(ncft)+nirrig,config)+=data->net_irrig_amount;
            getoutputindex(&stand->cell->output,CFT_NIR,rmgrass(ncft)+nirrig,config)+=data->net_irrig_amount;
          }
          break;
        case WOODPLANTATION:
          if (config->pft_output_scaled)
            getoutputindex(&stand->cell->output,CFT_NIR,rwp(ncft) +nirrig,config)+= data->net_irrig_amount*stand->cell->ml.landfrac[1].woodplantation;
          else
            getoutputindex(&stand->cell->output,CFT_NIR,rwp(ncft) +nirrig,config)+= data->net_irrig_amount;
          break;
        case BIOMASS_TREE:
          if(config->pft_output_scaled)
            getoutputindex(&stand->cell->output,CFT_NIR,rbtree(ncft)+nirrig,config)+=data->net_irrig_amount*stand->cell->ml.landfrac[1].biomass_tree;
          else
            getoutputindex(&stand->cell->output,CFT_NIR,rbtree(ncft)+nirrig,config)+=data->net_irrig_amount;
          break;
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
    getoutput(&stand->cell->output,CONV_LOSS_DRAIN,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
    stand->cell->balance.aconv_loss_drain+=conv_loss*(1-data->conv_evap)*stand->frac;
    getoutput(&stand->cell->output,CONV_LOSS_EVAP,config)+=conv_loss*data->conv_evap*stand->frac;
    stand->cell->balance.aconv_loss_evap+=conv_loss*data->conv_evap*stand->frac;
#if defined(IMAGE) && defined(COUPLED)
    if(stand->cell->ml.image_data!=NULL)
    {
      stand->cell->ml.image_data->mirrwatdem[month]+=conv_loss*stand->frac;
      stand->cell->ml.image_data->mevapotr[month] += conv_loss*stand->frac;
    }
#endif

    /* write cft-specific conveyance losses, ATTENTION: full conv losses incl. evaporation and drainage */
    switch(stand->type->landusetype)
    {
      case AGRICULTURE:
        pft=getpft(&stand->pftlist,0);
        if(config->pft_output_scaled)
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,pft->par->id-npft+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,pft->par->id-npft+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,pft->par->id-npft+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,pft->par->id-npft+nirrig,config)+=conv_loss*(1-data->conv_evap);
        }
        break;
      case AGRICULTURE_TREE: case AGRICULTURE_GRASS:
        if(config->pft_output_scaled)
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+nirrig,config)+=conv_loss*(1-data->conv_evap);
        }
        break;
      case GRASSLAND:
        if(config->pft_output_scaled)
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rothers(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].grass[0];
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rmgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].grass[1];
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rothers(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].grass[0];
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rmgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].grass[1];
        }
        else
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rothers(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rmgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rothers(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rmgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
        }
        break;
      case BIOMASS_GRASS:
        if(config->pft_output_scaled)
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rbgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].biomass_grass;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rbgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_grass;
        }
        else
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rbgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rbgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
        }
        break;
      case WOODPLANTATION:
        if(config->pft_output_scaled)
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rwp(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].woodplantation;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rwp(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].woodplantation;
        }
        else
        {
          getoutputindex(&stand->cell->output,CFT_CONV_LOSS_EVAP,rwp(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output,CFT_CONV_LOSS_DRAIN,rwp(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
        }
        break;
      case BIOMASS_TREE:
        if(config->pft_output_scaled)
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rbtree(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].biomass_tree;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rbtree(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_tree;
        }
        else
        {
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_EVAP ,rbtree(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
          getoutputindex(&stand->cell->output, CFT_CONV_LOSS_DRAIN ,rbtree(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
        }
        break;
    } /* of switch */
  } /* if data->irrigation */
}   /* of 'irrig_amount' */
