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

typedef enum {NATURAL,WETLAND,SETASIDE_RF,SETASIDE_IR,SETASIDE_WETLAND,AGRICULTURE,MANAGEDFOREST,
              GRASSLAND,OTHERS,BIOMASS_TREE,BIOMASS_GRASS,AGRICULTURE_TREE,AGRICULTURE_GRASS,WOODPLANTATION,KILL} Landusetype;

typedef struct landuse *Landuse;

typedef struct
{
  Real *crop;
  Real grass[NGRASS];
  Real biomass_grass;
  Real biomass_tree;
  Real *ag_tree;
  Real woodplantation;
} Landfrac;

typedef struct
{
  IrrigationType *crop;
  IrrigationType *ag_tree;
  IrrigationType grass[NGRASS];
  IrrigationType biomass_grass;
  IrrigationType biomass_tree;
  IrrigationType woodplantation;
} Irrig_system;


typedef enum {NO_SEASONALITY, PRECIP, PRECIPTEMP, TEMPERATURE, TEMPPRECIP} Seasonality;

typedef enum {GS_DEFAULT, GS_MOWING, GS_GRAZING_EXT, GS_GRAZING_INT, GS_GRAZING_LIVE, GS_NONE} GrassScenarioType;

typedef struct
{
  Bool irrigation;        /**< stand irrigated? (TRUE/FALSE) */
  Bool irrig_event;        /**< irrigation water applied to field that day? depends on soil moisture and precipitation, if not irrig_amount is put to irrig_stor */
  IrrigationType irrig_system; /**< irrigation system type (NOIRRIG=0,SURF=1,SPRINK=2,DRIP=3) */
  Real ec;                /**< conveyance efficiency */
  Real conv_evap;         /**< fraction of conveyance losses that is assumed to evaporate */
  Real net_irrig_amount;  /**< deficit in upper 50cm soil to fulfill demand (mm) */
  Real dist_irrig_amount; /**< water requirements for uniform field distribution, depending on irrigation system */
  Real irrig_amount;      /**< irrigation water (mm) that reaches the field, i.e. without conveyance losses */
  Real irrig_stor;        /**< storage buffer (mm), filled if irrig_threshold not reached and if irrig_amount > GIR and with irrig_amount-prec */
  int pft_id;             /**< PFT of agriculture tree established */
} Irrigation;

typedef struct
{
  Landfrac *landfrac;     /**< land use fractions */
  Landfrac *fertilizer_nr;   /**< reactive nitrogen fertilizer */
  Landfrac *manure_nr;     /* manure nitrogen fertilizer */
  Landfrac *residue_on_field;     /* fraction of residues left on field */
  Irrig_system *irrig_system; /**< irrigation system type (SURF=1,SPRINK=2,DRIP=3) */
  Manage manage;
  Cropdates *cropdates;
  Real cropfrac_rf;       /**< rain-fed crop fraction (0..1) */
  Real cropfrac_ir;       /**< irrigated crop fraction (0..1) */
  Real cropfrac_wl;       /**< crop fraction on wetland(0..1) */
  int *sowing_month;      /**< sowing month (index of month, 1..12), rainfed, irrigated*/
  int *gs;                /**< length of growing season (number of consecutive months, 0..11)*/
  int sowing_day_cotton[2];
  int growing_season_cotton[2];
  Seasonality seasonality_type;  /**< seasonality type (0..4) 0:no seasonality, 1 and
                                    2:precipitation seasonality, 3 and 4:temperature
                                    seasonality*/
  int *sdate_fixed;       /**< array to store fixed or prescribed sowing dates */
  Real *crop_phu_fixed;   /**< array to store prescribed crop phus */
  Resdata *resdata;       /**< Reservoir data */
  Real *fraction;
  Real reservoirfrac;     /**< reservoir fraction (0..1) */
  Real mdemand;           /**< monthly irrigation demand */
  Bool dam;               /**< dam inside cell (TRUE/FALSE) */
  Bool with_tillage;      /* simulation with tillage implementation */
  GrassScenarioType grass_scenario; /**< 0=default, 1=mowing, 2=ext.grazing, 3=int.grazing */
  Real grassland_lsuha;             /**< livestock density on grassland in LSU/ha */
#if defined IMAGE && defined COUPLED
  Image_data *image_data; /**< pointer to IMAGE data structure */
#endif
  Pool product;
} Managed_land;

/* Definitions of macros */

