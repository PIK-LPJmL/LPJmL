/**************************************************************************************/
/**                                                                                \n**/
/**       d a i l y _ a g r i c u l t u r e _ g r a s s . c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of agriculture grass stand                        \n**/
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
#include "agriculture_grass.h"
#include "agriculture_tree.h"

Real daily_agriculture_grass(Stand *stand,                /**< stand pointer */
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
                             int UNUSED(year),            /**< simulation year (AD) */
                             Bool UNUSED(intercrop),      /**< enabled intercropping */
                             Real UNUSED(agrfrac),        /**< [in] total agriculture fraction (0..1) */
                             const Config *config         /**< LPJ config */
                            )                             /** \return runoff (mm/day) */

{
  int p,l,nnat,index;
  Pft* pft;
  Output* output;
  Harvest harvest={{0,0},{0,0},{0,0},{0,0}};
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Real evap, evap_blue, rd, gpp, frac_g_evap, runoff, wet_all, intercept, sprink_interc;
  Real* wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real transp;
  Real rainmelt, irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Soil* soil;
  Real npp; /* net primary productivity (gC/m2) */
  Real gc_pft, gcgp;
  Real wdf; /* water deficit fraction */
  Real fertil;
  Real manure;
  Bool isphen;
  Irrigation* data;
  Pftgrass* grass;
  irrig_apply = 0.0;

  soil = &stand->soil;
  data = stand->data;
  output = &stand->cell->output;
  evap = evap_blue = cover_stand = intercep_stand = intercep_stand_blue = runoff = return_flow_b = wet_all = intercept = sprink_interc = 0;
  if (getnpft(&stand->pftlist) > 0)
  {
    wet = newvec(Real, getnpft(&stand->pftlist)); /* wet from pftlist */
    check(wet);
    for (p = 0; p < getnpft(&stand->pftlist); p++)
      wet[p] = 0;
  }
  else
    wet = NULL;
  gp_pft=newvec(Real,npft+ncft);
  check(gp_pft);
  gp_stand=gp_sum(&stand->pftlist,co2,climate->temp,par,daylength,
                  &gp_stand_leafon,gp_pft,&fpc_total_stand,config);

  if (!config->river_routing)
    irrig_amount(stand,data,npft,ncft,month,config);

  for (l = 0; l < LASTLAYER; l++)
    aet_stand[l] = green_transp[l] = 0;
  if (config->with_nitrogen && stand->cell->ml.fertilizer_nr!=NULL) /* has to be adapted if fix_fertilization option is added */
  {
    if(day==fertday_biomass(stand->cell,config))
    {
      fertil = stand->cell->ml.fertilizer_nr[data->irrigation].ag_tree[data->pft_id-npft+config->nagtree];
      stand->soil.NO3[0]+=fertil*param.nfert_no3_frac;
      stand->soil.NH4[0]+=fertil*(1-param.nfert_no3_frac);
      stand->cell->balance.influx.nitrogen+=fertil*stand->frac;
      getoutput(output,NFERT_AGR,config)+=fertil*stand->frac;
      getoutput(output,NAPPLIED_MG,config)+=fertil*stand->frac;
    } /* end fday==day */
  }
  if (config->with_nitrogen && stand->cell->ml.manure_nr!=NULL)
  {
    if(day==fertday_biomass(stand->cell,config))
    {
      manure = stand->cell->ml.manure_nr[data->irrigation].ag_tree[data->pft_id-npft+config->nagtree];
      stand->soil.NH4[0] += manure*param.nmanure_nh4_frac;
      stand->soil.litter.item->agsub.leaf.carbon += manure*param.manure_cn;
      stand->soil.litter.item->agsub.leaf.nitrogen += manure*(1-param.nmanure_nh4_frac);
      stand->cell->balance.influx.carbon += manure*param.manure_cn*stand->frac;
      getoutput(&stand->cell->output,NBP,config) += manure*param.manure_cn*stand->frac;
      stand->cell->balance.influx.nitrogen += manure*stand->frac;
      getoutput(&stand->cell->output,NMANURE_AGR,config)+=manure*stand->frac;
      getoutput(&stand->cell->output,NAPPLIED_MG,config)+=manure*stand->frac;
    } /* end fday==day */
  }

  /* green water inflow */
  rainmelt = climate->prec + melt;
  if (rainmelt < 0)
    rainmelt = 0.0;

  nnat=getnnat(npft,config);
  index=agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+data->irrigation*getnirrig(ncft,config);

  if (data->irrigation && data->irrig_amount > epsilon)
  {
    irrig_apply = max(data->irrig_amount - rainmelt, 0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrig_stor += data->irrig_amount - irrig_apply;
    data->irrig_amount = 0.0;
    if (irrig_apply < 1 && data->irrig_system != DRIP) /* min. irrigation requirement of 1mm */
    {
      data->irrig_stor += irrig_apply;
      irrig_apply = 0.0;
    }
    else
    {
      /* write irrig_apply to output */
      getoutput(output,IRRIG,config) += irrig_apply * stand->frac;
      stand->cell->balance.airrig+=irrig_apply*stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]+=irrig_apply*stand->frac;
        stand->cell->ml.image_data->mevapotr[month] += irrig_apply*stand->frac;
      }
#endif

      if (config->pft_output_scaled)
        getoutputindex(output,CFT_AIRRIG,index,config) += irrig_apply * stand->frac;
      else
        getoutputindex(output,CFT_AIRRIG,index,config) += irrig_apply;
    }
  }

  /* INTERCEPTION */
  foreachpft(pft, p, &stand->pftlist)
  {
    sprink_interc = (data->irrig_system == SPRINK) ? 1 : 0;

    intercept = interception(&wet[p], pft, eeq, climate->prec + irrig_apply * sprink_interc);
    wet_all += wet[p] * pft->fpc;
    intercep_stand_blue += (climate->prec + irrig_apply * sprink_interc > epsilon) ? intercept * (irrig_apply * sprink_interc) / (climate->prec + irrig_apply * sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand += intercept;
  }
  irrig_apply -= intercep_stand_blue;
  rainmelt -= (intercep_stand - intercep_stand_blue);

  /* soil inflow: infiltration and percolation */
  if (irrig_apply > epsilon)
  {
    runoff += infil_perc_irr(stand, irrig_apply, &return_flow_b,npft,ncft,config);
    /* count irrigation events*/
    getoutputindex(output,CFT_IRRIG_EVENTS,index,config)++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
    }

    runoff += infil_perc_rain(stand, rainmelt, &return_flow_b,npft,ncft,config);

  /* Version with daily allocation and grass management */
  /* #ifdef NEW_GRASS */

  isphen = FALSE;

  foreachpft(pft, p, &stand->pftlist)
  {
    // pft->phen = 1.0; /* phenology is calculated from biomass */
    if (config->gsi_phenology)
      phenology_gsi(pft, climate->temp, climate->swdown, day,climate->isdailytemp,config);
    else
      leaf_phenology(pft, climate->temp, day,climate->isdailytemp,config);
    cover_stand += pft->fpc * pft->phen;

    /* calculate albedo and FAPAR of PFT */
    albedo_grass(pft, soil->snowheight, soil->snowfraction);

    /*
     *  Calculate net assimilation, i.e. gross primary production minus leaf
     *  respiration, including conversion from FPC to grid cell basis.
     *
     */

    gpp = water_stressed(pft, aet_stand, gp_stand, gp_stand_leafon,
                       gp_pft[getpftpar(pft, id)], &gc_pft, &rd,
                       &wet[p], eeq, co2, climate->temp, par, daylength,
                       &wdf,nnat+index,npft,ncft,config);
    getoutput(output,RD,config)+=rd*stand->frac;
    if (gp_pft[getpftpar(pft, id)] > 0.0)
    {
      gcgp = gc_pft / gp_pft[getpftpar(pft, id)];
      if (stand->frac > 0.0)
      {
         getoutputindex(output,PFT_GCGP_COUNT,nnat + index,config)++;
         getoutputindex(output,PFT_GCGP,nnat + index,config) += gcgp;
      }
    }
    npp = npp_grass(pft, gtemp_air, gtemp_soil, gpp - rd - pft->npp_bnf,config->with_nitrogen);
    pft->npp_bnf=0.0;
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    getoutput(output,NPP,config) += npp * stand->frac;
    getoutput(output,NBP,config) += npp * stand->frac;
    output->dcflux -= npp * stand->frac;
    getoutput(output,GPP,config) += gpp * stand->frac;
    getoutput(output,FAPAR,config) += pft->fapar * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMIN,config)+= pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMAX,config)+= pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_LIGHT,config)+= pft->fpc * pft->phen_gsi.light * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_WATER,config)+= pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,WSCAL,config)+= pft->fpc * pft->wscal * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac));

    getoutputindex(output,CFT_FPAR,index,config) += (fpar(pft) * stand->frac * (1.0 / (1 - stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

    if (config->pft_output_scaled)
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp*stand->frac;
    else
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp;
  } /* of foreachpft */
  free(gp_pft);
  /* calculate water balance */
  waterbalance(stand, aet_stand, green_transp, &evap, &evap_blue, wet_all, eeq, cover_stand,
                 &frac_g_evap, config->rw_manage);

    /* allocation, turnover and harvest AFTER photosynthesis */
  stand->growing_days = 1;
    /* turnover must happen before allocation */
  foreachpft(pft, p, &stand->pftlist)
    turnover_grass(&stand->soil.litter, pft,(Real)stand->growing_days / NDAYYEAR,config);
  allocation_today(stand, config);

  /* daily turnover and harvest check*/
  isphen = FALSE;
  foreachpft(pft, p, &stand->pftlist)
  {
    grass = pft->data;
    grass->max_leaf = max(grass->max_leaf, grass->ind.leaf.carbon);
    if (grass->ind.leaf.carbon >= 350 || (grass->ind.leaf.carbon > 1 && grass->ind.leaf.carbon < (0.75 * grass->max_leaf))) //changed from 100 to 350
      isphen = TRUE;
  } /* of foreachpft() */

  if (isphen)
  {
    harvest = harvest_stand(output, stand, 0.75,config);
    if (data->irrigation)
    {
      stand->cell->discharge.dmass_lake += (data->irrig_stor + data->irrig_amount) * stand->cell->coord.area * stand->frac;
      stand->cell->balance.awater_flux -= (data->irrig_stor + data->irrig_amount) * stand->frac;
      getoutput(output,STOR_RETURN,config) += (data->irrig_stor + data->irrig_amount) * stand->frac;

      /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
      stand->cell->discharge.dmass_lake += (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * data->conv_evap * stand->cell->coord.area * stand->frac;
      stand->cell->balance.awater_flux -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * data->conv_evap * stand->frac;
      getoutput(output,CONV_LOSS_EVAP,config)-= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * data->conv_evap * stand->frac;
      stand->cell->balance.aconv_loss_evap -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * data->conv_evap * stand->frac;
      getoutput(output,CONV_LOSS_DRAIN,config) -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * (1 - data->conv_evap) * stand->frac;
      stand->cell->balance.aconv_loss_drain -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * (1 - data->conv_evap) * stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        stand->cell->ml.image_data->mevapotr[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
      }
#endif

      if (config->pft_output_scaled)
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config) -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * data->conv_evap * stand->frac;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config) -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * (1 - data->conv_evap) * stand->frac;

      }
      else
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config) -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * data->conv_evap;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config) -= (data->irrig_stor + data->irrig_amount) * (1 / data->ec - 1) * (1 - data->conv_evap);
      }

      data->irrig_stor = 0;
      data->irrig_amount = 0;
    }
  }

  if (data->irrigation && stand->pftlist.n > 0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,data,gp_stand, wet, eeq,config->others_to_crop);
  transp=0;
  forrootsoillayer(l)
  {
    transp += aet_stand[l] * stand->frac;
    getoutput(output,TRANSP_B,config) += (aet_stand[l] - green_transp[l]) * stand->frac;
  }
  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,EVAP,config) += evap * stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  getoutput(output,INTERC,config) += intercep_stand * stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;

  getoutput(output,EVAP_B,config) += evap_blue * stand->frac;   /* blue soil evap */

  getoutput(output,INTERC_B,config) += intercep_stand_blue * stand->frac;   /* blue interception and evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  getoutput(output,RETURN_FLOW_B,config) += return_flow_b * stand->frac;

  if (config->pft_output_scaled)
  {
#if defined(IMAGE) && defined(COUPLED)
    stand->cell->pft_harvest[index]+=harvest.harvest.carbon*stand->frac;
#endif
    getoutputindex(output,PFT_HARVESTC,index,config)+= harvest.harvest.carbon * stand->frac;
    getoutputindex(output,PFT_HARVESTN,index,config)+= harvest.harvest.nitrogen * stand->frac;
  }
  else
  {
#if defined(IMAGE) && defined(COUPLED)
    stand->cell->pft_harvest[index]+=harvest.harvest.carbon;
#endif
    getoutputindex(output,PFT_HARVESTC,index,config)+= harvest.harvest.carbon;
    getoutputindex(output,PFT_HARVESTN,index,config)+= harvest.harvest.nitrogen;
  }

  /* harvested area */
  if (isphen)
  {
    getoutputindex(output,CFTFRAC,index,config) += stand->frac;
    getoutputindex(output,CFT_NHARVEST,index,config) += 1.0;
  }
  output_gbw(output, stand, frac_g_evap, evap, evap_blue,
             return_flow_b, aet_stand, green_transp,
             intercep_stand, intercep_stand_blue,
             index, data->irrigation, config);
  free(wet);
  return runoff;
} /* of 'daily_agriculture_grass' */
