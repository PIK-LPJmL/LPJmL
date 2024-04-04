/**************************************************************************************/
/**                                                                                \n**/
/**                         s  o  i  l  .  h                                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header file for soil functions                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef SOIL_H /* Already included? */
#define SOIL_H

/* This is following macro defines a static keyword that vanishes for unit testing.
 When compiling for unit testing (with U_TEST defined), the STATIC macro is defined as
 an empty string, omitting the 'STATIC' keyword. This makes functions accessible outside
 of the respective file, facilitating testing. In standard compilation (without U_TEST),
 STATIC is defined as 'static', ensuring that these functions have only within-file
 access, maintaining encapsulation. */
#ifdef U_TEST
#define STATIC
#else
#define STATIC static
#endif

/* Definition of constants */

#define NSOILLAYER 6 /* Number of soil layers */
#define LASTLAYER (NSOILLAYER-1)
#define FRACGLAYER NSOILLAYER
#define BOTTOMLAYER (NSOILLAYER-1)
#define NFUELCLASS 4 /* Number of fuel classes */
#define TOPLAYER 0
#define NTILLLAYER 1 /* number of layers to be tilled */
#ifndef U_TEST
  #define GPLHEAT 1 /* Gripoints per soil layer for the heat conduction scheme  */
#endif
#define NHEATGRIDP NSOILLAYER*GPLHEAT /* Total number of gridpoints for the heatflow scheme */
#define SNOWLAYER NSOILLAYER

#define snow_skin_depth 40.0 /* snow skin layer depth (mm water equivalent)*/
#define c_water 4.2e6 /* J/m3/K */
#define c_ice   2.1e6 /* J/m3/K */
#define c_snow  2.1e6 /* snow heat capacity (J/ton/K) */
#define m_heat  4.0e4  /*J/gC microb. heating*/
#define lambda_snow 0.2
#define th_diff_snow (lambda_snow/snowheatcap) /* thermal diffusivity of snow [m2/s]*/
#define snow_density     0.3   /*ton/m3*/
#define snowheatcap     6.3e5  /*c_snow*snow_density [J/m3/K]*/
#define c_soilheatcapdry 1.2e6/*1.2e6   J/m3/K */
#define c_albsnow        0.65 /* Albedo of snow (0-1) */
#define c_albsoil        0.30 /* Albedo of bare soil (0-1). Should be soil and soil moisture dependent */
#define c_albwater       0.10 /* Albedo of water (0-1). */
#define c_snowcanopyfrac  0.40 /* Maximum snow cover fraction in the canopy (with leaves) */
#define c_watertosnow    6.70 /* Conversion factor from water to snowdepth, i.e. 1 cm water equals 6.7 cm of snow. Taken from RAKMO and personal communication with KNMI. */
#define c_mineral 1.9259e6 /* [J/m3/K] after deVries 1963*/
#define c_water2ice 0.3e9 /* the energy that is needed/released during water/ice conversion (J/m3)*/
#define tsnow 0.0
/* assuming lambda_air=0.025 and c_air = 1300; admit_air=sqrt(lambda_air*c_air)*/
#define admit_air 5.7009
#define T_zero 0   /* [deg C] */
#define maxheatsteps 100
#define NSTEP_DAILY 1
#define LAG_CONV (NDAYYEAR*0.5*M_1_PI)  /* conversion factor for oscillation
                                lag from angular units to days (=365/(2*PI))*/
#define MINERALDENS 2700.0 /* mineral density in kg/m3*/
#define ORGDENS 1400     /* density of organic soil substances [kg/m3]*/
#define PRIESTLEY_TAYLOR 1.32 /* Priestley-Taylor coefficient */
#define SOILDEPTH_IRRIG 500 /*size of layer considered for calculation of irrigation ammount*/
#define CDN 1.2         /* shape factor for denitrification from SWAT; beta_denit eq 3:1.4.1, SWAT Manual 2009 , take smaller value as it seems to be to high 1.4 originally*/
#define K_SOLID 8       /* Thermal conductivity of solid components in saturated state */
#define K_ICE   2.2     /* Thermal conductivity of ice */
#define K_WATER 0.57    /* Thermal conductivity of liquid water*/
#define K_SOLID_LOG 0.90308998699    
#define K_ICE_LOG   0.34242268082    
#define K_WATER_LOG -0.24412514432 
/* Declaration of variables */

