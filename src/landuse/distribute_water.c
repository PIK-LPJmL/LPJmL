/**************************************************************************************/
/**                                                                                \n**/
/**              d  i  s  t  r  i  b  u  t  e  _  w  a  t  e  r  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Mathematical description of the river routing algorithm can be             \n**/
/**     found in:                                                                  \n**/
/**     Rost, S., Gerten, D., Bondeau, A., Lucht, W., Rohwer, J.,                  \n**/
/**     Schaphoff, S.: Agricultural green and blue water consumption and           \n**/
/**     its influence on the global water system. Water Resources                  \n**/
/**     Research (in press).                                                       \n**/
/**                                                                                \n**/
/**     Parallel version using the Pnet library                                    \n**/
/**     Pnet is described in:                                                      \n**/
/**     W. von Bloh, 2008. Sequential and Parallel Implementation of               \n**/
/**     Networks. In P. beim Graben, C. Zhou, M. Thiel, and J. Kurths              \n**/
/**     (eds.), Lectures in Supercomputational Neuroscience, Dynamics in           \n**/
/**     Complex Brain Networks, Springer, New York, 279-318.                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void distribute_water(Cell *cell,            /**< pointer to LPJ cell */
                      int npft,              /**< number of natural PFTs */
                      int ncft,              /**< number of crop PFTs */
                      int month,             /**< month of year (0..11) */
                      const Config *config   /**< LPJmL configuration */
                     )
{
  int s,p,nirrig,isrice;
  Real conv_loss,irrig_stand;
  Real frac_irrig_amount,frac_unsustainable;
#ifdef IMAGE
  Real frac_sw;
#endif
  Stand *stand;
  Pft *pft;
  Irrigation *data;
  Pftcrop *crop;
  conv_loss=0.0;
  nirrig=getnirrig(ncft,config);
  isrice=FALSE;
#ifdef IMAGE
  /* actual irrigation requirement */
  if(config->irrig_scenario==LIM_IRRIGATION && cell->discharge.aquifer==0)
  {
    frac_irrig_amount=cell->discharge.gir>0 ? (cell->discharge.withdrawal+cell->discharge.withdrawal_gw)/cell->discharge.gir : 0.0;
    frac_sw=frac_irrig_amount>0 ? cell->discharge.withdrawal/(cell->discharge.withdrawal+cell->discharge.withdrawal_gw) : 0.0;
  }
  else
  {
    /* potential irrigation requirement */
    frac_irrig_amount=cell->discharge.gir>0 ? (cell->discharge.withdrawal+cell->discharge.withdrawal_gw+cell->discharge.irrig_unmet)/cell->discharge.gir : 0.0;
    frac_sw=frac_irrig_amount>0 ? cell->discharge.withdrawal/(cell->discharge.withdrawal+cell->discharge.withdrawal_gw+cell->discharge.irrig_unmet):0.0;
    frac_unsustainable=cell->discharge.gir>0 ? (cell->discharge.irrig_unmet/cell->discharge.gir) : 0.0;
    frac_unsustainable=frac_unsustainable>0 ? frac_unsustainable : 0.0;
    // coord area added to change to mm
    cell->balance.awd_unsustainable+=frac_unsustainable*cell->discharge.gir/cell->coord.area;
    getoutput(&cell->output,WD_UNSUST,config)+=frac_unsustainable*cell->discharge.gir/cell->coord.area;
    if(cell->discharge.aquifer)
      getoutput(&cell->output,WD_AQ,config)+=frac_unsustainable*cell->discharge.gir/cell->coord.area;
  }
#else
   /* actual irrigation requirement */
  if(config->irrig_scenario==LIM_IRRIGATION)
  {
    frac_irrig_amount=cell->discharge.gir>0 ? (cell->discharge.withdrawal+cell->discharge.withdrawal_gw)/cell->discharge.gir : 0.0;
  }
  else
  {
    /* potential irrigation requirement */
    frac_irrig_amount=cell->discharge.gir>0 ? 1.0 : 0.0;
    frac_unsustainable=cell->discharge.gir>0 ? 1 - (cell->discharge.withdrawal+cell->discharge.withdrawal_gw)/cell->discharge.gir : 0.0;
    frac_unsustainable=frac_unsustainable>0 ? frac_unsustainable : 0.0;
    cell->balance.awd_unsustainable+=frac_unsustainable*cell->discharge.gir;
    getoutput(&cell->output,WD_UNSUST,config)+=frac_unsustainable*cell->discharge.gir;
  }
#endif
  foreachstand(stand,s,cell->standlist)
    stand->soil.wa-=cell->discharge.withdrawal_gw/cell->coord.area*stand->frac * (1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));

  cell->discharge.withdrawal= cell->discharge.withdrawal_gw=0.0;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==WOODPLANTATION || stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS)
    {
      data=stand->data;
      data->irrig_event=FALSE;
      data->irrig_amount=0;
      foreachpft(pft, p, &stand->pftlist)
       if(!strcmp(pft->par->name,"rice")) isrice=TRUE;

      if(data->irrigation||isrice)
      {
        /* determine if irrigation today */
        data->irrig_event=isirrigevent(stand);

        irrig_stand=max(data->net_irrig_amount+data->dist_irrig_amount-data->irrig_stor,0)*frac_irrig_amount;

        /* conveyance losses */
        conv_loss=irrig_stand*(1/data->ec-1);

        if(data->irrig_event) /* if irrigation today */
        {
          switch(stand->type->landusetype)
          {
            case AGRICULTURE:
               /* write net irrigation requirement */
              pft=getpft(&stand->pftlist,0);
              crop=pft->data;
              if(config->pft_output_scaled)
              {
                if(crop->sh!=NULL)
                  crop->sh->nirsum+=data->net_irrig_amount*stand->frac;
                else
                  getoutputindex(&cell->output,CFT_NIR,pft->par->id-npft+nirrig,config)+=data->net_irrig_amount*stand->frac;
              }
              else
              {
                if(crop->sh!=NULL)
                  crop->sh->nirsum+=data->net_irrig_amount;
                else
                  getoutputindex(&cell->output,CFT_NIR,pft->par->id-npft+nirrig,config)+=data->net_irrig_amount;
              }
              break;
            case GRASSLAND:
              if(config->pft_output_scaled)
              {
                getoutputindex(&cell->output,CFT_NIR,rmgrass(ncft)+nirrig,config)+=data->net_irrig_amount*stand->frac;
              }
              else
              {
                getoutputindex(&cell->output,CFT_NIR,rmgrass(ncft)+nirrig,config)+=data->net_irrig_amount;
              }
              break;
            case OTHERS:
              if(config->pft_output_scaled)
              {
                getoutputindex(&cell->output,CFT_NIR,rothers(ncft)+nirrig,config)+=data->net_irrig_amount*stand->frac;
              }
              else
              {
                getoutputindex(&cell->output,CFT_NIR,rothers(ncft)+nirrig,config)+=data->net_irrig_amount;
              }
              break;
            case BIOMASS_GRASS:
              if(config->pft_output_scaled)
                getoutputindex(&cell->output,CFT_NIR,rbgrass(ncft)+nirrig,config)+=data->net_irrig_amount*stand->frac;
              else
                getoutputindex(&cell->output,CFT_NIR,rbgrass(ncft)+nirrig,config)+=data->net_irrig_amount;
              break;
            case AGRICULTURE_GRASS: case AGRICULTURE_TREE:
              if(config->pft_output_scaled)
                getoutputindex(&cell->output,CFT_NIR,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)+=data->net_irrig_amount*stand->frac;
              else
                getoutputindex(&cell->output,CFT_NIR,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)+=data->net_irrig_amount;
              break;
            case WOODPLANTATION:
              if(config->pft_output_scaled)
                getoutputindex(&cell->output,CFT_NIR,rwp(ncft)+nirrig,config)+=data->net_irrig_amount*stand->frac;
              else
                getoutputindex(&cell->output,CFT_NIR,rwp(ncft)+nirrig,config)+=data->net_irrig_amount;
              break;
            case BIOMASS_TREE:
              if(config->pft_output_scaled)
                getoutputindex(&cell->output,CFT_NIR,rbtree(ncft)+nirrig,config)+=data->net_irrig_amount*stand->frac;
              else
                getoutputindex(&cell->output,CFT_NIR,rbtree(ncft)+nirrig,config)+=data->net_irrig_amount;
              break;
          } /* of switch */

          data->irrig_amount=data->irrig_stor+irrig_stand;
          data->irrig_stor=0.0;
          /* excess water goes back to irrig_stor */
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

        cell->discharge.dmass_lake+=conv_loss*cell->coord.area*(1-data->conv_evap)*stand->frac; /* Note: conveyance losses are not included in return flow */
        cell->discharge.mfin+=conv_loss*cell->coord.area*(1-data->conv_evap)*stand->frac;
        getoutput(&cell->output,CONV_LOSS_DRAIN,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
        cell->balance.aconv_loss_drain+=conv_loss*(1-data->conv_evap)*stand->frac;
        getoutput(&cell->output,CONV_LOSS_EVAP,config)+=conv_loss*data->conv_evap*stand->frac;
        cell->balance.aconv_loss_evap+=conv_loss*data->conv_evap*stand->frac;
#if defined(IMAGE) && defined(COUPLED)
  if(cell->ml.image_data!=NULL)
  {
    cell->ml.image_data->mirrwatdem[month]+=conv_loss*stand->frac;
    cell->ml.image_data->mevapotr[month] += conv_loss*stand->frac;
  }
#endif

        /* write cft-specific conveyance losses */
        /* not written within irrig_event loop, because irrig_stor consists of water already transported to the field */
        switch(stand->type->landusetype)
        {
          case AGRICULTURE:
            pft=getpft(&stand->pftlist,0);
            if(config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,pft->par->id-npft+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,pft->par->id-npft+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,pft->par->id-npft+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,pft->par->id-npft+nirrig,config)+=conv_loss*(1-data->conv_evap);
            }
            break;
          case GRASSLAND:
            if(config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rmgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rmgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rmgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rmgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
            }
            break;
          case OTHERS:
            if(config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rothers(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rothers(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rothers(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rothers(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
            }
            break;
          case BIOMASS_GRASS:
            if(config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rbgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rbgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rbgrass(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rbgrass(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
            }
            break;
          case WOODPLANTATION:
            if (config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rwp(ncft) +nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rwp(ncft) +nirrig,config)+=conv_loss*(1 - data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rwp(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rwp(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
            }
            break;
          case AGRICULTURE_GRASS: case AGRICULTURE_TREE:
            if(config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)+=conv_loss*(1 - data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)+=conv_loss*(1 - data->conv_evap);
            }
            break;
          default:
            if(config->pft_output_scaled)
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rbtree(ncft)+nirrig,config)+=conv_loss*data->conv_evap*stand->frac;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rbtree(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(&cell->output, CFT_CONV_LOSS_EVAP ,rbtree(ncft)+nirrig,config)+=conv_loss*data->conv_evap;
              getoutputindex(&cell->output, CFT_CONV_LOSS_DRAIN ,rbtree(ncft)+nirrig,config)+=conv_loss*(1-data->conv_evap);
            }
        } /* of 'switch' */

      } /* if data->irrigation = TRUE */
    }/* end of for each stand loop */
} /* of 'distribute_water' */
