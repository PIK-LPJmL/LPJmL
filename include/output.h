/**************************************************************************************/
/**                                                                                \n**/
/**                        o  u  t  p  u  t  .  h                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header file for output datatype and functions                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

/* Definition of datatypes */

#define DEFAULT_PORT 2222 /* default port for socket connection */
#define issoil(index) (index==KS_NAT || index==KS_GRASS || index==KS_AGR ||index==WHC_NAT || index==WHC_GRASS || index==WHC_AGR ||\
                       index==SOILC_LAYER || index == SOILC_AGR_LAYER || index==SOILN_LAYER || index==SOILNO3_LAYER || index==SOILNH4_LAYER || index==SOILTEMP || index==SWC || \
                       index==RESPONSE_LAYER_AGR || index==RESPONSE_LAYER_NV || index==CSHIFT_FAST_NV || index==CSHIFT_SLOW_NV || index == SOILC_AGR_LAYER || index==PERC)

#define getoutput(output,index,config) (output)->data[(config)->outputmap[index]]
#ifdef CHECK_BOUNDARY
#define getoutputindex(output,index,i,config) (output)->data[outputindex(index,i,config)]
#else
#define getoutputindex(output,index,i,config) (output)->data[(config)->outputmap[index]+i]
#endif

typedef struct
{
  Real co2;
  Real co;
  Real ch4;
  Real voc;
  Real tpm;
  Real nox;
} Tracegas;

typedef struct
{
  Real *data;            /**< storage for output */
  Real mpet;             /**< monthly PET (mm) */
  Real dcflux;           /**< daily carbon flux from LPJ to atmosphere (gC/m2/day) */
  int *syear2;           /**< sowing year of second season, used for double_harvest */
  int *syear;            /**< sowing year of first season, used for double_harvest */
#ifdef COUPLING_WITH_FMS
  Real dwflux;           /**< daily water flux from LPJ to atmosphere (kg/m2/day) */
#endif
} Output;

typedef struct
{
  Real npp;      /**< Total NPP (gC) */
  Real gpp;      /**< Total GPP (gC) */
  Real rh;       /**< Total heterotrophic respiration (gC) */
  Real fire;     /**< Total fire (gC) */
  Real estab;    /**< Total extablishment flux (gC) */
  Real harvest;  /**< Total harvested carbon (gC) */
  Real transp;   /**< Total transpiration (dm3) */
  Real evap;     /**< Total evaporation (dm3) */
  Real interc;   /**< Total interception (dm3) */
  Real wd;       /**< Total irrigation water withdrawal (dm3) */
  Real wd_unsustainable;      /**< Total irrigation water withdrawal from unsustainable source (dm3) */
  Real wateruse;              /**< Total wateruse for household industry and livestock */
  Real discharge;             /**< Total discharge (dm3) */
  Real ext;                   /**< Total external flux (dm3) */
  Real evap_lake;             /**< Total evaporation from lakes (dm3) */
  Real evap_res;              /**< Total evaporation from reservoirs (dm3) */
  Real irrig;                 /**< Total field irrigation (dm3) */
  Real conv_loss_evap;        /**< Total evaporation during conveyance (dm3) */
  Real prec;                  /**< Total precipitation (dm3) */
  Real delta_surface_storage; /**< Total change of surface storage (dm3), increase positive */
  Real delta_soil_storage;    /**< Total change of soil storage (dm3), increase positive */
  Real area;                  /**< Total area (m2) */
  Real total_reservoir_out;   /**< Total water extracted from reservoirs for irrigation */
  Real total_irrig_from_reservoir; /*Total water added to fields from reservoirs */
  Real n_demand;              /**< total N demand by plants */
  Real n_uptake;              /**< total N uptake by plants */
  Real n_influx;              /**< total N inputs */
  Real n_outflux;             /**< total N losses */
  Real excess_water;          /**< Exess water (dm3) */
  Real soilc;                 /**< soil carbon (gC) */
  Real soilc_slow;            /**< slow soil carbon (gC) */
  Real litc;                  /**< litter carbon (gC) */
  Real vegc;                  /**< vegetation carbon (gC) */
  Real productc;              /**< product pool carbon (gC) */
  Real product_turnover;      /**< product pool carbon turnover (gC/yr) */
  Real neg_fluxes;
  Real area_agr;              /**< agriculture area (m2) */
} Flux;

typedef enum {LPJ_FILES,LPJ_MPI2,LPJ_GATHER,LPJ_SOCKET} Outputmethod;

typedef enum { MISSING_TIME,SECOND,DAY,MONTH,YEAR } Time;

typedef struct
{
  char *name;  /**< variable name */
  char *var;   /**< data name in NetCDF file */
  char *descr; /**< description */
  char *unit;  /**< units */
  float scale;
  float offset;
  Time time;
  int timestep; /**< time step (ANNUAL,MONTHLY,DAILY) */
} Variable;

/* Declaration of variables */

/* Declaration of functions */

extern void freeoutput(Output *);
extern int outputsize(int,int,int,const Config *);
extern Type getoutputtype(int,Bool);
extern int getnyear(const Variable *,int);
extern Bool isnitrogen_output(int);
extern void fwriteoutputdata(FILE *,const Output *,const Config *);
extern Bool freadoutputdata(FILE *,Output *,Bool,Config *);
extern Bool isannual(int,const Config *);
extern int outputindex(int,int,const Config *);
extern int getmintimestep(int);
extern Bool fprintoutputjson(int,int,const Config *);
#ifdef USE_MPI
extern int mpi_write(FILE *,void *,MPI_Datatype,int,int *,
                     int *,int,MPI_Comm);
extern int mpi_write_txt(FILE *,void *,MPI_Datatype,int,int *,
                         int *,int,char,MPI_Comm);
#endif

/* Definition of macros */

#define isopen(output,index) output->files[index].isopen
#define output_flux(output,flux) writedouble_socket(output->socket,(Real *)&flux,sizeof(Flux)/sizeof(Real))

#endif
