/**************************************************************************************/
/**                                                                                \n**/
/**              d i s t r i b u t e _ w a t e r . c                               \n**/
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
#include "agriculture.h"

void distribute_water(Cell *cell,             /* cell pointer */
                      int irrig_scenario,     /* irrigation scenario */
                      Bool pft_output_scaled,
                      int npft,               /* number of natural PFTs */
                      int ncft                /* number of crop PFTs */
                     )
{
  int s,p,l,m,count;
  Real wr;
  Real conv_loss,irrig_stand;
  Real frac_irrig_amount,frac_unsustainable,aprec,irrig_threshold;
  Stand *stand;
  Pft *pft;
  Irrigation *data;
#ifdef DOUBLE_HARVEST
  Pftcrop *crop;
#endif
  aprec=irrig_threshold=0.0;
  conv_loss=0.0;

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
    cell->output.awd_unsustainable+=frac_unsustainable*cell->discharge.gir;
    cell->output.mwd_unsustainable+=frac_unsustainable*cell->discharge.gir;
  }

  cell->discharge.withdrawal=0.0;

  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE)
    {
      data=stand->data;
      data->irrig_event=0;
      data->irrig_amount=0;

      if(data->irrigation)
      {
        /* determine if irrigation today */
        for(m=0;m<NMONTH;m++)
          aprec+=max(0,stand->cell->climbuf.mprec20[m]);
        count=0;
        foreachpft(pft,p,&stand->pftlist)
        {
          wr=0;
          for(l=0;l<LASTLAYER;l++)
            wr+=pft->par->rootdist[l]*(stand->soil.w[l]+stand->soil.ice_depth[l]/stand->soil.par->whcs[l]);

          if(pft->par->path==C3)
          {
            if(aprec<param.aprec_lim)
              irrig_threshold=param.irrig_threshold_c3_dry;
            else
              irrig_threshold=param.irrig_threshold_c3_humid;
          }
          else
            irrig_threshold=param.irrig_threshold_c4;
          if(pft->par->id==RICE)
           irrig_threshold=param.irrig_threshold_rice;

          count+=(wr>irrig_threshold) ? 0 : 1; /* if single grass pft needs irrigation both grass pft are irrigated */
        } /*for each pft*/
        data->irrig_event=(count>0) ? 1 : 0;

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
#ifdef DOUBLE_HARVEST
              crop=pft->data;
              crop->nirsum+=data->net_irrig_amount;
#else
              if(pft_output_scaled)
                stand->cell->output.cft_nir[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->frac;
              else
                stand->cell->output.cft_nir[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
#endif
              break;
            case GRASSLAND: 
              if(pft_output_scaled)
              {
                stand->cell->output.cft_nir[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].grass[0];
                stand->cell->output.cft_nir[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].grass[1];
              }
              else
              {
                stand->cell->output.cft_nir[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
                stand->cell->output.cft_nir[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
              }
              break;
            case BIOMASS_GRASS:
              if(pft_output_scaled)
                stand->cell->output.cft_nir[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].biomass_grass;
              else
                stand->cell->output.cft_nir[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
              break;
            default:
              if(pft_output_scaled)
                stand->cell->output.cft_nir[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount*stand->cell->ml.landfrac[1].biomass_tree;
              else
                stand->cell->output.cft_nir[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=data->net_irrig_amount;
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
            cell->output.mconv_loss_evap+=conv_loss*data->conv_evap*stand->frac;

		    /* write cft-specific conveyance losses */
		    /* not written within irrig_event loop, because irrig_stor consists of water already transported to the field */
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
              stand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].grass[0];
              stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].grass[1];
              stand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].grass[0];
              stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].grass[1];
            }
            else
            {
              stand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
              stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
              stand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
              stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
            }
            break;
          case BIOMASS_GRASS:
            if(pft_output_scaled)
            {
              stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].biomass_grass;
              stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_grass;
            }
            else
            {
              stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
              stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
            }
            break;
          default:
            if(pft_output_scaled)
            {
              stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap*stand->cell->ml.landfrac[1].biomass_tree;
              stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_tree;
            }
            else
            {
              stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*data->conv_evap;
              stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=conv_loss*(1-data->conv_evap);
            }
        } /* of switch */

      } /* if data->irrigation = TRUE */
    }/* end of for each stand loop */
}
