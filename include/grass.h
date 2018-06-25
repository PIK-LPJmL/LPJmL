/**************************************************************************************/
/**                                                                                \n**/
/**                   g  r  a  s  s  .  h                                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header file for grass PFTs                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef GRASS_H /* Already included? */
#define GRASS_H


/* Definition of datatypes */

typedef struct
{
  Real leaf,root;
} Grassphys;

typedef struct
{
  Grassphys turnover;	/**< turnover period (years) (9,11,12) */
  Grassphys cn_ratio;	/**< C:N mass ratio (13-15) */
  Grassphys sapl;       /**< sapling */
  Real sapling_C;	/**< sapling C (gC/m2) */
  Real reprod_cost;     /**< reproduction cost */
} Pftgrasspar;

typedef struct
{
  Grassphys turn;
  Grassphys ind;
  Real max_leaf;
} Pftgrass;

/* Declaration of functions */

extern void new_grass(Pft *,int,int);
extern Real npp_grass(Pft *,Real,Real,Real);
extern Real fpc_grass(Pft *);
extern Real fpar_grass(const Pft *);
extern Real alphaa_grass(const Pft *);
extern void litter_update_grass(Litter *, Pft*,Real);
extern Bool allocation_grass(Litter *,Pft *);
extern Real lai_grass(const Pft *);
extern Real actual_lai_grass(const Pft *);
extern Real turnover_grass(Litter *,Pft *,Real);
extern void phenology_grass(Pft *,Real,int,Bool);
extern Bool fwrite_grass(FILE *,const Pft *);
extern void fprint_grass(FILE *,const Pft *);
extern Bool fread_grass(FILE *,Pft *,Bool);
extern Bool fscanpft_grass(LPJfile *,Pftpar *,Verbosity);
extern Real establishment_grass(Pft *,Real,Real,int);
extern Real vegc_sum_grass(const Pft *);
extern Real agb_grass(const Pft *);
extern void mix_veg_grass(Pft *,Real);
extern Real fire_grass(Pft *,Real *);
extern void init_grass(Pft *);
extern void free_grass(Pft *);
extern void light_grass(Litter *,Pft *,Real,Real);
extern void adjust_grass(Litter *,Pft *,Real,Real);
extern void reduce_grass(Litter *,Pft *,Real);
extern void fprintpar_grass(FILE *,const Pftpar *);
extern Real livefuel_consum_grass(Litter *,Pft *,const Fuel *,Livefuel *,
                                  Bool *,Real, Real);
extern Bool annual_grass(Stand *,Pft *,Real *,Bool);
extern Harvest harvest_stand(Output *,Stand *,Real);
extern void turnover_monthly_grass(Litter *,Pft *);
extern void turnover_daily_grass(Litter *,Pft *,Real,Bool);
extern void albedo_grass(Pft *,Real,Real);

/* Definition of macros */

#define isgrass(pft) (getpftpar(pft,type)==GRASS)
#define fprintgrassphys(file,phys) fprintf(file,"%.2f %.2f (gC/m2)",phys.leaf,phys.root)
#define phys_sum_grass(ind) (ind.leaf+ind.root)

#endif
