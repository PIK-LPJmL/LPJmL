/**************************************************************************************/
/**                                                                                \n**/
/**                        o  u  t  f  i  l  e  .  h                               \n**/
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

#ifndef OUTFILE_H
#define OUTFILE_H

/* Definition of datatypes */

typedef struct
{
  Bool isopen;       /**< file is open for output (TRUE/FALSE) */
  int fmt;           /**< file format (TXT/RAW/CDF) */
  Bool oneyear;      /**< separate output files for each year (TRUE/FALSE) */
  Bool compress;     /**< compress file after write (TRUE/FALSE) */
  const char *filename;
  union
  { 
#ifdef USE_MPI
    MPI_File mpi_file; /**< MPI-2 file descriptor */
#endif
    FILE *file;        /**< file pointer */
    Netcdf cdf;
  } fp;
} File;

typedef struct
{
#ifdef USE_MPI
  int *counts;         /**< sizes for MPI_Gatherv */
  int *offsets;        /**< offsets for MPI_Gatherv */
#endif
  Outputmethod method;
  Socket *socket;
  File *files;
  int n;          /**< size of File array */
  Coord_array *index;
  Coord_array *index_all;
  Bool withdaily; /**< with daily output (TRUE/FALSE) */
} Outputfile;

extern int findfile(const Outputvar *,int,int);
extern Outputfile *fopenoutput(const Cell *,int,const Config *);
extern void openoutput_yearly(Outputfile *,int,const Config *);
extern void closeoutput_yearly(Outputfile *,const Config *);
extern void fcloseoutput(Outputfile *,const Config *);
extern Coord_array *createcoord(Outputfile *,const Cell *,const Config *);
extern Coord_array *createcoord_all(const Cell *,const Config *);
extern Coord_array *createindex(const Coord *,int,Coord,Bool);
extern void outputnames(Outputfile *,const Config *);
#endif
