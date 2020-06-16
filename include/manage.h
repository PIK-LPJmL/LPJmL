/**************************************************************************************/
/**                                                                                \n**/
/**                        m  a  n  a  g  e  .  h                                  \n**/
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

#ifndef MANAGE_H /* Already included? */
#define MANAGE_H

#include "managepar.h"

/* Declaration of datatypes */

typedef struct
{
  short country,region;
} Code;

typedef struct
{
  int id;     /* country id (0-196)*/
  char *name; /* country name */
  int default_irrig_system; /* default irrig system at country-level (SURF=1,SPRINK=2,DRIP=3), used for ALLCROPS or ALL_IRRIGATION */
  Real laimax_tempcer;  /*laimax for temperate cereals*/
  Real laimax_maize;    /*laimax for maize*/
  /* parameters defined for each country */
  Real *laimax_cft;  /* pointer to crop-specific laimax */

} Countrypar;


typedef struct
{
  int id;     /* region id (0-432)*/
  char *name; /* region name */
  Real bifratio;   /* Yevich's data/rharvest von LPJ; for all crops (no rharvest for grass)*/
  Real fuelratio;  /* Yevich's data (withoutdung)/rharvest von LPJ; for all crops (no rharvest for grass)*/
  Real woodconsum; /* domestic firewood consumption [kgDM/capita/day]*/
} Regionpar; /* parameters defined for each region */


typedef struct
{
  const Countrypar *par;    /* pointer to country-specific parameter */
  const Regionpar *regpar;  /* pointer to region-specific parameter */
  Real *laimax;             /* maximum crop specific LAI */
} Manage;

/* Declaration of functions */

extern int fscancountrypar(LPJfile *,Countrypar **,Bool,int,Verbosity);
extern int fscanregionpar(LPJfile *,Regionpar **,Verbosity);
extern void fprintcountrypar(FILE *,const Countrypar *,int,int);
extern void fprintregionpar(FILE *,const Regionpar[],int);
extern void initmanage(Manage *, const Countrypar *, const Regionpar *,int,
                       int,Bool,Real);
extern void freemanage(Manage *,int);
extern FILE *opencountrycode(const Filename *,Bool *,Type *,long *,Bool);
extern FILE *createcountrycode(const char *,int,int);
extern Bool readcountrycode(FILE *,Code *,Type,Bool);
extern void freecountrypar(Countrypar [],int);
extern void freeregionpar(Regionpar [],int);

/* Definitions of macros */

#define fwritecountrycode(file,code,n) fwrite(code,sizeof(Code),n,file)
#define seekcountrycode(file,index,type,offset) fseek(file,offset+(index)*typesizes[type]*2,SEEK_SET)

#endif
