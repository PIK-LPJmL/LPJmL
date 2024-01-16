/**************************************************************************************/
/**                                                                                \n**/
/**                      c  e  l  l  .  h                                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of cell datatype                                               \n**/
/**                                                                                \n**/
/**     Hierarchy:                                                                 \n**/
/**                                                                                \n**/
/**     --cell                                                                     \n**/
/**       +-Coord                                                                  \n**/
/**       +-Managed_land                                                           \n**/
/**       | +-Landfrac                                                             \n**/
/**       | +-Resdata                                                              \n**/
/**       | | +-Reservoir                                                          \n**/
/**       | +-Cropdates                                                            \n**/
/**       | +-Manage                                                               \n**/
/**       | +-Image_data (only for version with -DIMAGE set)                       \n**/
/**       |   +-Pool                                                               \n**/
/**       +-Ignition                                                               \n**/
/**       +-Discharge                                                              \n**/
/**       | +-Queue                                                                \n**/
/**       +-Output                                                                 \n**/
/**       +-Climbuf                                                                \n**/
/**       | +-Buffer                                                               \n**/
/**       +-Balance                                                                \n**/
/**       +-Standlist                                                              \n**/
/**         +-Stand                                                                \n**/
/**           +-Pftlist                                                            \n**/
/**           | +-Pft                                                              \n**/
/**           +-Soil                                                               \n**/
/**             +-Pool                                                             \n**/
/**             +-Litter                                                           \n**/
/**               +-Litteritem                                                     \n**/
/**                 +-Trait                                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef CELL_H /* Already included? */
#define CELL_H

/* Definition of datatypes */

typedef struct
{
  Real anpp;                /**< annual NPP (gC/m2) */
  Real agpp;                /**< annual GPP (gC/m2) */
  Real arh;                 /**< annual heterotrophic respiration (gC/m2) */
  Real awater_flux;         /**< annual water flux (mm) */
  Real aprec;               /**< annual precipitation (mm) */
  Real aevap;               /**< annual evaporation (mm) */
  Real ainterc;             /**< annual interception (mm) */
  Real atransp;             /**< annual transpiration (mm) */
  Real airrig;              /**< annual irrigation (mm) */
  Real aevap_res;           /**< annual reservoir evaporation (mm) */
  Real aevap_lake;          /**< annual lake evaporation (mm) */
  Real aconv_loss_evap;     /**< annual evaporative conveyance loss of irrigation water withdrawals (mm) */
  Real aconv_loss_drain;    /**< annual drainage conveyance loss (mm) */
  Real awateruse_hil;       /**< annual water withdrawal of household, industry and livestock */
  Real adischarge;          /**< annual discharge (1.000.000 m3/year) */
  Real awd_unsustainable;   /**< annual withdrawal from unsustainable source (mm) */
  Real soil_storage;        /**< Water stored in the soil column by the end of year (dm3) */
  Stocks fire;              /**< annual fire carbon and nitrogen emissions (g/m2)*/
  Stocks flux_firewood;     /**< carbon and nitrogen emissions from domestic wood use [g/m2/a]*/
  Stocks flux_estab;        /**< establishment flux (gC/m2,gN/m2) */
  Stocks flux_harvest;      /**< harvest flux (gC/m2,g/N/m2) */
  Stocks timber_harvest;    /**< timber flux (gC/m2,g/N/m2) */
  Stocks deforest_emissions;/**< carbon and nitrogen emissions from deforested wood burnt [g/m2/a] in IMAGE coupling */
  Pool prod_turnover;     /**< carbon and nitrogen emissions from product turnover [gX/m2/a] */
  Stocks neg_fluxes;        /**< negative carbon and nitrogen fluxes which occur for negative allocation; needed for balance check*/
  Stocks trad_biofuel;        /**< carbon emissions from traditional biofuel burnt [gC/m2/a] in IMAGE coupling */
  Stocks tot;                /**< total carbon and nitrogen (g/m2) */
  Stocks biomass_yield;      /**< harvested biomass (gC/m2, gN/m2)*standfrac*/
  Stocks estab_storage_tree[2];  /**< carbon and nitrogen taken out from annual NPP to satisfy tree establishment rate */
  Stocks estab_storage_grass[2]; /**< carbon and nitrogen taken out from annual NPP to satisfy grass establishment rate */
  Real totw;                /**< total water (mm) */
  Real surface_storage;     /**< total water in surface storages (dm3) */
  Real surface_storage_last;     /**< total water in surface storages (dm3) */
  Real soil_storage_last;        /**< total water in soil storages (dm3) */
  Real excess_water;        /**< excess water (mm) */
  Real total_reservoir_out; /**< total water extracted from reservoirs (dm3) */
  Real total_irrig_from_reservoir; /**< total water added to fields from reservoirs (dm3)*/
  Stocks influx;            /**< all C,N inputs: deposition, fertilizer, manure, BNF */
  Real n_outflux;           /**< all N outputs: n2onit, n2odenit, n2denit, leaching */
  Real n_demand;            /**< N demand by plants (gN)*/
  Real n_uptake;            /**< N uptake by plants (gN) */
  Real aCH4_em;            /* includes mCH4_em */
  Real aCH4_sink;          /* mCH4_sink */
  Real aCH4_fire;
  Real aCH4_rice;
  Real aCH4_setaside;
  Real aMT_water;          /* water produced during Methanogenesis */
  Real temp;               /**< air temperature (celsius) */
} Balance;

typedef struct celldata *Celldata;

