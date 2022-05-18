/**************************************************************************************/
/**                                                                                \n**/
/**        d  a  i  l  y  f  i  r  e  .  c                                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Daily fire of stand                                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define CG 0.2   /* cloud to ground flashes ratio */

void dailyfire(Stand *stand,            /**< pointer to stand */
               Real popdens, /**< population density (capita/km2) */
               Real avgprec,                /**< monthly averaged precipitation (mm/day) */
               const Dailyclimate *climate, /**< daily climate data */
               const Config *config /**< prescribed burnt area (TRUE/FALSE) */
              )
{
  Real fire_danger_index,human_ignition,num_fires,windsp_cover,ros_forward;
  Real burnt_area,fire_frac;
  Real fuel_consump;
  Real fireduration;
  Stocks deadfuel_consump,livefuel_consump,livefuel_consump_pft;
  Real surface_fi;
  Stocks total_fire;
  Fuel fuel;
  Bool isdead;
  int p;
  Output *output;
  Pft *pft;
  Livefuel livefuel={0,0,0,0,0};
  Tracegas emission={0,0,0,0,0,0};
  if(stand->type->landusetype==GRASSLAND && !config->fire_on_grassland)
    return;
  output=&stand->cell->output;
  initfuel(&fuel);

  /*use maximum Nesterov index in previous 90 days for fire simulations if burnt area is prescribed */
  if (config->prescribe_burntarea)
  {
    if (stand->cell->ignition.nesterov_accum > stand->cell->ignition.nesterov_max)
    {
      /* set new maximum NI if actual NI is larger than the previous NI */
      stand->cell->ignition.nesterov_max = stand->cell->ignition.nesterov_accum;
      stand->cell->ignition.nesterov_day = 0;
    }
    else
    {
      /* if actual NI is lower than previous NI, don't change maximum and NI but increase its age by 1 day */
      stand->cell->ignition.nesterov_day++;
    }
    if (stand->cell->ignition.nesterov_day > 90)
    {
      /* if maximum NI is older than 90 days, set maximum to the actual NI value and its age to 0 */
      stand->cell->ignition.nesterov_max = stand->cell->ignition.nesterov_accum;
      stand->cell->ignition.nesterov_day = 0;
    }
  }
  else
  {
    /* if burnt area is simulated use the actual Nesterov index instead the maximum */
    stand->cell->ignition.nesterov_max = stand->cell->ignition.nesterov_accum;
  }
  fuelload(stand, &fuel,&livefuel, stand->cell->ignition.nesterov_max);
  fire_danger_index=firedangerindex(fuel.char_moist_factor,
                                    stand->cell->ignition.nesterov_max,
                                    &stand->pftlist,climate->humid,
                                    avgprec,config->fdi,climate->temp);
  if(config->prescribe_ignition)
    num_fires=climate->ignition;
  else
  {
    human_ignition=humanignition(popdens,&stand->cell->ignition);
    num_fires=wildfire_ignitions(fire_danger_index,
                                 human_ignition+climate->lightning*param.cg_ratio*param.ler,
                                 stand->cell->coord.area*stand->frac);

    if(stand->type->landusetype==NATURAL)
    {
      getoutput(output,LIGHTNING,config) +=climate->lightning*param.cg_ratio*param.ler*fire_danger_index*stand->cell->coord.area*stand->frac*1e-4;
      getoutput(output,HUMAN_IGNITION,config) +=human_ignition*fire_danger_index*stand->cell->coord.area*stand->frac*1e-4;
    }
  }
  windsp_cover=windspeed_fpc(climate->windspeed,&stand->pftlist);
  ros_forward=rateofspread(windsp_cover,&fuel);
  /* use prescribed burnt area or calculate burnt area */
  if (config->prescribe_burntarea)
    burnt_area = climate->burntarea;
  else
  {
    burnt_area = area_burnt(&fireduration,stand->type->max_fireduration,fire_danger_index, num_fires, windsp_cover, ros_forward, config->ntypes, &stand->pftlist);
    if(stand->type->landusetype==NATURAL)
      getoutput(output,FIREDURATION,config)+=fireduration;
  }
  fire_frac=burnt_area*1e4 / (stand->cell->coord.area * stand->frac);  /*in m2*/
  if(fire_frac > 1.0)
  {
    burnt_area = stand->cell->coord.area*1e-4 * stand->frac; /*burnt area in ha*/
    fire_frac = 1.0;
  }
  stand->afire_frac+=fire_frac;
  if(stand->afire_frac > 1.0)
  {
    fire_frac = 1.0 - stand->afire_frac + fire_frac;
    burnt_area = stand->cell->coord.area * 1e-4 * stand->frac* fire_frac;
    stand->afire_frac = 1.0;
  }
  /*fuel consumption in gBiomass/m2 for calculation of surface fire intensity*/
  fuel_consump=deadfuel_consumption(&stand->soil.litter,&fuel,fire_frac);
  surface_fi=surface_fire_intensity(fuel_consump, fire_frac, ros_forward);
  /* if not enough surface fire energy to sustain burning */
  if(surface_fi<50)  //&& !prescribe_burntarea)
  {
    stand->afire_frac-=fire_frac;
    num_fires=0;
    burnt_area=0;
    fire_frac=0;
    deadfuel_consump.carbon=deadfuel_consump.nitrogen=0;
  }
  else
  {
    deadfuel_consump=litter_update_fire(&stand->soil.litter,&emission,&fuel);
  }

  fraction_of_consumption(&fuel);

  livefuel_consump.carbon=livefuel_consump.nitrogen=0;
  foreachpft(pft,p,&stand->pftlist)
  {
    if(surface_fi>50)
    {
      livefuel_consump_pft=pft->par->livefuel_consumption(&stand->soil.litter, pft,
                                                          &fuel, &livefuel, &isdead, surface_fi, fire_frac,config);
#ifdef WITH_FIRE_MOISTURE
      emission.co2+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.co2 * (livefuel.CME/0.94);
      emission.co+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.co * (2- livefuel.CME/0.94);
#else
      emission.co2+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.co2;
      emission.co+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.co;
#endif
      emission.ch4+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.ch4;
      emission.voc+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.voc;
      emission.tpm+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.tpm;
      emission.nox+=c2biomass(livefuel_consump_pft.carbon)*pft->par->emissionfactor.nox;

      livefuel_consump.carbon+=livefuel_consump_pft.carbon;
      livefuel_consump.nitrogen+=livefuel_consump_pft.nitrogen;
      if(isdead)
      {
        delpft(&stand->pftlist, p);
        p--;
      }
    }
  }
  total_fire.carbon = (deadfuel_consump.carbon + livefuel_consump.carbon) * stand->frac;
  total_fire.nitrogen = (deadfuel_consump.nitrogen + livefuel_consump.nitrogen);

  /* write SPITFIRE outputs to LPJ output structures */
  getoutput(output,NFIRE,config) += num_fires;
  getoutput(output,FIREF,config) += fire_frac;
  getoutput(output,BURNTAREA,config) += burnt_area; /*ha*/
  getoutputindex(output,STAND_BURNTAREA,0,config) += burnt_area; /*ha*/
  switch(stand->type->landusetype)
  {
    case NATURAL:
      getoutputindex(output,STAND_BURNTAREA,1,config) += burnt_area; /*ha*/
      break;
    case GRASSLAND:
      getoutputindex(output,STAND_BURNTAREA,2,config) += burnt_area; /*ha*/
      break;
    case AGRICULTURE:
      getoutputindex(output,STAND_BURNTAREA,3,config) += burnt_area; /*ha*/
      break;
  }     
  getoutput(output,FIREC,config) += total_fire.carbon;
  if(stand->type->landusetype==NATURAL)
  {
    if (burnt_area>0)
      getoutput(output,FIREDAYS,config) += 1;
    getoutput(output,SURFACE_FI,config) += surface_fi;
    getoutput(output,FIREDI,config) += fire_danger_index;
    getoutput(output,ROS,config) += ros_forward;
    if (num_fires>0)
      getoutput(output,FIRESIZE,config) += burnt_area*1e4/num_fires;
  }
  stand->cell->balance.fire.carbon+=total_fire.carbon;
  getoutput(output,FIREN,config)+=total_fire.nitrogen*(1-param.q_ash)*stand->frac;
  stand->cell->balance.fire.nitrogen+=total_fire.nitrogen*(1-param.q_ash)*stand->frac;
  stand->soil.NO3[0]+=total_fire.nitrogen*param.q_ash;
  output->dcflux+=total_fire.carbon;
  getoutput(output,FIREEMISSION_CO2,config)+=emission.co2*stand->frac;
  getoutput(output,FIREEMISSION_CO,config)+=emission.co*stand->frac;
  getoutput(output,FIREEMISSION_CH4,config)+=emission.ch4*stand->frac;
  getoutput(output,FIREEMISSION_VOC,config)+=emission.voc*stand->frac;
  getoutput(output,FIREEMISSION_TPM,config)+=emission.tpm*stand->frac;
  getoutput(output,FIREEMISSION_NOX,config)+=emission.nox*stand->frac;
}  /* of 'dailyfire' */
