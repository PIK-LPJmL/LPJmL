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
                      int irrig_scenario,    /**< irrigation scenario */
                      int pft_output_scaled, /**< output PFT scaled? (TRUE/FALSE) */
                      int npft,              /**< number of natural PFTs */
                      int ncft,              /**< number of crop PFTs */
                      int month              /**< month of year (0..11) */
                     )
{
  int s,p,count;
  Real wr;
  Real conv_loss,irrig_stand;
  Real frac_irrig_amount,frac_unsustainable,irrig_threshold;
#ifdef IMAGE
  Real frac_sw;
#endif
  Stand *stand;
  Pft *pft;
  Irrigation *data;
  Pftcrop *crop;
  irrig_threshold=0.0;
  conv_loss=0.0;

#ifdef IMAGE
  /* actual irrigation requirement */
  if(irrig_scenario==LIM_IRRIGATION && cell->discharge.aquifer==0)
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
    cell->output.mwd_unsustainable+=frac_unsustainable*cell->discharge.gir/cell->coord.area; 
    if(cell->discharge.aquifer)
      cell->output.mwd_aq+=frac_unsustainable*cell->discharge.gir/cell->coord.area; 
  }
#else
   /* actual irrigation requirement */
  if(irrig_scenario==LIM_IRRIGATION)
  {
    frac_irrig_amount=cell->discharge.gir>0 ? cell->discharge.withdrawal/cell->discharge.gir : 0.0;
  }
  else
  {
    /* potential irrigation requirement */
    frac_irrig_amount=cell->discharge.gir>0 ? 1.0 : 0.0;
    frac_unsustainable=cell->discharge.gir>0 ? 1 - cell->discharge.withdrawal/cell->discharge.gir : 0.0;
    frac_unsustainable=frac_unsustainable>0 ? frac_unsustainable : 0.0;
    cell->balance.awd_unsustainable+=frac_unsustainable*cell->discharge.gir;
    cell->output.mwd_unsustainable+=frac_unsustainable*cell->discharge.gir;
  }
#endif

  cell->discharge.withdrawal=0.0;
#ifdef IMAGE
  cell->discharge.withdrawal_gw=0.0;
#endif
  foreachstand(stand,s,cell->standlist)
#ifdef IMAGE
    if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==WOODPLANTATION)
#else
    if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE)
#endif
    {
      data=stand->data;
      data->irrig_event=FALSE;
      data->irrig_amount=0;

      if(data->irrigation)
      {
        /* determine if irrigation today */
        count=0;
        foreachpft(pft,p,&stand->pftlist)
        {
          wr=getwr(&stand->soil,pft->par->rootdist);
          if(pft->par->path==C3)
          {
            if(cell->climbuf.aprec<param.aprec_lim)
              irrig_threshold=param.irrig_threshold_c3_dry;
            else
              irrig_threshold=param.irrig_threshold_c3_humid;
          }
          else
            irrig_threshold=param.irrig_threshold_c4;
          if(!strcmp(pft->par->name,"rice"))
            irrig_threshold=param.irrig_threshold_rice;

          count+=(wr>irrig_threshold) ? 0 : 1; /* if single grass pft needs irrigation both grass pft are irrigated */
        } /*for each pft*/
        data->irrig_event=(count>0);

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
              if(crop->dh!=NULL)
                crop->dh->nirsum+=data->net_irrig_amount;
              else
              {
                if(pft_output_scaled)
                  cell->output.cft_nir[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->frac;
                else
                  cell->output.cft_nir[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount;
              }
              break;
            case GRASSLAND:
              if(pft_output_scaled)
              {
                cell->output.cft_nir[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount*cell->ml.landfrac[1].grass[0];
                cell->output.cft_nir[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount*cell->ml.landfrac[1].grass[1];
              }
              else
              {
                cell->output.cft_nir[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount;
                cell->output.cft_nir[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount;
              }
              break;
            case BIOMASS_GRASS:
              if(pft_output_scaled)
                cell->output.cft_nir[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount*cell->ml.landfrac[1].biomass_grass;
              else
                cell->output.cft_nir[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount;
              break;
            default:
              if(pft_output_scaled)
                cell->output.cft_nir[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount*cell->ml.landfrac[1].biomass_tree;
              else
                cell->output.cft_nir[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->net_irrig_amount;
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
        cell->output.mconv_loss_drain+=conv_loss*(1-data->conv_evap)*stand->frac;
        cell->balance.aconv_loss_drain+=conv_loss*(1-data->conv_evap)*stand->frac;
        cell->output.mconv_loss_evap+=conv_loss*data->conv_evap*stand->frac;
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
            if(pft_output_scaled)
            {
              cell->output.cft_conv_loss_evap[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap*stand->frac;
              cell->output.cft_conv_loss_drain[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              cell->output.cft_conv_loss_evap[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap;
              cell->output.cft_conv_loss_drain[pft->par->id-npft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap);
            }
            break;
          case GRASSLAND:
            if(pft_output_scaled)
            {
              cell->output.cft_conv_loss_evap[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap*cell->ml.landfrac[1].grass[0];
              cell->output.cft_conv_loss_evap[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap*cell->ml.landfrac[1].grass[1];
              cell->output.cft_conv_loss_drain[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap)*cell->ml.landfrac[1].grass[0];
              cell->output.cft_conv_loss_drain[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap)*cell->ml.landfrac[1].grass[1];
            }
            else
            {
              cell->output.cft_conv_loss_evap[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap;
              cell->output.cft_conv_loss_evap[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap;
              cell->output.cft_conv_loss_drain[rothers(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap);
              cell->output.cft_conv_loss_drain[rmgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap);
            }
            break;
          case BIOMASS_GRASS:
            if(pft_output_scaled)
            {
              cell->output.cft_conv_loss_evap[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap*cell->ml.landfrac[1].biomass_grass;
              cell->output.cft_conv_loss_drain[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap)*cell->ml.landfrac[1].biomass_grass;
            }
            else
            {
              cell->output.cft_conv_loss_evap[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap;
              cell->output.cft_conv_loss_drain[rbgrass(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap);
            }
            break;
#ifdef IMAGE
          case WOODPLANTATION:
            if (pft_output_scaled)
            {
              cell->output.cft_conv_loss_evap[rwp(ncft) + (ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].woodplantation;
              cell->output.cft_conv_loss_drain[rwp(ncft) + (ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1 - data->conv_evap)*stand->cell->ml.landfrac[1].woodplantation;
            }
            else
            {
              cell->output.cft_conv_loss_evap[rwp(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap;
              cell->output.cft_conv_loss_drain[rwp(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap);
            }
            break;
#endif
          default:
            if(pft_output_scaled)
            {
              cell->output.cft_conv_loss_evap[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap*cell->ml.landfrac[1].biomass_tree;
              cell->output.cft_conv_loss_drain[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap)*cell->ml.landfrac[1].biomass_tree;
            }
            else
            {
              cell->output.cft_conv_loss_evap[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*data->conv_evap;
              cell->output.cft_conv_loss_drain[rbtree(ncft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=conv_loss*(1-data->conv_evap);
            }
        } /* of 'switch' */

      } /* if data->irrigation = TRUE */
    }/* end of for each stand loop */
} /* of 'distribute_water' */
