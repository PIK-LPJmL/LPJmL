/**************************************************************************************/
/**                                                                                \n**/
/**               f  u  e  l  l  o  a  d  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function pulls fuel loads and litter moisture, calculates weighting factors    \n**/
/** and live grass and fuel moisture for use in rate of spread and fuel            \n**/
/** consumption                                                                    \n**/
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
#include "crop.h"

#define fbd_c3_livegrass 4.0
#define fbd_c4_livegrass 4.0
#define NGLIM 5 /* length of Albini weighting factor array */

Real sigma_dead[NFUELCLASS]={66.0,3.58,0.98,66.0}; /* surface area to volume ratio of 1,
                                                      10, 100 hr and cured grass fuel
                                                      component, respectively */
Real sigma_live[2]={66.0,0}; /* surface area to volume ratio of live herbaceous and
                                live woody component, respectively (live woody currently
                                not implemented */
static Real glim[NGLIM]={16,48,96,1200,1000000}; /* size bins for Albini weighting factors */

void fuelload(const Stand *stand,  /**< pointer to stand */
              Fuel *fuel,          /**< fuel characteristics */
              Livefuel *livefuel,  /**< live fuel characteristics */
              Real nesterov_accum, /**< accumulated Nesterov index */
              const Config *config /**< LPJmL configuration */
             )

