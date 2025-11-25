/**************************************************************************************/
/**                                                                                \n**/
/**                      t  i  m  i  n  g  .  c                                    \n**/
/**                                                                                \n**/
/**     Declaration of global variable timing and timing_fcn                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_TIMING

/* Global variable for timing */

double timing[N_FCN]={};

char *timing_fcn[N_FCN]=
{
  "daily_littersom",
  "drain",
  "fopenoutput",
  "fwriterestart",
  "fwriteoutput",
  "gasdiffusion",
  "getclimate",
  "initinput",
  "initoutput",
  "irrig_amount_reservoir",
  "MPI_Barrier",
  "MPI_Init",
  "newgrid",
  "pedotransfer",
  "readconfig",
  "read_socket",
  "setupannual_grid",
  "storeclimate",
  "update_daily_cell",
  "updatedaily_grid",
  "water_stressed",
  "wateruse",
  "withdrawal_demand",
  "write_socket",
};

#endif /* of USE_TIMING */
