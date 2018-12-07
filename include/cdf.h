/**************************************************************************************/
/**                                                                                \n**/
/**                              c  d  f  .  h                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Header file for NetCDF interface for input/output                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef CDF_H
#define CDF_H

#define MISSING_VALUE_FLOAT -1e32
#define MISSING_VALUE_SHORT -9999
#define MISSING_VALUE_INT -999999
#define MISSING_VALUE_BYTE 99
#define NO_TIME -1
#define LON_NAME "longitude"
#define LAT_NAME "latitude"
#define TIME_NAME "time"
#define LON_DIM_NAME "longitude"
#define LAT_DIM_NAME "latitude"
#define TIME_DIM_NAME "time"

typedef enum { ONEFILE,CREATE,APPEND,CLOSE} State_nc;

typedef struct
{
  Real lon_min;
  Real lat_min;
  int nlon;
  int nlat;
  int *index;
} Coord_array;

typedef struct cdf 
{
  State_nc state;
  struct cdf *root;
  int ncid;
  int varid;
  int time_dim_id,lon_dim_id,lat_dim_id;
  int time_var_id,lon_var_id,lat_var_id;
  int n;
  const Coord_array *index;
  float missing_value;
} Netcdf;

typedef enum { MISSING_TIME,DAY,MONTH,YEAR } Time;

typedef struct
{
  int firstyear;    /**< first year of climate data (AD) */
  int n;            /**< number of grid cell data to be read */
  long long offset; /**< file offset in bytes */
  long long size;   /**< size of dataset for each year in bytes */
  int nyear;        /**< number of years of climate data */
  Time time_step;   /**< time steps (DAY/MONTH/YEAR) */
  Bool ready;       /**< data was already averaged */
  Bool swap;        /**< byte order has to be changed (TRUE/FALSE) */
  FILE *file;       /**< file pointer */
  int fmt;          /**< file format (RAW/CLM/CDF) */
  int version;      /**< file version number */
  Real scalar;      /**< conversion factor */
  Type datatype;    /**< datatype */
  char *filename;
  const char *var;  /**< variable name */
  const char *units;/**< variable units or NULL */
  Bool oneyear;     /**< one file for each year (TRUE/FALSE) */
  size_t var_len;
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int ncid;
  int varid;
  Bool isleap;      /**< leap days in file (TRUE/FALSE) */
  Bool is360;       /**< lon coordinates are in [0,360] (TRUE/FALSE) */
  size_t nlon,nlat; /**< dimensions of longitude/latitude */
  float lon_min;
  float lat_min;
  float lon_res;
  float lat_res;
  union
  {
    short s;
    int i;
    float f;
    double d;
  } missing_value;  /**< missing/fill value in file */
  double slope,intercept; /**< conversion values derived from udunits */
#endif
} Climatefile;

typedef struct coord_netcdf *Coord_netcdf;

typedef struct input_netcdf *Input_netcdf;

typedef union
{
  FILE *file;
  Input_netcdf cdf;
} Infile;

extern Bool create1_netcdf(Netcdf *,const char *,const char *,
                          const char *,const char *,Type,int,
                          const Coord_array *,int,const Config *);
extern Bool create_netcdf(Netcdf *,const char *,const char *,
                          const char *,const char *,Type,int,
                          const Coord_array *,const Config *);
extern Bool openclimate_netcdf(Climatefile *,const char *,const char *,
                               const char *,const Config *);
extern Bool mpi_openclimate_netcdf(Climatefile *,const char *,const char *,
                                   const char *,const Config *);
extern Bool create_pft_netcdf(Netcdf *,const char *,int,int,int,const char *,
                              const char *,const char *,Type,int,
                              const Coord_array *,const Config *);
extern Bool create1_pft_netcdf(Netcdf *,const char *,int,int,int,const char *,
                              const char *,const char *,Type,int,int,
                              const Coord_array *,const Config *);
extern Bool close_netcdf(Netcdf *);
extern Bool readclimate_netcdf(Climatefile *,Real *,const Cell *,int,
                               const Config *);
extern int checkvalidclimate_netcdf(Climatefile *,Cell *,int,const Config *);
extern Bool readintclimate_netcdf(Climatefile *,int *,const Cell *,int,
                                  const Config *);
extern void closeclimate_netcdf(Climatefile *,Bool);

extern Bool write_float_netcdf(const Netcdf *,const float[],int,int);
extern Bool write_int_netcdf(const Netcdf *,const int[],int,int);
extern Bool write_short_netcdf(const Netcdf *,const short[],int,int);
extern Bool write_pft_float_netcdf(const Netcdf *,const float[],int,int,int);
extern Bool write_pft_short_netcdf(const Netcdf *,const short[],int,int,int);
extern void freecoordarray(Coord_array *);
extern Bool openfile_netcdf(Climatefile *,const char *,
                     const char *,const char *,const Config *);
extern Bool opendata_netcdf(Climatefile *,const char *,
                     const char *,const char *,const Config *);
extern Bool readdata_netcdf(const Climatefile *,Real *,const Cell *,
                            int,const Config *);
extern Bool readintdata_netcdf(const Climatefile *,int *,const Cell *,
                               int,const Config *);
extern Bool readshortdata_netcdf(const Climatefile *,short *,const Cell *,
                                 int,const Config *);
extern Coord_netcdf opencoord_netcdf(const char *,const char *,Bool);
extern void closecoord_netcdf(Coord_netcdf);
extern Bool seekcoord_netcdf(Coord_netcdf,int);
extern Bool readcoord_netcdf(Coord_netcdf,Coord *,const Coord *,unsigned int *);
extern int numcoord_netcdf(const Coord_netcdf);
extern int *getindexcoord_netcdf(const Coord_netcdf);
extern void getresolution_netcdf(const Coord_netcdf,Coord *);
extern void getextension_netcdf(Extension *,const Coord_netcdf);
extern Input_netcdf openinput_netcdf(const char *,const char *,const char *,
                                     size_t,const Config *);
extern void closeinput_netcdf(Input_netcdf);
extern size_t getindexinput_netcdf(const Input_netcdf,const Coord *);
extern size_t getindexsize_netcdf(const Input_netcdf);
extern Bool readinput_netcdf(const Input_netcdf,Real *,const Coord *);
extern Bool readintinput_netcdf(const Input_netcdf,int *,const Coord *);
extern Bool readshortinput_netcdf(const Input_netcdf,short *,const Coord *);
extern Input_netcdf dupinput_netcdf(const Input_netcdf);
extern Type getinputtype_netcdf(const Input_netcdf);
extern Bool getlatlon_netcdf(Climatefile *,const char *,const Config *);
extern Bool getvar_netcdf(Climatefile *,const char *,const char *,
                          const char *,const Config *);
extern void closeinput(Infile,int);
extern int open_netcdf(const char *,int *,Bool *);
extern void free_netcdf(int);

#ifdef USE_MPI
extern Bool mpi_write_netcdf(const Netcdf *,void *,MPI_Datatype,int,int,
                             int [],int [],int,MPI_Comm);
extern Bool mpi_write_pft_netcdf(const Netcdf *,void *,MPI_Datatype,int,
                                 int,int,int [],int [],int,MPI_Comm);
#endif

/* Definition of macros */

#define isdaily(climate) ((climate).time_step==DAY)

#endif
