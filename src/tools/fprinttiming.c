/**************************************************************************************/
/**                                                                                \n**/
/**                     f  p  r  i  n  t  t  i  m  i  n  g  .  c                   \n**/
/**                                                                                \n**/
/**     Function prints timing of individual functions of LPJmL                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include "timing.h"

Timing timing={}; /* Global variable for timing */

void fprinttiming(FILE *file,  /**< pointer to text file */
                  double total /**< Total running time (sec) */
                 )
{
  fprintf(file,"Function                  Time (sec) %%Total\n"
               "------------------------- ---------- ------\n");
  fprintf(file,"drain                     %10.2f %6.1f\n",timing.drain,timing.drain/total*100);
  fprintf(file,"fopenoutput               %10.2f %6.1f\n",timing.fopenoutput,timing.fopenoutput/total*100);
  fprintf(file,"fwriteoutput              %10.2f %6.1f\n",timing.fwriteoutput,timing.fwriteoutput/total*100);
  fprintf(file,"fwriterestart             %10.2f %6.1f\n",timing.fwriterestart,timing.fwriterestart/total*100);
  fprintf(file,"getclimate                %10.2f %6.1f\n",timing.getclimate,timing.getclimate/total*100);
  fprintf(file,"initinput                 %10.2f %6.1f\n",timing.initinput,timing.initinput/total*100);
  fprintf(file,"initoutput                %10.2f %6.1f\n",timing.initoutput,timing.initoutput/total*100);
  fprintf(file,"irrig_amount_reservoir    %10.2f %6.1f\n",timing.irrig_amount_reservoir,timing.irrig_amount_reservoir/total*100);
  fprintf(file,"iterate                   %10.2f %6.1f\n",timing.iterate,timing.iterate/total*100);
  fprintf(file,"iterateyear               %10.2f %6.1f\n",timing.iterateyear,timing.iterateyear/total*100);
  fprintf(file,"littersom                 %10.2f %6.1f\n",timing.littersom,timing.littersom/total*100);
#ifdef USE_MPI
  fprintf(file,"MPI_Init                  %10.2f %6.1f\n",timing.MPI_Init,timing.MPI_Init/total*100);
#endif
  fprintf(file,"newgrid                   %10.2f %6.1f\n",timing.newgrid,timing.newgrid/total*100);
  fprintf(file,"readconfig                %10.2f %6.1f\n",timing.readconfig,timing.readconfig/total*100);
  if(timing.read_socket>0)
    fprintf(file,"read_socket               %10.2f %6.1f\n",timing.read_socket,timing.read_socket/total*100);
  fprintf(file,"update_soil_thermal_state %10.2f %6.1f\n",timing.update_soil_thermal_state,timing.update_soil_thermal_state/total*100);
  fprintf(file,"setupannual_grid          %10.2f %6.1f\n",timing.setupannual_grid,timing.setupannual_grid/total*100);
  fprintf(file,"storeclimate              %10.2f %6.1f\n",timing.storeclimate,timing.storeclimate/total*100);
  fprintf(file,"wateruse                  %10.2f %6.1f\n",timing.wateruse,timing.wateruse/total*100);
  fprintf(file,"withdrawal_demand         %10.2f %6.1f\n",timing.withdrawal_demand,timing.withdrawal_demand/total*100);
  if(timing.write_socket>0)
    fprintf(file,"write_socket           %10.2f %6.1f\n",timing.write_socket,timing.write_socket/total*100);
  fprintf(file,"------------------------- ---------- ------\n");
} /* of 'fprinttiming' */
