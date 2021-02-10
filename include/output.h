/**************************************************************************************/
/**                                                                                \n**/
/**                        o  u  t  p  u  t  .  h                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header file for output datatype and functions                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

/* Definition of datatypes */

#define DEFAULT_PORT 2222 /* default port for socket connection */

typedef struct
{
  Real lai;
  Real phen;
  Stocks leaf;
  Stocks root;
  Stocks so;
  Stocks pool;
  Real wdf;
  Real growingday;
  Real pvd;
  Real phu;
  Real fphu;
  Real laimaxad;
  Real lainppdeficit;
  Real husum;
  Real vdsum;
  Real wscal;
  Real npp;
  Real gpp;
  Real rd;
  Real rroot;
  Real rso;
  Real rpool;
  Real gresp;
  Real trans;
  Real evap;
  Real perc;
  Real irrig;
  Real w0;
  Real w1;
  Real wevap;
  Real hi;
  Real fhiopt;
  Real himind;
  Real froot;
  Real par;
  Real swe; /**< snow water equivalent*/
  Real discharge;
  Real rh;
  Real interc;
  Real nuptake;
  Real n2o_denit;
  Real n2o_nit;
  Real n2_denit;
  Real leaching;
  Real bnf;
  Real no3;
  Real nh4;
  Real nsoil_slow;
  Real nsoil_fast;
  Real assim;
  Real rot_mode;
  Real nlimit;
  Real vscal;
  Real pet;
  int cft;
  Bool irrigation;
} Daily_outputs;

typedef struct
{
  Real co2;
  Real co;
  Real ch4;
  Real voc;
  Real tpm;
  Real nox;
} Tracegas;

typedef struct
{
  int *sdate2;              /**< sowing date */
  int *hdate2;              /**< Harvest date */
  int *syear;
  int *syear2;
  Real *husum2;             /**< accumulated heat units through growing season */
  Real *cft_airrig2;        /**< Yearly irrigation per cft (mm) */
  Harvest *pft_harvest2;
  Real *growing_period2;    /**< lenght of growing period in days */
  Real *cft_pet2;           /**< cft PET */
  Real *cft_transp2;        /**< cft specific transpiration (mm) */
  Real *cft_evap2;          /**< cft specific soil evaporation (mm) */
  Real *cft_nfert2;         /**< cft specific N ferizlier application (gN/m2/growing season) */
  Real *cft_interc2;        /**< cft specific interception (mm) */
  Real *cft_nir2;           /**< cft specific net irrigation requirement (mm) */
  Real *cft_temp2;          /**< cft specific temperature sum (day degC) */
  Real *cft_prec2;          /**< cft specific precipitation (mm) */
  Real *cft_srad2;          /**< cft specific short-wave radiation (W/m2) */
  Stocks *cft_aboveground_biomass2; /**< above ground biomass for crops before harvest (for grass before last harvest of year)*/
  Real *cftfrac2;           /**< cft fraction */
  Real *cft_runoff2;        /**< cft specific runoff (mm) */
  Real *cft_n2o_denit2;     /**< cft specific N2O emissions from denitrification (gN/m2/growing season) */
  Real *cft_n2o_nit2;       /**< cft specific N2O emissions from nitrification (gN/m2/growing season) */
  Real *cft_n2_emis2;       /**< cft specific N2 emissions (gN/m2/growing season) */
  Real *cft_leaching2;      /**< cft specific leaching (gN/m2/growing season) */
  Real *cft_c_emis2;        /**< cft specific C emissions (gC/m2/growing season) */
  Real *pft_nuptake2;       /**< nitrogen uptake per PFT */
} Output_doubleharvest;