struct cell
{
  Coord coord;              /**< Cell coordinate and area */
  Standlist standlist;      /**< Stand list */
  Climbuf climbuf;
  Ignition ignition;
  Real landfrac;            /**< land fraction ((0..1]) */
  Real afire_frac;          /**< fraction of grid cell burnt this year */
  Real *gdd;                /**< Growing degree days array */
  Real lakefrac;            /**< lake fraction (0..1) */
  Real icefrac;             /**< ice fraction (0..1) */
#ifdef COUPLING_WITH_FMS
  Real laketemp;            /**< temperatures in this cell, for now only one layer */
  Real day_after_snowfall;  /**< days after snowfall, to compute the albedo of lakes following:

Albedo models for snow and ice on a freshwater lake
Heather E. Henneman, Heinz G. Stefan
St. Anthony Falls Laboratory, Department of Civil Engineering, University of Minnesota, Minneapolis, MN 55414, USA
Received 19 November 1997; accepted 15 January 1999*/

  Real albedo_lake;         /**< for a changing albedo of lakes*/
  Real snowpool_above_lake; /**< temporarily storing the snow mass on a lake*/
#endif
  Real soilph;              /**< soil pH */
  Bool skip;                /**< Invalid soil code in cell (TRUE/FALSE) */
  Bool was_glaciated;
  Bool is_glaciated;
  Managed_land ml;          /**< Managed land */
  Output output;            /**< Output data */
  Discharge discharge;
  int elevation;            /**< cell elevation (m) */
  const Real *landcover;    /**< prescribed landcover or NULL */
  Real slope;
  Real slope_min;
  Real slope_max;
  Real kbf;                 /* baseflow recession coefficient */
  Real Hag_beta;            /* Haggard et al. 2005, effects of slope on runoff 2005*/
  Real ground_st;
  Real ground_st_am;
  Real lateral_water;       /* water which will be transported from upland area to lowland area */
  Hydrotope hydrotopes;
  Balance balance;          /**< balance checks */
  Seed seed;                /**< seed for random generator */
#if defined IMAGE && defined COUPLED
  Real npp_nat;             /**< NPP natural stand */
  Real npp_wp;              /**< NPP woodplantation */
  Real npp_grass;           /**< NPP woodplantation */
  Real flux_estab_nat;      /**< flux_estab natural stand */
  Real rh_nat;              /**< soil respiration natural stand */
  Real flux_estab_wp;       /**< flux_estab woodplantation */
  Real rh_wp;               /**< soil respiration woodplantation */
  Real ydischarge;          /**< annual discharge */
  Real *pft_harvest;        /**< harvested carbon */
#endif
};

/* Declaration of functions */

extern void freegrid(Cell [],int,const Config *);
extern void freecell(Cell *,int,const Config *);
extern void update_daily(Cell *,Real,Real,Real,Dailyclimate,int,
                         int,int,int,int,Bool,const Config *);
extern void update_annual(Cell *,int,int,
                          Real,int,Bool,Bool,const Config *);
extern void update_monthly(Cell *,Real,Real,int,const Config *);
extern void init_annual(Cell *,int,const Config *);
extern int fwritecell(FILE *,long long [],const Cell [],int,int,int,Bool,const Config *);
extern void fprintcell(FILE *,const Cell [],int,int,int,const Config *);
extern Bool freadcell(FILE *,Cell *,int,int,const Soilpar *,
                      const Standtype [],int,Bool,Config *);
extern int writecoords(Outputfile *,int,const Cell [],const Config *);
extern int writearea(Outputfile *,int,const Cell [],const Config *);
extern int writecountrycode(Outputfile *,int,const Cell [],const Config *);
extern int writeregioncode(Outputfile *,int,const Cell [],const Config *);
extern int iterate(Outputfile *,Cell [],Input,
                   int,int,Config *);
extern void iterateyear(Outputfile *,Cell [],Input,
                        Real,Real,int,int,int,const Config *);
extern void fwriteoutput_ch4(Outputfile *,Real,Real,const Config *);
extern void initoutputdata(Output *,int,int,const Config *);
extern void fwriteoutput(Outputfile *,Cell [],int,int,int,int,int,const Config *);
extern void equilsom(Cell *,int, const Pftpar [],Bool);
extern void equilveg(Cell *,int);
extern void check_fluxes(Cell *,int,int,const Config *);
extern void check_balance(Flux,int,const Config *);
extern Bool initdrain(Cell [],Config *);
extern Bool inithydro(Cell *, Config *);
extern void drain(Cell [],int,const Config *);
extern Real nep_sum(const Cell [],const Config *);
extern Real cflux_sum(const Cell [],const Config *);
extern Real flux_sum(Flux *,Cell [],const Config *);
extern Bool getwateruse(Wateruse, Cell [],int,const Config *);
extern Wateruse initwateruse(const Filename *,const Config *);
#ifdef IMAGE
extern Bool getwateruse_wd(Wateruse, Cell[], int, const Config *);
#endif
extern void freewateruse(Wateruse,Bool);
extern void killstand(Cell *,int,Bool,Bool,int,int,const Config *);
extern Bool initsoiltemp(Climate *, Cell*,const Config *);
extern Celldata opencelldata(Config *);
extern Bool seekcelldata(Celldata,int);
extern Bool readcelldata(Celldata,Cell *,unsigned int *,int,Config *);
extern void closecelldata(Celldata,const Config *);
extern Real albedo(Cell *, Real , Real );
extern void hydrotopes(Cell*);
extern void update_wetland(Cell *, int, int,const Config *);
extern void check_glaciated(Cell *,const Config *);
extern Bool initoutput(Outputfile *,Cell [],int,int,Config *);

/* Definition of macros */

#define printcell(cell,n,npft,ncft,config) fprintcell(stdout,cell,n,npft,ncft,config)

#endif
