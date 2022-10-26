/**************************************************************************************/
/**                                                                                \n**/
/**                     h  e  a  d  e  r  .  h                                     \n**/
/**                                                                                \n**/
/**     Reading/Writing file header for LPJ related files. Detects                 \n**/
/**     whether byte order has to be changed                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef HEADER_H /* Already included? */
#define HEADER_H

/* Definition of constants */

#define RESTART_HEADER "LPJRESTART"
#define RESTART_VERSION 26
#define LPJ_CLIMATE_HEADER "LPJCLIM"
#define LPJ_CLIMATE_VERSION 3
#define LPJ_LANDUSE_HEADER "LPJLUSE"
#define LPJ_LANDUSE_VERSION 3
#define LPJ_SOWING_HEADER "LPJSOWD"
#define LPJ_SOWING_VERSION 3
#define LPJGRID_HEADER "LPJGRID"
#define LPJGRID_VERSION 3
#define LPJDRAIN_HEADER "LPJDRAI"
#define LPJDRAIN_VERSION 3
#define LPJEXTFLOW_HEADER "LPJFLOW"
#define LPJEXTFLOW_VERSION 3
#define LPJ_COUNTRY_HEADER "LPJ_COW"
#define LPJ_COUNTRY_VERSION 3
#define LPJWATERUSE_HEADER "LPJWUSE"
#define LPJWATERUSE_VERSION 3
#define LPJNEIGHB_IRRIG_HEADER "LPJNIRR"
#define LPJNEIGHB_IRRIG_VERSION 3
#define LPJSOIL_HEADER "LPJSOIL"
#define LPJSOIL_VERSION 3
#define LPJRESERVOIR_HEADER "LPJDAMS"
#define LPJRESERVOIR_VERSION 3
#define LPJELEVATION_HEADER "LPJELEV"
#define LPJELEVATION_VERSION 3
#define LPJ_POPDENS_HEADER "LPJPOPD"
#define LPJ_POPDENS_VERSION 3
#define LPJ_PRODINI_HEADER "LPJPROD"
#define LPJ_PRODINI_VERSION 3
#define LPJ_LAKEFRAC_HEADER "LPJLAKE"
#define LPJ_LAKEFRAC_VERSION 3
#define LPJOUTPUT_HEADER "LPJ_OUT"
#define LPJOUTPUT_VERSION 4
#define LPJ_LANDCOVER_HEADER "LPJLCOV"
#define LPJ_LANDCOVER_VERSION 3
#define LPJ_FERTILIZER_HEADER "LPJFERT"
#define LPJ_FERTILIZZER_VERSION 3
#define LPJ_SOILPH_HEADER "LPJ_SPH"
#define LPJ_SOILPH_VERSION 3
#define LPJ_TILLAGE_HEADER "LPJTILL"
#define LPJ_TILLAGE_VERSION 2
#define LPJ_CROPPHU_HEADER "LPJ_PHU"
#define LPJ_CROPPHU_VERSION 3
#define CELLYEAR 1
#define YEARCELL 2
#define CELLINDEX 3
#define CELLSEQ 4
#define READ_VERSION -1
#define CLM_MAX_VERSION 4  /**< highest version for clm files supported */
#define MAP_NAME "map"     /**< name of map in JSON files */
#define BAND_NAMES "band_names" /**< name of band string array in JSON metafiles */

extern const char *ordernames[];

/* Definition of datatypes */

typedef struct
{
  int order;          /**< order of data items , either CELLYEAR,YEARCELL or CELLINDEX */
  int firstyear;      /**< first year for data */
  int nyear;          /**< number of years */
  int firstcell;      /**< index of first data item */
  int ncell;          /**< number of data item per year */
  int nbands;         /**< number of data elements per cell */
  float cellsize_lon; /**< longitude cellsize in deg */
  float scalar;       /**< conversion factor*/
  float cellsize_lat; /**< latitude cellsize in deg */
  Type datatype;      /**< data type in file */
  int nstep;          /**< time steps per year (1/12/365) */
  int timestep;       /**< time steps (yrs) */
} Header;

typedef struct
{
  int order;          /**< order of data items , either CELLYEAR,YEARCELL or CELLINDEX */
  int firstyear;      /**< first year for data */
  int nyear;          /**< number of years */
  int firstcell;      /**< index of first data item */
  int ncell;          /**< number of data item per year */
  int nbands;         /**< number of data elements per cell */
  float cellsize_lon; /**< longitude cellsize in deg */
  float scalar;       /**< conversion factor*/
  float cellsize_lat; /**< latitude cellsize in deg */
  Type datatype;      /**< data type in file */
} Header3;

typedef struct
{
  int order;       /**< order of data items , either CELLYEAR or YEARCELL */
  int firstyear;   /**< first year for data */
  int nyear;       /**< number of years */
  int firstcell;   /**< index of first data item */
  int ncell;       /**< number of data item per year */
  int nbands;      /**< number of data elements per cell */
  float cellsize;  /**< cellsize in deg */
  float scalar;    /**< conversion factor*/
} Header2;

typedef struct
{
  int order;       /**< order of data items , either CELLYEAR or YEARCELL */
  int firstyear;   /**< first year for data */
  int nyear;       /**< number of years */
  int firstcell;   /**< index of first data item */
  int ncell;       /**< number of data item per year */
  int nbands;      /**< number of data elements per cell */
} Header_old;

typedef struct
{
  Bool landuse;        /**< land use enabled (TRUE/FALSE) */
  Bool river_routing;  /**< river routing enabled (TRUE/FALSE) */
  int sdate_option;    /**< sowing date option (0-2)*/
  Bool crop_option;    /**< prescribe crop PHU? (TRUE/FALSE) */
  Bool double_harvest; /**< double harvest output enabled */
  Seed seed;           /**< Random seed */
} Restartheader;

typedef struct
{
  List *list;
  Bool isfloat;
} Map;

/* Declaration of functions */

extern Bool fwriteheader(FILE *,const Header *, const char *,int);
extern Bool freadheader(FILE *,Header *,Bool *,const char *,int *,Bool);
extern Bool freadrestartheader(FILE *,Restartheader *,Bool);
extern Bool fwriterestartheader(FILE *,const Restartheader *);
extern Bool freadanyheader(FILE *,Header *,Bool *,String,int *,Bool);
extern size_t headersize(const char *,int);
extern FILE *openinputfile(Header *, Bool *,const Filename *,
                           String,const char *,int *,size_t *,Bool,const Config *);
extern FILE *openmetafile(Header *,Map **,const char *,Attr **,int *,String,String,String,String,Bool *,size_t *,const char *,Bool);
extern char *getfilefrommeta(const char *,Bool);
extern void fprintheader(FILE *,const Header *);
extern char *parse_json_metafile(LPJfile *,char *,Header *,Map **,const char *,Attr **,int *,String,String,String,String,size_t *,Bool *,Verbosity);
extern Map *fscanmap(LPJfile *,const char *,Verbosity);
extern void freemap(Map *);
extern void fprintmap(FILE *,Map *);
extern void fprintjson(FILE *,const char *,const char *,const Header *,
                       Map *,const char *,const Attr *,int,const char *,const char *,
                       const char *,const char *,int,const char *,Bool,int);

/* Definition of macros */

#define printheader(header) fprintheader(stdout,header)
#define printmap(map) fprintmap(stdout,map)
#define restartsize() (5*sizeof(int)+sizeof(Seed)) /* size of Restartheader without padding */
#define getmapsize(map) getlistlen((map)->list)
#define getmapitem(map,i) getlistitem((map)->list,i)

#endif