extern Real soildepth[NSOILLAYER];
extern Real layerbound[NSOILLAYER];
extern Real midlayer[NSOILLAYER];
extern Real logmidlayer[NSOILLAYER];
extern Real fbd_fac[NFUELCLASS];


#include "soilpar.h"

/* Definition of datatypes */

typedef enum {NOSTATE,BELOW_T_ZERO,AT_T_ZERO,ABOVE_T_ZERO,FREEZING,
              THAWING} Soilstate;

typedef struct
{
  Real fast;       /**< fast-decomposing component */
  Real slow;       /**< slow-decomposing component */
} Poolpar;

typedef struct
{
  Stocks fast;       /**< fast-decomposing component */
  Stocks slow;       /**< slow-decomposing component */
} Pool;

typedef struct
{
  Real leaf;       /**< leaf litter (gC/m2)  */
  Real wood;       /**< woody litter (gC/m2) */
  Real q10_wood;
} Traitpar;

typedef struct
{
  Stocks leaf;             /**< leaf litter (gC/m2, gN/m2)  */
  Stocks wood[NFUELCLASS]; /**< woody litter (gC/m2, gN/m2) */
} Trait;

typedef struct
{
  const struct Pftpar *pft; /**< PFT id for litter */
  Trait agtop;              /**< above-ground leaf and wood litter */
  Trait agsub;              /**< above-ground leaf and wood litter incorporated to first layer through tillage */
  Stocks bg;                /**< below-ground litter (gC/m2, gN/m2) */
} Litteritem;

typedef struct
{
  Real avg_fbd[NFUELCLASS+1]; /**< average fuel bulk densities */
  Litteritem *item;           /**< litter list for PFTs */
  int n;                      /**< number of litter pools */
  Real agtop_wcap;            /**< capacity of ag litter to store water in mm */
  Real agtop_moist;           /**< amount of water stored in ag litter in mm */
  Real agtop_cover;           /**< fraction of soil coverd by ag litter */
  Real agtop_temp;            /**< temperature of ag litter */
#ifdef MICRO_HEATING
  Real decomC;                /**< litter decomposed (gC/m2) */
#endif
} Litter;

typedef struct
{
  Stocks harvest,residual,residuals_burnt,residuals_burntinfield;
} Harvest;

typedef struct
{
  int type;   /**< unique soil type id */
  char *name; /**< soil name */
  Real Ks;    /**< hydraulic conductivity (mm/h)*/
  Real Sf;    /**< Suction head (mm)*/
  Real wpwp;  /**< relative water content at wilting point */
  Real wfc;   /**< relative water content at field capacity */
  Real wsat;  /**< relative water content at saturation */
  Real sand;  /**< fraction of sand content in soil texture*/
  Real silt;  /**< fraction of silt content in soil texture*/
  Real clay;  /**< fraction of clay content in soil texture*/
  int hsg;        /**< hydrological soil group for CN */
  Real tdiff_0;   /**< thermal diffusivity (mm^2/s) at wilting point (0% whc) */
  Real tdiff_15;  /**< thermal diffusivity (mm^2/s) at 15% whc */
  Real tdiff_100; /**< thermal diffusivity (mm^2/s) at field capacity (100% whc) */
  Real tcond_pwp, tcond_100, tcond_100_ice; /**< thermal conductivity [W/m/K]*/
  Real z_nit;
  Real a_nit;
  Real b_nit;
  Real c_nit;
  Real d_nit;
  Real m_nit;
  Real n_nit;
  Real anion_excl; /* fraction of porosity from which anions are excluded (from SWAT) */
  Real cn_ratio; /* C:N ration in soil pools */
} Soilpar;  /* soil parameters */