typedef struct
{
  Real npp;              /**< NPP (gC/m2) */
  Real npp_agr;          /**< NPP for agricultural stands (gC/m2) */
  Real gpp;              /**< GPP (gC/m2) */
  Real rh;               /**< heterotrophic respiration (gC/m2) */
  Real rh_agr;           /**< heterotrophic respiration of agricultural stands (gC/m2) */
  Real rh_litter;        /**< heterotrophic respiration from litter (gC/m2) */
  Real transp;           /**< transpiration (mm) */
  Real mtransp_b;        /**< Monthly transpired irrigation water (mm) */
  Real runoff;           /**< runoff (mm) */
  Real mdischarge;       /**< Monthly discharge (1.000.000 m3/day) */
  Real mwateramount;     /**< Mean wateramount in month (1.000.000 m3) */
  Real evap;             /**< evaporation (mm) */
  Real mevap_b;          /**< Monthly evaporation of irrigation water (mm) */
  Real interc;           /**< Monthly interception (mm) */
  Real minterc_b;        /**< Monthly blue interception (mm) */
  Real pet;              /**< PET (mm) */
  Real mpet;             /**< monthly PET (mm) */
  Real temp;
  Real sun;
  Real mswc[NSOILLAYER]; /**< monthly soil water content*/
  Real mswc2[NSOILLAYER]; /**< monthly soil water content*/
  Real mrootmoist;        /**< monthly plant available water for evapotranspiration fractional*/
  Stocks fire;           /**< fire carbon and nitrogen emissions (g/m2)*/
  Real mnfire;           /**< monthly number of fires */
  Real mfiredi;          /**< monthly fire danger index */
  Tracegas mfireemission;    /**< monthly fire emissions */
  Real burntarea;       /**< burnt area */
  Real mprec_image;      /**< monthly precipitation received from IMAGE [mm/month]*/
  Real mtemp_image;      /**< monthly temperature received from IMAGE [K] */
  Real msun_image;       /**< monthly cloudiness received from IMAGE [% sunshine = 100-%cloud]*/
  Real mwet_image;       /**< monthly wet days received from IMAGE [days/month]*/
  Real firef;            /**< fire frequency */
  Stocks flux_estab;     /**< establishment flux (gC/m2,gN/m2) */
  Stocks flux_harvest;   /**< harvest flux (gC/m2,g/N/m2) */
  Stocks flux_rharvest_burnt; /**< crop residuals burnt outside of field (gC/m2,gN/m2)*/
  Stocks flux_rharvest_burnt_in_field; /*crop residuals burnt in field (gC/m2,gN/m2)*/
  Stocks alittfall;       /**< litter fall (gC/m2/yr,gN/m2/yr) */
  Real irrig;             /**<irrigation (mm) */
  Real mwd_unsustainable; /**< Monthly withdrawal from unsustainable source (mm) */
  Real munmet_demand;     /**< monthly unavailable requested irrigation water (mm) */
  Real *husum;           /**< accumulated heat units through growing season */
  Real *cft_airrig;      /**< Yearly irrigation per cft (mm) */
  int *sdate;            /**< sowing date */
  int *hdate;            /**< Harvest date */
  Output_doubleharvest *dh; /**< output for double harvest or NULL */
  Real *cft_runoff;         /**< cft specific runoff (mm) */
  Real *cft_n2o_denit;      /**< cft specific N2O emissions from denitrification (gN/m2/growing season) */
  Real *cft_n2o_nit;        /**< cft specific N2O emissions from nitrification (gN/m2/growing season) */
  Real *cft_n2_emis;        /**< cft specific N2 emissions (gN/m2/growing season) */
  Real *cft_leaching;       /**< cft specific leaching (gN/m2/growing season) */
  Real *cft_c_emis;         /**< cft specific C emissions (gC/m2/growing season) */
  Real *pft_npp;            /**< Pft specific NPP (gC/m2) */
  Real *mpft_lai;           /**< Pft specific LAI */
  Harvest *pft_harvest;
  Real *fpc;             /**< foliar projective cover (FPC) */
  Real *pft_mort;        /**< annual mortality  */
  Real *pft_gcgp;
  Real *gcgp_count;
  Real mevap_lake;       /**< Monthly lake evaporation (mm) */
  Real mevap_res;        /**< Monthly reservoir evaporation (mm) */
  Real mprec_res;        /**< Monthly reservoir precipitation (mm) */
  Real mres_storage;     /**< Monthly mean reservoir storage (million m3) */
  Real mres_demand;      /**< Monthly reservoir demand (million m3) */
  Real mtarget_release;  /**< Monthly target release (1.000.000 m3/day) */
  Real mres_cap;         /**< monthly output of reservoir capacity (in million m3) */
  Real mwd_local;        /**< monthly local withdrawal (mm) total water used for local and neighbour irrigation, including water that goes into irrig_stor, which could go back to river */
  Real mwd_neighb;       /**< monthly neighbour withdrawal (mm); negative values means give away to neighbour, positive taken for local irrigation from neighbour */
  Real mwd_res;          /**< monthly reservoir withdrawal (mm) */
#ifdef IMAGE
  Real mwd_gw;           /**< monthly renewable groundwater withdrawal (mm) */
  Real mwd_aq;           /**< monthly aquifer withdrawal (mm) NOT YET DEFINED*/
  Real mwateruse_hil;    /**< monthly waterwithdrawal of household, industry and livestock */
#endif
  Real mconv_loss_evap;  /**< Monthly evaporative conveyance loss (mm) */
  Real mconv_loss_drain; /**< Monthly drainage conveyance loss (mm) */
  Real mstor_return;     /**< Monthly water amount returned from irrig stor into river system*/
  Real prec;             /**< Monthly precipitation (mm)*/
  Real mrain;            /**< Monthly precipitation fallen as rain (minus snowfall) (mm)*/
  Real msnowf;           /**< Monthly precipitation fallen as snow */
  Real mmelt;            /**< Monthly snowmelt (mm)*/
  Real msnowrunoff;      /**< Monthly runoff from snow above snowpack maximum */
  Real mswe;             /**< Monthly average snow water equivalent (mm)*/
  Real awateruse_hil;    /**< yearly water withdrawal of household, industry and livestock */
  Real waterusecons;
  Real waterusedem;
  Real mreturn_flow_b;   /**< monthly blue water return flow of runoff, conv. loss is not included */
  Real input_lake;       /**< yearly precipitation input to lakes (mm) */
  Real adischarge;       /**< Annual discharge (1.000.000 m3/year) */
#ifdef IMAGE
  Real ydischarge;       /**< Annual (1.000.000 m3/year) to send to IMAGE */
#endif
  Real *wft_vegc;        /**< WFT specific vegetation carbon for wood plantation stand (gC/m2) */
#if defined IMAGE && defined COUPLED
  Real npp_nat;          /**< NPP natural stand */
  Real npp_wp;           /**< NPP woodplantation */
  Real flux_estab_nat;   /**< flux_estab natural stand */
  Real rh_nat;           /**< soil respiration natural stand */
  Real flux_estab_wp;    /**< flux_estab woodplantation */
  Real rh_wp;            /**< soil respiration woodplantation */
#endif
  Real *cftfrac;         /**< cft fraction */
  Real *cft_consump_water_g; /**< CFT specific green water consumption (mm) */
  Real *cft_consump_water_b; /**< CFT specific blue water consumption (mm) */
  Real *growing_period;      /**< lenght of growing period in days */
  Real *cft_pet;           /**< cft PET */
  Real *cft_transp;        /**< cft specific transpiration (mm) */
  Real *cft_transp_b;      /**< cft specific transpiration (mm) */
  Real *cft_evap;          /**< cft specific soil evaporation (mm) */
  Real *cft_evap_b;        /**< cft specific blue soil evaporation (mm) */
  Real *cft_interc;        /**< cft specific interception (mm) */
  Real *cft_interc_b;      /**< cft specific blue interception (mm) */
  Real *cft_return_flow_b; /**< cft specific irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
  Real *cft_nir;           /**< cft specific net irrigation requirement (mm) */
  Real *cft_fpar;          /**< cft specific fpar */
  Real *cft_temp;          /**< cft specific temperature sum (day degC) */
  Real *cft_prec;          /**< cft specific precipitation (mm) */
  Real *cft_srad;          /**< cft specific short-wave radiation (W/m2) */
  Stocks *cft_aboveground_biomass; /**< above ground biomass for crops before harvest (for grass before last harvest of year)*/
  Real *pft_nuptake;       /* nitrogen uptake per PFT */
  Real *pft_ndemand;       /* nitrogen demand per PFT */
  Real *cft_conv_loss_evap; /**< cft specific evaporative conveyance losses (mm) */
  Real *cft_conv_loss_drain; /**< cft specific drainage conveyance losses (mm) */
  int  *cft_irrig_events;  /**< number of irrigation days within growing season */
  Stocks deforest_emissions; /**< carbon and nitrogen emissions from deforested wood burnt [g/m2/a] in IMAGE coupling */
  Real trad_biofuel;       /**< carbon emissions from traditional biofuel burnt [gC/m2/a] in IMAGE coupling */
  Stocks flux_firewood;    /**< carbon and nitrogen emissions from domestic wood use [g/m2/a]*/
  Real fburn;              /**< fraction of deforested wood burnt [0-1]*/
  Real ftimber;            /**< fraction of deforested wood harvested as timber [0-1]*/
  Stocks timber_harvest;   /**< carbon and nitrogen harvested as timber [g/m2/a] */
  Poolpar product_pool;  /**< carbon and nitrogen in the fast product pool */
  Real prod_turnover;      /**< carbon and nitrogen emissions from product turnover [gX/m2/a] */
  Real msoilc1;            /**< slow+fast+bg litter carbon in the first soil layer (gC/m2/month) */
  Real *cft_luc_image;     /**< LUC data received by IMAGE [0-1], CFT specific */
  Real msoiltemp[NSOILLAYER]; /**< monthly soil temperature in deg C for  6 layer*/
  Real msoiltemp2[NSOILLAYER]; /**< monthly soil temperature in deg C for  6 layer*/
  Real mrunoff_surf;       /**< monthly surface runoff in mm*/
  Real mrunoff_lat;        /**< monthly lateral runoff in mm*/
  Real runoff_surf;        /**< annual surface runoff in mm*/
  Real runoff_lat;         /**< annual lateral runoff in mm*/
  Real mseepage;           /**< monthly seepage water in mm*/
  Real mgcons_rf;          /**< monthly green water consumption on rainfed stands */
  Real mgcons_irr;         /**< monthly green water consumption on irrigated stands */
  Real mbcons_irr;         /**< monthly blue water consumption on irrigated stands */
  Real fapar;              /**< FAPAR (unitless) */
  Real malbedo;            /**< monthly albedo (unitless) */
  Real mphen_tmin;         /**< monthly phenology cold-temperature limiting function */
  Real mphen_tmax;         /**< monthly phenology heat stress limiting function */
  Real mphen_light;        /**< monthly phenology light limiting function */
  Real mphen_water;        /**< monthly phenology water limiting function */
  Real mwscal;             /**< monthly water scalar */
  Real dcflux;             /**< daily carbon flux from LPJ to atmosphere (gC/m2/day) */
#ifdef COUPLING_WITH_FMS
  Real dwflux;             /**< daily water flux from LPJ to atmosphere (kg/m2/day) */
#endif
  Real mirrig_rw;          /**< monthly supplementary rain water irrigation in mm */
  Real mlakevol;           /**< monthly mean lake content volume in dm3 */
  Real mlaketemp;          /**< monthly mean lake surface temperature in deg C */
  Real mn_uptake;          /**< monthly N uptake gN/m2 */
  Real mn_leaching;        /**< monthly N leached to groundwater gN/m2 */
  Real mn2o_denit;         /**< monthly N2O emissions from denitrification gN/m2 */
  Real mn2o_nit;           /**< monthly N2O emissions from nitrification gN/m2 */
  Real mn2_emissions;      /**< monthly N2 emissions gN/m2 */
  Real mbnf;               /**< monthly biological N fixation gN/m2 */
  Real mn_mineralization;  /**< monthly N mineralization rate gN/m2 */
  Real mn_volatilization;  /**< monthly N volatilization rate gN/m2 */
  Real mn_immo;            /**< monthly N immobilization rate gN/m2 */
  Stocks neg_fluxes;       /**< negative carbon and nitrogen fluxes which occur for negative allocation; needed for balance check*/
  Real mean_vegc_mangrass; /**< annual mean vegetation carbon of managed grasslands */
  Stocks veg;              /**< vegetation carbon (gC/m2) */
  Stocks soil;             /**< soil carbon and nitrogen (gC/m2, gN/m2) */
  Stocks mgrass_soil;      /**< grassland soil carbon and nitrogen (gC/m2, gN/m2) */
  Stocks mgrass_litter;    /**< grassland litter carbon and nitrogen (gC/m2, gN/m2) */
  Stocks soil_slow;        /**< slow carbon and nitrogen pool (gC/m2, gN/m2) */
  Stocks litter;           /**< litter carbon and nitrogen (gC/m2, gN/m2) */
  Real maxthaw_depth;      /**< maximum thawing depth (mm) */
  Real soilno3;                 /**< soil NO3 content (gC/m2) */
  Real soilnh4;                 /**< soil NH4 content (gC/m2) */
  Stocks soil_layer[LASTLAYER]; /**< layer-specific soil carbon (gC2/m2) */
  Real soilno3_layer[LASTLAYER];
  Real soilnh4_layer[LASTLAYER];
  Real agb;                     /**< above-ground biomass (gC/m2) */
  Real agb_tree;                /**< above-ground tree biomass (gC/m2) */
  Real mg_vegc;                 /**< above-ground managed biomass (gC/m2) */
  Real mg_soilc;                /**< above-ground managed soil carbon (gC/m2) */
  Real mg_litc;                 /**< above-ground managed litter carbon (gC/m2) */
  Real *pft_laimax;             /**< PFT-specific maximum LAI (m2/m2) */
  Stocks *pft_root;             /**< PFT-specific root mass (gC/ind, gN/ind) */
  Stocks *pft_leaf;             /**< PFT-specific leaf mass (gC/ind, gN/ind) */
  Stocks *pft_sapw;
  Stocks *pft_hawo;
  Real *pft_nlimit;
  Stocks *pft_veg;
  Real vegc_avg;
  Real *nv_lai;
  Real *fpc_bft;
  Real daylength;
  Real *cft_mswc;          /**< cft-specific monthly absolute soil water content in mm (same as rootmoist but cft-specific) */
  int *nday_month;        /**< day count for monthly cft-specific outputs, needed in update_monthly to divide by number of days */
  Real *cft_nfert;        /**< cft specific N ferizlier application (gN/m2/yr) */
  Real flux_nfert;        /**< automated N ferizlier application (gN/m2/yr) */
  Real abnf_agr;
  Real anfert_agr;
  Real anmanure_agr;
  Real andepo_agr;
  Real anmineralization_agr;
  Real animmobilization_agr;
  Real anuptake_agr;
  Real anleaching_agr;
  Real an2o_denit_agr;
  Real an2o_nit_agr;
  Real anh3_agr;
  Real an2_agr;
  Real alitfalln_agr;
  Real aharvestn_agr;
  Real aseedn_agr;
  Real adelta_norg_soil_agr;
  Real adelta_nmin_soil_agr;
  Real adelta_nveg_soil_agr;
  Real cellfrac_agr;
  Daily_outputs daily;     /**< structure for daily outputs */
} Output;

