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

/* Definition of constants */

#define NSOILLAYER 6 /* Number of soil layers */
#define LASTLAYER (NSOILLAYER-1)
#define FRACGLAYER NSOILLAYER
#define BOTTOMLAYER (NSOILLAYER-1)
#define NFUELCLASS 4 /* Number of fuel classes */
#define TOPLAYER 0

#define SNOWLAYER NSOILLAYER

#define veg_equil_year 990
#define soil_equil_year veg_equil_year+1320
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
#define MINERALDENS 2700 /* mineral density in kg/m3*/
#define ORGDENS 1400     /* density of organic soil substances [kg/m3]*/
#define PRIESTLEY_TAYLOR 1.32 /* Priestley-Taylor coefficient */
#define SOILDEPTH_IRRIG 500 /*size of layer considered for calculation of irrigation ammount*/

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
} Pool;

typedef struct
{
  Real leaf;       /**< leaf litter (gC/m2)  */
  Real wood;       /**< woody litter (gC/m2) */
  Real q10_wood;
} Traitpar;

typedef struct
{
  Real leaf;             /**< leaf litter (gC/m2)  */
  Real wood[NFUELCLASS]; /**< woody litter (gC/m2) */
} Trait;

typedef struct
{
  const struct Pftpar *pft; /**< PFT id for litter */
  Trait trait;              /**< leaf and wood litter */
} Litteritem;

typedef struct
{
  Real avg_fbd[NFUELCLASS+1]; /**< average fuel bulk densities */
  Litteritem *ag; /**< above ground litter list for PFTs (gC/m2) */
  Real *bg;        /**< below ground litter (gC/m2) */
  int n;          /**< Number of above ground litter pools */
#ifdef MICRO_HEATING
  Real decomC;  /**< litter decomposed*/
#endif
} Litter;

typedef struct
{
  Real harvest,residual,residuals_burnt,residuals_burntinfield;
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
  Real beta_soil;
  Real whcs_all;
  Real whc[NSOILLAYER];   /**< water holding capacity (fraction), whc = wfc - wpwp */
  Real wsats[NSOILLAYER]; /**< absolute water content at saturation (mm), wsats = wsat * soildepth*/
  Real whcs[NSOILLAYER];  /**< absolute water holding capacity (mm), whcs = whc * soildepth */
  Real wpwps[NSOILLAYER]; /**< water at permanent wilting point in mm, depends on soildepth*/
  Real bulkdens[NSOILLAYER]; /**< bulk density of soil [kg/m3]*/
  Real k_dry[NSOILLAYER]; /**< thermal conductivity of dry soil */
  int hsg;        /**< hydrological soil group for CN */
  Real tdiff_0;   /**< thermal diffusivity (mm^2/s) at wilting point (0% whc) */
  Real tdiff_15;  /**< thermal diffusivity (mm^2/s) at 15% whc */
  Real tdiff_100; /**< thermal diffusivity (mm^2/s) at field capacity (100% whc) */
  Real tcond_pwp, tcond_100, tcond_100_ice; /**< thermal conductivity [W/m/K]*/
  //Real albedo; /**< albedo of the soil */
} Soilpar;  /* soil parameters */

typedef struct
{
  const Soilpar *par; /**< pointer to soil parameters */
  Pool cpool[LASTLAYER];         /**< fast and slow carbon pool for all layers*/
  Pool k_mean[LASTLAYER];        /**< fast and slow decay constant */
  Real *c_shift_fast[LASTLAYER];       /**< shifting rate of carbon matter to the different layer*/
  Real *c_shift_slow[LASTLAYER];       /**< shifting rate of carbon matter to the different layer*/
  Real w[NSOILLAYER],            /**< fraction of whc*/
    w_fw[NSOILLAYER];            /**< mm */
  Real w_evap;                   /**< soil moisture content which is not transpired and can evaporate? correct? */
  Real perc_energy[NSOILLAYER]; /**< energy transfer by percolation*/
#ifdef MICRO_HEATING
  Real micro_heating[NSOILLAYER]; /**< energy of microbiological heating*/
  Real decomC[NSOILLAYER];
#endif
  Real alag,amp,meanw1;
  Real snowpack;
  Real snowheight; /**< height of snow */
  Real snowfraction;  /**< fraction of snow-covered ground */
  Real temp[NSOILLAYER+1];      /**< [deg C]; last layer=snow*/
  Real ice_depth[NSOILLAYER];   /**< mm */
  Real ice_fw[NSOILLAYER];      /**< mm */
  Real freeze_depth[NSOILLAYER]; /**< mm */
  Real ice_pwp[NSOILLAYER];      /**< fraction of water below pwp frozen */
  short state[NSOILLAYER];
  Real maxthaw_depth;
  Real mean_maxthaw;
  Real decomp_litter_mean;
  int count;
  Real YEDOMA;       /**< g/m2 */
  Litter litter;     /**< Litter pool */
  Real rw_buffer;    /**< available rain water amount in buffer (mm) */
} Soil;

