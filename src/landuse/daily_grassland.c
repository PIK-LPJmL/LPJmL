/**************************************************************************************/
/**                                                                                \n**/
/**             d  a  i  l  y  _  g  r  a  s  s  l  a  n  d  .  c                  \n**/
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
#include "grassland.h"

Real daily_grassland(Stand *stand, /**< stand pointer */
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
                     Bool withdailyoutput,
                     Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
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
  Real rw_apply; /*applied irrigation water from rainwater harvesting storage, counted as green water */
  Real cover_stand,intercep_pft;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Real npp; /* net primary productivity (gC/m2) */
  Real gc_pft,gcgp;
  Real wdf; /* water deficit fraction */
  Bool isphen;
  Irrigation *data;
  Pftgrass *grass;
  Real hfrac=0.5;
  Real cleaf=0,cleaf_max=0;
  irrig_apply=0.0;

  data=stand->data;
  output=&stand->cell->output;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=wet_all=rw_apply=intercept=sprink_interc=rainmelt=intercep_pft=0.0;
  runoff=return_flow_b=0.0;
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
      {
        stand->cell->output.cft_airrig[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=irrig_apply*stand->cell->ml.landfrac[data->irrigation].grass[0];
        stand->cell->output.cft_airrig[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=irrig_apply*stand->cell->ml.landfrac[data->irrigation].grass[1];
      }
      else
      {
        stand->cell->output.cft_airrig[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=irrig_apply;
        stand->cell->output.cft_airrig[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=irrig_apply;
      }
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

  /* rain-water harvesting*/
  if(!data->irrigation && config->rw_manage && rainmelt<5)
    rw_apply=rw_irrigation(stand,gp_stand,wet,eeq); /* Note: RWH supplementary irrigation is here considered green water */

  /* soil inflow: infiltration and percolation */
  if(irrig_apply>epsilon)
  {
    runoff+=infil_perc_irr(stand,irrig_apply,&return_flow_b,config->rw_manage);
    /* count irrigation events*/
    output->cft_irrig_events[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
    output->cft_irrig_events[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
  }

  runoff+=infil_perc_rain(stand,rainmelt+rw_apply,&return_flow_b,config->rw_manage);

  isphen = FALSE;
  foreachpft(pft,p,&stand->pftlist)
  {
    pft->phen = 1.0; /* phenology is calculated from biomass */
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT */
    albedo_grass(pft, stand->soil.snowheight, stand->soil.snowfraction);

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
      if(stand->cell->ml.landfrac[data->irrigation].grass[0]>0.0)
      {
        output->gcgp_count[(npft-config->nbiomass)+rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]++;
        output->pft_gcgp[(npft-config->nbiomass)+rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=gcgp;
      }
      if(stand->cell->ml.landfrac[data->irrigation].grass[1]>0.0)
      {
        output->gcgp_count[(npft-config->nbiomass)+rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]++;
        output->pft_gcgp[(npft-config->nbiomass)+rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=gcgp;
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


    output->cft_fpar[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation].grass[0]*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));
    output->cft_fpar[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation].grass[1]*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

    if(config->pft_output_scaled)
    {
      output->pft_npp[(npft-config->nbiomass)+rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=npp*stand->cell->ml.landfrac[data->irrigation].grass[0];
      output->pft_npp[(npft-config->nbiomass)+rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=npp*stand->cell->ml.landfrac[data->irrigation].grass[1];
    }
    else
    {
      output->pft_npp[(npft-config->nbiomass)+rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=npp;
      output->pft_npp[(npft-config->nbiomass)+rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=npp;
    }
    grass = pft->data;
    if(withdailyoutput)
      if(output->daily.cft == TEMPERATE_HERBACEOUS && data->irrigation == output->daily.irrigation)
      {
        output->daily.interc += intercep_pft;
        output->daily.npp += npp;
        output->daily.gpp += gpp;

        output->daily.croot += grass->ind.root;
        output->daily.cleaf += grass->ind.leaf;

        output->daily.rd += rd;
        output->daily.assim += gpp-rd;
      }
  }

  /* calculate water balance */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);
  /* allocation, turnover and harvest AFTER photosynthesis */
  stand->growing_days = 1;
  /* turnover must happen before allocation */
  foreachpft(pft,p,&stand->pftlist)
    turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR);
  allocation_today(stand,config->ntypes);

  /* daily harvest check*/
  isphen=FALSE;
  hfrac=0.0;
  /* cleaf and cleaf_max for all grass PFTs */
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    cleaf+=grass->ind.leaf;
    cleaf_max+=grass->max_leaf;
  }
  if(day==31 || day==59 || day==90 || day==120 || day==151 || day==181 || day==212 || day==243 || day==273 || day==304 || day==334 || day==365)
  {
    if(cleaf>cleaf_max)
    {
      isphen=TRUE;
      hfrac=1-1000/(1000+cleaf);
    }
  }
  if(isphen)
  {
    harvest=harvest_stand(output,stand,hfrac);

    /* return irrig_stor and irrig_amount in case of harvest */
    if(data->irrigation)
    {
      stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
      /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
      stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      stand->cell->output.mstor_return+=(data->irrig_stor+data->irrig_amount)*stand->frac;
      stand->cell->output.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      stand->cell->output.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;

      if(config->pft_output_scaled)
      {
        stand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[data->irrigation].grass[0];
        stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[data->irrigation].grass[1];
        stand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[data->irrigation].grass[0];
        stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[data->irrigation].grass[1];
      }
      else
      {
        stand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
        stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
        stand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
      }
      data->irrig_stor=0;
      data->irrig_amount=0;
    }
  }


  if(withdailyoutput)
  {
    foreachpft(pft,p,&stand->pftlist)
      if(output->daily.cft == TEMPERATE_HERBACEOUS && data->irrigation == output->daily.irrigation)
      {
        output->daily.evap+=evap;
        forrootsoillayer(l)
          output->daily.trans+=aet_stand[l];
        output->daily.irrig=irrig_apply;
        output->daily.w0+=stand->soil.w[1];
        output->daily.w1+=stand->soil.w[2];
        output->daily.wevap+=stand->soil.w[0];
        output->daily.par=par;
        output->daily.daylength=daylength;
        output->daily.pet+=eeq*PRIESTLEY_TAYLOR;
      }
  }

  if(data->irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,gp_stand,wet,eeq);

  forrootsoillayer(l)
  {
    output->mtransp+=aet_stand[l]*stand->frac;
    output->mtransp_b+=(aet_stand[l]-green_transp[l])*stand->frac;
  }

  output->minterc+=intercep_stand*stand->frac; /* Note: including blue fraction*/
  output->minterc_b+=intercep_stand_blue*stand->frac;   /* blue interception and evap */

  output->mevap+=evap*stand->frac;
  output->mevap_b+=evap_blue*stand->frac;   /* blue soil evap */

  output->mreturn_flow_b+=return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

  if(config->pft_output_scaled)
  {
    output->pft_harvest[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest*stand->cell->ml.landfrac[data->irrigation].grass[0];
    output->pft_harvest[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest*stand->cell->ml.landfrac[data->irrigation].grass[1];

    output->pft_harvest[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual+=harvest.residual*stand->cell->ml.landfrac[data->irrigation].grass[0];
    output->pft_harvest[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual+=harvest.residual*stand->cell->ml.landfrac[data->irrigation].grass[1];
  }
  else
  {
    output->pft_harvest[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest;
    output->pft_harvest[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=harvest.harvest;

    output->pft_harvest[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual+=harvest.residual;
    output->pft_harvest[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].residual+=harvest.residual;
  }

    /* harvested area */

  if (isphen)
  {
    output->cftfrac[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]=stand->cell->ml.landfrac[data->irrigation].grass[0];
    output->cftfrac[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]=stand->cell->ml.landfrac[data->irrigation].grass[1];
  }
  output->cft_pet[rothers(ncft)+data->irrigation*(ncft+NGRASS)]+=eeq*PRIESTLEY_TAYLOR;
  output->cft_temp[rothers(ncft)+data->irrigation*(ncft+NGRASS)]+= climate->temp >= 5 ? climate->temp : 0;
  output->cft_prec[rothers(ncft)+data->irrigation*(ncft+NGRASS)]+=climate->prec;
  output->cft_srad[rothers(ncft)+data->irrigation*(ncft+NGRASS)]+=climate->swdown;
  output->cft_pet[rmgrass(ncft)+data->irrigation*(ncft+NGRASS)]+=eeq*PRIESTLEY_TAYLOR;
  output->cft_temp[rmgrass(ncft)+data->irrigation*(ncft+NGRASS)]+=climate->temp;
  output->cft_prec[rmgrass(ncft)+data->irrigation*(ncft+NGRASS)]+=climate->prec;
  output->cft_srad[rmgrass(ncft)+data->irrigation*(ncft+NGRASS)]+=climate->swdown;

  /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw_grassland(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
                       intercep_stand,intercep_stand_blue,ncft,config->pft_output_scaled);
  free(wet);
  return runoff;
} /* of 'daily_grassland' */
