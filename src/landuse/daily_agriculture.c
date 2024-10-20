/**************************************************************************************/
/**                                                                                \n**/
/**       d  a  i  l  y  _  a  g  r  i  c  u  l  t  u  r  e  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of NPP update of agriculture stand                                \n**/
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
#include "agriculture.h"

Real daily_agriculture(Stand *stand,                /**< [inout] stand pointer */
                       Real co2,                    /**< [in] atmospheric CO2 (ppmv) */
                       const Dailyclimate *climate, /**< [in] Daily climate values */
                       int day,                     /**< [in] day (1..365) */
                       int month,                   /**< [in] month (0..11) */
                       Real daylength,              /**< [in] length of day (h) */
                       Real gtemp_air,              /**< [in] value of air temperature response function */
                       Real gtemp_soil,             /**< [in] value of soil temperature response function */
                       Real eeq,                    /**< [in] equilibrium evapotranspiration (mm) */
                       Real par,                    /**< [in] photosynthetic active radiation flux  (J/m2/day) */
                       Real melt,                   /**< [in] melting water (mm/day) */
                       int npft,                    /**< [in] number of natural PFTs */
                       int ncft,                    /**< [in] number of crop PFTs   */
                       int year,                    /**< [in] simulation year (AD) */
                       Bool UNUSED(intercrop),      /**< [in] enabled intercropping */
                       Real agrfrac,                /**< [in] total agriculture fraction (0..1) */
                       const Config *config         /**< [in] LPJ config */
                      )                             /** \return runoff (mm/day) */
{
  int p,l,nnat,nirrig,isrice,index=-1;
  Pft *pft;
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap,evap_blue,rd,gpp,frac_g_evap,runoff,wet_all,intercept,sprink_interc;
  Real rw_apply; /*applied irrigation water from rainwater harvesting storage, counted as green water */

  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Real npp; /* net primary productivity (gC/m2) */
  Real gc_pft;
  Real wdf; /* water deficit fraction */
  Real transp;
  Real cft_rm=0.0; /* cft-specific monthly root moisture */
  Real vol_water_enth; /* volumetric enthalpy of water (J/m3) */
  Bool negbm;
  Irrigation *data;
  Output *output;
  Pftcrop *crop;
  irrig_apply=0.0;
  isrice=FALSE;
  //Stocks flux_estab={0,0};

  data=stand->data;
  negbm=FALSE;
  output=&stand->cell->output;
  cover_stand=intercep_stand=intercep_stand_blue=wet_all=rw_apply=intercept=sprink_interc=rainmelt=irrig_apply=0.0;
  evap=evap_blue=runoff=return_flow_b=0.0;
#ifdef CHECK_BALANCE
  Real wfluxes_old=(stand->cell->balance.excess_water+stand->cell->lateral_water+stand->cell->balance.awater_flux+stand->cell->balance.aevap_res+stand->cell->balance.aevap_lake-stand->cell->balance.aMT_water);
  Real wstore_old=(stand->cell->discharge.dmass_lake+stand->cell->discharge.dmass_river)/stand->cell->coord.area;
  Real water_before=0;
  Real water_after,wstore_new,balancew;
  water_before+=soilwater(&stand->soil);
#endif

  stand->growing_days++;

  if(getnpft(&stand->pftlist)>0)
  {
    wet=newvec(Real,getnpft(&stand->pftlist)); /* wet from pftlist */
    check(wet);
    for(p=0;p<getnpft(&stand->pftlist);p++)
      wet[p]=0;
  }
  else
    wet=NULL;
  gp_pft=newvec(Real,npft+ncft);
  check(gp_pft);
  gp_stand=gp_sum(&stand->pftlist,co2,climate->temp,par,daylength,
                  &gp_stand_leafon,gp_pft,&fpc_total_stand,config);

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  if(!config->river_routing)
    irrig_amount(stand,data,npft,ncft,month,config);

  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);

  foreachpft(pft,p,&stand->pftlist)
  {
    index=(stand->type->landusetype==OTHERS) ? data->irrigation*nirrig+rothers(ncft) : pft->par->id-npft+data->irrigation*nirrig;
    crop=pft->data;
    if(!strcmp(pft->par->name,"rice")) isrice=TRUE;
    /* kill crop at frost events */
    /* trigger 2nd fertilization */
    /* GGCMI phase 3 rule: apply second dosis at fphu=0.25*/
    if(crop->fphu>0.25 && crop->nfertilizer>0)
    {
      stand->soil.NO3[0]+=crop->nfertilizer*param.nfert_no3_frac;
      stand->soil.NH4[0]+=crop->nfertilizer*(1-param.nfert_no3_frac);
      stand->cell->balance.influx.nitrogen+=crop->nfertilizer*stand->frac;
      getoutput(output,NFERT_AGR,config)+=crop->nfertilizer*pft->stand->frac;
      getoutput(output,NAPPLIED_MG,config)+=crop->nfertilizer*pft->stand->frac;
      crop->nfertilizer=0;
    }
    if(crop->fphu>0.25 && crop->nmanure>0)
    {
      stand->soil.NH4[0] += crop->nmanure*param.nmanure_nh4_frac;
      /* no tillage at second application, so manure goes to ag litter not agsub as at cultivation */
      stand->soil.litter.item->agtop.leaf.carbon += crop->nmanure*param.manure_cn;
      stand->soil.litter.item->agtop.leaf.nitrogen += crop->nmanure*(1-param.nmanure_nh4_frac);
      stand->cell->balance.influx.carbon += crop->nmanure*param.manure_cn*stand->frac;
      stand->cell->balance.influx.nitrogen += crop->nmanure*stand->frac;
      getoutput(output,NMANURE_AGR,config)+=crop->nmanure*stand->frac;
      getoutput(output,NAPPLIED_MG,config)+=crop->nmanure*stand->frac;
      crop->nmanure=0;
    }
    if(!isannual(PFT_NLEAF,config))
      getoutputindex(output,PFT_NLEAF,nnat+index,config)=crop->ind.leaf.nitrogen;
    if(!isannual(PFT_NLIMIT,config))
      getoutputindex(output,PFT_NLIMIT,nnat+index,config)=pft->nlimit;
    if(!isannual(PFT_CLEAF,config))
      getoutputindex(output,PFT_CLEAF,nnat+index,config)=crop->ind.leaf.carbon;
    if(!isannual(PFT_NROOT,config))
      getoutputindex(output,PFT_NROOT,nnat+index,config)=crop->ind.root.nitrogen;
    if(!isannual(PFT_CROOT,config))
      getoutputindex(output,PFT_CROOT,nnat+index,config)=crop->ind.root.carbon;
    if(!isannual(PFT_VEGN,config))
      getoutputindex(output,PFT_VEGN,nnat+index,config)=vegn_sum(pft);
    if(!isannual(PFT_VEGC,config))
      getoutputindex(output,PFT_VEGC,nnat+index,config)=vegc_sum(pft);

    if(phenology_crop(pft,climate->temp,daylength,npft,config))
    {
      update_separate_harvests(output,pft,data->irrigation,day,npft,ncft,config);
      harvest_crop(output,stand,pft,npft,ncft,year,config);
      /* return irrig_stor and irrig_amount */
      if(data->irrigation||isrice)
      {
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac; /* cell water balance account for cell inflow */
        getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;

        /* pay back conveyance losses that have already been consumed by transport into irrig_stor, only evaporative conv. losses, drainage conv. losses already returned */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif
        if(config->pft_output_scaled)
        {
          getoutputindex(output, CFT_CONV_LOSS_EVAP ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutputindex(output, CFT_CONV_LOSS_DRAIN ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(output, CFT_CONV_LOSS_EVAP ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          getoutputindex(output, CFT_CONV_LOSS_DRAIN ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }
        data->irrig_stor=0;
        data->irrig_amount=0;
      }
      /* delete PFT from list */
      delpft(&stand->pftlist,p);
      /* adjust index */
      p--;
//      if(!strcmp(pft->par->name,"rice") && stand->growing_days<300)
//      {
//    	  flux_estab=cultivate(stand->cell,data->irrigation,day,FALSE,stand,
//    	                     npft,ncft,RICE,year,config);
//    	    if(data->irrigation)
//    	      c=ncft+RICE;
//    	    if(!config->double_harvest)
//    	      getoutputindex(&stand->cell->output,SDATE,c,config)=day;
//    	    if(config->sdate_option==FIXED_SDATE)
//    	    	stand->cell->ml.sdate_fixed[RICE]=day;
//         getoutput(&stand->cell->output,FLUX_ESTABC,config)+=flux_estab.carbon;
//         getoutput(&stand->cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen;
//         stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen;
//         stand->cell->balance.flux_estab.carbon+=flux_estab.carbon;
//      }
//      else
//      {
        stand->type=&kill_stand;
        stand->growing_days=0;
//      }
    }
  } /* of foreachpft() */

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;
  /* blue water inflow*/
  if((data->irrigation||isrice) && data->irrig_amount>epsilon)
  { /* data->irrigation defines if stand is irrigated in general and not if water is delivered that day, initialized in new_agriculture.c and changed in landusechange.c*/
    irrig_apply=max(data->irrig_amount-rainmelt,0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrig_stor+=data->irrig_amount-irrig_apply;
    data->irrig_amount=0.0;
    if(irrig_apply<1 && data->irrig_system!=DRIP)  /* min. irrigation requirement of 1mm */
    {
      data->irrig_stor+=irrig_apply;
      irrig_apply=0.0;
    }
    else
    {
      /* write irrig_apply to output */
      getoutput(output,IRRIG,config)+=irrig_apply*stand->frac;
      stand->cell->balance.airrig+=irrig_apply*stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]+=irrig_apply*stand->frac;
        stand->cell->ml.image_data->mevapotr[month] += irrig_apply*stand->frac;
      }
#endif

      if(getnpft(&stand->pftlist)>0)
      {
        pft=getpft(&stand->pftlist,0);
        index=(stand->type->landusetype==OTHERS) ? data->irrigation*nirrig+rothers(ncft) : pft->par->id-npft+data->irrigation*nirrig;
        crop=pft->data;
        if(crop->sh!=NULL)
        {
          if(config->pft_output_scaled)
            crop->sh->irrig_apply+=irrig_apply*stand->frac;
          else
            crop->sh->irrig_apply+=irrig_apply;
        }
        else
        {
          if(config->pft_output_scaled)
            getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply*stand->frac;
          else
            getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply;
        }
      }
    }
  }

  /* INTERCEPTION */
  sprink_interc=(data->irrig_system==SPRINK) ? 1 : 0;

  foreachpft(pft,p,&stand->pftlist)
  {
    intercept=interception(&wet[p],pft,eeq,climate->prec+irrig_apply*sprink_interc); /* in case of sprinkler, irrig_amount goes through interception */
    wet_all+=wet[p]*pft->fpc;
    intercep_stand_blue+=(climate->prec+irrig_apply*sprink_interc>epsilon) ? intercept*(irrig_apply*sprink_interc)/(climate->prec+irrig_apply*sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand+=intercept;
  }

  irrig_apply-=intercep_stand_blue;
  if(irrig_apply<0)
    intercep_stand_blue+=irrig_apply;
  irrig_apply=max(0,irrig_apply);
  rainmelt-=(intercep_stand-intercep_stand_blue);

  /* rain-water harvesting*/
  if(!data->irrigation && config->rw_manage && rainmelt<5)
    rw_apply=rw_irrigation(stand,gp_stand,wet,eeq,config); /* Note: RWH supplementary irrigation is here considered green water */

  /* INFILTRATION and PERCOLATION */
  if(irrig_apply>epsilon)
  {
    /* count irrigation events*/
    if(index!=-1)
      getoutputindex(output,CFT_IRRIG_EVENTS,index,config)++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
  }

  if((climate->prec+melt+rw_apply+irrig_apply)>0) /* enthalpy of soil infiltration */
    vol_water_enth = climate->temp*c_water*(climate->prec+rw_apply+irrig_apply)/(climate->prec+rw_apply+irrig_apply+melt)+c_water2ice;
  else
    vol_water_enth=0;


  runoff+=infil_perc(stand,rainmelt+rw_apply+irrig_apply, vol_water_enth,climate->prec,&return_flow_b,npft,ncft,config);

  foreachpft(pft,p,&stand->pftlist)
  {
    index=(stand->type->landusetype==OTHERS) ? data->irrigation*nirrig+rothers(ncft) : pft->par->id-npft+data->irrigation*nirrig;
    cover_stand+=fpar(pft);
    /* calculate albedo and FAPAR of PFT */
    albedo_crop(pft, stand->soil.snowheight, stand->soil.snowfraction);

/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,
                       nnat+index,npft,ncft,config);
    getoutput(output,RD,config)+=rd*stand->frac;
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      getoutputindex(output,PFT_GCGP_COUNT,nnat+index,config)++;
      getoutputindex(output,PFT_GCGP,nnat+index,config)+=gc_pft/gp_pft[getpftpar(pft,id)];
    }
    npp=npp_crop(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,&negbm,wdf,config);
    pft->npp_bnf=0.0;
    getoutput(output,NPP,config)+=npp*stand->frac;
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    getoutput(output,NPP_AGR,config) += npp*stand->frac / agrfrac;
    output->dcflux-=npp*stand->frac;
    getoutput(output,GPP,config)+=gpp*stand->frac;
    getoutput(output,FAPAR,config) += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,WSCAL,config) += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutputindex(output,CFT_FPAR,index,config)+=(fpar(pft)*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)))*(1-pft->albedo);

    if(config->pft_output_scaled)
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp*stand->frac;
    else
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp;
    getoutputindex(output,PFT_LAI,nnat+index,config)+=actual_lai_crop(pft);
    crop=pft->data;
    if(stand->type->landusetype==AGRICULTURE && config->separate_harvests)
    {
      crop->sh->lgp+=1;
      if(config->pft_output_scaled)
      {
        crop->sh->petsum+=eeq*((1-pft->phen)*PRIESTLEY_TAYLOR+pft->phen*(1-wet[p])*PRIESTLEY_TAYLOR+pft->phen*wet[p]*PRIESTLEY_TAYLOR)*stand->frac; //???
        crop->sh->precsum+=climate->prec*stand->frac;
        crop->sh->sradsum+=climate->swdown*stand->frac;
        crop->sh->tempsum+=climate->temp*stand->frac;
      }
      else
      {
        crop->sh->petsum+=eeq*((1-pft->phen)*PRIESTLEY_TAYLOR+pft->phen*(1-wet[p])*PRIESTLEY_TAYLOR+pft->phen*wet[p]*PRIESTLEY_TAYLOR); //???
        crop->sh->precsum+=climate->prec;
        crop->sh->sradsum+=climate->swdown;
        crop->sh->tempsum+=climate->temp;
      }
      if(stand->type->landusetype==AGRICULTURE)
        crop->sh->runoffsum+=runoff;
    }
    else
    {
      index=(stand->type->landusetype==OTHERS) ? data->irrigation*(ncft+NGRASS)+rothers(ncft) : pft->par->id-npft+data->irrigation*(ncft+NGRASS);
      getoutputindex(output,GROWING_PERIOD,index,config)+=1.;
      getoutputindex(output,CFT_PET,index,config)+=eeq*PRIESTLEY_TAYLOR;
      getoutputindex(output,CFT_TEMP,index,config)+= climate->temp >= 5 ? climate->temp : 0;
      getoutputindex(output,CFT_PREC,index,config)+=climate->prec;
      getoutputindex(output,CFT_SRAD,index,config)+=climate->swdown;
    }
    if(negbm)
    {
      update_separate_harvests(output,pft,data->irrigation,day,npft,ncft,config);
      harvest_crop(output,stand,pft,npft,ncft,year,config);
      if(data->irrigation||isrice)
      {
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;

        /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif

        index=(stand->type->landusetype==OTHERS) ? data->irrigation*nirrig+rothers(ncft) : pft->par->id-npft+data->irrigation*nirrig;
        if(config->pft_output_scaled)
        {
          getoutputindex(output, CFT_CONV_LOSS_EVAP ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutputindex(output, CFT_CONV_LOSS_DRAIN ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(output, CFT_CONV_LOSS_EVAP ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          getoutputindex(output, CFT_CONV_LOSS_DRAIN ,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor=0;
        data->irrig_amount=0;
      } /* of if(data->irrigation) */
      delpft(&stand->pftlist,p);
      stand->type=&kill_stand;
      p--;
    } /* of if(negbm) */
#if DEBUG2
  if(stand->type->landusetype!=KILL && !strcmp(pft->par->name,"rice"))
  {
    fprintf(stdout,"\n year:%d day:%d isrice: %d bm_inc:%g inun_stress: %g\n",year,day,isrice,pft->bm_inc.carbon,pft->inun_stress);
    fprintf(stdout,"\n irrig_amount:%g irrig_stor:%g irrig_apply: %g satwater:%g rootwater:%g net_irrig_amount:%g\n",
        data->irrig_amount,data->irrig_stor,irrig_apply,satwater(&stand->soil),rootwater(&stand->soil),data->net_irrig_amount);
    if(day==240)
      fprintstand(stdout,stand,config->pftpar,npft+ncft,config->with_nitrogen);
  }
#endif
  } /* of foreachpft */
  free(gp_pft);
  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);
  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    getoutput(output,TRANSP_B,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
  }

  /* calculate net irrigation requirements (NIR) for next days irrigation */
  if((data->irrigation||isrice) && stand->pftlist.n>0) /* second element to avoid irrigation on just harvested fields */
    calc_nir(stand,data,gp_stand,wet,eeq,config->others_to_crop);

  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,INTERC,config)+=intercep_stand*stand->frac; /* Note: including blue fraction*/
  getoutput(output,INTERC_B,config)+=intercep_stand_blue*stand->frac;   /* blue interception and evap */

  getoutput(output,EVAP,config)+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;
  getoutput(output,EVAP_B,config)+=evap_blue*stand->frac;   /* blue soil evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  getoutput(output,RETURN_FLOW_B,config)+=return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

  /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw_agriculture(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
                         intercep_stand,intercep_stand_blue,npft,ncft,config);

  /* write CFT-specific soil moisture output */
  foreachpft(pft,p,&stand->pftlist)
  {
    cft_rm=0.0;
    forrootmoist(l)
      cft_rm+=pft->stand->soil.w[l]*pft->stand->soil.whcs[l]; /* absolute soil water content between wilting point and field capacity (mm) */
    if(stand->type->landusetype==AGRICULTURE)
    {
      getoutputindex(output,NDAY_MONTH,pft->par->id-npft+data->irrigation*ncft,config)+=1; /* track number of growing season days in each month for calculating the mean in update_monthly.c */
      getoutputindex(output,CFT_SWC,pft->par->id-npft+data->irrigation*ncft,config)+=cft_rm; /* no secondary output needed as monthly outputs are written even in double harvest situation */
    }
  }

  free(wet);

#ifdef CHECK_BALANCE
  transp=0;
  water_after=0;
  wstore_new=(stand->cell->discharge.dmass_lake+stand->cell->discharge.dmass_river)/stand->cell->coord.area;
  water_after+=soilwater(&stand->soil);
  Real wfluxes_new=(stand->cell->balance.excess_water+stand->cell->lateral_water+stand->cell->balance.awater_flux+stand->cell->balance.aevap_res+stand->cell->balance.aevap_lake-stand->cell->balance.aMT_water);
  forrootsoillayer(l)
   transp+=aet_stand[l];
  balancew=water_after-water_before-(climate->prec+melt+rw_apply+irrig_apply)+(transp+evap+intercep_stand+runoff)+(wfluxes_new-wfluxes_old)/stand->frac+(wstore_new-wstore_old)/stand->frac;
  if(fabs(balancew)>0.001 && stand->frac>0.00001)
  {

    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid water balance in %s:  y: %d day: %d balanceW: %g water_before: %.6f water_after: %.6f aet: %g evap: %g intercep_stand %g runoff: %g influx: %g fluxes: %g irrig_apply: %g irrig_stor: %g frac: %g rice: %d wstore: %g wstore_new: %g wstore_old: %g \n",
        __FUNCTION__,year,day,balancew,water_before,water_after,transp,evap,intercep_stand,runoff,(climate->prec+melt+rw_apply+irrig_apply),(wfluxes_new-wfluxes_old)/stand->frac,irrig_apply,data->irrig_stor,stand->frac,isrice,(wstore_new-wstore_old)/stand->frac,wstore_new,wstore_old);
  }
 #endif
  return runoff;
} /* of 'daily_agriculture' */
