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
#define RESTART_VERSION 17
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
#define LPJOUTPUT_VERSION 3
#define LPJ_LANDCOVER_HEADER "LPJLCOV"
#define LPJ_LANDCOVER_VERSION 1

#define CELLYEAR 1
#define YEARCELL 2
#define CELLINDEX 3
#define CELLSEQ 4
#define READ_VERSION -1

/* Definition of datatypes */

typedef struct
{
  int order;          /**< order of data items , either CELLYEAR,YEARCELL or CELLINDEX */
  int firstyear;      /**< first year for data */
  int nyear;          /**< number of years */
  int firstcell;      /**< index of first data item */
  int ncell;          /**< number of data item per year */
  int nbands;         /**< number of data elements per cell */
  float cellsize_lon; /**< latitude cellsize in deg */
  float scalar;       /**< conversion factor*/
  float cellsize_lat; /**< longitude cellsize in deg */
  Type datatype;      /**< data type in file */
} Header;

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
  Bool landuse;       /**< land use enabled (TRUE/FALSE) */
  Bool river_routing; /**< river routing enabled (TRUE/FALSE) */
  int sdate_option;   /**< sowing date option (0-2)*/
} Restartheader;

/* Declaration of functions */

extern Bool fwriteheader(FILE *,const Header *, const char *,int);
extern Bool freadheader(FILE *,Header *,Bool *,const char *,int *);
extern Bool freadrestartheader(FILE *,Restartheader *,Bool);
extern Bool freadanyheader(FILE *,Header *,Bool *,String,int *);
extern size_t headersize(const char *,int);
extern FILE *openinputfile(Header *, Bool *,const Filename *,
                           String, int *,size_t *,const Config *);
extern FILE *openmetafile(Header *, Bool *,size_t *,const char *,Bool);
extern void fprintheader(FILE *,const Header *);

/* Definition of macros */

#define printheader(header) fprintheader(stdout,header)

#endif
