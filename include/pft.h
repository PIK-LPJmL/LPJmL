/**************************************************************************************/
/**                                                                                \n**/
/**                     p  f  t  .  h                                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of plant functional type (PFT) datatype                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef PFT_H /* Already included? */
#define PFT_H

#include "pftpar.h"

/* Definition of constants */

#define NO_WDF -9
#define PREC_MAX 1e38
#define LAMBDA_OPT 0.8  /* optimal Ci/Ca ratio */
#define NHSG 4 /* number of hydrological soil groups */
#define CCpDM 0.4763 /*leaf carbon content per dry mass  Kattge et al. 2011*/

/* Definitions of datatypes */

typedef struct
{
  Real char_moist_factor;
  Real char_alpha_fuel;    /**< parameter to calculate fuel moisture */
  Real char_net_fuel;
  Real char_dens_fuel_ave; /**< average fbd */
  Real cf;
  Real daily_litter_moist; /**< fuel moisture */
  Real deadfuel_consum[NFUELCLASS+1];
  Real gamma;
  Real moist_1hr;
  Real moist_10_100hr;
  Real mw_weight;
  Real sigma;
  Real CME;
} Fuel;

typedef struct
{
  Real disturb;
  Real dlm_livegrass;
  Real non_combust;
  Real pot_fc_lg_c3; /**< Biomass of C3 grass in g/m2 */
  Real pot_fc_lg_c4; /**< Biomass of C4 grass in g/m2 */
  Real CME;
} Livefuel;

typedef struct
{
  Real low;  /**< lower tolerance limits */
  Real high; /**< upper tolerance limits */
} Limit;

typedef struct
{
  Real tmin;  /**< daily cold-temperature limiting function value for phenology */
  Real tmax;  /**< daily heat-stress limiting function value for phenology */
  Real light; /**< daily light limiting function value for phenology */
  Real wscal; /**< daily water limiting function value for phenology */
} Phenology;  /* new phenology */

typedef struct
{
  Real sl;   /**< steepness of the response function */
  Real base; /**< half-value value at which the function is 0.5 */
  Real tau;  /**< rate of change of function to actual value */
} Phen_param;

