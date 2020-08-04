/**************************************************************************************/
/**                                                                                \n**/
/**                      i  m  a  g  e  .  h                                       \n**/
/**                                                                                \n**/
/**     Declaration of IMAGE coupler functions and related datatypes               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/


#ifndef IMAGE_H /* already included? */
#define IMAGE_H

/* Definition of constants for biomass pools used in IMAGE */

#define PRODUCT_FAST 0.1
#define PRODUCT_SLOW 0.01

#define LPJIMAGE "LPJIMAGE"  /* Environment variable for IMAGE coupler */
#define LPJWAITIMAGE "LPJWAITIMAGE"  /* time to wait for IMAGE connection */
#define DEFAULT_IMAGE_HOST "localhost" /* Default host for IMAGE model */
#define DEFAULT_IMAGE_INPORT 2225  /* Default port for ingoing connection */
#define DEFAULT_IMAGE_OUTPORT 2224 /* Default port for outgoing connection */
#define WAIT_IMAGE 12000             /* wait for IMAGE connection (sec) */
#define NIMAGETREEPARTS (sizeof(Takeaway)/sizeof(float))

#define INCLUDEWP /*wood plantations are always switched on in coupled mode*/

typedef struct
{
   /* Fraction of harvest wood pools that is taken out of forest (-)  */
   float stems;
   float branches;
   float leaves;
   float roots;
} Takeaway;

typedef struct
{
  /* Fraction of grid harvested for timber (-) */
  float fast;
  float slow;
} Timber;

typedef struct
{
  Real fast;
  Real slow;
} Product;

typedef struct
{
  float stems;
  float branches; 
  float leaves; 
  float roots; 
  float litter;
  float humus;
  float charcoal;
  float product_fast;
  float product_slow;
} Biomass;

typedef struct
{
  FILE *file;     /* file pointer for open wateruse file */
  int firstyear;  /* first year in file */
  int nband;      /* number of bands in file */
  int size;       /* number of data items per year */
  int offset;     /* file offset for first grid element */
  Bool swap;      /* data has to be swapped for correct byte order */
  float scalar;   /* scaling factor for input data */
  Type datatype;  /* datatype in file */
} Productinit;

/* typedef struct 22 crops
{
  float pasture;
  float temp_cereals;
  float rice;
  float maize;
  float trop_cereals;
  float pulses;
  float roots_tubers;
  float oil_crops;
  float other_crops;
  float sugar_cane;
  float maize_biofuels;
  float woody_biofuels;
  float non_woody_biofuels;
  float irrig_temp_cereals;
  float irrig_rice;
  float irrig_maize;
  float irrig_trop_cereals;
  float irrig_pulses;
  float irrig_roots_tubers;
  float irrig_oil_crops;
  float irrig_other_crops;
  float woodplantation;
} Image_landuse;
*/
typedef struct
{
   float pasture;               // 0  Grassland
   float wheat_rf;              // 1  Wheat
   float rice_rf;               // 2  Rice
   float maize_rf;              // 3  Maize
   float trop_cereals_rf;       // 4  Tropical cereals
   float oth_temp_cereals_rf;   // 5  Other temperate cereals
   float pulses_rf;             // 6  Pulses
   float soybeans_rf;           // 7  Soybeans
   float temp_oilcrops_rf;      // 8  Temperate oil crops
   float trop_oilcrops_rf;      // 9  Tropical oil crops
   float temp_roots_tub_rf;     // 10 Temperate roots & tubers
   float trop_roots_tub_rf;     // 11 Tropical roots & tubers
   float suger_cain_rf;         // 12 Sugar cane
   float oil_palmfruit_rf;      // 13 Oil, palm fruit
   float veg_fruits_rf;         // 14 Vegetables & fruits
   float oth_non_rf;            // 15 Other non - food, luxury, spices
   float plant_fibres_rf;       // 16 Plant based fibres
   float grains_rf;             // 17 Grains(biofuel)
   float oilcrops_bf;           // 18 Oil crops(biofuel)
   float sugercrops_bf;         // 19 Sugercrops(biofuel)
   float woody_bf;              // 20 Woody biofuel
   float non_woody_bf;          // 21 Non woody biofuel
   float wheat_ir;              // 22 Wheat
   float rice_ir;               // 23 Rice
   float maize_ir;              // 24 Maize
   float trop_cereals_ir;       // 25 Tropical cereals
   float oth_temp_cereals_ir;   // 26 Other temperate cereals
   float pulses_ir;             // 27 Pulses
   float soybeans_ir;           // 28 Soybeans
   float temp_oilcrops_ir;      // 29 Temperate oil crops
   float trop_oilcrops_ir;      // 30 Tropical oil crops
   float temp_roots_tub_ir;     // 31 Temperate roots & tubers
   float trop_roots_tub_ir;     // 32 Tropical roots & tubers
   float suger_cain_ir;         // 33 Sugar cane
   float oil_palmfruit_ir;      // 34 Oil, palm fruit
   float veg_fruits_ir;         // 35 Vegetables & fruits
   float oth_non_ir;            // 36 Other non - food, luxury, spices
   float plant_fibres_ir;       // 37 Plant based fibres
   float forest_plant;          // 38 forest plantation
} Image_landuse;

