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
#include "crop.h"

#define fbd_c3_livegrass 4.0
#define fbd_c4_livegrass 4.0
#define NGLIM 5

static Real alpha[NFUELCLASS]={0.001,0.00005424,0.00001485,0};
Real sigma_dead[NFUELCLASS]={66.0,3.58,0.98,66.0};
Real sigma_live[2]={66.0,0};
static Real glim[NGLIM]={16,48,96,1200,1000000};

void fuelload(const Stand *stand, /**< pointer to stand */
              Fuel *fuel,
              Livefuel *livefuel,
              Real nesterov_accum, /**< accumulated Nesterov index */
              const Config *config /**< LPJmL configuration */
             )

{
  Real dead_fuel, mean_w;
  Real ratio_dead_fuel, ratio_live_fuel;
  Real alpha_fuel, alpha_livegrass;
  Real moist_extinct, moist_livegrass_1hr;
  Real ratio_c3_livegrass, ratio_c4_livegrass;
  Real livegrass,dlm_1hr,fbd_livefuel,fbd_deadfuel;
  Real fuel_gBiomass[NFUELCLASS];
  Real adead[NFUELCLASS];
  Real alive[2];
  Real fsum[NGLIM+1];
  Real alive_sum=0;
  Real adead_sum=0;
  Pft *pft;
  Pftgrass *grass;
  Pftcrop *crop;
  int p,i,index;
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
  getoutputindex(&stand->cell->output,FUEL,0,config)+=fuel_gBiomass[0];
  for (i=1; i<NFUELCLASS;++i) /* 1hr fuel consumption not included*/
  {
    fuel_gBiomass[i]=c2biomass(litter_ag_tree(&stand->soil.litter,i));
    getoutputindex(&stand->cell->output,FUEL,i,config)+=fuel_gBiomass[i];
  }
  /* dead fuel biomass */
  dead_fuel = c2biomass(litter_ag_sum_quick(&stand->soil.litter));
  /* Calculate livegrass biomass [g/m2]*/
  livegrass = 0;
  foreachpft(pft,p,&stand->pftlist)
  {
    if(isgrass(pft))
    {
      grass=pft->data;
      livegrass += c2biomass((grass->ind.leaf.carbon * pft->nind )* pft->phen);
      if(pft->par->path==C3)
        livefuel->pot_fc_lg_c3 = c2biomass(grass->ind.leaf.carbon*pft->nind*pft->phen);
      else
        livefuel->pot_fc_lg_c4 = c2biomass(grass->ind.leaf.carbon*pft->nind*pft->phen);
    }
    else if(iscrop(pft))
    {
      crop=pft->data;
      livegrass += c2biomass((crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon) * pft->nind );
      if(pft->par->path==C3)
        livefuel->pot_fc_lg_c3 = c2biomass((crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon)*pft->nind);
      else
        livefuel->pot_fc_lg_c4 = c2biomass((crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon)*pft->nind);
    }

  }
  getoutput(&stand->cell->output,LIVEGRASS,config)+=livegrass;

  /* Calculate Rothermel's f factors, neglecting unit conversions in a values since they cancel*/
  for(i=0;i<NFUELCLASS-1;++i) /* looping to second last value to exclude 1000 hour fuels */
  {
    fuel->w[i]=fuel_gBiomass[i];
  }
  fuel->w[NFUELCLASS-1]=0; /*cured grass weight, setting to 0 as placeholder until curing can be included */
  for(i=0;i<NFUELCLASS;++i)
  {
    adead[i]=sigma_dead[i]*fuel->w[i]/PART_DENS;
    adead_sum+=adead[i];
  }
  livefuel->w[0]=livegrass; 
  livefuel->w[1]=0; /* setting to 0 as placeholder for live woody component*/
  for(i=0;i<2;++i)
  {
    alive[i]=sigma_live[i]*livefuel->w[i]/PART_DENS;
    alive_sum+=alive[i];
  }
  for(i=0;i<NFUELCLASS;++i)
   {
    fuel->f[i]=(adead_sum>0) ? adead[i]/adead_sum : 0;
   } 
  for(i=0;i<2;++i)
   {
   livefuel->f[i]=(alive_sum>0) ? alive[i]/alive_sum : 0;
   }
  fuel->fi=(adead_sum || alive_sum > 0) ? adead_sum/(adead_sum+alive_sum) : 0;
  livefuel->fi=(adead_sum || alive_sum > 0) ? alive_sum/(adead_sum+alive_sum) : 0;
  /* calculating g factors from Albini 1976 (loop to find sum of f factors in sigma bin and then second loop to assign the sum to each fuel class)*/
  for(i=0;i<NGLIM+1;++i)
    fsum[i]=0;
  for(i=0;i<NFUELCLASS;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_dead[i]<glim[index]/30.48) /*conversion from ft^-1 to cm^-1*/
        break;
    fsum[index]+=fuel->f[i];
  }
      /* assume nothing falls in class 0 (surface area to volume ratio < 16 ft^-1)*/ 
  for(i=0;i<NFUELCLASS;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_dead[i]<glim[index]/30.48)
        break;
    fuel->g[i]=fsum[index];
  }
   /* live fuels calculation of g factors */ 
  for(i=0;i<NGLIM+1;++i)
    fsum[i]=0;
  for(i=0;i<2;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_live[i]<glim[index]/30.48) /*conversion from ft^-1 to cm^-1*/
        break;
    fsum[index]+=livefuel->f[i];
  }
      /* assume nothing falls in class 0 (surface area to volume ratio < 16 ft^-1)*/ 
  for(i=0;i<2;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_live[i]<glim[index]/30.48)
        break;
    livefuel->g[i]=fsum[index];
  }
 /*calculating live and dead moisture and fbd as in standard 5.3 spitfire*/ 
 dlm_1hr=ratio_dead_fuel=ratio_live_fuel=fbd_deadfuel=mean_w=0;


  /* Compute live fuel moisture, including livegrass moisture from soil moisture (average of top 2 layers rather than 1st layer as in Thonicke 2010)*/
  if(livegrass > 0)
  {
    /*TODO*/
     mean_w=((stand->soil.w[0]*stand->soil.whcs[0]+stand->soil.w_fw[0]+stand->soil.wpwps[0]+
               stand->soil.ice_depth[0]+stand->soil.ice_fw[0])/stand->soil.wsats[0]+
               (stand->soil.w[1]*stand->soil.whcs[1]+stand->soil.w_fw[1]+stand->soil.wpwps[1]+
               stand->soil.ice_depth[1]+stand->soil.ice_fw[1])/stand->soil.wsats[1])/2 ;
   //mean_w=mean_w/((stand->soil.bulkdens[0]*soildepth[0]+stand->soil.bulkdens[1]*soildepth[1])/2*1e-3); /*converting mean_w to g water / g soil (denom mm to m) RESULTED IN VERY LOW GRASS MOISTURE, RETURNING TO ORIGINAL DIVISION BY WSATS APPROACH NOW*/
    livefuel->M[0] = (0.0 > ((10.0/9.0) * mean_w -(1.0/9.0)) ?
                                0 : ((10.0/9.0) * mean_w -(1.0/9.0)));
    ratio_c3_livegrass = livefuel->pot_fc_lg_c3 / livegrass;
    ratio_c4_livegrass = livefuel->pot_fc_lg_c4 / livegrass;
  }
  else
  {
    livefuel->M[0] = 0;
    ratio_c3_livegrass = 0;
    ratio_c4_livegrass = 0;
  }
  livefuel->M[1] = 9999; /* placeholder value for live woody */
  /* Livegrass weighted average fbd - OLD METHOD, SHOULD BE REPLACED*/
  getoutput(&stand->cell->output,DLM_LIVEGRASS,config)+=livefuel->M[0];
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
#if 0
  if(fuel->sigma_dead > 2*SIGMA[0])
  {
    /* may happen if litter is negative and causes numerical problems in rateofspread*/
    fuel->sigma_dead=SIGMA[0];
  }
  else if(fuel->sigma_dead<epsilon)
    fuel->sigma_dead=epsilon;
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
/* dead litter moisture calculation */
  fuel->daily_litter_moist = exp(-(alpha_fuel) * nesterov_accum); /* old setup using the nesterov index, corrected here to use only dead fuels */
//  fuel->daily_litter_moist =  (dead_fuel>0) ? stand->soil.litter.agtop_moist*1e3/dead_fuel : 999; /* new version making use of new litter moisture calculation from tillage version */
 /* setting litter moisture values for all classes to the same value until this can be replaced with a new system */
  fuel->M[0]=fuel->daily_litter_moist;
  fuel->M[1]=fuel->daily_litter_moist;
  fuel->M[2]=fuel->daily_litter_moist;
  fuel->M[3]=fuel->M[0]; /* cured grass moisture always set to same value as 1h fuel class */
  /* combustion efficiency for litter */
  fuel->CME = 0.0005*pow(fuel->daily_litter_moist*100,2)-0.02*fuel->daily_litter_moist*100+0.94;  
  dlm_1hr = fuel->M[0]; /* corrected 1 hour fuel moisture, replaces the Nesterov-based one that only uses alpha[0]*/

  /* moisture of extinction (as PFT param.) weighted over litter amount */
  fuel->char_moist_factor= moistfactor(&stand->soil.litter);

  /* influence of livefuel on 1hr fuel moisture content RE-EXAMINE WITH NEW MOISTURE CALCULATIONS*/
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

} /* of 'fuelload' */