#define mixpool(pool1,pool2,frac1,frac2) pool1=(pool1*frac1+pool2*frac2)/(frac1+frac2)
#define rothers(ncft) ncft
#define rmgrass(ncft) (ncft+1)
#define rbgrass(ncft) (ncft+2)
#define rbtree(ncft) (ncft+3)
#define rwp(ncft) (ncft+4)
#define agtree(ncft,nwpt) (ncft+4+nwpt)
#define getnnat(npft,config) (npft-config->nbiomass-config->nagtree-config->nwft)
#define getnirrig(ncft,config) (ncft+NGRASS+NBIOMASSTYPE+config->nagtree+config->nwptype)
#define isagriculture(type) (type==AGRICULTURE || type==OTHERS || type==SETASIDE_RF || type==SETASIDE_IR || type==AGRICULTURE_TREE || type==AGRICULTURE_GRASS || type==SETASIDE_WETLAND)

/* Declaration of functions */

extern Landuse initlanduse(const Config *);
extern void freelanduse(Landuse,const Config *);
extern Bool getintercrop(const Landuse);
extern Landfrac *newlandfrac(int,int);
extern void initlandfracitem(Landfrac *,int,int);
extern void initlandfrac(Landfrac [2],int,int);
extern void scalelandfrac(Landfrac [2],int,int,Real);
extern void freelandfrac(Landfrac *);
extern Bool fwritelandfrac(Bstruct ,const char *,const Landfrac [2],int,int);
extern void fprintlandfrac(FILE *,const Landfrac *,int,int);
extern Bool freadlandfrac(Bstruct,const char *,Landfrac [2],int,int);
extern Bool readlandfracmap(Landfrac *,const int [],int,const Real [],int *,int,int);
extern Real landfrac_sum(const Landfrac [2],int,int,Bool);
extern Real crop_sum_frac(Landfrac [2],int,int,Real,Bool);
extern Stocks cultivate(Cell *,Bool,int,Bool,Stand *,
                        int,int,int,int,Bool,const Config *);
#ifdef IMAGE
extern void deforest_for_timber(Cell *,Real,int,Bool,int,Real,int,const Config *);
#endif
extern void reclaim_land(const Stand *, Stand *,Cell *,Bool,int,const Config *);
extern void remove_vegetation_copy(Soil *soil,const Stand *,Cell *,Real,Bool,Bool,const Config *);
extern Bool getlanduse(Landuse,Cell *,int,int,int,int,const Config *);
extern void landusechange(Cell *,int,int,Bool,int,const Config *);
extern Bool setaside(Cell *,Stand *,Bool,Bool,int,int,Bool,Bool,int,const Config *);
extern void sowingcft(Stocks *,Bool *,Cell *,Bool,Bool,Bool,int,int,int,int,int,Bool,const Config *);
extern Stocks sowing_season(Cell *,int,int,int,Real,int,const Config *);
extern Stocks sowing_prescribe(Cell *,int,int,int,int,const Config *);
extern Stocks sowing(Cell *,Real,int,int,int,int,const Config *);
extern void deforest(Cell *,Real,Bool,int,Bool,Bool,Bool,int,int,Real,const Config *);
extern void calc_nir(Stand *,Irrigation *,Real,Real [],Real,const Config *);
extern Real rw_irrigation(Stand *,Real,const Real [],Real,const Config *);
extern void irrig_amount_river(Cell *,const Config *);
extern void irrig_amount(Stand *,Irrigation *,int,int,int,const Config *);
extern void mixsetaside(Stand *,Stand *,Bool,int,int,const Config *);
extern void set_irrigsystem(Stand *,int,int,int,const Config *);
extern void init_irrigation(Irrigation *);
extern Bool fwrite_irrigation(Bstruct,const char *,const Irrigation *);
extern void fprint_irrigation(FILE *,const Irrigation *,const Pftpar *);
extern Bool fread_irrigation(Bstruct,const char *,Irrigation *);
extern Harvest harvest_stand(Output *,Stand *,Real,const Config *);
extern int *scancftmap(LPJfile *,int *,const char *,Bool,int,int,const Config *);
extern int *fscanagtreemap(LPJfile *,const char *,int,const Config *);
extern Bool fscanmowingdays(LPJfile *,Config *);
extern void tillage(Soil *, Real);
extern void getnsoil_agr(Real *,Real *,Real *,const Cell *);
extern Bool readcottondays(Cell *,const Config *);
extern void update_irrig(Stand *,int,int,const Config *);
extern void update_separate_harvests(Output *,Pft *,Bool,int,int,int,const Config *);
extern Bool isirrigevent(const Stand *);
extern int fertday_biomass(const Cell *,const Config *);
extern void fertilize_tree(Stand *,Real,Real,int,const Config *);
extern void setotherstocrop(void);
extern Bool ispftinstand(const Pftlist *,int);
extern void output_gbw(Output *,const Stand *,Real,Real,Real,Real,
                       const Real [LASTLAYER],const Real [LASTLAYER],Real,Real,int,
                       Bool,const Config *);

/* Declaration of variables */

extern char *biomass_names[NBIOMASSTYPE];

#endif
