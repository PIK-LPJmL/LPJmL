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

#ifndef AGRICULTURE_H
#define AGRICULTURE_H

extern Standtype agriculture_stand;
extern Standtype managedforest_stand;
extern Standtype setaside_rf_stand;
extern Standtype setaside_ir_stand;
extern Standtype kill_stand;

extern Real daily_agriculture(Stand *,Real,const Dailyclimate *,int,int,
                              Real,const Real [],Real,
                              Real,Real,Real,Real,Real,Real,int,int,int,
                              Bool,Real,const Config *);
extern Bool annual_agriculture(Stand *,int,int,Real,int,Bool,Bool,const Config *);

extern Bool annual_setaside(Stand *,int,int,Real,int,Bool,Bool,const Config *);

extern Real daily_setaside(Stand *,Real,const Dailyclimate *,int,int,
                           Real,const Real [],Real,
                           Real,Real,Real,Real,Real,Real,int,int,int,
                           Bool,Real,const Config *);

extern Bool isdailyoutput_agriculture(const Output *,const Stand *);

extern void output_gbw_agriculture(Output *,const Stand *,Real,Real,Real,Real,
                                   const Real[LASTLAYER],const Real[LASTLAYER],Real,Real,int,int,
                                   Bool);
extern void new_agriculture(Stand *);
extern void free_agriculture(Stand *);
extern Bool fread_agriculture(FILE *,Stand *,Bool);
extern Bool fwrite_agriculture(FILE *,const Stand *);
extern void fprint_agriculture(FILE *,const Stand *);
#endif
