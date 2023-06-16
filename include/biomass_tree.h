/**************************************************************************************/
/**                                                                                \n**/
/**                     b  i  o  m  a  s  s  _  t  r  e  e  .  h                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Declaration of biomass tree stand                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef BIOMASS_TREE_H
#define BIOMASS_TREE_H

typedef struct
{
  Irrigation irrigation; /**< irrigation data */
  int growing_time;      /**< years since harvest*/
  int age;               /**< plantation age (yr) */
} Biomass_tree;

extern Standtype biomass_tree_stand;

extern Bool annual_biomass_tree(Stand *,int, int,Real,int,Bool,Bool,const Config *);
extern Real daily_biomass_tree(Stand *,Real,const Dailyclimate *, int,int,
                               Real,
                               Real,Real,Real,Real,
                               Real ,int, int, int,Bool,Real,const Config *);
extern void new_biomass_tree(Stand *);
extern Bool fwrite_biomass_tree(FILE *,const Stand *);
extern void fprint_biomass_tree(FILE *,const Stand *,const Pftpar *);
extern Bool fread_biomass_tree(FILE *,Stand *,Bool);

#endif
