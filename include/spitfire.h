/**************************************************************************************/
/**                                                                                \n**/
/**                 s  p i  t  f  i  r  e  .  h                                    \n**/
/**                                                                                \n**/
/**     Header file for Spitfire fire model                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef SPITFIRE_H /* Already included? */
#define SPITFIRE_H

/* Declaration of datatypes */

typedef struct popdens *Popdens;

typedef struct
{
  Real human;
  Real nesterov_accum;
  Real nesterov_max;  /* maximum nesterov index */
  int nesterov_day;  /* number of days since the last nesterov_max value was set */
} Ignition;

/* Declaration of functions */

extern Real area_burnt(Real,Real,Real,Real, int,const Pftlist *);
extern Real deadfuel_consumption(const Litter *,Fuel *, Real);
extern Real fuel_consum_total(const Litter *,const Fuel *);
extern Real firedangerindex(Real,Real,const Pftlist *, Real, Real, int, Real);
extern Real firemortality_tree(Pft *,const Fuel *,Livefuel *,Real,Real);
extern void fraction_of_consumption(Fuel *);
extern Real fuel_consumption_1hr(Real, Real);
extern void fuelload(Stand*,Fuel*,Livefuel *,Real);
extern void initfuel(Fuel *);
extern void freepopdens(Popdens,Bool);
extern Real getpopdens(const Popdens,int);
extern Bool readpopdens(Popdens,int,const Cell *,const Config *);
extern Real humanignition(Real,Ignition *);
extern Bool initignition(Cell *,const Config *);
extern Popdens initpopdens(const Config *);          
extern Real litter_update_fire(Litter *,Tracegas *,const Fuel *);
extern Real rateofspread(Real,Fuel *);
extern Real surface_fire_intensity(Real, Real, Real);
extern void update_fbd_tree(Litter*,Real,Real,int);
extern void update_fbd_grass(Litter*,Real,Real);
extern Real wildfire_ignitions(Real,Real,Real);
extern Real windspeed_fpc(Real,const Pftlist *);
extern void dailyfire(Stand *,Livefuel *,Real,Real,const Dailyclimate *,const Config *);
extern void update_nesterov(Cell *,const Dailyclimate *);
extern Bool fwriteignition(FILE *,const Ignition *);
extern Bool freadignition(FILE *,Ignition *,Bool);
extern void fprintignition(FILE *,const Ignition *);

/* Definition of constants */

#define MINER_TOT 0.055

#endif