typedef struct
{
  const Soilpar *par; /**< pointer to soil parameters */
  Pool pool[LASTLAYER];          /**< fast and slow carbon pool for all layers*/
  Poolpar k_mean[LASTLAYER];     /**< fast and slow decay constant */
  Poolpar decay_rate[LASTLAYER]; /**< fast and slow decay rate */
  Poolpar *c_shift[LASTLAYER];   /**< shifting rate of carbon matter to the different layer*/
  Real NO3[LASTLAYER];           /**< NO3 per soillayer (gN/m2) */
  Real NH4[LASTLAYER];           /**< NH4 per soillayer (gN/m2) */
  Real w[NSOILLAYER],            /**< soil water as fraction of whc (fractional water holding capacity) */
    w_fw[NSOILLAYER];            /**< free water or gravitational water (mm), absolute water content between field capacity and saturation */
  Real w_evap;                   /**< soil moisture content which is not transpired and can evaporate? correct? */
  Real perc_energy[NSOILLAYER];  /**< energy transfer by percolation */
#ifdef MICRO_HEATING
  Real micro_heating[NSOILLAYER]; /**< energy of microbiological heating*/
  Real decomC[NSOILLAYER];
#endif
  Real alag,amp,meanw1;
  Real snowpack;
  Real snowheight; /**< height of snow */
  Real snowfraction;  /**< fraction of snow-covered ground */
  Real temp[NSOILLAYER+1];      /**< [deg C]; last layer=snow*/
  Real enth[NHEATGRIDP];  /**< volumetric enthalpy (i.e. thermal energy/heat) [J/m^3] */
  Real Ks[NSOILLAYER];    /**< saturated hydraulic conductivity (mm/h) per layer*/
  Real wpwp[NSOILLAYER];  /**< relative water content at wilting point */
  Real wfc[NSOILLAYER];   /**< relative water content at field capacity */
  Real wsat[NSOILLAYER];  /**< relative water content at saturation */
  Real whc[NSOILLAYER];   /**< water holding capacity (fraction), whc = wfc - wpwp */
  Real wsats[NSOILLAYER]; /**< absolute water content at saturation (mm), wsats = wsat * soildepth */
  Real whcs[NSOILLAYER];  /**< absolute water holding capacity (mm), whcs = whc * soildepth */
  Real wpwps[NSOILLAYER]; /**< water at permanent wilting point in mm, depends on soildepth */
  Real ice_depth[NSOILLAYER];   /**< mm */
  Real ice_fw[NSOILLAYER];      /**< mm */
  Real freeze_depth[NSOILLAYER]; /**< mm */
  Real ice_pwp[NSOILLAYER];      /**< fraction of water below pwp frozen */
  Real k_dry[NSOILLAYER];        /**< thermal conductivity of dry soil */
  Real bulkdens[NSOILLAYER];     /**<  bulk density of soil [kg/m3]*/
  Real df_tillage[NTILLLAYER];
  Real beta_soil[NSOILLAYER];
  short state[NSOILLAYER];
  Real maxthaw_depth;
  Real mean_maxthaw;
  Stocks decomp_litter_mean;
  Stocks *decomp_litter_pft;
  int count;
  Real YEDOMA;       /**< g/m2 */
  Litter litter;     /**< Litter pool */
  Real rw_buffer;    /**< available rain water amount in buffer (mm) */
  Real wi_abs_enth_adj[NSOILLAYER];  /* = WaterIcecontent_Absolute_Enthalpy_Adjusted
                                        absolute water ice contents with computed corresponding enthalpies,
                                        allowing obervation of changes made without enthalpy adjustments */
  Real sol_abs_enth_adj[NSOILLAYER]; /* = Solidcontent_Absolute_Enthalpy_Adjusted  */
} Soil;

#ifndef TESTSCENARIO_HEAT
typedef struct
{
  Real lam_frozen[NHEATGRIDP];    /* conductivity of soil in frozen state [W/K/m] */
  Real lam_unfrozen[NHEATGRIDP];  /* conductivity of soil in unfrozen state [W/K/m]*/
  Real c_frozen[NHEATGRIDP];      /* heat capacity of soil in frozen state [J/m3/K]*/
  Real c_unfrozen[NHEATGRIDP];    /* heat capacity of soil in unfrozen state [J/m3/K]*/
  Real latent_heat[NHEATGRIDP];   /* latent heat of fusion of soil [J/m3]*/
  /* lamFrozen and lamNormal define thermal conductivities for each interval of the grid
    (e.g. lamFrozen[0] <-> interval directly below the surface), while the other variables define
    their properties only at the gridpoints, (e.g. cFrozen[0] <-> first point below surface) */
} Soil_thermal_prop;
#endif