typedef struct
{
  Real nep;      /**< Total NEP (gC) */
  Real fire;     /**< Total fire (gC) */
  Real estab;    /**< Total extablishment flux (gC) */
  Real harvest;  /**< Total harvested carbon (gC) */
  Real transp;   /**< Total transpiration (dm3) */
  Real evap;     /**< Total evaporation (dm3) */
  Real interc;   /**< Total interception (dm3) */
  Real wd;       /**< Total irrigation water withdrawal (dm3) */
  Real wd_unsustainable;      /**< Total irrigation water withdrawal from unsustainable source (dm3) */
  Real wateruse;              /**< Total wateruse for household industry and livestock */
  Real discharge;             /**< Total discharge (dm3) */
  Real evap_lake;             /**< Total evaporation from lakes (dm3) */
  Real evap_res;              /**< Total evaporation from reservoirs (dm3) */
  Real irrig;                 /**< Total field irrigation (dm3) */
  Real conv_loss_evap;        /**< Total evaporation during conveyance (dm3) */
  Real prec;                  /**< Total precipitation (dm3) */
  Real delta_surface_storage; /**< Total change of surface storage (dm3), increase positive */
  Real delta_soil_storage;    /**< Total change of soil storage (dm3), increase positive */
  Real area;                  /**< Total area (m2) */
  Real total_reservoir_out;   /**< Total water extracted from reservoirs for irrigation */
  Real total_irrig_from_reservoir; /*Total water added to fields from reservoirs */
  Real n_demand;              /**< total N demand by plants */
  Real n_uptake;              /**< total N uptake by plants */
  Real n_influx;              /**< total N inputs */
  Real n_outflux;             /**< total N losses */
  Real anpp_flux;             /**< Total NPP (gC/yr) */
  Real excess_water;          /**< Exess water (dm3) */

} Flux;

