/**************************************************************************************/
/**                                                                                \n**/
/**               d a i l y _ a g r i c u l t u r e _ t r e e . c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of agriculture tree stand                         \n**/
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

static int findagtree(Pftlist *pftlist,int pft_id)
{
  Pft *pft;
  int p;
  foreachpft(pft,p,pftlist)
    if(pft->par->id==pft_id)
      return p;
  return NOT_FOUND; /* not found */
} /* of 'findagtree' */

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
  Real gp_stand;        /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon; /**< pot. canopy conduct.at full leaf cover  (mm/s) */
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
  String line;
  Bool iscotton;
  irrig_apply=0.0;

  soil = &stand->soil;
  data=stand->data;
  output=&stand->cell->output;
  iscotton=!strcmp(config->pftpar[data->irrigation.pft_id].name,"cotton");
  if(!iscotton  || getnpft(&stand->pftlist)>0)
    stand->growing_days++;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=runoff=return_flow_b=wet_all=intercept=sprink_interc=rainmelt=irrig_apply=0.0;
  if(iscotton && day==stand->cell->ml.sowing_day_cotton[data->irrigation.irrigation])
  {
    pft=addpft(stand,config->pftpar+data->irrigation.pft_id,year,0,config);
    flux_estab=establishment(pft,0,0,1);
    getoutput(output,FLUX_ESTABC,config)+=flux_estab.carbon*stand->frac;
    getoutput(output,FLUX_ESTABN,config)+=flux_estab.nitrogen*stand->frac;
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
        adjust_tree(&stand->soil.litter,pft,fpc_type[pft->par->type], param.fpc_tree_max,config);
    fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
    if (fpc_total>1.0)
      foreachpft(pft,p,&stand->pftlist)
        if(pft->par->type==GRASS)
          reduce(&stand->soil.litter,pft,fpc_type[GRASS]/(1+fpc_type[GRASS]-fpc_total),config);
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

  /* The index for ag_tree in ml.fertilizer_nr.ag_tree structure (fertil)
     takes PFT id (according to pft.cjson or similar),
     subtracts the number of all non-crop PFTs (npft),
     adds the number of ag_trees (config->nagtree),
     so that the first element in ag_tree fertilizer vector (indexed as 0)
     is read for the first ag_tree in pftpar list */
  nnat=getnnat(npft,config);
  index=agtree(ncft,config->nwptype)+data->irrigation.pft_id-npft+config->nagtree+data->irrigation.irrigation*getnirrig(ncft,config);

  /* Apply fertilizers */
  if(config->with_nitrogen)
    fertilize_tree(stand,
                   (stand->cell->ml.fertilizer_nr==NULL) ? 0.0 : stand->cell->ml.fertilizer_nr[data->irrigation.irrigation].ag_tree[data->irrigation.pft_id-npft+config->nagtree],
                   (stand->cell->ml.manure_nr==NULL) ? 0.0 : stand->cell->ml.manure_nr[data->irrigation.irrigation].ag_tree[data->irrigation.pft_id-npft+config->nagtree],
                   day, config);

  /* Apply irrigation */
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
      getoutput(output,IRRIG,config)+=irrig_apply*stand->frac;
      stand->cell->balance.airrig+=irrig_apply*stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]+=irrig_apply*stand->frac;
        stand->cell->ml.image_data->mevapotr[month] += irrig_apply*stand->frac;
      }
#endif

      if(config->pft_output_scaled)
        getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply*stand->frac;
      else
        getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply;
    }
  }

  /* INTERCEPTION */
  foreachpft(pft,p,&stand->pftlist)
  {
    /* calculate old or new phenology */
    if (config->gsi_phenology)
      phenology_gsi(pft, climate->temp, climate->swdown, day,climate->isdailytemp,config);
    else
      leaf_phenology(pft,climate->temp,day,climate->isdailytemp,config);
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
      getoutputindex(output,CFT_IRRIG_EVENTS,index,config)++;
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
      getoutputindex(output,PFT_GCGP_COUNT,nnat+index,config)++;
      getoutputindex(output,PFT_GCGP,nnat+index,config)+=gc_pft/gp_pft[getpftpar(pft,id)];
    }
    npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config->with_nitrogen);
    pft->npp_bnf=0.0;