/* enum indicating whether the sign of temperatures in soil column is uniform positive uniform negative or mixed*/

typedef enum {ALL_BELOW_0, MIXED_SIGN, ALL_ABOVE_0, UNKNOWN} Uniform_temp_sign;


struct Pftpar; /* forward declaration */
struct Dailyclimate; /* forward declaration */

/* Declaration of functions
 * NOTE: initsoiltemp() in cell.h */

extern int addlitter(Litter *,const struct Pftpar *);
extern void convert_water(Soil*,int,Real*);
extern void copysoil(Soil *,const Soil *, int);
extern int findlitter(const Litter *,const struct Pftpar *);
extern Real fire_prob(const Litter *,Real);
extern Bool fscansoilpar(LPJfile *,Config *);
extern int *fscansoilmap(LPJfile *,int *,const Config *);
extern int *defaultsoilmap(int *,const Config *);
extern Bool fscanpoolpar(LPJfile *,Poolpar *,const char *,Verbosity);
extern Bool freadlitter(FILE *,Litter *,const struct Pftpar *,int,Bool);
extern Bool freadsoil(FILE *,Soil *,const Soilpar *,const struct Pftpar *,int,Bool);
extern Bool freadsoilcode(FILE *,unsigned int *,Bool,Type);
extern void freesoil(Soil *);
extern void freelitter(Litter *);
extern void freesoilpar(Soilpar [],int);
extern void fprintlitter(FILE *,const Litter *,int);
extern void fprintsoilpar(FILE *,const Config *);
extern void fprintsoil(FILE *,const Soil *,const struct Pftpar *,int,int);
extern FILE *fopensoilcode(const Filename *,Map **,Bool *,size_t *,Type *,unsigned int,Bool);
extern int *getsoilmap(Map *,const Config *);
extern Bool fwritesoil(FILE *,const Soil *,int);
extern Bool fwritelitter(FILE *,const Litter *);
extern void getlag(Soil *,int);
extern int getnsoilcode(const Filename *,unsigned int,Bool);
extern Soilstate getstate(Real *); /*temperature above/below/at T_zero?*/
extern Bool initsoil(Stand *soil,const Soilpar *,int,const Config *);
extern Real litter_agtop_sum(const Litter *);
extern Real litter_agtop_sum_n(const Litter *);
extern Real litter_agsub_sum(const Litter *);
extern Real litter_agsub_sum_n(const Litter *);
extern Real litter_agtop_grass(const Litter *);
extern Real litter_agtop_sum_quick(const Litter *);
extern Stocks littersom(Stand *,const Real [NSOILLAYER],Real,int,int,const Config *);
extern Real littercarbon(const Litter *);
extern Stocks litterstocks(const Litter *);
extern Real moistfactor(const Litter *);
extern void moisture2soilice(Soil *,Real *,int);
extern void newsoil(Soil *);
extern int seeksoilcode(FILE *,int,size_t,Type);
extern Real snow(Soil *,Real *,Real *,Real, Real *,Real *);
extern Real snow_old(Real *,Real *,Real *,Real);
extern void soiltemp(Soil*, Real,const Config *);
#ifdef COUPLING_WITH_FMS
extern void laketemp(Cell*, const struct Dailyclimate *);
#endif
extern Real soiltemp_lag(const Soil *,const Climbuf *);
extern Real soilcarbon(const Soil *);
extern Real soilcarbon_slow(const Soil *);
extern Stocks soilstocks(const Soil *);
extern Real soilwater(const Soil *);
extern Real soilconduct(const Soil *,int,Bool);
extern Real soilheatcap(const Soil *,int);
extern void apply_heatconduction_of_a_day(Uniform_temp_sign, Real *, const Real *, Real, const Soil_thermal_prop *);
extern void calc_soil_thermal_props(Uniform_temp_sign, Soil_thermal_prop *, const Soil *, const Real *, const Real * , Bool, Bool);
extern void compute_mean_layer_temps_from_enth(Real *, const Real *,const  Soil_thermal_prop *);
extern void apply_enth_of_untracked_mass_shifts(Real *, const Real *, const Real *, const Real *, const Real *);
extern void apply_perc_enthalpy(Soil *);
extern void freezefrac2soil(Soil *, const Real *);
extern void enth2freezefrac(Real *, const Real * ,const  Soil_thermal_prop *);
extern void soilice2moisture(Soil *, Real *,int);
extern void update_soil_thermal_state(Soil *,Real,const Config *);
extern Real temp_response(Real);
extern Real litter_agtop_tree(const Litter *,int);
extern Real litter_agtop_nitrogen_tree(const Litter *,int);
extern Real biologicalnfixation(const Stand *,int,int,const Config *);
extern void leaching(Soil *,const Real);
extern Real volatilization(Real,Real,Real,Real,Real);
extern Real nuptake_temp_fcn(Real);
extern void denitrification(Stand *,int,int,const Config *);
extern void getrootdist(Real [],const Real[],Real);
extern Stocks checklitter(Litter *);
extern Real getwr(const Soil *,const Real []);
extern void updatelitterproperties(Stand *,Real);
extern Real calc_litter_dm_sum(const Soil *);
extern void pedotransfer(Stand *, Real *, Real *,Real);
extern void soilpar_output(Cell *,Real,const Config *);
extern int findsoilid(const char *,const Soilpar *,int);
extern void cmpsoilmap(const int*,int,const Config *);

