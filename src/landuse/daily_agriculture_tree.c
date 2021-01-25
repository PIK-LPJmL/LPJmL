/**************************************************************************************/
/**                                                                                \n**/
/**               d a i l y _ a g r i c u l t u r e _ t r e e . c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of NPP update of agriculture tree stand                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "natural.h"
#include "grass.h"
#include "tree.h"
#include "agriculture.h"
#include "agriculture_tree.h"

Real daily_agriculture_tree(Stand *stand,                /**< stand pointer */
                            Real co2,                    /**< atmospheric CO2 (ppmv) */
                            const Dailyclimate *climate, /**< Daily climate values */
                            int day,                     /**< day (1..365) */
                            int month,                   /**< [in] month (0..11) */
                            Real daylength,              /**< length of day (h) */
                            Real gtemp_air,              /**< value of air temperature response function */
                            Real gtemp_soil,             /**< value of soil temperature response function */
                            Real eeq,                    /**< equilibrium evapotranspiration (mm) */
                            Real par,                    /**< photosynthetic active radiation flux  (J/m2/day) */
                            Real melt,                   /**< melting water (mm/day) */
                            int npft,                    /**< number of natural PFTs */
                            int ncft,                    /**< number of crop PFTs   */
                            int year,                    /**< simulation year (AD) */
                            Bool UNUSED(intercrop),      /**< enabled intercropping */
                            Real UNUSED(agrfrac),        /**< [in] total agriculture fraction (0..1) */
                            const Config *config         /**< LPJ config */
                           )                             /** \return runoff (mm/day) */
{
  int p,l,nnat,index;
  Pft *pft;
  Real transp;
  Pfttree *tree;
  Pfttreepar *treepar;
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Output *output;
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap; /* evaporation (mm) */
  Real evap_blue; /* evaporation of irrigation water (mm) */
  Real rd,gpp,frac_g_evap,runoff,wet_all,intercept,sprink_interc;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Real npp; /* net primary productivity (gC/m2) */
  Real wdf; /* water deficit fraction */
  Real gc_pft;
  Stocks flux_estab,yield;
  Real fpc_inc,fpc_total,*fpc_type;
  Real cnratio_fruit;
  Biomass_tree *data;
  Soil *soil;
  irrig_apply=0.0;

  soil = &stand->soil;
  data=stand->data;
  output=&stand->cell->output;
  if(strcmp(config->pftpar[data->irrigation.pft_id].name,"cotton")  || getnpft(&stand->pftlist)>0)
    stand->growing_days++;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=runoff=return_flow_b=wet_all=intercept=sprink_interc=rainmelt=irrig_apply=0.0;
  if(!strcmp(config->pftpar[data->irrigation.pft_id].name,"cotton") && day==stand->cell->ml.sowing_day_cotton[data->irrigation.irrigation])
  {
    //printf("day=%d, sowing %d\n",day,data->irrigation); 
    pft=addpft(stand,config->pftpar+data->irrigation.pft_id,year,0,config->with_nitrogen,config->double_harvest);
    flux_estab=establishment(pft,0,0,1);
    stand->cell->output.flux_estab.carbon+=flux_estab.carbon*stand->frac;
    stand->cell->output.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
    stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
    stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
    stand->growing_days=0;
    foreachpft(pft,p,&stand->pftlist)
       if(pft->par->type==GRASS)
         fpc_grass(pft);
    fpc_type=newvec(Real,config->ntypes);
    check(fpc_type);

    fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
    foreachpft(pft,p,&stand->pftlist)
      if(pft->par->type==TREE)
        adjust_tree(&stand->soil.litter,pft,fpc_type[pft->par->type], param.fpc_tree_max);
    fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
    if (fpc_total>1.0)
      foreachpft(pft,p,&stand->pftlist)
        if(pft->par->type==GRASS)
          reduce(&stand->soil.litter,pft,fpc_type[GRASS]/(1+fpc_type[GRASS]-fpc_total));
    free(fpc_type);
    albedo_stand(stand);
  }
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

  if(!config->river_routing)
    irrig_amount(stand,&data->irrigation,npft,ncft,month,config);

  for(l=0;l<LASTLAYER;l++)
     aet_stand[l]=green_transp[l]=0;

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
      rainmelt=0.0;

  nnat=getnnat(npft,config);
  index=agtree(ncft,config->nwptype)+data->irrigation.pft_id-npft+config->nagtree+data->irrigation.irrigation*getnirrig(ncft,config);

  if(data->irrigation.irrigation && data->irrigation.irrig_amount>epsilon)
  {
    irrig_apply=max(data->irrigation.irrig_amount-rainmelt,0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrigation.irrig_stor+=data->irrigation.irrig_amount-irrig_apply;
    data->irrigation.irrig_amount=0.0;
    if(irrig_apply<1 && data->irrigation.irrig_system!=DRIP)  /* min. irrigation requirement of 1mm */
    {
      data->irrigation.irrig_stor+=irrig_apply;
      irrig_apply=0.0;
    }
    else
    {
      /* write irrig_apply to output */
      stand->cell->output.irrig+=irrig_apply*stand->frac;
      stand->cell->balance.airrig+=irrig_apply*stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]+=irrig_apply*stand->frac;
        stand->cell->ml.image_data->mevapotr[month] += irrig_apply*stand->frac;
      }
#endif

      if(config->pft_output_scaled)
        stand->cell->output.cft_airrig[index]+=irrig_apply*stand->frac;
      else
        stand->cell->output.cft_airrig[index]+=irrig_apply;
    }
  }

  /* INTERCEPTION */
  foreachpft(pft,p,&stand->pftlist)
  {
    /* calculate old or new phenology */
    if (config->new_phenology)
      phenology_gsi(pft, climate->temp, climate->swdown, day,climate->isdailytemp);
    else
      leaf_phenology(pft,climate->temp,day,climate->isdailytemp);
    sprink_interc=(data->irrigation.irrig_system==SPRINK) ? 1 : 0;

    intercept=interception(&wet[p],pft,eeq,climate->prec+irrig_apply*sprink_interc); /* in case of sprinkler, irrig_amount goes through interception, in case of mixed, 0.5 of irrig_amount */
    wet_all+=wet[p]*pft->fpc;
    intercep_stand_blue+=(climate->prec+irrig_apply*sprink_interc>epsilon) ? intercept*(irrig_apply*sprink_interc)/(climate->prec+irrig_apply*sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand+=intercept;
  }

  irrig_apply-=intercep_stand_blue;
  rainmelt-=(intercep_stand-intercep_stand_blue);

  /* soil inflow: infiltration and percolation */
  if(irrig_apply>epsilon)
  {
      runoff+=infil_perc_irr(stand,irrig_apply,&return_flow_b,npft,ncft,config);
      /* count irrigation events*/
      output->cft_irrig_events[index]++;
  }

  runoff+=infil_perc_rain(stand,rainmelt,&return_flow_b,npft,ncft,config);

  foreachpft(pft,p,&stand->pftlist)
  {
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT  already called in update_daily via albedo_stand*/ 
    albedo_pft(pft, soil->snowheight, soil->snowfraction);

    /*
    *  Calculate net assimilation, i.e. gross primary production minus leaf
    *  respiration, including conversion from FPC to grid cell basis.
    *
    */
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,npft,ncft,config);
    if(stand->frac>0.0 && gp_pft[getpftpar(pft,id)]>0.0)
    {
      output->gcgp_count[nnat+index]++;
      output->pft_gcgp[nnat+index]+=gc_pft/gp_pft[getpftpar(pft,id)];
    }
    npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd,config->with_nitrogen);
    //printf("day=%d, irrig=%d, npp=%g, c_fruit=%g,phen=%g\n",day,data->irrigation,npp,tree->c_fruit,pft->phen);
    //printf("tmin=%g, tmax=%g, light=%g, wscal=%g\n",pft->phen_gsi.tmin,pft->phen_gsi.tmax,pft->phen_gsi.light,pft->phen_gsi.wscal);
    treepar=pft->par->data;
    //printf("%s %d %d\n",pft->par->name,stand->age,treepar->rotation);
    //if(istree(pft) && pft->par->id!=TEA && npp>0 && stand->age>treepar->rotation)
    if(istree(pft) && strcmp(pft->par->name,"tea") && npp>0)
    {
      //printf("%s %g %g NPP\n",pft->par->name,pft->gdd,npp);
      tree=pft->data;
      switch(pft->par->phenology)
      {
        case SUMMERGREEN:
          if(pft->phen>param.phen_limit)
          {
            if(!strcmp(pft->par->name,"cotton"))
            {
              if(tree->boll_age<45)
                tree->boll_age++;
              cnratio_fruit=0.0054*tree->boll_age*tree->boll_age-0.3984*tree->boll_age+9.4091;
              //printf("cnratio(%d)=%g\n",cnratio_fruit,tree->boll_age);
            }
            else
              cnratio_fruit=treepar->cnratio_fruit;
            tree->fruit.carbon+=treepar->harvest_ratio*npp;
            tree->fruit.nitrogen+=treepar->harvest_ratio*npp/cnratio_fruit;
            pft->bm_inc.carbon-=treepar->harvest_ratio*npp;
            pft->bm_inc.nitrogen-=treepar->harvest_ratio*npp/cnratio_fruit;
          //           if(pft->par->id==TEA) printf("PFT %s summergreen fruit are growing at month %d, phen=%g, aphen=%g, gdd=%g, temp %.2f, harvest=%g t/ha\n",pft->par->name,day/30+1,pft->phen,pft->aphen,pft->gdd,climate.temp, tree->c_fruit*1e4/0.45*1e-6);
          }
          break;
        case EVERGREEN:
    //         if(pft->gdd>0 && pft->gdd<1/pft->par->ramp)
          if(pft->gdd>0)
          {

            //		if(pft->par->id==TEA) printf("PFT %s evergreen fruit are growing at month %d, harvest=%g,gdd=%g,ramp:%g t/ha\n",pft->par->name,day/30+1,tree->c_fruit*1e4/0.45*1e-6, pft->gdd,pft->par->ramp);

            tree->fruit.carbon+=treepar->harvest_ratio*npp;
            tree->fruit.nitrogen+=treepar->harvest_ratio*npp/treepar->cnratio_fruit;
            //printf("%s %g fruit\n",pft->par->name,tree->fruit.carbon);
            pft->bm_inc.carbon-=treepar->harvest_ratio*npp;
            pft->bm_inc.nitrogen-=treepar->harvest_ratio*npp/treepar->cnratio_fruit;
          }
          break;
      }
    } /* of foreachpft() */

    output->npp+=npp*stand->frac;
    stand->cell->balance.anpp+=npp*stand->frac;
    output->dcflux-=npp*stand->frac;
    output->gpp+=gpp*stand->frac;
    output->fapar+= pft->fapar * stand->frac*(1.0/(1-stand->cell->lakefrac));
    output->mphen_tmin += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac));
    output->mphen_tmax += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac));
    output->mphen_light += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac));
    output->mphen_water += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac));
    output->mwscal += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac));
    output->cft_fpar[index]+=(fpar(pft)*stand->frac*(1.0/(1-stand->cell->lakefrac)));

    if(config->pft_output_scaled)
      output->pft_npp[nnat+index]+=npp*stand->frac;
    else
      output->pft_npp[nnat+index]+=npp;
  } /* of foreachpft */
  free(gp_pft);
  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);

  if(config->withdailyoutput)
  {
    foreachpft(pft,p,&stand->pftlist)
      if(pft->par->id==output->daily.cft && data->irrigation.irrigation==output->daily.irrigation)
      {
        output->daily.evap+=evap;
        output->daily.interc+=intercep_stand;
        forrootsoillayer(l)
          output->daily.trans+=aet_stand[l];
        /*output->daily.w0=stand->soil.w[1];
          output->daily.w1=stand->soil.w[2];
          output->daily.wevap=stand->soil.w[0];*/
        output->daily.par+=par;
        output->daily.so.carbon=tree->fruit.carbon;
        output->daily.npp+=npp;
        output->daily.gpp+=gpp;
        output->daily.phen+=pft->phen;
      }
  }

  if(data->irrigation.irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,&data->irrigation,gp_stand,wet,eeq);

  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    output->mtransp_b+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  output->transp+=transp;
  stand->cell->balance.atransp+=transp;
  output->interc+=intercep_stand*stand->frac; /* Note: including blue fraction*/
  output->minterc_b+=intercep_stand_blue*stand->frac;   /* blue interception and evap */

  output->evap+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;
  output->mevap_b+=evap_blue*stand->frac;   /* blue soil evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  output->mreturn_flow_b+=return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

                                                       /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw_agriculture_tree(output,stand,frac_g_evap,evap,evap_blue,
                              return_flow_b,aet_stand,green_transp,
                              intercep_stand,intercep_stand_blue,npft,
                              ncft,config);

  free(wet);
  //if(stand->pft_id==COTTON)
  //  printf("growing_day: %d %d %d\n",stand->growing_days,data->irrigation,stand->cell->ml.growing_season_cotton[data->irrigation]);
  if(!strcmp(config->pftpar[data->irrigation.pft_id].name,"cotton") && stand->growing_days==stand->cell->ml.growing_season_cotton[data->irrigation.irrigation])
  {
    //printf("day=%d, harvest %d\n",day,data->irrigation); 
    pft=getpft(&stand->pftlist,0);
    yield=harvest_tree(pft);
    tree=pft->data;
    tree->boll_age=0;
    //printf("%s yield %s=%g t/ha, %g indiv/ha, wstress=%g, fpc=%g\n",(irrigation->irrigation) ? "irrigated" :"",pft->par->name,yield*1e4/1e6/0.45,pft->nind*1e4,pft->wscal_mean/365,pft->fpc);
    //printf("index=%d, yield=%g\n",agtree(ncft)+stand->pft_id-npft+nagtree+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE+nagtree),yield);
    //printf("harvest(%s) %d: %g %g\n",config->pftpar[stand->pft_id].name,data->irrigation,yield.carbon,yield.nitrogen);
    if(config->pft_output_scaled)
    {
      stand->cell->output.pft_harvest[index].harvest.carbon+=yield.carbon*stand->frac;
      stand->cell->output.pft_harvest[index].harvest.nitrogen+=yield.nitrogen*stand->frac;
    }
    else
    {
      stand->cell->output.pft_harvest[index].harvest.carbon+=yield.carbon;
      stand->cell->output.pft_harvest[index].harvest.nitrogen+=yield.nitrogen;
    }
    stand->cell->balance.biomass_yield.carbon+=yield.carbon*stand->frac;
    stand->cell->balance.biomass_yield.nitrogen+=yield.nitrogen*stand->frac;
    stand->cell->output.dcflux+=yield.carbon*stand->frac;
    annualpft(stand,pft,&fpc_inc,config->new_phenology,config->with_nitrogen,climate->isdailytemp);
    litter_update(&stand->soil.litter,pft,pft->nind);
    pft->nind=0;
    delpft(&stand->pftlist,0);
    stand->growing_days=0;
    cutpfts(stand);
    data->growing_time=0;
    if(data->irrigation.irrigation)
    {
      stand->cell->discharge.dmass_lake+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->frac;
      stand->cell->discharge.dmass_lake+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
      stand->cell->output.mstor_return+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->frac;
      stand->cell->output.mconv_loss_evap-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
      stand->cell->balance.aconv_loss_evap-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
      stand->cell->output.mconv_loss_drain-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->frac;
      stand->cell->balance.aconv_loss_drain-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif

      if(config->pft_output_scaled)
      {
        stand->cell->output.cft_conv_loss_evap[index]-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
        stand->cell->output.cft_conv_loss_drain[index]-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->frac;
      }
      else
      {
        stand->cell->output.cft_conv_loss_evap[index]-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap;
        stand->cell->output.cft_conv_loss_drain[index]-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap);
      }
      data->irrigation.irrig_stor=0;
      data->irrigation.irrig_amount=0;
    }
    //if(setaside(stand->cell,stand,config->pftpar,TRUE,npft,data->irrigation,year))
    // return TRUE;
  }
  return runoff;
} /* of 'daily_agriculture_tree' */
