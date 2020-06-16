/**************************************************************************************/
/**                                                                                \n**/
/**               f  u  e  l  l  o  a  d  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Kirsten Thonicke                                                           \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"

#define moist_extinct_livegrass 0.2
#define fbd_c3_livegrass 4.0
#define fbd_c4_livegrass 4.0

static Real alpha[NFUELCLASS]={0.001,0.00005424,0.00001485,0};
static Real SIGMA[NFUELCLASS]={66.0,3.58,0.98,0};


void fuelload(Stand *stand,
              Fuel *fuel,
              Livefuel *livefuel,
              Real nesterov_accum /**< accumulated Nesterov index */
             )

{
  Real dead_fuel, net_fuel,mean_w;
  Real ratio_dead_fuel, ratio_live_fuel;
  Real alpha_fuel, alpha_livegrass;
  Real moist_extinct, moist_livegrass_1hr;
  Real ratio_c3_livegrass, ratio_c4_livegrass;
  Real livegrass,dlm_1hr,fbd_livefuel,fbd_deadfuel;
  Real fuel_gBiomass[NFUELCLASS];
  Pft *pft;
  Pftgrass *grass;
  int p,i;

  dlm_1hr=ratio_dead_fuel=ratio_live_fuel=fbd_deadfuel=mean_w=0;

  /* for alpha_fuel calculation */
  for(i=0;i<NFUELCLASS;i++)
    fuel_gBiomass[0]=0;

  /*foreachpft(pft,p,stand->pftlist) fuel_sum(pft,fuel->fuel_total);*/
  /*for(i=0;i<stand->litter.n;i++)
  {
    fuel_gBiomass[0]+=c2biomass(litter->ag[i].trait.leaf+litter->ag[i].trait.wood[0]);
    for(j=1;j<NFUELCLASS;j++)
      fuel_gBiomass[j]+=c2biomass(litter->ag[i].trait.wood[j]);
  } */
  /*TODO: simplify loop with new function litter_ag_tree.c!! */
  fuel_gBiomass[0]=c2biomass(litter_ag_grass(&stand->soil.litter)+litter_ag_tree(&stand->soil.litter,0));
  for (i=1; i<NFUELCLASS;++i) /* 1hr fuel consumption not included*/
    fuel_gBiomass[i]=c2biomass(litter_ag_tree(&stand->soil.litter,i));

  /* Dead fuel load, excluding 1000 hr fuels & convert to biomass (g m-2) */
  dead_fuel = c2biomass(litter_ag_sum_quick(&stand->soil.litter));

  /* Net fuel load (kg biomass)*/
  if (dead_fuel > 0)
    net_fuel = (1.0 - MINER_TOT) * (dead_fuel*1e-3);
  else
    net_fuel = 0;

  /* Calculate livegrass biomass [g/m2]*/
  livegrass = 0;
  foreachpft(pft,p,&stand->pftlist)
  {
    if(isgrass(pft))
    {
      grass=pft->data;
      livegrass += c2biomass((grass->ind.leaf * pft->nind )* pft->phen);
      if(pft->par->path==C3)
        livefuel->pot_fc_lg_c3 = c2biomass(grass->ind.leaf*pft->nind*pft->phen);
      else
        livefuel->pot_fc_lg_c4 = c2biomass(grass->ind.leaf*pft->nind*pft->phen);
    }
  }
  fuel->char_net_fuel = net_fuel +(1.0-MINER_TOT)*livegrass*1e-3;  /*in kg biomass */

  /* Compute dry litter moisture for livegrass from soil moisture */
  if(livegrass > 0)
  {
    /*TODO*/
    mean_w=((stand->soil.w[0]*stand->soil.par->whcs[0]+stand->soil.w_fw[0]+stand->soil.par->wpwps[0]+
            stand->soil.ice_depth[0]+stand->soil.ice_fw[0])/stand->soil.par->wsats[0]+
            (stand->soil.w[1]*stand->soil.par->whcs[1]+stand->soil.w_fw[1]+stand->soil.par->wpwps[1]+
            stand->soil.ice_depth[1]+stand->soil.ice_fw[1])/stand->soil.par->wsats[1])/2 ;
    livefuel->dlm_livegrass = (0.0 > ((10.0/9.0) * mean_w -(1.0/9.0)) ?
                                0 : ((10.0/9.0) * mean_w -(1.0/9.0)));
    ratio_c3_livegrass = livefuel->pot_fc_lg_c3 / livegrass;
    ratio_c4_livegrass = livefuel->pot_fc_lg_c4 / livegrass;
  }
  else
  {
    livefuel->dlm_livegrass = 0;
    ratio_c3_livegrass = 0;
    ratio_c4_livegrass = 0;
  }

  /* Livegrass weighted average fbd */

  /*   NEED TO STORE C3/C4 FBD and STORE GRASS FBD AVE -???*/
  /* average fuel bulk density for live and dead fuel*/
  fbd_livefuel = fbd_c3_livegrass * ratio_c3_livegrass +
                 fbd_c4_livegrass * ratio_c4_livegrass;
  fbd_deadfuel = stand->soil.litter.avg_fbd[NFUELCLASS]*litter_ag_grass(&stand->soil.litter);
  for (i=0; i<NFUELCLASS-1;++i)
    fbd_deadfuel += stand->soil.litter.avg_fbd[i]*litter_ag_tree(&stand->soil.litter,i)*fbd_fac[i]; /*fbd_fac replaces FBD_A + FBD_B*/
  if(dead_fuel > epsilon)
    fbd_deadfuel /= biomass2c(dead_fuel);
    /*fbd_deadfuel /= litter_ag_sum_quick(&stand->soil.litter); */

  if (dead_fuel > epsilon && livegrass > epsilon)
  {
    ratio_dead_fuel = dead_fuel  / (dead_fuel + livegrass);
    ratio_live_fuel = livegrass / (dead_fuel + livegrass);
    fuel->char_dens_fuel_ave = fbd_deadfuel * ratio_dead_fuel +  fbd_livefuel  * ratio_live_fuel;
  }
  else
    fuel->char_dens_fuel_ave = 0;

  /* Calculate sigma */
  if (dead_fuel > epsilon)
  {
    fuel->sigma=0;
    for(i=0;i<NFUELCLASS-1;++i)
      fuel->sigma += fuel_gBiomass[i]*SIGMA[i];
    /* TODO: if some components of dead_fuel <0, sigma is very large*/
    fuel->sigma /= dead_fuel;
  }
  else
    fuel->sigma=0.00001;
#ifdef SAFE
  if(fuel->sigma > 2*SIGMA[0])
  {
    /* may happen if litter is negative and causes numerical problems in rateofspread*/
    fuel->sigma=SIGMA[0];
  }
#endif
  /* Calculate weighted fuel moisture */
  /* To be sent do firedangerindex */
  alpha_fuel = 0.0;
  if(dead_fuel > epsilon)
  {
    for(i=0;i<NFUELCLASS-1;++i)
      alpha_fuel += alpha[i] * fuel_gBiomass[i];
    alpha_fuel /= dead_fuel;
  }

  /* Calculate live grass moisture - backcalculate from NI relationship with dlm_lg */
  if(nesterov_accum > epsilon)
  {
    if(livefuel->dlm_livegrass > 0)
      alpha_livegrass =(log(livefuel->dlm_livegrass)/nesterov_accum)*(-1.0);
    else
      alpha_livegrass = 0.0;

    /* weight moisture by amount for dead and live (grass) fuel available */
    fuel->char_alpha_fuel = alpha_fuel * ratio_dead_fuel + alpha_livegrass * ratio_live_fuel;
  }
  else
    fuel->char_alpha_fuel = 0.0001;
#ifdef SAFE
  if(fuel->char_alpha_fuel < 0)
  {
    fprintf(stderr,"fuel->char_alpha_fuel: %f alpha_fuel %f alpha_livegrass %f\n",
            fuel->char_alpha_fuel,alpha_fuel,alpha_livegrass);
    fuel->char_alpha_fuel = 0.0001;
  }
#endif
  /* daily litter moisture back-calculated from nesterov_accum */
  fuel->daily_litter_moist = exp(-(fuel->char_alpha_fuel) * nesterov_accum);

  /* combustion efficiency for litter */
  fuel->CME = 0.0005*pow(fuel->daily_litter_moist*100,2)-0.02*fuel->daily_litter_moist*100+0.94;  
  dlm_1hr = exp(-alpha[0] * nesterov_accum);

  /* moisture of extinction (as PFT param.) weighted over litter amount */
  moist_extinct = moistfactor(&stand->soil.litter);

  /* and influence of livegrass considered */
  fuel->char_moist_factor = moist_extinct * ratio_dead_fuel + moist_extinct_livegrass * ratio_live_fuel;

  /* influence of livefuel on 1hr fuel moisture content */
  if (livegrass <= epsilon || fuel_gBiomass[0] <= epsilon)
    moist_livegrass_1hr = 1.0;
  else
    moist_livegrass_1hr=(fuel->daily_litter_moist*livegrass + dlm_1hr*fuel_gBiomass[0])
                            / (livegrass + fuel_gBiomass[0]);

  if(fuel->char_moist_factor <= epsilon)
  {
    fuel->moist_1hr=1.0;
    fuel->moist_10_100hr=1.0;
  }
  else
  {
    fuel->moist_1hr=moist_livegrass_1hr/fuel->char_moist_factor;
    fuel->moist_10_100hr=fuel->daily_litter_moist/fuel->char_moist_factor;
  }
  livefuel->CME = 0.0005*pow(fuel->moist_10_100hr*100,2)-0.02*fuel->moist_10_100hr*100+0.94;

  /* mw_weight for rate of spread and fuel consumption */
  /* TODO: equals fuel->moist_10_100hr (correct??)*/
  if (fuel->char_moist_factor <= epsilon)
    fuel->mw_weight = 0.0;
  else
    fuel->mw_weight = fuel->daily_litter_moist / fuel->char_moist_factor;
} /* of 'fuelload' */
