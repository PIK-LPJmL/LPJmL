/**************************************************************************************/
/**                                                                                \n**/
/**         a  g  r  i  c  u  l  t  u  r  e  _  t  r  e  e  .  h                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJ                                                    \n**/
/**                                                                                \n**/
/**     Declaration of agricultural tree stand                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef AGRICULTURE_TREE_H
#define AGRICULTURE_TREE_H

extern Standtype agriculture_tree_stand;

extern Real daily_agriculture_tree(Stand *,Real,const Dailyclimate *,int,int,
                                   Real,
                                   Real,Real,Real,Real,Real,int,int,int,
                                   Bool,Real,const Config *);

extern Bool annual_agriculture_tree(Stand *,int,int,Real,int,Bool,Bool,const Config *);

extern void output_gbw_agriculture_tree(Output *,const Stand *,Real,Real,Real,Real,
                                        const Real[LASTLAYER],const Real[LASTLAYER],Real,Real,int,int,
                                        const Config *);

#endif