typedef struct
{
  /* for the moment hardcoded, must be consistent with pft_2layer.par */
  float temp_cereals;
  float rice;
  float maize;
  float trop_cereals;
  float pulses;
  float temp_roots;
  float trop_roots;
  float sunflower;
  float soybean;
  float groundnut;
  float rapeseed;
  float sugarcane;
  float others;
  float managed_grass;
  float biomass_grass;
  float biomass_tree;
  float woodplantation;
  float irrig_temp_cereals;
  float irrig_rice;
  float irrig_maize;
  float irrig_trop_cereals;
  float irrig_pulses;
  float irrig_temp_roots;
  float irrig_trop_roots;
  float irrig_sunflower;
  float irrig_soybean;
  float irrig_groundnut;
  float irrig_rapeseed;
  float irrig_sugarcane;
  float irrig_others;
  float irrig_managed_grass;
  float irrig_biomass_grass;
  float irrig_biomass_tree;
  float irrig_woodplantation;
} cropyield_to_image;

typedef struct
{
  float jan;
  float feb;
  float mar;
  float apr;
  float may;
  float jun;
  float jul;
  float aug;
  float sep;
  float oct;
  float nov;
  float dec;
} mirrig_to_image;

typedef struct
{
  Real fburnt;             /* fraction of deforested wood burnt */
  Real timber_frac;        /* fraction of gridcell harvested for timber */
  Real timber_frac_wp;     /* fraction of gridcell harvested for woodplantation */
  Real totwatcons;         /* total water consumption */
  Real totwatdem;          /* total water demand */
  Real deforest_emissions; /* carbon emissions from deforested wood burnt [gC/m2]*/
  Pool timber_f;           /* fraction that enters fast and slow product pool */
  Pool timber;             /* fast and slow timber pool [gC/m2] */
  Real timber_turnover;    /* annual sum of decayed product pool [gC/m2] */
  Real anpp;               /* annual NPP (gC/m2) */
  Real arh;                /* annual Rh (gC/m2) */
  Real prod_turn_fast;     /* product turnover from fast product pool (gC/m2/a) */
  Real prod_turn_slow;     /* product turnover from slow product pool (gC/m2/a) */
  Real store_bmtree_yield; /* yield of biomass trees storage to distribute over following years (gC/m2/8a) */
  Real biomass_yield_annual; /* biomass yield scaled to annual fractions */
  Real takeaway[NIMAGETREEPARTS];          /* fraction of harvested carbonpools taken from forest */
  Real mirrwatdem[NMONTH]; /* monthly  irrigation water demand (mm) for output to image*/
  Real mevapotr[NMONTH];   /* monthly  actual evaporaton       (mm) for output to image*/
  Real mpetim[NMONTH];     /* monthly  potential evaporation   (mm) for output to image*/
} Image_data;

/* Declaration of functions */
extern Image_data *new_image(const Product *);
extern Bool open_image(Config *);
extern void close_image(const Config *);
extern Real product_turnover(Image_data *);
extern Bool send_image_data(const Config *,const Cell *,const  Climate *,int, int);
extern Bool receive_image_climate(Climate *,int,const Config *);
extern Bool receive_image_productpools(Cell *,const Config *);
extern Bool receive_image_luc(Cell *,int,int,const Config *);
extern Bool receive_image_data(Cell *,int,int,const Config *);
extern Real receive_image_co2(const Config *);
extern Real receive_image_finish(const Config *);
extern Productinit *initproductinit(const Config *);
extern Bool getproductpools(Productinit *,Product [],int);
extern void freeproductinit(Productinit *);
extern void setoutput_image(Cell *,int);
extern void monthlyoutput_image(Output *,const Climate *,int,int);

#endif /* IMAGE_H */
