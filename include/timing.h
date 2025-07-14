/**************************************************************************************/
/**                                                                                \n**/
/**                     t  i  m  i  n  g  . h                                      \n**/
/**                                                                                \n**/
/**     Definition of performance timing datatype                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef TIMING_H
#define TIMING_H

typedef struct
{
  double drain;
  double fopenoutput;
  double fwriterestart;
  double fwriteoutput;
  double getclimate;
  double initinput;
  double initoutput;
  double irrig_amount_reservoir;
  double iterate;
  double iterateyear;
  double MPI_Init;
  double newgrid;
  double readconfig;
  double read_socket;
  double setupannual_grid;
  double storeclimate;
  double wateruse;
  double withdrawal_demand;
  double write_socket;
} Timing;

extern Timing timing;

void fprinttiming(FILE *file,double);

#define printtiming(total) fprinttiming(stdout,total)
#endif
