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
  Real *data;       /**< atmospheric CO2 (ppmv) */
} Tracedata;

typedef struct
{
  int firstyear;    /**< first year of data available for all variables (AD) */
  Tracedata co2;      /**< CO2 data */
  Tracedata ch4;      /* CH4 data */
  Climatefile file_temp,file_prec,file_wet; /**< file pointers */
  Climatefile file_cloud,file_lwnet,file_swdown;
  Climatefile file_wind,file_tamp,file_tmax,file_tmin,file_lightning;
  Climatefile file_no3deposition,file_nh4deposition;
  Climatefile file_humid;
  Climatefile file_delta_temp, file_delta_prec, file_delta_lwnet, file_delta_swdown;
#if defined IMAGE && defined COUPLED
  Climatefile file_temp_var,file_prec_var;
#endif
  Climatefile file_burntarea;
  Climatedata data[4]; /**< climate data arrays */
} Climate;

/* Definitions of macros */

#define getcelltemp(climate,cell) climate->data[0].temp+(cell)*NMONTH
#define getcellprec(climate,cell) climate->data[0].prec+(cell)*NMONTH
#define getcellsun(climate,cell) climate->data[0].sun+(cell)*NMONTH
#define getcelllwnet(climate,cell) climate->data[0].lwnet+(cell)*NMONTH
#define getcellswdown(climate,cell) climate->data[0].swdown+(cell)*NMONTH
#define getcellwet(climate,cell) climate->data[0].wet+(cell)*NMONTH
#define getcellwind(climate,cell) climate->data[0].wind+(cell)*NMONTH
#define getcelltamp(climate,cell) climate->data[0].tamp+(cell)*NMONTH
#define getcelltmax(climate,cell) climate->data[0].tmax+(cell)*NMONTH
#define getcellhumid(climate,cell) climate->data[0].humid+(cell)*NMONTH
#define getcelltmin(climate,cell) climate->data[0].tmin+(cell)*NMONTH
#define getcelllightning(climate,cell) climate->data[0].lightning+(cell)*NMONTH
#define getcellburntarea(climate,cell) climate->data[0].burntarea+(cell)*NMONTH
#define getcellno3deposition(climate,cell) climate->data[0].no3deposition+(cell)*NMONTH
#define getcellnh4deposition(climate,cell) climate->data[0].nh4deposition+(cell)*NMONTH
#define getprec(cell,d) (cell).climbuf.dval_prec[(d)+1]
#define israndomprec(climate) ((climate)->data[0].wet!=NULL)

/* Declaration of functions */

extern Climate *initclimate(const Cell *,const Config *);
extern Bool getclimate(Climate *,const Cell *,int,int,const Config *);
extern Bool getco2(const Climate *,Real *,int);
extern Bool getch4(const Climate *,Real *,int);
extern void closeclimateanomalies(Climate *,const Config *);
extern void closeclimatefiles(Climate *,const Config *);
extern void freeclimate(Climate *,Bool);
extern Bool storeclimate(Climatedata *,Climate *,const Cell *,int,int,
                         const Config *);
extern void freeclimatedata(Climatedata *);
extern void restoreclimate(Climate *,const Climatedata *,int);
extern void moveclimate(Climate *,const Climatedata *,int,int);
extern void prdaily(Real [],int,Real,Real,Seed);
extern void dailyclimate(Dailyclimate *,const Climate *,Climbuf *,
                         int,int,int,int);
extern Real getmtemp(const Climate *,const Climbuf *,int,int);
extern Real getmprec(const Climate *,const Climbuf *,int,int);
extern void initclimate_monthly(const Climate *,Climbuf *,int,int,Seed);
extern Bool openclimate(Climatefile *,const Filename *,const char *,Type,int,Real,
                        const Config *);
extern Real avgtemp(const Climate *,int cell);
extern Real avgprec(const Climate *,int cell);
extern void closeclimatefile(Climatefile *,Bool);
extern Bool readclimate(Climatefile *,Real *,Real,Real,const Cell *,int,int,
                        const Config *);
extern Bool checkvalidclimate(Climate *,Cell *,Config *);
extern Bool readtracegas(Tracedata *,const Filename *,Bool);
extern void radiation(Real *, Real *,Real *,Real,int,Dailyclimate *,Real,int);
extern void interpolate_climate(Climate *, int, Real);
extern void addanomaly_climate(Climate *, int);
extern Real *readdata(Climatefile *,Real *data,const Cell [],const char *,int,const Config *);
extern int *readintdata(Climatefile *,const Cell [],const char *,int,const Config *);
extern Bool opendata(Climatefile *,const Filename *,const char *,const char *,
                     Type,Real,int,Bool,const Config *config);
extern Bool openinputdata(Infile *,const Filename *,const char *,const char *,
                          Type,Real,const Config *config);
extern Bool readinputdata(Infile *,Real *,const Coord *,int,const Filename *);
extern Bool readintinputdata(Infile *,int *,Bool *,const Coord *,int,const Filename *);

#endif
