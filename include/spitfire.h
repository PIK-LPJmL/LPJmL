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

#define SPITFIRE_VERSION "1.9.0"

/* Definition of constants */

#define MINER_TOT 0.055
#define PART_DENS 513.0

/* Declaration of global variables */

extern Real sigma_dead[NFUELCLASS];
extern Real sigma_live[2];

/* Declaration of datatypes */

typedef struct popdens *Popdens;
typedef struct human_ign_prob *Human_ign_prob;

typedef struct human_ignition *Human_ignition;

typedef struct
{
  Real human;
  Real nesterov_accum;
  Real nesterov_max;  /* maximum nesterov index */
  int nesterov_day;  /* number of days since the last nesterov_max value was set */
} Ignition;

typedef struct
{
  Real num_fires;
  Real burnt_area;
  Real dbf;
  Real wind_cover;
  Real days_burning;
} Fire;

typedef struct
{
  Real ffmc;
  Real dmc;
  Real dc;
} FWIdata;

/* Declaration of functions */

extern Bool fscanfireduration(LPJfile *,Standtype **,int,Verbosity);
extern Bool fscanfirestand(LPJfile *,Standtype **,int,Verbosity);
extern void fprintfireduration(FILE *,Standtype **,int);
extern Real area_burnt(Real *,Real *,Real *,Real *,const Real [2],Real,Real,Real,Real, int,Stand *,Bool);
extern Real deadfuel_consumption(const Litter *,Fuel *, Real);
extern Real firedangerindex(Real,const Stand *,const Dailyclimate *,Real,int,Bool);
extern Real firemortality_tree(Pft *,const Fuel *,Livefuel *,Real,Real);
extern Real fuel_consumption_1hr(Real, Real);
extern void fuelload(const Stand*,Fuel*,Livefuel *,Real,const Config *);
extern void initfuel(Fuel *);
extern void freepopdens(Popdens,Bool);
extern Real getpopdens(const Popdens,int);
extern Bool readpopdens(Popdens,int,const Cell *,const Config *);
extern void freehuman_ign_prob(Human_ign_prob,Bool);
extern Real gethuman_ign_prob(const Human_ign_prob,int);
extern Bool readhuman_ign_prob(Human_ign_prob,int,const Cell *,const Config *);
extern Real humanignition(Real,Ignition *);
extern Bool initmax_firesize(Cell *,const Config *);
extern Popdens initpopdens(const Config *);
extern Human_ign_prob inithuman_ign_prob(const Config *);
extern Human_ignition inithumanignition(const Config *);
extern Bool gethumanignition(Human_ignition,int,Cell *,const Config *);
extern void freehumanignition(Human_ignition,Bool);
extern Stocks litter_update_fire(Litter *,Tracegas *,const Fuel *);
extern Real rateofspread(Real,Fuel *,Livefuel *);
extern Real surface_fire_intensity(Real,Fuel *);
extern void update_fbd_tree(Litter*,Real,Real,int);
extern void update_fbd_grass(Litter*,Real,Real);
extern Real wildfire_ignitions(Real,Real,Real);
extern Real windspeed_fpc(Real,const Pftlist *);
extern void dailyfire(Stand *,Real,Real,Real,Input *,int,int,const Dailyclimate *,const Config *);
extern void update_nesterov(Cell *,const Dailyclimate *);
extern Bool fwriteignition(Bstruct,const char *,const Ignition *);
extern Bool freadignition(Bstruct,const char *,Ignition *);
extern void fprintignition(FILE *,const Ignition *);
extern Real getvpd(const Dailyclimate *,Bool);
extern Real growing_season_index(Real,Real *,const Dailyclimate *,Bool,Real);
extern Real getfwi(FWIdata *,const Coord *,const Dailyclimate  *,int,Bool);
extern void initfwi(FWIdata *);
extern Bool fwritefwi(Bstruct,const char *,const FWIdata *);
extern Bool freadfwi(Bstruct,const char *,FWIdata *);

/* Definition of macros */

#define isspitfire(config) ((config)->fire==SPITFIRE || (config)->fire==SPITFIRE_TMAX)
#define fprintfwi(file,fwi) fprintf(file,"ffmc:\t\t%g\ndmc:\t\t%g\ndc:\t\t%g\n",(fwi)->ffmc,(fwi)->dmc,(fwi)->dc)

#endif
