/**************************************************************************************/
/**                                                                                \n**/
/**                    c  o  o  r  d  .  h                                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of coordinate datatype                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef COORD_H /* Already included? */
#define COORD_H

/* Definition of datatypes */

typedef struct
{
  short lon; /**< longitudein degrees * 100 */
  short lat; /**< latitude in degrees * 100 */
} Intcoord;

typedef struct
{
  Real lon;  /**< longitude in degrees */
  Real lat;  /**< latitude in degrees */
  Real area; /**< cell area (m^2) */
} Coord;

typedef struct coordfile *Coordfile;

/* Declaration of functions */

extern Coordfile opencoord(const Filename *,Bool);
extern Bool readintcoord(FILE *,Intcoord *,Bool);
extern int seekcoord(Coordfile,int);
extern Bool readcoord(Coordfile,Coord *,const Coord *);
extern void closecoord(Coordfile);
extern Bool writecoord(FILE *,const Coord *);
extern Bool writefloatcoord(FILE *,const Coord *);
extern Real cellarea(const Coord *,const Coord *);
extern Bool fscancoord(LPJfile *,Coord *,Verbosity);
extern int numcoord(const Coordfile);
extern void getcellsizecoord(float *,float *,const Coordfile);
extern char *sprintcoord(String,const Coord *);
extern void fprintcoord(FILE *,const Coord *);
extern int findcoord(const Coord *,const Coord [],int);

/* Definition of macros */

#define printcoord(coord) fprintcoord(stdout,coord)

#endif
