/**************************************************************************************/
/**                                                                                \n**/
/**     d  a  i  l  y  _  b  i  o  m  a  s  s  _  g  r  a  s  s  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of grassland stand                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"
#include "agriculture.h"
#include "biomass_grass.h"

Real daily_biomass_grass(Stand *stand, /**< stand pointer */
                         Real co2,   /**< atmospheric CO2 (ppmv) */
                         const Dailyclimate *climate, /**< Daily climate values */
                         int day,    /**< day (1..365) */
                         Real daylength, /**< length of day (h) */
                         const Real gp_pft[], /**< pot. canopy conductance for PFTs & CFTs*/
                         Real gtemp_air,  /**< value of air temperature response function */
                         Real gtemp_soil, /**< value of soil temperature response function */
                         Real gp_stand,   /* potential stomata conductance */
                         Real gp_stand_leafon, /**< pot. canopy conduct.at full leaf cover */
                         Real eeq,   /**< equilibrium evapotranspiration (mm) */
                         Real par,   /**< photosynthetic active radiation flux */
                         Real melt,  /**< melting water (mm) */
                         int npft,   /**< number of natural PFTs */
                         int ncft,   /**< number of crop PFTs   */
                         int UNUSED(year), /**< simulation year */
                         Bool UNUSED(withdailyoutput),
                         Bool  UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                         const Config *config /**< LPJ config */
                        )            /** \return runoff (mm) */
{
  int p,l;
  Pft *pft;
  Output *output;
  Harvest harvest={0,0,0,0};
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap,evap_blue,rd,gpp,frac_g_evap,runoff,wet_all,intercept,sprink_interc;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Soil *soil;
  Real npp; /* net primary productivity (gC/m2) */
  Real gc_pft,gcgp;
  Real wdf; /* water deficit fraction */
  Bool isphen;
  Irrigation *data;
  Pftgrass *grass;
  irrig_apply=0.0;

  soil = &stand->soil;
  data=stand->data;
  output=&stand->cell->output;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=runoff=return_flow_b=wet_all=intercept=sprink_interc=0;
  if(getnpft(&stand->pftlist)>0)
  {
    wet=newvec(Real,getnpft(&stand->pftlist)); /* wet from pftlist */
    check(wet);
    for(p=0;p<getnpft(&stand->pftlist);p++)
      wet[p]=0;
  }
  else
    wet=NULL;

  if(!config->river_routing)
    irrig_amount(stand,config->pft_output_scaled,npft,ncft);

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;

  if(data->irrigation && data->irrig_amount>epsilon)
  {
    irrig_apply=max(data->irrig_amount-rainmelt,0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrig_stor+=data->irrig_amount-irrig_apply;
    data->irrig_amount=0.0;
    if(irrig_apply<1 && data->irrig_system!=DRIP) /* min. irrigation requirement of 1mm */
    {
      data->irrig_stor+=irrig_apply;
      irrig_apply=0.0;
    }
    else
    {
      /* write irrig_apply to output */
      stand->cell->output.mirrig+=irrig_apply*stand->frac;
      if(config->pft_output_scaled)
        stand->cell->output.cft_airrig[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=irrig_apply*stand->cell->ml.landfrac[1].biomass_grass;
      else
        stand->cell->output.cft_airrig[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=irrig_apply;
    }
  }

  /* INTERCEPTION */
  foreachpft(pft,p,&stand->pftlist)
  {
    sprink_interc=(data->irrig_system==SPRINK) ? 1 : 0;

    intercept=interception(&wet[p],pft,eeq,climate->prec+irrig_apply*sprink_interc);
    wet_all+=wet[p]*pft->fpc;
    intercep_stand_blue+=(climate->prec+irrig_apply*sprink_interc>epsilon) ? intercept*(irrig_apply*sprink_interc)/(climate->prec+irrig_apply*sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand+=intercept;
  }
  irrig_apply-=intercep_stand_blue;
  rainmelt-=(intercep_stand-intercep_stand_blue);

  /* soil inflow: infiltration and percolation */
  if(irrig_apply>epsilon)
  {
    runoff+=infil_perc_irr(stand,irrig_apply,&return_flow_b,config->rw_manage);
    /* count irrigation events*/
    output->cft_irrig_events[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
  }

  runoff+=infil_perc_rain(stand,rainmelt,&return_flow_b,config->rw_manage);

  isphen=FALSE;
  foreachpft(pft,p,&stand->pftlist)
  {
    leaf_phenology(pft,climate->temp,day,climate->isdailytemp);
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT */
    albedo_grass(pft, soil->snowheight, soil->snowfraction);

/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */

    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,config->permafrost);

    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      gcgp=gc_pft/gp_pft[getpftpar(pft,id)];
      if(stand->cell->ml.landfrac[data->irrigation].biomass_grass>0.0)
      {
        output->gcgp_count[(npft-config->nbiomass)+rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]++;
        output->pft_gcgp[(npft-config->nbiomass)+rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=gcgp;
      }
    }
    npp=npp_grass(pft,gtemp_air,gtemp_soil,gpp-rd);
    output->mnpp+=npp*stand->frac;
    output->dcflux-=npp*stand->frac;
    output->mgpp+=gpp*stand->frac;
    output->mfapar += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_tmin += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_tmax += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_light += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_water += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mwscal += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));

    output->cft_fpar[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation].biomass_grass*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

    if(config->pft_output_scaled)
      output->pft_npp[(npft-config->nbiomass)+rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=npp*stand->cell->ml.landfrac[data->irrigation].biomass_grass;
    else
      output->pft_npp[(npft-config->nbiomass)+rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=npp;
  } /* of foreachpft */

  /* calculate water balance */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);

  /* allocation, turnover and harvest AFTER photosynthesis */
  stand->growing_days = 1;
  /* turnover must happen before allocation */
  foreachpft(pft,p,&stand->pftlist)
    turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR);
  allocation_today(stand,config->ntypes);

  /* daily turnover and harvest check*/
  isphen=FALSE;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    grass->max_leaf=max(grass->max_leaf,grass->ind.leaf);
    if (grass->ind.leaf >= 350 || (grass->ind.leaf>1 && grass->ind.leaf<(0.75*grass->max_leaf))) //changed from 100 to 350
      isphen=TRUE;
 } /* of foreachpft() */

  if(isphen)
  {
    harvest=harvest_stand(output,stand,0.75);
    if(data->irrigation)
    {
      stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
      output->mstor_return+=(data->irrig_stor+data->irrig_amount)*stand->frac;

      /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
      stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      output->aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      output->aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;

      if(config->pft_output_scaled)
      {
        stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[1].biomass_grass;
        stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_grass;
      }
      else
      {
        stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
        stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
      }

      data->irrig_stor=0;
      data->irrig_amount=0;
    }
  }

  if(data->irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,gp_stand,wet,eeq);

  forrootsoillayer(l)
  {
    output->mtransp+=aet_stand[l]*stand->frac;
    output->mtransp_b+=(aet_stand[l]-green_transp[l])*stand->frac;
  }

  output->mevap+=evap*stand->frac;
  output->mevap_b+=evap_blue*stand->frac;   /* blue soil evap */

  output->minterc+=intercep_stand*stand->frac;
  output->minterc_b+=intercep_stand_blue*stand->frac;   /* blue interception and evap */

  output->mreturn_flow_b+=return_flow_b*stand->frac;

  if(config->pft_output_scaled)
    output->pft_harvest[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest*stand->cell->ml.landfrac[data->irrigation].biomass_grass;
  else
    output->pft_harvest[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest;

    /* harvested area */
  if(isphen)
    output->cftfrac[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]=stand->cell->ml.landfrac[data->irrigation].biomass_grass;
  output_gbw_biomass_grass(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
                           intercep_stand,intercep_stand_blue,ncft,config->pft_output_scaled);
  free(wet);
  return runoff;
} /* of 'daily_biomass_grass' */
