/**************************************************************************************/
/**                                                                                \n**/
/**                     w o o d p l a n t a t i o n . h                            \n**/
/**                                                                                \n**/
/**     Declaration of woodplantation stand                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef WOODPLANTATION_H
#define WOODPLANTATION_H

extern Standtype woodplantation_stand;
extern const char *woodplantation_names[NWPTYPE];

extern Bool annual_woodplantation(Stand *, int,int,Real,int,Bool,Bool,const Config *);
extern Real daily_woodplantation(Stand *, Real, const Dailyclimate *, int,int,
                                 Real, 
                                 Real, Real, Real, Real,
                                 Real, int, int, int, Bool,Real, const Config *);
#endif
