/**************************************************************************************/
/**                                                                                \n**/
/**                     n  a  t  u  r  a  l  .  h                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of natural stand                                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef NATURAL_H
#define NATURAL_H

extern Standtype natural_stand;

extern Real daily_natural(Stand *,Real,const Dailyclimate *,int,
                          Real,const Real [],
                          Real,Real,Real,Real,
                          Real,Real,Real,int,int,int,Bool,Bool,const Config *);
extern Bool annual_natural(Stand *,int,int,Real,int,Bool,Bool,const Config *);

extern void dailyfire_natural(Stand *,Output *,Livefuel *,Real *,Real,
                              Dailyclimate,const Pftpar *,int);
extern Real woodconsum_natural(Stand*, Pftlist *,Litter *,Real,Real,Real,int);

extern void new_natural(Stand *);
extern void free_natural(Stand *);
extern Bool fread_natural(FILE *,Stand *,Bool);
extern Bool fwrite_natural(FILE *,const Stand *);
extern void fprint_natural(FILE *,const Stand *);

#endif