typedef struct Pft
{
  const struct Pftpar
  {
    int id;                     /**< unique PFT identifier */
    int type;                   /**< type --> whether CROP or TREE or GRASS*/
    int cultivation_type;       /**< cultivation_type----> NONE, BIOMASS, ANNUAL_CROP*/
    char *name;                 /**< Pft name */
    Real cn[NHSG];              /**< pft specific curve number for each hydr. soil group */
    Real beta_root;             /**< root distribution parameter */
    Real rootdist[LASTLAYER];   /**< fraction of roots in upper soil layer par1*/
    Real minwscal;              /**< water scalar value at which leaves shed by
                                   drought deciduous PFT par3 */
    Real gmin;                  /**< canopy conductance component (4) */
    Real respcoeff;             /**< maintenance respiration coefficient (5) */
    Real nmax;                  /**< maximum foliar N content (mg/g) (7) */
    Real resist;                /**< fire resistance index (8) */
    Real longevity;             /**< leaf longevity (10) */
    Real lmro_ratio;            /**< leaf to root ratio under non-water stressed
                                   conditions (18) */
    Real ramp;                  /**< number of GDDs to attain full leaf cover
                                   (par19) */
    Real gdd5min;               /**< PFT-specific minimum GDD(30) */
    Real twmax;                 /**< (31) */
    Real twmax_daily;           /**< (31) */
    Real gddbase;               /**< GDD base  (33) */
    Real min_temprange;         /**<  (34) */
    Real emax;                  /**< emax (mm/day)(35) */
    Real intc;                  /**< Interception storage parameter (36) */
    Real alphaa;                /**< alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    Real albedo_leaf;           /**< albedo of green leaves*/
    Real albedo_stem;           /**< albedo of stems */
    Real albedo_litter;         /**< albedo of litter */
    Real snowcanopyfrac;        /**< maximum snow coverage in the canopy (with leaves) */
    Real lightextcoeff;         /**< light extinction coeffcient in Lamber-Beer relation */
    Phen_param tmin;            /**< minimum temperature response function */
    Phen_param tmax;            /**< maximum temperature response function */
    Phen_param light;           /**< lightresponse function */
    Phen_param wscal;           /**< water availibity response function */
    Real mort_max;              /**< asymptotic maximum mortality rate (1/year) */
    Real lai_sapl;
    int phenology;              /**< par17 */
    int path;                   /**< par2 */
    Real sla;                   /**< specific leaf area (m2/gC) */
    Real k1,k2,k3;
    Real soc_k;                 /**< shape-factor for vertical distribution function of soil organic carbon, following Jobbagy et al. 2000*/
    Limit temp_co2;             /**< temperature limit for CO2 uptake (24,27) */
    Limit temp_photos;          /**< range of temperature optimum for
                                   photosynthesis(25,26) */
    Limit temp;                 /**< bioclimatic limits (28,29) */
    Real aprec_min;             /**< minimum annual precipitation (mm) */
    Real flam;
    Traitpar k_litter10;
    Real windspeed;             /**< windspeed dampening */
    Real roughness;             /**< roughness length */
    Real alpha_fuelp;           /**< scaling factor for Nesterov fire danger index */
    Real vpd_par;               /**< scaling factor for VPD fire danger index  */
    Real fuelbulkdensity;       /**< fuel bulk density*/
    Tracegas emissionfactor;    /**< trace gas emission factors */
    void *data;                 /**< pointer for PFT specific extensions */

    /* list of pointers for PFT specific functions */
    /* (Virtual functions in C++)                  */

    void (*newpft)(struct Pft *,int,int);
    void (*init)(struct Pft *);
    Real (*wdf)(struct Pft *,Real,Real);
    Real (*npp)(struct Pft*,Real,Real,Real);
    Real (*fpar) (const struct Pft*);
    void (*snow_canopy) (struct Pft*, Real, Real);
    Real (*alphaa_manage) (const struct Pft*);
    void (*leaf_phenology)(struct Pft *,Real,int,Bool);
    void (*albedo_pft) (struct Pft *, Real, Real);
    Bool (*fwrite)(FILE *,const struct Pft *);
    Bool (*fread)(FILE *,struct Pft *,Bool);
    void (*fprint)(FILE *,const struct Pft *);
    void (*litter_update)(Litter *,struct Pft *,Real);
    Real (*establishment)(struct Pft *,Real,Real,int);
    Real (*fire)(struct Pft *,Real *);
    Real (*actual_lai)(const struct Pft *);
    void (*adjust)(Litter *,struct Pft *,Real,Real);
    void (*reduce)(Litter *,struct Pft *,Real);
    void (*free)(struct Pft *);
    Real (*vegc_sum)(const struct Pft *);
    Real (*agb)(const struct Pft *);
    void (*mix_veg)(struct Pft *,Real);
    void (*fprintpar)(FILE *,const struct Pftpar *);
    //void (*output_daily)(Daily_outputs *,const struct Pft *);
    void (*turnover_monthly)(Litter *,struct Pft *);
    void (*turnover_daily)(Litter *,struct Pft *,Real,Bool);
    Real (*livefuel_consumption)(Litter *,struct Pft *,const Fuel *,
                                 Livefuel *,Bool *,Real,Real);
    Bool (*annual)(Stand *,struct Pft *,Real *,Bool);
  } *par;                /**< PFT parameters */
  Real fpc;              /**< foliar projective cover (FPC) under full leaf
                            cover as fraction of modelled area */
  Real fpc_obs;          /**< foliar projective cover (FPC) under full leaf
                            cover as prescribed from observation */
  Bool prescribe_fpc;    /**< prescribe FPC? */
  Real albedo;           /**< albedo of the entire PFT (mix of green leaves, branches and snow albedo) */
  Real fapar;            /**< green fraction of absorbed photosythetic active radiation */
  Real nind;             /**< individual density (indiv/m2) */
  Real gdd;              /**< current-year growing degree days */
  Real bm_inc;           /**< annual biomass increment (gC/m2) */
  Real wscal;            /**< mean daily water scalar (among leaf-on days) */
  Real wscal_mean;
  Real phen,aphen;
#ifdef DAILY_ESTABLISHMENT
  Bool established;
#endif
  Phenology phen_gsi;    /**< new phenology variables */
  int litter;            /**< index of above-ground litter pool */
  Stand *stand;          /**< pointer to stand */
  void *data;            /**< pointer for PFT specific extensions */
} Pft;

typedef struct Pftpar Pftpar;


/* Defines a pointer to a function that takes a pointer to FILE, Pftpar
 * and a char and returns a Bool.
 * This construction is necessary to make a function return a function
 * pointer.
 */

typedef Bool (*Fscanpftparfcn)(LPJfile *,Pftpar *,Verbosity);

/* Declaration of functions */