#ifdef DEBUG2
    printf("day=%d, irrig=%d, npp=%g, c_fruit=%g,phen=%g\n",day,data->irrigation.irrigation,npp,tree->fruit.carbon,pft->phen);
    printf("tmin=%g, tmax=%g, light=%g, wscal=%g\n",pft->phen_gsi.tmin,pft->phen_gsi.tmax,pft->phen_gsi.light,pft->phen_gsi.wscal);
    if(istree(pft))
    {
      treepar=tree->data;
      printf("%s %d %d\n",pft->par->name,data->age,treepar->rotation);
    }
#endif
    //if(istree(pft) && pft->par->id!=TEA && npp>0 && stand->age>treepar->rotation)
    if(istree(pft) && strcmp(pft->par->name,"tea") && npp>0)
    {
      treepar=pft->par->data;
      tree=pft->data;
      switch(pft->par->phenology)
      {
        case SUMMERGREEN:
          if(pft->phen>param.phen_limit)
          {
            if(iscotton)
            {
              if(tree->boll_age<45)
                tree->boll_age++;
              cnratio_fruit=0.0054*tree->boll_age*tree->boll_age-0.3984*tree->boll_age+9.4091;
            }
            else
              cnratio_fruit=treepar->cnratio_fruit;
            tree->fruit.carbon+=treepar->harvest_ratio*npp;
            tree->fruit.nitrogen+=treepar->harvest_ratio*npp/cnratio_fruit;
            pft->bm_inc.carbon-=treepar->harvest_ratio*npp;
            pft->bm_inc.nitrogen-=treepar->harvest_ratio*npp/cnratio_fruit;
#ifdef DEBUG
            if(!strcmp(pft->par->name,"tea"))
              printf("PFT %s summergreen fruit are growing at month %d, phen=%g, aphen=%g, gdd=%g, temp %.2f, harvest=%g t/ha\n",pft->par->name,day/30+1,pft->phen,pft->aphen,pft->gdd,climate->temp, tree->fruit.carbon*1e4/0.45*1e-6);
#endif
          }
          break;
        case EVERGREEN:
    //         if(pft->gdd>0 && pft->gdd<1/pft->par->ramp)
          if(pft->gdd>0)
          {

#ifdef DEBUG
           if(!strcmp(pft->par->name,"tea"))
             printf("PFT %s evergreen fruit are growing at month %d, harvest=%g,gdd=%g,ramp:%g t/ha\n",pft->par->name,day/30+1,tree->fruit.carbon*1e4/0.45*1e-6, pft->gdd,pft->par->ramp);
#endif
            tree->fruit.carbon+=treepar->harvest_ratio*npp;
            tree->fruit.nitrogen+=treepar->harvest_ratio*npp/treepar->cnratio_fruit;
            pft->bm_inc.carbon-=treepar->harvest_ratio*npp;
            pft->bm_inc.nitrogen-=treepar->harvest_ratio*npp/treepar->cnratio_fruit;
          }
          break;
      } /* of switch */
    } /* of istree() */

    getoutput(output,NPP,config)+=npp*stand->frac;
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    output->dcflux-=npp*stand->frac;
    getoutput(output,GPP,config)+=gpp*stand->frac;
    getoutput(output,FAPAR,config)+= pft->fapar * stand->frac*(1.0/(1-stand->cell->lakefrac));
    getoutput(output,PHEN_TMIN,config) += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac));
    getoutput(output,PHEN_TMAX,config) += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac));
    getoutput(output,PHEN_LIGHT,config) += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac));
    getoutput(output,PHEN_WATER,config) += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac));
    getoutput(output,WSCAL,config) += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac));
    getoutputindex(output,CFT_FPAR,index,config)+=(fpar(pft)*stand->frac*(1.0/(1-stand->cell->lakefrac)));

    if(config->pft_output_scaled)
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp*stand->frac;
    else
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp;
      getoutputindex(output,PFT_LAI,nnat+index,config)+=actual_lai(pft);
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
  output_gbw_agriculture_tree(output,stand,frac_g_evap,evap,evap_blue,
                              return_flow_b,aet_stand,green_transp,
                              intercep_stand,intercep_stand_blue,npft,
                              ncft,config);
#ifdef DEBUG
  if(iscotton)
    printf("growing_day: %d %d %d\n",stand->growing_days,data->irrigation.irrigation,stand->cell->ml.growing_season_cotton[data->irrigation.irrigation]);