struct Pftpar; /* forward declaration */
struct Dailyclimate; /* forward declaration */

/* Declaration of functions
 * NOTE: initsoiltemp() in cell.h */

extern int addlitter(Litter *,const struct Pftpar *);
extern void convert_water(Soil*,int,Real*);
extern void copysoil(Soil *,const Soil *, int);
extern int findlitter(const Litter *,const struct Pftpar *);
extern Real fire_prob(const Litter *,Real);
extern unsigned int fscansoilpar(LPJfile *,Soilpar **,Verbosity);
extern Bool fscanpool(LPJfile *,Pool *,const char *,Verbosity);
extern Bool freadlitter(FILE *,Litter *,const struct Pftpar *,int,Bool);
extern Bool freadsoil(FILE *,Soil *,const Soilpar *,const struct Pftpar *,int,Bool);
extern Bool freadsoilcode(FILE *,unsigned int *,Bool,Type);
extern void freesoil(Soil *);
extern void freelitter(Litter *);
extern void freesoilpar(Soilpar [],int);
extern void fprintlitter(FILE *,const Litter *);
extern void fprintsoilpar(FILE *,const Soilpar [],int);
extern void fprintsoil(FILE *,const Soil *);
extern FILE *fopensoilcode(const Filename *,Bool *,size_t *,Type *,unsigned int,Bool);
extern Bool fwritesoil(FILE *,const Soil *,int);
extern Bool fwritelitter(FILE *,const Litter *);
extern void getlag(Soil *,int);
extern int getnsoilcode(const Filename *,unsigned int,Bool);
extern Soilstate getstate(Real *); /*temperature above/below/at T_zero?*/
extern Bool initsoil(Soil *soil,const Soilpar *, int);
extern Real litter_ag_sum(const Litter *);
extern Real litter_ag_grass(const Litter *);
extern Real litter_ag_sum_quick(const Litter *);
extern Real littersom(Soil *,Real [NSOILLAYER]);
extern Real littersum(const Litter *);
extern Real moistfactor(const Litter *);
extern void moisture2soilice(Soil *,Real *,int);
extern void newsoil(Soil *);
extern int seeksoilcode(FILE *,int,size_t,Type);
extern Real snow(Soil *,Real *,Real *,Real, Real *,Real *);
extern Real snow_old(Real *,Real *,Real *,Real);
extern void soiltemp(Soil*, Real);
#ifdef COUPLING_WITH_FMS
extern void laketemp(Cell*, const struct Dailyclimate *);
#endif
extern Real soiltemp_lag(const Soil *,const Climbuf *);
extern Real soilcarbon(const Soil *);
extern Real soilcarbon_slow(const Soil *);
extern Real soilwater(const Soil *);
extern Real soilconduct(const Soil *,int);
extern Real soilheatcap(const Soil *,int);
extern void soilice2moisture(Soil *, Real *,int);
extern Real temp_response(Real);
extern Real litter_ag_tree(const Litter *,int);


/* Definition of macros */

#define getsoilpar(soil,var) (soil)->par->var
#define foreachsoillayer(l) for(l=0;l<NSOILLAYER;l++)
#define forrootmoist(l) for(l=0;l<3;l++)                   /* here defined for the first 1 m*/
#define forrootsoillayer(l) for(l=0;l<LASTLAYER;l++)
#define allice(soil,l) (soil->ice_depth[l]+soil->par->wpwps[l]*soil->ice_pwp[l]+soil->ice_fw[l])
#define allwater(soil,l) (soil->w[l]*soil->par->whcs[l]+soil->par->wpwps[l]*(1-soil->ice_pwp[l])+soil->w_fw[l])
#define timestep2sec(timestep,steps) (24.0*3600.0*((timestep)/(steps))) /* convert timestep --> sec */
#define fprintpool(file,pool) fprintf(file,"%.2f %.2f",pool.slow,pool.fast)

#endif /* SOIL_H */