/* Definition of macros */

#define getsoilpar(soil,var) (soil)->par->var
#define foreachsoillayer(l) for(l=0;l<NSOILLAYER;l++)
#define forrootmoist(l) for(l=0;l<3;l++)                   /* here defined for the first 1 m*/
#define forrootsoillayer(l) for(l=0;l<LASTLAYER;l++)
#define allice(soil,l) (soil->ice_depth[l]+soil->wpwps[l]*soil->ice_pwp[l]+soil->ice_fw[l])
#define allwater(soil,l) (soil->w[l]*soil->whcs[l]+soil->wpwps[l]*(1-soil->ice_pwp[l])+soil->w_fw[l])
#define timestep2sec(timestep,steps) (24.0*3600.0*((timestep)/(steps))) /* convert timestep --> sec */
#define fprintpool(file,pool) fprintf(file,"%.2f %.2f",pool.slow,pool.fast)
#define f_temp(soiltemp) exp(-(soiltemp-18.79)*(soiltemp-18.79)/(2*5.26*5.26)) /* Parton et al 2001*/
#define f_NH4(nh4) (1-exp(-0.0105*(nh4))) /* Parton et al 1996 */
#define getsoilmoist(soil,l) (((soil)->w[l] * (soil)->whcs[l] + ((soil)->wpwps[l] * (1 - (soil)->ice_pwp[l])) + (soil)->w_fw[l]) / (soil)->wsats[l])
/* The macro computes the temperature at a gridpoint (gp)
given an enthalpy vector (enth) and a Soil_thermal_prop (th)*/
#define ENTH2TEMP(e, th, gp)\
 (((e)[(gp)]<0                      ?  (e)[(gp)]                           / (th)->c_frozen[(gp)]   : 0) +\
  ((e)[(gp)]>(th)->latent_heat[(gp)] ? ((e)[(gp)] - (th)->latent_heat[(gp)]) / (th)->c_unfrozen[(gp)] : 0))
#define reconcile_layer_energy_with_water_shift(soil, layer, amount, vol_enthalpy, config) ({\
        if(config->percolation_heattransfer)\
        {\
          soil->perc_energy[layer]+=amount/1000*vol_enthalpy; /* add enthalpy of water coming from above */ \
          soil->wi_abs_enth_adj[layer]+=amount; /* update enth adjusted water ice content */ \
        }\
        })
#endif /* SOIL_H */
