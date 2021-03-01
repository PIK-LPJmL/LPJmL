/**************************************************************************************/
/**                                                                                \n**/
/**                     g  r  a  s  s  l  a  n  d  .  h                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of grassland stand                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef GRASSLAND_H
#define GRASSLAND_H

/* Definition of settings */

#define DEMAND_COW_INT 4000.0         // [g.C.day-1] same for extensive and intensive grazing;
#define DEMAND_COW_EXT 4000.0         // corresponds to 8.9 kg DW which is in between the observed values of 6 and 12.

#define STUBBLE_HEIGHT_MOWING 25      // [g.C.m-2] equals 5 cm. The leaf biomass remaining on paddock after mowing.

#define STOCKING_DENSITY_EXT 0.5      // [LSU.ha-1] (LiveStockUnit). Stocking densities for
#define STOCKING_DENSITY_INT 1.2      // both grazing systems are rough estimates.

#define MANURE 0.25                   // [-] 25 % portion of grazed biomass going to the fast soil litter pool; literature value

#define MAX_PADDOCKS 16               // Rough estimate from Faustzahlen.

#define MAX_ROTATION_LENGTH 50        // [days] Rough estimate when usually rotations take 20 to 35 days
#define MIN_ROTATION_LENGTH 5         // [days] Rough estimate

#define GRAZING_STUBBLE 5             // [g.C.m-2] Grazing stops when less than this threshold is left in the leaves

#define STUBBLE_HEIGHT_GRAZING_EXT 5  // [g.C.m-2] Minimal threshold to start extensive grazing
#define STUBBLE_HEIGHT_GRAZING_INT 40 // [g.C.m-2] equals 7-8 cm. Threshold to start intensive grazing


#define STUBBLE_HEIGHT_MOWING 25

#define DEMAND_COW_INT 4000.0         // [g.C.day-1] same for extensive and intensive grazing;
#define DEMAND_COW_EXT 4000.0         // corresponds to 8.9 kg DW which is in between the observed values of 6 and 12.



#define STOCKING_DENSITY_EXT 0.5      // [LSU.ha-1] (LiveStockUnit). Stocking densities for
#define STOCKING_DENSITY_INT 1.2      // both grazing systems are rough estimates.

#define MANURE 0.25                   // [-] 25 % portion of grazed biomass going to the fast soil litter pool; literature value

#define MAX_PADDOCKS 16               // Rough estimate from Faustzahlen.

#define MAX_ROTATION_LENGTH 50        // [days] Rough estimate when usually rotations take 20 to 35 days
#define MIN_ROTATION_LENGTH 5         // [days] Rough estimate

#define GRAZING_STUBLE 5              // [g.C.m-2] Grazing stops when less than this threshold is left in the leaves

#define STUBBLE_HEIGHT_GRAZING_EXT 5  // [g.C.m-2] Minimal threshold to start extensive grazing
#define STUBBLE_HEIGHT_GRAZING_INT 40 // [g.C.m-2] equals 7-8 cm. Threshold to start intensive grazing

/* Definition of datatypes */

typedef enum {RM_UNDEFINED, RM_GRAZING, RM_RECOVERY} RotationModeType;

typedef struct
{
  int grazing_days;
  int recovery_days;
  int paddocks;
  RotationModeType mode;
} Rotation;

typedef struct
{
  Irrigation irrigation;
  Rotation rotation;
  Real nr_of_lsus_ext;   /**< nr of livestock units for extensive grazing */
  Real nr_of_lsus_int;   /**< nr of livestock units for intensive grazing */
} Grassland;

/* Declaration of functions */

extern Standtype grassland_stand;
extern const char *grassland_names[NGRASS];

extern Real daily_grassland(Stand *,Real,const Dailyclimate *, int,int,
                            Real,
                            Real,Real,Real,Real,
                            Real ,int, int, int,Bool,Real,const Config *);
extern Bool annual_grassland(Stand *,int,int,Real,int,Bool,Bool,const Config *);
extern Bool isdailyoutput_grassland(const Config *,const Stand *);
extern void output_gbw_grassland(Output *,const Stand *,Real,Real,Real,Real,
                                 const Real [LASTLAYER],const Real [LASTLAYER],Real,Real,int,
                                 const Config *);
extern void new_grassland(Stand *);
extern Bool fread_grassland(FILE *,Stand *,Bool);
extern Bool fwrite_grassland(FILE *,const Stand *);
extern void fprint_grassland(FILE *,const Stand *,const Pftpar *);

#endif
