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

#ifdef USE_TIMING

#ifdef USE_MPI
static void getminmax(const double vec[],int size,int nband,double v_min[],
                      double v_max[],double v_avg[])
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
#endif

void fprinttiming(FILE *file,          /**< pointer to text file */
                  double total,        /**< Total running time (sec) */
                  const Config *config /**< LPJmL configuration */
                 )
{
  int i;
#ifdef USE_MPI
  double *vec=NULL,*t_min,*t_max,*t_avg;
  if(isroot(*config))
  {
    vec=newvec(double,N_FCN*config->ntask);
  }
  MPI_Gather(timing,N_FCN,MPI_DOUBLE,
             vec,N_FCN,MPI_DOUBLE,0,config->comm);

  if(isroot(*config))
  {
    fprintf(file,"\nLPJmL performance summary\n\n");
    if(config->ntask>1)
    {
      t_min=newvec(double,N_FCN);
      t_max=newvec(double,N_FCN);
      t_avg=newvec(double,N_FCN);
      getminmax((const double *)vec,config->ntask,N_FCN,t_min,t_max,t_avg);
      fprintf(file,"Function                  Tmin (sec) %%Total Tavg (sec) %%Total Tmax (sec) %%Total\n"
              "------------------------- ---------- ------ ---------- ------ ---------- ------\n");
      for(i=0;i<N_FCN;i++)
        fprintf(file,"%-25s %10.2f %6.1f %10.2f %6.1f %10.2f %6.1f\n",
                timing_fcn[i],t_min[i],t_min[i]/total*100,t_avg[i],t_avg[i]/total*100,t_max[i],t_max[i]/total*100);
      fprintf(file,"------------------------- ---------- ------ ---------- ------ ---------- ------\n");
      free(t_min);
      free(t_avg);
      free(t_max);
    }
    else
    {
#else
  fprintf(file,"\nLPJmL performance summary\n\n");
#endif
  fprintf(file,"Function                  Time (sec) %%Total\n"
               "------------------------- ---------- ------\n");
  for(i=0;i<N_FCN;i++)
    fprintf(file,"%-25s %10.2f %6.1f\n",timing_fcn[i],timing[i],timing[i]/total*100);
  fprintf(file,"------------------------- ---------- ------\n");
#ifdef USE_MPI
    }
    free(vec);
  }
#endif
} /* of 'fprinttiming' */

#endif /* of USE_TIMING */
