/**************************************************************************************/
/**                                                                                \n**/
/**       d  a  i  l  y  _  w  o  o  d  p  l  a  n  t  a  t  i  o  n  .  c         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of NPP update of woodplantation stand                             \n**/
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
#include "woodplantation.h"

#if defined IMAGE || defined INCLUDEWP
Real daily_woodplantation(Stand *stand,       /**< stand pointer */
                        Real co2,             /**< atmospheric CO2 (ppmv) */
                        const Dailyclimate *climate, /**< Daily climate values */
                        int day,              /**< day (1..365) */
                        Real daylength,       /**< length of day (h) */
                        const Real gp_pft[],  /**< pot. canopy conductance for PFTs & CFTs*/
                        Real gtemp_air,       /**< value of air temperature response function */
                        Real gtemp_soil,      /**< value of soil temperature response function */
                        Real gp_stand,        /**< potential stomata conductance */
                        Real gp_stand_leafon, /**< pot. canopy conduct.at full leaf cover */
                        Real eeq,             /**< equilibrium evapotranspiration (mm) */
                        Real par,             /**< photosynthetic active radiation flux */
                        Real melt,            /**< melting water (mm) */
                        int npft,             /**< number of natural PFTs */
                        int ncft,             /**< number of crop PFTs   */
                        int UNUSED(year),     /**< simulation year */
                        Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                        const Config *config  /**< LPJ config */
                       )                      /** \return runoff (mm) */
{
  int p,l;
  Pft *pft;
  Output *output;
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap; /* evaporation (mm) */
  Real evap_blue; /* evaporation of irrigation water (mm) */
  Real rd,gpp,frac_g_evap,runoff,wet_all,intercept,sprink_interc;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real rainmelt, irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Real npp; /* net primary productivity (gC/m2) */
  Real wdf; /* water deficit fraction */
  Real gc_pft;
  Biomass_tree *data;
  Soil *soil;
  irrig_apply = 0;

  soil = &stand->soil;
  data=stand->data;
  output=&stand->cell->output;
  stand->growing_days++;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=runoff=return_flow_b=wet_all=intercept=sprink_interc=rainmelt=irrig_apply=0.0;
  if (getnpft(&stand->pftlist)>0)
  {
    wet = newvec(Real, getnpft(&stand->pftlist)); /* wet from pftlist */
    check(wet);
    for (p = 0;p<getnpft(&stand->pftlist);p++)
      wet[p] = 0;
  }
  else
    wet = NULL;
  if (!config->river_routing)
    irrig_amount(stand, &data->irrigation,config->pft_output_scaled, npft, ncft);

  if(config->pft_output_scaled)
    stand->cell->output.cft_airrig[rwp(ncft)+data->irrigation.irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->irrigation.irrig_amount*stand->cell->ml.landfrac[1].woodplantation;
  else
    stand->cell->output.cft_airrig[rwp(ncft)+data->irrigation.irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=data->irrigation.irrig_amount;

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[1]=0;

  /* green water inflow */
  rainmelt = climate->prec + melt;
  if (rainmelt<0)
    rainmelt = 0.0;

  if (data->irrigation.irrigation && data->irrigation.irrig_amount>epsilon)
  {
    irrig_apply = max(data->irrigation.irrig_amount - rainmelt, 0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrigation.irrig_stor += data->irrigation.irrig_amount - irrig_apply;
    data->irrigation.irrig_amount = 0.0;
    if (irrig_apply<1 && data->irrigation.irrig_system != DRIP)  /* min. irrigation requirement of 1mm */
    {
      data->irrigation.irrig_stor += irrig_apply;
      irrig_apply = 0.0;
    }
    else
    {
      /* write irrig_apply to output */
      stand->cell->output.mirrig += irrig_apply*stand->frac;
      if (config->pft_output_scaled)
        stand->cell->output.cft_airrig[rwp(ncft) + data->irrigation.irrigation*(ncft + NGRASS + NBIOMASSTYPE+NWPTYPE)] += irrig_apply*stand->cell->ml.landfrac[1].woodplantation;
      else
        stand->cell->output.cft_airrig[rwp(ncft) + data->irrigation.irrigation*(ncft + NGRASS + NBIOMASSTYPE+NWPTYPE)] += irrig_apply;
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
    sprink_interc = (data->irrigation.irrig_system == SPRINK) ? 1 : 0;

    intercept = interception(&wet[p], pft, eeq, climate->prec + irrig_apply*sprink_interc); /* in case of sprinkler, irrig_amount goes through interception, in case of mixed, 0.5 of irrig_amount */
    wet_all += wet[p] * pft->fpc;
    intercep_stand_blue += (climate->prec + irrig_apply*sprink_interc>epsilon) ? intercept*(irrig_apply*sprink_interc) / (climate->prec + irrig_apply*sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand += intercept;
  }

  irrig_apply -= intercep_stand_blue;
  rainmelt -= (intercep_stand - intercep_stand_blue);

  /* soil inflow: infiltration and percolation */
  if (irrig_apply>epsilon)
  {
    runoff+=infil_perc_irr(stand,irrig_apply,&return_flow_b,config->rw_manage);
    /* count irrigation events*/
    output->cft_irrig_events[rwp(ncft) + data->irrigation.irrigation*(ncft + NGRASS + NBIOMASSTYPE+NWPTYPE)]++;
  }

  runoff+=infil_perc_rain(stand,rainmelt,&return_flow_b,config->rw_manage);

  foreachpft(pft, p, &stand->pftlist)
  {
    cover_stand += pft->fpc*pft->phen;
   /* calculate albedo and FAPAR of PFT */
    albedo_pft(pft, soil->snowheight, soil->snowfraction);

/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */

    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,config);

    if(stand->cell->ml.landfrac[data->irrigation.irrigation].woodplantation>0.0 &&
      gp_pft[getpftpar(pft,id)]>0.0)
   {
     output->gcgp_count[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation.irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]++;
     output->pft_gcgp[(npft-config->nbiomass)+rwp(ncft)+data->irrigation.irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=gc_pft/gp_pft[getpftpar(pft,id)];
   }
   npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd);
   output->mnpp+=npp*stand->frac;
#ifdef COUPLED
   stand->cell->output.npp_wp+=npp*stand->frac;
#endif
   output->dcflux-=npp*stand->frac;
   output->mgpp+=gpp*stand->frac;
   output->cft_fpar[rwp(ncft) + data->irrigation.irrigation*(ncft + NGRASS + NBIOMASSTYPE+NWPTYPE)] += (fpar(pft)*stand->cell->ml.landfrac[data->irrigation.irrigation].woodplantation*(1.0 / (1 - stand->cell->lakefrac)));
   if (config->pft_output_scaled)
     output->pft_npp[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation.irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=npp*stand->cell->ml.landfrac[data->irrigation.irrigation].woodplantation;
   else
     output->pft_npp[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation.irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=npp;
  } /* of foreachpft */

  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);

  if (data->irrigation.irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand, &data->irrigation,gp_stand, wet, eeq);

  forrootsoillayer(l)
  {
    output->mtransp += aet_stand[l] * stand->frac;
    output->mtransp_b += (aet_stand[l] - green_transp[l])*stand->frac;
  }

  output->minterc += intercep_stand*stand->frac; /* Note: including blue fraction*/
  output->minterc_b += intercep_stand_blue*stand->frac;   /* blue interception and evap */

  output->mevap += evap*stand->frac;
  output->mevap_b += evap_blue*stand->frac;   /* blue soil evap */

  output->mreturn_flow_b += return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

  /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw_woodplantation(output, stand, frac_g_evap, evap, evap_blue, return_flow_b, aet_stand, green_transp,
    intercep_stand, intercep_stand_blue, ncft, config->pft_output_scaled);
  free(wet);
  return runoff;
} /* of 'daily_woodplantation' */
#endif

