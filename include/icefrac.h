/**************************************************************************************/
/**                                                                                \n**/
/**                     i  c  e  f  r  a  c  .  h                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of datatype Icefrac                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef ICEFRAC_H
#define ICEFRAC_H

typedef struct icefrac *Icefrac;

extern Icefrac initicefrac(const Config *);
extern void freeicefrac(Icefrac, Bool);
extern void interpolate_icefrac(Icefrac, int, Real);
extern Real geticefrac(const Icefrac, int);
extern Bool readicefrac(Icefrac, const Cell *, int, int, const Config *);

#endif