extern void newpft(Pft *,Stand *,const Pftpar *,int,int);
extern void freepft(Pft *);
extern void freepftpar(Pftpar [],int);
extern int* fscanpftpar(LPJfile *,Pftpar **,const Fscanpftparfcn [],int,Verbosity);
extern Real temp_stress(const Pftpar *,Real,Real);
extern Real photosynthesis(Real *,Real *,int,Real,Real,Real,Real,Real ,Real);
extern Bool survive(const Pftpar *,const Climbuf *);
extern Real interception(Real *,const Pft *,Real,Real);
extern void initgdd(Real [],int);
extern void updategdd(Real [],const Pftpar [],int,Real);
extern Real gp(Pft *,Real,Real,Real,Real);
extern Real water_stressed(Pft *,Real [LASTLAYER],Real,Real,
                           Real,Real *,Real *,Real *,Real,Real,
                           Real,Real,Real,Real *,Bool);
extern Bool fwritepft(FILE *,const Pft *);
extern void fprintpft(FILE *,const Pft *);
extern Bool freadpft(FILE *,Stand *,Pft *,const Pftpar[],int,Bool);
extern void noinit(Pft *);
extern Real nofire(Pft *,Real *);
extern Real nowdf(Pft *,Real,Real);
extern void noadjust(Litter *,Pft *,Real,Real);
extern void nomix_veg(Pft *,Real);
extern Bool establish(Real,const Pftpar *,const Climbuf *);
extern Real noestablishment(Pft *,Real,Real,int);
extern Bool fscanlimit(LPJfile *,Limit *,const char *,Verbosity);
extern Bool fscanemissionfactor(LPJfile *,Tracegas *,const char *,Verbosity);
extern Bool fscanphenparam(LPJfile *,Phen_param *,const char *,Verbosity);
extern Real fire_sum(const Litter *,Real);
extern void fprintpftpar(FILE *,const Pftpar []);
extern void output_daily(Daily_outputs *,const Pft *,Real,Real);
extern void equilsoil(Soil *, int, const Pftpar []);
extern void noturnover_monthly(Litter *,Pft *);
extern char **createpftnames(int,int,int,int,const Pftpar []);
extern void freepftnames(char **,int,int,int,int);
extern int getnbiomass(const Pftpar [],int);
extern void phenology_gsi(Pft *, Real, Real, int,Bool);

/* needed for IMAGE, but can also be used otherwise */

extern Real timber_burn(const Pft *, Real,Litter *,Real);
extern Real timber_harvest(Pft *,Soil *,Pool *,Pool,Real,Real,Real *,Real *);

/* Definition of macros */

#define isphoto(tstress) (tstress>=1e-2)
#define getpftpar(pft,val) (pft)->par->val
#define newgdd(npft) newvec(Real,npft)

/*
 * The following macros allow to call the PFT specific functions like virtual
 * functions in C++
 */

#define fpar(pft) pft->par->fpar(pft)
#define turnover_monthly(litter,pft) pft->par->turnover_monthly(litter,pft)
#define turnover_daily(litter,pft,temp,isdaily) pft->par->turnover_daily(litter,pft,temp,isdaily)
#define alphaa(pft) pft->par->alphaa_manage(pft)
#define npp(pft,gtemp_air,gtemp_soil,assim) pft->par->npp(pft,gtemp_air,gtemp_soil,assim)
#define leaf_phenology(pft,temp,day,isdaily) pft->par->leaf_phenology(pft,temp,day,isdaily)
#define litter_update(litter,pft,frac) pft->par->litter_update(litter,pft,frac)
#define fire(pft,fireprob) pft->par->fire(pft,fireprob)
#define actual_lai(pft) pft->par->actual_lai(pft)
#define init(pft) pft->par->init(pft)
#define vegc_sum(pft) pft->par->vegc_sum(pft)
#define agb(pft) pft->par->agb(pft)
#define mix_veg(pft,scaler) pft->par->mix_veg(pft,scaler)
#define adjust(litter,pft,fpc,fpc_max) pft->par->adjust(litter,pft,fpc,fpc_max)
#define reduce(litter,pft,fpc) pft->par->reduce(litter,pft,fpc)
#define wdf(pft,demand,supply) pft->par->wdf(pft,demand,supply)
#define establishment(pft,fpc_total,fpc,n_est) pft->par->establishment(pft,fpc_total,fpc,n_est)
#define annualpft(stand,pft,fpc_inc,isdaily) pft->par->annual(stand,pft,fpc_inc,isdaily)
#define albedo_pft(pft,snowheight,snowfraction) pft->par->albedo_pft(pft,snowheight,snowfraction)

#endif