#endif
  if(iscotton && stand->growing_days==stand->cell->ml.growing_season_cotton[data->irrigation.irrigation])
  {
    /* find cotton in PFT list */
    p=findagtree(&stand->pftlist,data->irrigation.pft_id);
    if(p!=NOT_FOUND)
    {
      pft=getpft(&stand->pftlist,p);
      yield=harvest_tree(pft);
      tree=pft->data;
      tree->boll_age=0;
#ifdef DEBUG
      printf("%s yield %s=%g t/ha, %g indiv/ha, wstress=%g, fpc=%g\n",(data->irrigation.irrigation) ? "irrigated" :"",pft->par->name,yield.carbon*1e4/1e6/0.45,pft->nind*1e4,pft->wscal_mean/365,pft->fpc);
      printf("index=%d\n",index);
      printf("harvest(%s) %d: %g %g\n",config->pftpar[data->irrigation.pft_id].name,data->irrigation.irrigation,yield.carbon,yield.nitrogen);
#endif
      getoutput(output,HARVESTC,config)+=yield.carbon*stand->frac;
      getoutput(output,HARVESTN,config)+=yield.nitrogen*stand->frac;
      if(config->pft_output_scaled)
      {
#if defined IMAGE && defined COUPLED
        stand->cell->pft_harvest[index]+=yield.carbon*stand->frac;
#endif
        getoutputindex(output,PFT_HARVESTC,index,config)+=yield.carbon*stand->frac;
        getoutputindex(output,PFT_HARVESTN,index,config)+=yield.nitrogen*stand->frac;
      }
      else
      {
#if defined IMAGE && defined COUPLED
        stand->cell->pft_harvest[index]+=yield.carbon;
#endif
        getoutputindex(output,PFT_HARVESTC,index,config)+=yield.carbon;
        getoutputindex(output,PFT_HARVESTN,index,config)+=yield.nitrogen;
      }
      stand->cell->balance.biomass_yield.carbon+=yield.carbon*stand->frac;
      stand->cell->balance.biomass_yield.nitrogen+=yield.nitrogen*stand->frac;
      stand->cell->output.dcflux+=yield.carbon*stand->frac;
      annual_tree(stand,pft,&fpc_inc,climate->isdailytemp,config);
      getoutputindex(&stand->cell->output,PFT_VEGC,nnat+index,config)+=vegc_sum(pft);
      getoutputindex(&stand->cell->output,PFT_VEGN,nnat+index,config)+=vegn_sum(pft)+pft->bm_inc.nitrogen;
      getoutputindex(&stand->cell->output,PFT_CROOT,nnat+index,config)+=tree->ind.root.carbon;
      getoutputindex(&stand->cell->output,PFT_NROOT,nnat+index,config)+=tree->ind.root.nitrogen;
      getoutputindex(&stand->cell->output,PFT_CLEAF,nnat+index,config)+=tree->ind.leaf.carbon;
      getoutputindex(&stand->cell->output,PFT_NLEAF,nnat+index,config)+=tree->ind.leaf.nitrogen;
      getoutputindex(&stand->cell->output,PFT_CSAPW,nnat+index,config)+=tree->ind.sapwood.carbon;
      getoutputindex(&stand->cell->output,PFT_NSAPW,nnat+index,config)+=tree->ind.sapwood.nitrogen;
      getoutputindex(&stand->cell->output,PFT_CHAWO,nnat+index,config)+=tree->ind.heartwood.carbon;
      getoutputindex(&stand->cell->output,PFT_NHAWO,nnat+index,config)+=tree->ind.heartwood.nitrogen;
      litter_update_tree(&stand->soil.litter,pft,pft->nind,config);
      delpft(&stand->pftlist,p);
      stand->growing_days=0;
    }
    else
      fprintf(stderr,"ERROR124: Cotton PFT not found in cell %s.\n",sprintcoord(line,&stand->cell->coord));
    update_irrig(stand,agtree(ncft,config->nwptype)+data->irrigation.pft_id-npft,ncft,config);
    //if(setaside(stand->cell,stand,config->pftpar,TRUE,npft,data->irrigation,year))
    // return TRUE;
  }
  if(data->irrigation.irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,&data->irrigation,gp_stand,wet,eeq,config->others_to_crop);
  free(wet);

  return runoff;
} /* of 'daily_agriculture_tree' */