typedef enum {LPJ_FILES,LPJ_MPI2,LPJ_GATHER,LPJ_SOCKET} Outputmethod;

typedef enum { MISSING_TIME,SECOND,DAY,MONTH,YEAR } Time;

typedef struct
{
  char *name;  /**< variable name */
  char *var;   /**< data name in NetCDF file */
  char *descr; /**< description */
  char *unit;  /**< units */
  float scale;
  float offset;
  Time time;
  int timestep; /**< time step (ANNUAL,MONTHLY,DAILY) */
} Variable;

/* Declaration of variables */

/* Declaration of functions */

extern Bool initoutput(Output *,int,int,const Config *);
extern void freeoutput(Output *);
extern int outputsize(int,int,int,const Config *);
extern Type getoutputtype(int,Bool);
extern int getnyear(const Variable *,int);
extern Bool isnitrogen_output(int);
extern void fwriteoutputdata(FILE *,const Output *,int,int,const Config *);
extern Bool freadoutputdata(FILE *,Output *,int,int,Bool,const Config *);
extern Bool isannual(int,const Config *);
#ifdef USE_MPI
extern int mpi_write(FILE *,void *,MPI_Datatype,int,int *,
                     int *,int,MPI_Comm);
extern int mpi_write_txt(FILE *,void *,MPI_Datatype,int,int *,
                         int *,int,MPI_Comm);
#endif

/* Definition of macros */

#define isopen(output,index) output->files[index].isopen
#define output_flux(output,flux) writedouble_socket(output->socket,(Real *)&flux,sizeof(Flux)/sizeof(Real))

#endif
