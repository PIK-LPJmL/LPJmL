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

#include "lpj.h"

static void getminmax(const double vec[],int size,int nband,double v_min[],double v_max[],double v_avg[])
{
  int i,j;
  for(i=0;i<nband;i++)
  {
    v_avg[i]=v_min[i]=v_max[i]=vec[i];
  }
  for(i=1;i<size;i++)
    for(j=0;j<nband;j++)
    {
      if(v_min[j]>vec[i*nband+j])
       v_min[j]=vec[i*nband+j];
      if(v_max[j]<vec[i*nband+j])
       v_max[j]=vec[i*nband+j];
      v_avg[j]+=vec[i*nband+j];
    }
  for(j=0;j<nband;j++)
    v_avg[j]/=size;
} /* of 'getminmax' */

void fprinttiming(FILE *file,          /**< pointer to text file */
                  double total,        /**< Total running time (sec) */
                  const Config *config /**< LPJmL configuration */
                 )
{
#ifdef USE_MPI
  Timing *vec=NULL,t_min,t_max,t_avg;
  if(isroot(*config))
    vec=newvec(Timing,config->ntask);
  MPI_Gather(&timing,sizeof(Timing)/sizeof(double),MPI_DOUBLE,
             vec,sizeof(Timing)/sizeof(double),MPI_DOUBLE,0,config->comm);

  if(isroot(*config))
  {
    if(config->ntask>1)
    {
      getminmax((const double *)vec,config->ntask,sizeof(Timing)/sizeof(double),(double *)&t_min,(double *)&t_max,(double *)&t_avg);
      fprintf(file,"Function                  Tmin (sec) %%Total Tavg (sec) %%Total Tmax (sec) %%Total\n"
                   "------------------------- ---------- ------ ---------- ------ ---------- ------\n");
      fprintf(file,"drain                     %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.drain,t_min.drain/total*100,
              t_avg.drain,t_avg.drain/total*100,
              t_max.drain,t_max.drain/total*100);
      fprintf(file,"fopenoutput               %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.fopenoutput,t_min.fopenoutput/total*100,
              t_avg.fopenoutput,t_avg.fopenoutput/total*100,
              t_max.fopenoutput,t_max.fopenoutput/total*100);
      fprintf(file,"fwriteoutput              %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.fwriteoutput,t_min.fwriteoutput/total*100,
              t_avg.fwriteoutput,t_avg.fwriteoutput/total*100,
              t_max.fwriteoutput,t_max.fwriteoutput/total*100);
      fprintf(file,"fwriterestart             %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.fwriterestart,t_min.fwriterestart/total*100,
              t_avg.fwriterestart,t_avg.fwriterestart/total*100,
              t_max.fwriterestart,t_max.fwriterestart/total*100);
      fprintf(file,"getclimate                %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.getclimate,t_min.getclimate/total*100,
              t_avg.getclimate,t_avg.getclimate/total*100,
              t_max.getclimate,t_max.getclimate/total*100);
      fprintf(file,"initinput                 %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.initinput,t_min.initinput/total*100,
              t_avg.initinput,t_avg.initinput/total*100,
              t_max.initinput,t_max.initinput/total*100);
      fprintf(file,"initoutput                %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.initoutput,t_min.initoutput/total*100,
              t_avg.initoutput,t_avg.initoutput/total*100,
              t_max.initoutput,t_max.initoutput/total*100);
      fprintf(file,"irrig_amount_reservoir    %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.irrig_amount_reservoir,t_min.irrig_amount_reservoir/total*100,
              t_avg.irrig_amount_reservoir,t_avg.irrig_amount_reservoir/total*100,
              t_max.irrig_amount_reservoir,t_max.irrig_amount_reservoir/total*100);
      fprintf(file,"iterate                   %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.iterate,t_min.iterate/total*100,
              t_avg.iterate,t_avg.iterate/total*100,
              t_max.iterate,t_max.iterate/total*100);
      fprintf(file,"iterateyear               %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.iterateyear,t_min.iterateyear/total*100,
              t_avg.iterateyear,t_avg.iterateyear/total*100,
              t_max.iterateyear,t_max.iterateyear/total*100);
      fprintf(file,"littersom                 %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.littersom,t_min.littersom/total*100,
              t_avg.littersom,t_avg.littersom/total*100,
              t_max.littersom,t_max.littersom/total*100);
      fprintf(file,"MPI_Barrier               %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.barrier,t_min.barrier/total*100,
              t_avg.barrier,t_avg.barrier/total*100,
              t_max.barrier,t_max.barrier/total*100);
      fprintf(file,"MPI_Init                  %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.MPI_Init,t_min.MPI_Init/total*100,
              t_avg.MPI_Init,t_avg.MPI_Init/total*100,
              t_max.MPI_Init,t_max.MPI_Init/total*100);
      fprintf(file,"newgrid                   %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.newgrid,t_min.newgrid/total*100,
              t_avg.newgrid,t_avg.newgrid/total*100,
              t_max.newgrid,t_max.newgrid/total*100);
      fprintf(file,"pedotransfer              %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.pedotransfer,t_min.pedotransfer/total*100,
              t_avg.pedotransfer,t_avg.pedotransfer/total*100,
              t_max.pedotransfer,t_max.pedotransfer/total*100);
      fprintf(file,"readconfig                %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.readconfig,t_min.readconfig/total*100,
              t_avg.readconfig,t_avg.readconfig/total*100,
              t_max.readconfig,t_max.readconfig/total*100);
      if(t_max.read_socket>0)
        fprintf(file,"read_socket               %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
                t_min.read_socket,t_min.read_socket/total*100,
                t_avg.read_socket,t_avg.read_socket/total*100,
                t_max.read_socket,t_max.read_socket/total*100);
      fprintf(file,"setupannual_grid          %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.setupannual_grid,t_min.setupannual_grid/total*100,
              t_avg.setupannual_grid,t_avg.setupannual_grid/total*100,
              t_max.setupannual_grid,t_max.setupannual_grid/total*100);
      fprintf(file,"storeclimate              %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.storeclimate,t_min.storeclimate/total*100,
              t_avg.storeclimate,t_avg.storeclimate/total*100,
              t_max.storeclimate,t_max.storeclimate/total*100);
      fprintf(file,"update_daily_cell         %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.update_daily_cell,t_min.update_daily_cell/total*100,
              t_avg.update_daily_cell,t_avg.update_daily_cell/total*100,
              t_max.update_daily_cell,t_max.update_daily_cell/total*100);
      fprintf(file,"updatedaily_grid          %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.updatedaily_grid,t_min.updatedaily_grid/total*100,
              t_avg.updatedaily_grid,t_avg.updatedaily_grid/total*100,
              t_max.updatedaily_grid,t_max.updatedaily_grid/total*100);
      fprintf(file,"water_stressed            %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.water_stressed,t_min.water_stressed/total*100,
              t_avg.water_stressed,t_avg.water_stressed/total*100,
              t_max.water_stressed,t_max.water_stressed/total*100);
      fprintf(file,"wateruse                  %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.wateruse,t_min.wateruse/total*100,
              t_avg.wateruse,t_avg.wateruse/total*100,
              t_max.wateruse,t_max.wateruse/total*100);
      fprintf(file,"withdrawal_demand         %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
              t_min.withdrawal_demand,t_min.withdrawal_demand/total*100,
              t_avg.withdrawal_demand,t_avg.withdrawal_demand/total*100,
              t_max.withdrawal_demand,t_max.drain/total*100);
      if(t_max.write_socket>0)
        fprintf(file,"write_socket           %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
                t_min.write_socket,t_min.write_socket/total*100,
                t_avg.write_socket,t_avg.write_socket/total*100,
                t_max.write_socket,t_max.write_socket/total*100);
      fprintf(file,"------------------------- ---------- ------ ---------- ------ ----------- ------\n");
    }
    else
    {
#endif
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
  fprintf(file,"MPI_Barrier               %10.2f %6.1f\n",timing.barrier,timing.barrier/total*100);
  fprintf(file,"MPI_Init                  %10.2f %6.1f\n",timing.MPI_Init,timing.MPI_Init/total*100);
#endif
  fprintf(file,"newgrid                   %10.2f %6.1f\n",timing.newgrid,timing.newgrid/total*100);
  fprintf(file,"pedotransfer              %10.2f %6.1f\n",timing.pedotransfer,timing.pedotransfer/total*100);
  fprintf(file,"readconfig                %10.2f %6.1f\n",timing.readconfig,timing.readconfig/total*100);
  if(timing.read_socket>0)
    fprintf(file,"read_socket               %10.2f %6.1f\n",timing.read_socket,timing.read_socket/total*100);
  fprintf(file,"setupannual_grid          %10.2f %6.1f\n",timing.setupannual_grid,timing.setupannual_grid/total*100);
  fprintf(file,"storeclimate              %10.2f %6.1f\n",timing.storeclimate,timing.storeclimate/total*100);
  fprintf(file,"update_daily_cell         %10.2f %6.1f\n",timing.update_daily_cell,timing.update_daily_cell/total*100);
  fprintf(file,"updatedaily_grid          %10.2f %6.1f\n",timing.updatedaily_grid,timing.updatedaily_grid/total*100);
  fprintf(file,"water_stressed            %10.2f %6.1f\n",timing.water_stressed,timing.water_stressed/total*100);
  fprintf(file,"wateruse                  %10.2f %6.1f\n",timing.wateruse,timing.wateruse/total*100);
  fprintf(file,"withdrawal_demand         %10.2f %6.1f\n",timing.withdrawal_demand,timing.withdrawal_demand/total*100);
  if(timing.write_socket>0)
    fprintf(file,"write_socket           %10.2f %6.1f\n",timing.write_socket,timing.write_socket/total*100);
  fprintf(file,"------------------------- ---------- ------\n");
#ifdef USE_MPI
    }
    free(vec);
  }
#endif
} /* of 'fprinttiming' */
