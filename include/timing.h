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

/* Definition of datatypes */

typedef enum
{
  DRAIN_FCN,
  FOPENOUTPUT_FCN,
  FWRITERESTART_FCN,
  FWRITEOUTPUT_FCN,
  GETCLIMATE_FCN,
  INITINPUT_FCN,
  INITOUTPUT_FCN,
  IRRIG_AMOUNT_RESERVOIR_FCN,
  ITERATE_FCN,
  ITERATEYEAR_FCN,
  LITTERSOM_FCN,
  MPI_BARRIER_FCN,
  MPI_INIT_FCN,
  NEWGRID_FCN,
  PEDOTRANSFER_FCN,
  READCONFIG_FCN,
  READ_SOCKET_FCN,
  SETUPANNUAL_GRID_FCN,
  STORECLIMATE_FCN,
  UPDATE_DAILY_CELL_FCN,
  UPDATEDAILY_GRID_FCN,
  WATER_STRESSED_FCN,
  WATERUSE_FCN,
  WITHDRAWAL_DEMAND_FCN,
  WRITE_SOCKET_FCN,
  N_FCN
} Timing_id;

/* Declaration of variables */

extern double timing[N_FCN];
extern char *timing_fcn[N_FCN];

/* Definition of macros */

#define timing_start(t) t=mrun()
#define timing_stop(id,t) timing[id]+=mrun()-t

#endif
