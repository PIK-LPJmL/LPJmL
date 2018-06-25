/**************************************************************************************/
/**                                                                                \n**/
/**                 r  e  s  e  r  v  o  i  r  .  h                                \n**/
/**                                                                                \n**/
/**     C implementation of reservoir operation in LPJ                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef RESERVOIR_H /* Already included? */
#define RESERVOIR_H

/* Definition of constants */

#define HIST_YEARS 20 /* the amount of years for the history of the reservoir*/
#define ALPHA 0.85 /*constant for definition of target release (Hanasaki, 2006) */
#define NIRRIGDAYS 5 /* size of irrigation buffer */
#define NPURPOSE 5
#define ENV_FLOW 0.1 /*fraction of mean monthly flow that is always released to the river */

/* Definition of datatypes */

typedef struct
{
  int year;
  float capacity; /* maximum capacity of reservoir (dm3) */
  float area;     /*area of the reservoirs in this cell in km2*/
  int inst_cap;
  int height;
  int purpose[NPURPOSE];
} Reservoir;

typedef struct
{
  Reservoir reservoir;
  Real *fraction;
  Real dmass;            /* water currently stored (dm3) */
  Real dfout_res;        /* daily outflow from reservoir to river */
  Real dfout_irrigation; /* daily outflow from reservoir for irrigation */
  Real dfout_irrigation_daily[NIRRIGDAYS]; /* buffer, after 5 days the water is released to the river if not used for irrigation*/
  Real ddemand; /* daily demand: sum of GIR in all gridcells from dam */
  Real mdemand; /* , because the mdemand is now calculated internally and updated after every month, theoretical demand based on evaporation deficit */
  Real demand_hist[HIST_YEARS][NMONTH]; /* to store the history of demand to this reservoir */
  Real inflow_hist[HIST_YEARS][NMONTH]; /* to store the history of inflow to this reservoir */
  Real level_hist[HIST_YEARS][NMONTH]; /* to store the history of reservoir level */
  Real mean_inflow_month[NMONTH];
  Real mean_demand_month[NMONTH];
  Real mean_level_month[NMONTH];
  int start_operational_year; /*month of start of operational year */
  Real mean_inflow; /* mean annual inflow in liters/day */
  Real mean_demand;
  Real mean_volume; /* Mean volume of the reservoir in liters*/
  Real c; /* storage capacity to mean annual inflow ratio */
  Real target_release_year; /* target release for the coming year */
  Real target_release_month[NMONTH]; /* target release for the coming months */
  Real k_rls;  /*release coefficient for this year (set at the beginning of operational year, Hanasaki 2006) */
  Real demand_fraction; /*fraction of the pot_irrig_demand to the dam that
                          is fulfilled by the daily release (note: this can be
                          higher than one eg the day after rain because of the
                          even release through the month*/

  Real c_pool;           /* pool where the carbon is stored from the land
                            that has been taken by the reservoir */
} Resdata;

/* Declaration of functions */

extern Bool readreservoir(Reservoir *,Bool,FILE *);
extern Real outflow_reservoir(Resdata *,int);
extern Bool initreservoir(Cell *,Config *);
extern void irrig_amount_reservoir(Cell *,const Config *);
extern void allocate_reservoir(Cell *,int,const Config *);
extern void landusechange_for_reservoir(Cell *,const Pftpar [],int,
                                        Bool,Bool,int,int);
extern Bool check_stand_fracs_for_reservoir(const Cell *,Real *);
extern void update_reservoir_daily(Cell *,Real,Real);
extern void update_reservoir_monthly(Cell *,int);
extern void update_reservoir_annual(Cell *);
extern void initresdata(Cell *);
extern void drain_reservoir(Cell *,const Config *,int,Real);
extern Bool fwriteresdata(FILE *,const Cell *);
extern Bool freadresdata(FILE *,Cell *,Bool);
extern void fprintresdata(FILE *,const Cell *);
extern Real reservoir_surface_storage(const Resdata *);

#endif
