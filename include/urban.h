/**************************************************************************************/
/**                                                                                \n**/
/**                  a  g  r  i  c  u  l  t  u  r  e  .  h                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of agriculture stand                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef URBAN_H
#define URBAN_H

extern Standtype urban_stand;
extern Bool annual_urban(Stand *,int,int,Real,int,Bool,Bool,const Config *);
#endif