{
  Real dead_fuel, mean_w;
  Real ratio_dead_fuel, ratio_live_fuel;
  Real ratio_c3_livegrass, ratio_c4_livegrass;
  Real livegrass,fbd_livefuel,fbd_deadfuel;
  Real fuel_gBiomass[NFUELCLASS];
  Real adead[NFUELCLASS];
  Real alive[2];
  Real fsum[NGLIM+1];
  Real alive_sum=0;
  Real adead_sum=0;
  Real cured_frac=0;
  Real fpc_grass_sum=0;
  Pft *pft;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Pftcrop *crop;
  int p,i,index;

  /* ==== pulling dead fuel loads ==== */

  /* for alpha_fuel calculation */
  for(i=0;i<NFUELCLASS;i++)
    fuel_gBiomass[0]=0;

  /*TODO: simplify loop with new function litter_ag_tree.c!! */
  fuel_gBiomass[0]=c2biomass(litter_agtop_grass(&stand->soil.litter)+litter_agtop_tree(&stand->soil.litter,0));
  getoutputindex(&stand->cell->output,FUEL,0,config)+=fuel_gBiomass[0];
  for (i=1; i<NFUELCLASS;++i) /* 1hr fuel consumption not included*/
  {
    fuel_gBiomass[i]=c2biomass(litter_agtop_tree(&stand->soil.litter,i));
    getoutputindex(&stand->cell->output,FUEL,i,config)+=fuel_gBiomass[i];
  }
  for(i=0;i<NFUELCLASS-1;++i) /* looping to second last value to exclude 1000 hour fuels */
  {
    fuel->w[i]=fuel_gBiomass[i];
  }
  /* dead fuel biomass */
  dead_fuel = c2biomass(litter_agtop_sum_quick(&stand->soil.litter));

  /* ==== pulling live grass fuel loads ==== */

  /* Calculate livegrass biomass [g/m2]*/
  livegrass = 0;
  livefuel->pot_fc_lg_c3 = 0;
  livefuel->pot_fc_lg_c4 = 0;
  foreachpft(pft,p,&stand->pftlist)
  {
    if(isgrass(pft))
    {
      grass=pft->data;
      livegrass += c2biomass((grass->ind.leaf.carbon * pft->nind ));
      if(pft->par->path==C3)
        livefuel->pot_fc_lg_c3 += c2biomass(grass->ind.leaf.carbon*pft->nind);
      else
        livefuel->pot_fc_lg_c4 += c2biomass(grass->ind.leaf.carbon*pft->nind);
    }
    else if(iscrop(pft))
    {
      crop=pft->data;
      livegrass += c2biomass((crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon) * pft->nind );
      if(pft->par->path==C3)
        livefuel->pot_fc_lg_c3 += c2biomass((crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon)*pft->nind);
      else
        livefuel->pot_fc_lg_c4 += c2biomass((crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon)*pft->nind);
    }

  }
  getoutput(&stand->cell->output,LIVEGRASS,config)+=livegrass;

  /* ==== calculating dead fuel moisture ==== */

  ratio_dead_fuel=ratio_live_fuel=fbd_deadfuel=mean_w=0;

  /* dead litter moisture calculation */
  fuel->daily_litter_moist =  (dead_fuel>epsilon) ? stand->soil.litter.agtop_moist*1e3/dead_fuel : 999; /* new version making use of new litter moisture calculation from tillage version */

  /* TODO: implement separate fuel moisture content by size class */
  fuel->M[0]=fuel->daily_litter_moist;
  fuel->M[1]=fuel->daily_litter_moist;
  fuel->M[2]=fuel->daily_litter_moist;
  fuel->M[3]=fuel->M[0]; /* cured grass moisture always set to same value as 1h fuel class */
  /* combustion efficiency for litter */
  fuel->CME = 0.0005*pow(fuel->daily_litter_moist*100,2)-0.02*fuel->daily_litter_moist*100+0.94;

  /* moisture of extinction (as PFT param.) weighted over litter amount, has to be changed if implementing PFT-specific moisture of extinctions */
  fuel->char_moist_factor= moistfactor(&stand->soil.litter);

  /* ==== calculating live fuel moisture ==== */

  if(livegrass > 0)
  {
    if(config->isgsi_livefuel)
      /* use GSI based phenolgy */
      livefuel->M[0]=max(0.3,min(2.5,4.4*stand->cell->gsi_cum-1.9));
    else
    {
      /*compute live fuel moisture based on phen-LFMC empirical relation for each grass PFT, weighted by FPC */
      fpc_grass_sum=0;
      livefuel->M[0]=0;

      foreachpft(pft,p,&stand->pftlist)
      {
        if(isgrass(pft))
        {
          grasspar=pft->par->data;
          livefuel->M[0] += max(grasspar->lfmc_a, min(2.5, grasspar->lfmc_b + grasspar->lfmc_c*pft->phen)) * pft->fpc;
          fpc_grass_sum += pft->fpc;
        }
      }
      if(fpc_grass_sum>0)
        livefuel->M[0] = livefuel->M[0]/fpc_grass_sum;
    }

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

  /*accounting for cured grass in fuel loads */
  cured_frac=max(0,min(1,-1/0.9*livefuel->M[0]+4.0/3.0));
  fuel->w[NFUELCLASS-1]=livegrass*cured_frac;
  livefuel->w[0]=livegrass*(1-cured_frac);
  livefuel->w[1]=0; /* setting to 0 as placeholder for live woody component*/

  /* ==== calculating weighting factors ==== */

  /* calculate Rothermel's f factors, neglecting unit conversions in values since they cancel */
  /* calculating total surface area of each component first */
  for(i=0;i<NFUELCLASS;++i)
  {
    adead[i]=sigma_dead[i]*fuel->w[i]/PART_DENS;
    adead_sum+=adead[i];
  }
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
  fuel->fi=(adead_sum>0) ? adead_sum/(adead_sum+max(0,alive_sum)) : 0;
  livefuel->fi=(alive_sum > 0) ? alive_sum/(max(0,adead_sum)+alive_sum) : 0;
  for(i=0;i<NGLIM+1;++i)
    fsum[i]=0;
  /* calculating total of f factors of each g bin */
  for(i=0;i<NFUELCLASS;++i)
  {
    /* assume nothing falls in class 0 (surface area to volume ratio < 16 ft^-1)*/
    for(index=0;index<NGLIM;++index)
      if(sigma_dead[i]<glim[index]/ft2cm(1)) /*conversion from ft^-1 to cm^-1*/
        break;
    fsum[index]+=fuel->f[i];
  }
  /* assigning the f factor totals to the fuel classes in each g bin */
  for(i=0;i<NFUELCLASS;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_dead[i]<glim[index]/ft2cm(1))
        break;
    fuel->g[i]=fsum[index];
  }
  /* live fuels calculation of g factors */
  for(i=0;i<NGLIM+1;++i)
    fsum[i]=0;
  for(i=0;i<2;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_live[i]<glim[index]/ft2cm(1)) /*conversion from ft^-1 to cm^-1*/
        break;
    fsum[index]+=livefuel->f[i];
  }
  for(i=0;i<2;++i)
  {
    for(index=0;index<NGLIM;++index)
      if(sigma_live[i]<glim[index]/ft2cm(1))
        break;
    livefuel->g[i]=fsum[index];
  }

  /* ==== calculating fuel bulk density ==== */

  /* average fuel bulk density for live and dead fuel*/
  /* TODO: develop accurate fuel bulk density calculation */
  fbd_livefuel = fbd_c3_livegrass * ratio_c3_livegrass +
                 fbd_c4_livegrass * ratio_c4_livegrass;
  fbd_deadfuel = stand->soil.litter.avg_fbd[NFUELCLASS]*litter_agtop_grass(&stand->soil.litter);
  for (i=0; i<NFUELCLASS-1;++i)
    fbd_deadfuel += stand->soil.litter.avg_fbd[i]*litter_agtop_tree(&stand->soil.litter,i)*fbd_fac[i]; /*fbd_fac replaces FBD_A + FBD_B*/
  if(dead_fuel > epsilon)
    fbd_deadfuel /= biomass2c(dead_fuel);

  if (dead_fuel > epsilon && livegrass > epsilon)
  {
    ratio_dead_fuel = dead_fuel  / (dead_fuel + livegrass);
    ratio_live_fuel = livegrass / (dead_fuel + livegrass);
    fuel->char_dens_fuel_ave = fbd_deadfuel * ratio_dead_fuel +  fbd_livefuel  * ratio_live_fuel;
  }
  else
    fuel->char_dens_fuel_ave = 0;
} /* of 'fuelload' */
