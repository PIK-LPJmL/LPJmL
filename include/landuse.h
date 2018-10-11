/**************************************************************************************/
/**                                                                                \n**/
/**                 l  a  n  d  u  s  e  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef LANDUSE_H /* Already included? */
#define LANDUSE_H

/* Definitions of datatypes */

typedef enum {NATURAL,SETASIDE_RF,SETASIDE_IR,AGRICULTURE,MANAGEDFOREST,
              GRASSLAND,BIOMASS_TREE,BIOMASS_GRASS,KILL} Landusetype;

typedef struct landuse *Landuse;

typedef struct
{
  Real *crop;
  Real grass[NGRASS];
  Real biomass_grass;
  Real biomass_tree;
} Landfrac;

typedef struct
{
  int *crop;
  int grass[NGRASS];
  int biomass_grass;
  int biomass_tree;
} Irrig_system;

typedef enum {NO_SEASONALITY, PREC, PRECTEMP, TEMP, TEMPPREC} Seasonality;

typedef struct
{
  Landfrac *landfrac;     /**< land use fractions */
  Irrig_system *irrig_system; /**< irrigation system type (SURF=1,SPRINK=2,DRIP=3) */
  Manage manage;
  Cropdates *cropdates;
  Real cropfrac_rf;       /**< rain-fed crop fraction (0..1) */
  Real cropfrac_ir;       /**< irrigated crop fraction (0..1) */
  int *sowing_month;      /**< sowing month (index of month, 1..12), rainfed, irrigated*/
  int *gs;                /**< length of growing season (number of consecutive months, 0..11)*/
  Seasonality seasonality_type;  /**< seasonality type (0..4) 0:no seasonality, 1 and
                                    2:precipitation seasonality, 3 and 4:temperature
                                    seasonality*/
  int *sdate_fixed;       /**< array to store fixed or prescribed sowing dates */
  Resdata *resdata;       /**< Reservoir data */
  Real *fraction;
  Real reservoirfrac;     /**< reservoir fraction (0..1) */
  Real mdemand;           /**< monthly irrigation demand */
  Bool dam;               /**< dam inside cell (TRUE/FALSE) */
  int fixed_grass_pft;              /**< fix C3 or C4 for GRASS pft */
#ifdef IMAGE
  Image_data *image_data; /**< pointer to IMAGE data structure */
#endif
} Managed_land;

/* Definitions of macros */

#define mixpool(pool1,pool2,frac1,frac2) pool1=(pool1*frac1+pool2*frac2)/(frac1+frac2)
#define rothers(ncft) ncft
#define rmgrass(ncft) (ncft+1)
#define rbgrass(ncft) (ncft+2)
#define rbtree(ncft) (ncft+3)

/* Declaration of functions */

extern Landuse initlanduse(int,const Config *);
extern void freelanduse(Landuse,Bool);
extern Bool getintercrop(const Landuse);
extern void newlandfrac(Landfrac [2],int);
extern void initlandfrac(Landfrac [2],int);
extern void scalelandfrac(Landfrac [2],int,Real);
extern void freelandfrac(Landfrac [2]);
extern Bool fwritelandfrac(FILE *,const Landfrac [2],int);
extern Bool freadlandfrac(FILE *,Landfrac [2],int,Bool);
extern Real landfrac_sum(const Landfrac [2],int,Bool);
extern Real crop_sum_frac(Landfrac *,int,Real,Bool);
extern Real cultivate(Cell *,const Pftpar *,int,Real,Bool,int,Bool,Stand *,
                      Bool,int,int,int,int);
extern void reclaim_land(const Stand *, Stand *,Cell *,Bool,int);
extern Bool getlanduse(Landuse,Cell *,int,int,const Config *);
extern void landusechange(Cell *,const Pftpar[],int,int,int,Bool,Bool,int,Bool);
extern Bool setaside(Cell *,Stand *,const Pftpar[],Bool,int,Bool,int);
extern Real sowing_season(Cell *,int,int,int,Real,int,const Config *);
extern Real sowing_prescribe(Cell *,int,int,int,int,const Config *);
extern Real sowing(Cell *,Real,int,int,int,int,const Config *);
extern void deforest(Cell *,Real,const Pftpar [],Bool,int,Bool,Bool,Bool,int,int,Real);
extern Real woodconsum(Stand*,Real);
extern void calc_nir(Stand *,Real,Real [],Real);
extern Real rw_irrigation(Stand *,Real,const Real [],Real);
extern void irrig_amount_river(Cell *,const Config *);
extern void irrig_amount(Stand *,Bool,int,int);
extern void mixsetaside(Stand *,Stand *,Bool);
extern void set_irrigsystem(Stand *,int,int,Bool);

#endif
