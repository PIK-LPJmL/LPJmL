/**************************************************************************************/
/**                                                                                \n**/
/**                     g  r  a  s  s  l  a  n  d  .  h                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of grassland stand                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef GRASSLAND_H
#define GRASSLAND_H

/* Definition of settings */

extern Standtype grassland_stand;

extern Real daily_grassland(Stand *,Real,const Dailyclimate *, int,
                            Real,const Real [],
                            Real ,Real,Real,Real,Real,Real,
                            Real ,int, int, int,  Bool,Bool,const Config *);
extern Bool annual_grassland(Stand *,int,int,Real,int,Bool,Bool,const Config *);
extern void output_gbw_grassland(Output *,const Stand *,Real,Real,Real,Real,
                                 const Real [LASTLAYER],const Real [LASTLAYER],Real,Real,int,
                                 Bool);

#endif
