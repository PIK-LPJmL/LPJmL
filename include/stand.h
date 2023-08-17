/**************************************************************************************/
/**                                                                                \n**/
/**                    s  t  a  n  d  .  h                                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header file for implementation of stands in LPJ                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef STAND_H  /* Already included? */
#define STAND_H

/* Definition of datatypes */

typedef struct
{
  Landusetype landusetype;
  char *name;
  void (*newstand)(Stand *);
  void (*freestand)(Stand *);
  Bool (*fwrite)(FILE *,const Stand *);
  Bool (*fread)(FILE *,Stand *,Bool);
  void (*fprint)(FILE *,const Stand *,const Pftpar *);
  Real (*daily)(Stand *,Real,const Dailyclimate *,int,int,Real,
                Real,Real,Real,
                Real,Real,int,int,int,Bool,Real,const Config *);
  Bool (*annual)(Stand *,int,int,
                 Real,int,Bool,Bool,const Config *);
  void (*dailyfire)(Stand *,Livefuel *,Real,Real,const Dailyclimate *,const Config *);
} Standtype;

struct stand
{
  const Standtype *type;
  Cell *cell;                 /**< pointer to cell */
  Pftlist pftlist;            /**< List of establishd PFTs */
  Soil soil;                  /**< soil specific variables */
  Real fire_sum;
  Real frac;                  /**< Stand fraction (0..1) */
  Real frac_change;           /**< Expansion fraction due to landuse change (only used for woodplantations) */
  Real frac_g[NSOILLAYER];    /**< fraction of green water in total available soil water, including free water */
  int growing_days;           /**< for GRASS days since harvest*/
  int prescribe_landcover;
  void *data;                 /**< stand-specific extensions */
};

typedef List *Standlist;
typedef struct landcover *Landcover;

/* Declaration of functions */

extern Bool fwritestand(FILE *,const Stand *,int);
extern void fprintstand(FILE *,const Stand *,const Pftpar[],int,int);
extern int fwritestandlist(FILE *,const Standlist,int);
extern void fprintstandlist(FILE *,const Standlist,const Pftpar[],int,int);
extern Stand *freadstand(FILE *,Cell *,const Pftpar[],int,
                         const Soilpar *,const Standtype [],int,Bool,Bool);
extern Standlist freadstandlist(FILE *,Cell *,const Pftpar [],int,
                                const Soilpar *,const Standtype [],int,Bool,Bool);
extern int addstand(const Standtype *,Cell *);
extern void initstand (Stand *);
extern void freestand(Stand *);
extern int delstand(Standlist,int);
extern void freestandlist(Standlist);
extern void mixsoil(Stand *,const Stand *,int,int,const Config *);
extern Bool check_lu(const Standlist ,Real,int,Landusetype,Bool);
extern void check_stand_fracs(const Cell *,Real);
extern int findstand(const Standlist, Landusetype, Bool);
extern int findstandpft(const Standlist,int,Bool);
extern int findlandusetype(const Standlist,Landusetype);
extern void allocation_today(Stand *,const Config *);
extern void light(Stand *,const Real[],const Config *);
extern Stocks establishmentpft(Stand *,int,Real,int,const Config *);
extern Stocks standstocks(const Stand *);
extern void cutpfts(Stand *,const Config *);
extern Real roughnesslength(const Standlist);
extern void waterbalance(Stand *,Real [BOTTOMLAYER],Real [BOTTOMLAYER],Real *,Real *,Real,Real,
                         Real,Real *,Bool);
extern Real water_stressed(Pft *,Real [LASTLAYER],Real,Real,
                           Real,Real *,Real *,Real *,Real,Real,
                           Real,Real,Real,Real *,int,int,int,const Config *);

extern Real infil_perc_irr(Stand *,Real,Real *,int,int,const Config *);
extern Real infil_perc_rain(Stand *,Real,Real *,int,int,const Config *);
extern Real albedo_stand(const Stand *);                            
extern Landcover initlandcover(int,const Config *);
extern Bool readlandcover(Landcover,const Cell *,int,const Config *);
extern Real *getlandcover(Landcover,int);
extern void freelandcover(Landcover,Bool);

/* Definition of macros */

#define getstand(list,index) ((Stand *)getlistitem(list,index))
#define foreachstand(stand,i,list) for(i=0;i<getlistlen(list) && (stand=getstand(list,i));i++)

/*
 * The following macros allow to call the stand-specific functions like virtual
 * functions in C++
 */

#define daily_stand(stand,co2,climate,day,month,daylength,gtemp_air,gtemp_soil,eeq,par,melt,npft,ncft,year,intercrop,agrfrac,config) stand->type->daily(stand,co2,climate,day,month,daylength,gtemp_air,gtemp_soil,eeq,par,melt,npft,ncft,year,intercrop,agrfrac,config)
#define annual_stand(stand,npft,ncft,popdens,year,isdaily,intercrop,config) stand->type->annual(stand,npft,ncft,popdens,year,isdaily,intercrop,config)
#define dailyfire_stand(stand,livefuel,popdens,avgprec,climate,config) if(stand->type->dailyfire!=NULL) stand->type->dailyfire(stand,livefuel,popdens,avgprec,climate,config)

#endif
