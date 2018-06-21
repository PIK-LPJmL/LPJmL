/**************************************************************************************/
/**                                                                                \n**/
/**                   i  n  p  u  t  .  h                                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef INPUT_H
#define INPUT_H

/* Definition of datatypes */

typedef struct
{
  Climate *climate;
  Landuse landuse;
  Wateruse wateruse;
  Popdens popdens;
  Landcover landcover;
} Input;

/* Declaration of functions */

extern Bool initinput(Input *,const Cell *,int,int,const Config *);
extern void freeinput(Input,Bool);

#endif
