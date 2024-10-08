/**************************************************************************************/
/**                                                                                \n**/
/**                 c  l  i  m  a  t  e  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header for the climate datatype and related functions                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef CLIMATE_H /* Already included? */
#define CLIMATE_H

#define COLDEST_DAY_NHEMISPHERE 14
#define COLDEST_DAY_SHEMISPHERE 195

/* Definitions of datatypes */

typedef struct
{
  Real *temp; /**< temperature data (deg C) */
  Real *prec; /**< precipitation data (mm) */
  Real *sun;  /**< sunshine data (%) */
  Real *wet;  /**< number of wet days in a month */
  Real *wind; /**< wind speed (m/min) */
  Real *tamp; /**< temperature amplitude */
  Real *tmax; /**< maximum temperature (deg C) */
  Real *humid; /**< specific humidity (kg/kg) */
  Real *tmin; /**< minimum temperature (deg C) */
  Real *lightning;
  Real *lwnet;   /**< long wave net downward flux (W m-2) */
  Real *swdown;  /**< short wave downward flux component (W m-2) */
  Real *burntarea;  /**< burnt area (ha) */
  Real *no3deposition; /**< dry and wet N deposition (gN m-2) */
  Real *nh4deposition; /**< dry and wet N deposition (gN m-2) */
} Climatedata;

typedef struct Dailyclimate
{
  Real temp;       /**< temperature (deg C) */
  Real prec;       /**< precipitation (mm) */
  Real sun;        /**< sunshine (%) */
  Real windspeed;  /**< windspeed (m/min) */
  Real tmin;       /**< minimum temperature (deg C) */
  Real tmax;       /**< maximum temperature (deg C) */
  Real humid;      /**< specific humidity (kg/kg) */
  Real lightning;  /**< daily lightning ignition  */
  Real lwnet;      /**< long wave net downward flux (W/m2) */
  Real swdown;     /**< short wave downward flux component (W/m2) */
  Real burntarea;  /**< burnt area (ha) */
  Real no3deposition; /**< nitrogen deposition  (gN/m2/day) */
  Real nh4deposition; /**< nitrogen deposition  (gN/m2/day) */
  Bool isdailytemp; /**< temperature data are true daily data */
} Dailyclimate;

typedef struct
{
  int firstyear;    /**< first year of CO2 data (AD) */
  int nyear;        /**< number of years of climate data */
  int id;           /**< id for socket communication */
  Real *data;       /**< atmospheric CO2 (ppmv) */
} Co2data;

typedef struct
{
  int firstyear;    /**< first year of data available for all variables (AD) */
  Co2data co2;      /**< CO2 data */
  Climatefile file_temp,file_prec,file_wet; /**< file pointers */
  Climatefile file_cloud,file_lwnet,file_swdown;
  Climatefile file_wind,file_tamp,file_tmax,file_tmin,file_lightning;
  Climatefile file_no3deposition,file_nh4deposition;
  Climatefile file_humid;
#if defined IMAGE && defined COUPLED
  Climatefile file_temp_var,file_prec_var;
#endif
  Climatefile file_burntarea;
  Climatedata data; /**< climate data arrays */
} Climate;

/* Definitions of macros */

#define getcelltemp(climate,cell) climate->data.temp+(cell)*NMONTH
#define getcellprec(climate,cell) climate->data.prec+(cell)*NMONTH
#define getcellsun(climate,cell) climate->data.sun+(cell)*NMONTH
#define getcelllwnet(climate,cell) climate->data.lwnet+(cell)*NMONTH
#define getcellswdown(climate,cell) climate->data.swdown+(cell)*NMONTH
#define getcellwet(climate,cell) climate->data.wet+(cell)*NMONTH
#define getcellwind(climate,cell) climate->data.wind+(cell)*NMONTH
#define getcelltamp(climate,cell) climate->data.tamp+(cell)*NMONTH
#define getcelltmax(climate,cell) climate->data.tmax+(cell)*NMONTH
#define getcellhumid(climate,cell) climate->data.humid+(cell)*NMONTH
#define getcelltmin(climate,cell) climate->data.tmin+(cell)*NMONTH
#define getcelllightning(climate,cell) climate->data.lightning+(cell)*NMONTH
#define getcellburntarea(climate,cell) climate->data.burntarea+(cell)*NMONTH
#define getcellno3deposition(climate,cell) climate->data.no3deposition+(cell)*NMONTH
#define getcellnh4deposition(climate,cell) climate->data.nh4deposition+(cell)*NMONTH
#define getprec(cell,d) (cell).climbuf.dval_prec[(d)+1]
#define israndomprec(climate) ((climate)->data.wet!=NULL)

/* Declaration of functions */

extern Climate *initclimate(const Cell *,Config *);
extern Bool getclimate(Climate *,const Cell *,int,const Config *);
extern Bool getco2(const Climate *,Real *,int,const Config *);
extern Bool getdeposition(Climate *,const Cell *,int,Config *);
extern void freeclimate(Climate *,Bool);
extern Bool storeclimate(Climatedata *,Climate *,const Cell *,int,int,
                         const Config *);
extern void freeclimatedata(Climatedata *);
extern void restoreclimate(Climate *,const Climatedata *,int);
extern void moveclimate(Climate *,const Climatedata *,int);
extern void prdaily(Real [],int,Real,Real,Seed);
extern void dailyclimate(Dailyclimate *,const Climate *,Climbuf *,
                         int,int,int,int);
extern Real getmtemp(const Climate *,const Climbuf *,int,int);
extern Real getmprec(const Climate *,const Climbuf *,int,int);
extern void initclimate_monthly(const Climate *,Climbuf *,int,int,Seed);
extern Bool openclimate(Climatefile *,const Filename *,const char *,Type,Real,
                        const Config *);
extern Real avgtemp(const Climate *,int cell);
extern Real avgprec(const Climate *,int cell);
extern void closeclimatefile(Climatefile *,Bool);
extern Bool readclimate(Climatefile *,Real *,Real,Real,const Cell *,int,
                        const Config *);
extern Bool checkvalidclimate(Climate *,Cell *,Config *);
extern Bool readco2(Co2data *,const Filename *,Config *);
extern void radiation(Real *, Real *,Real *,Real,int,Dailyclimate *,Real,int);
extern Real *readdata(Climatefile *,Real *data,const Cell *,const char *,int,const Config *);
extern int *readintdata(Climatefile *,const Cell *,const char *,int,const Config *);
extern Bool openclmdata(Climatefile *,const Filename *,const char *,const char *,
                     Type,Real,int,const Config *config);
extern Bool opendata(Climatefile *,const Filename *,const char *,const char *,
                     Type,Type,Real,int,Bool,const Config *config);
extern Bool opendata_seq(Climatefile *,const Filename *,const char *,const char *,
                         Type,Real,int,Bool,const Config *config);
extern Bool openinputdata(Infile *,const Filename *,const char *,const char *,
                          Type,Real,const Config *config);
extern Bool readinputdata(Infile *,Real *,const Coord *,int,const Filename *);
extern Bool readintinputdata(Infile *,int *,Bool *,const Coord *,int,const Filename *);

#endif
