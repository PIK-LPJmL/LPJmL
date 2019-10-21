/**************************************************************************************/
/**                                                                                \n**/
/**        d  a  i  l  y  f  i  r  e                                               \n**/
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
#define CG 0.2 /*cloud to ground flashes ratio */
#define LER 0.04 /*efficiency in starting fires */

void dailyfire(Stand *stand,            /**< pointer to stand */
               Livefuel *livefuel,
               Real popdens, /**< population density (capita/km2) */
	       Real avgprec,
               const Dailyclimate *climate, /**< daily climate data */
               const Config *config /**< prescribed burnt area (TRUE/FALSE) */
              )
{
  Real fire_danger_index,human_ignition,num_fires,windsp_cover,ros_forward;
  Real burnt_area,fire_frac;
  Real fuel_consump,deadfuel_consump,livefuel_consump,livefuel_consump_pft;
  Real total_firec,surface_fi;
  Fuel fuel;
  Bool isdead;
  int p;
  Output *output;
  Pft *pft;
  Tracegas emission={0,0,0,0,0,0};
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

  fuelload(stand, &fuel, livefuel, stand->cell->ignition.nesterov_max);
  fire_danger_index=firedangerindex(fuel.char_moist_factor,
                                    stand->cell->ignition.nesterov_max,
                                    &stand->pftlist,climate->humid,
                                    avgprec,config->fdi,climate->temp);
  human_ignition=humanignition(popdens,&stand->cell->ignition);
  num_fires=wildfire_ignitions(fire_danger_index,
                               human_ignition+climate->lightning*CG*LER,
                               stand->cell->coord.area*stand->frac);
  windsp_cover=windspeed_fpc(climate->windspeed  ,&stand->pftlist); 
  ros_forward=rateofspread(windsp_cover,&fuel);

  /* use prescribed burnt area or calculate burnt area */
  if (config->prescribe_burntarea)
    burnt_area = climate->burntarea;
  else
    burnt_area = area_burnt(fire_danger_index, num_fires, windsp_cover, ros_forward, config->ntypes, &stand->pftlist);
  fire_frac=burnt_area*1e4 / (stand->cell->coord.area * stand->frac);  /*in m2*/
  if(fire_frac > 1.0)
  {
    burnt_area = stand->cell->coord.area*1e-4 * stand->frac; /*burnt area in ha*/
    fire_frac = 1.0;
  }
  stand->cell->afire_frac+=fire_frac;
  if(stand->cell->afire_frac > 1.0)
  {
    fire_frac = 1.0 - stand->cell->afire_frac + fire_frac;
    burnt_area = stand->cell->coord.area * 1e-4 * stand->frac* fire_frac; 
    stand->cell->afire_frac = 1.0;
  }
  /*fuel consumption in gBiomass/m2 for calculation of surface fire intensity*/
  fuel_consump=deadfuel_consumption(&stand->soil.litter,&fuel,fire_frac);
  surface_fi=surface_fire_intensity(fuel_consump, fire_frac, ros_forward);
  /* if not enough surface fire energy to sustain burning */
  if(surface_fi<50)  //&& !prescribe_burntarea)
  {
    stand->cell->afire_frac-=fire_frac;
    num_fires=0;
    burnt_area=0;
    fire_frac=0;
    deadfuel_consump=0;
  }
  else
  {
    deadfuel_consump=litter_update_fire(&stand->soil.litter,&emission,&fuel);
  }

  fraction_of_consumption(&fuel);

  livefuel_consump=0;
  foreachpft(pft,p,&stand->pftlist)
  {
    if(surface_fi>50)
    {
      livefuel_consump_pft=pft->par->livefuel_consumption(&stand->soil.litter, pft,
                                                          &fuel, livefuel, &isdead, surface_fi, fire_frac);
#ifdef WITH_FIRE_MOISTURE
      emission.co2+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.co2 * (livefuel->CME/0.94);
      emission.co+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.co * (2- livefuel->CME/0.94);
#else
      emission.co2+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.co2;
      emission.co+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.co;
#endif
      emission.ch4+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.ch4;
      emission.voc+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.voc;
      emission.tpm+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.tpm;
      emission.nox+=c2biomass(livefuel_consump_pft)*pft->par->emissionfactor.nox;
      livefuel_consump+=livefuel_consump_pft;
      if(isdead)
      {
        delpft(&stand->pftlist, p);
        p--;
      }
    }
  }

  total_firec = (deadfuel_consump + livefuel_consump) * stand->frac;

  /* write SPITFIRE outputs to LPJ output structures */
  output->mfiredi +=fire_danger_index;
  output->mnfire +=num_fires;
  output->firef += fire_frac;
  output->mburntarea += burnt_area; /*ha*/
  output->firec+= total_firec;
  output->dcflux+=total_firec;
  output->mfirec+= total_firec;
  output->mfireemission.co2+=emission.co2*stand->frac;
  output->mfireemission.co+=emission.co*stand->frac;
  output->mfireemission.ch4+=emission.ch4*stand->frac;
  output->mfireemission.voc+=emission.voc*stand->frac;
  output->mfireemission.tpm+=emission.tpm*stand->frac;
  output->mfireemission.nox+=emission.nox*stand->frac;
}  /* of 'dailyfire' */
