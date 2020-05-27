/**************************************************************************************/
/**                                                                                \n**/
/**  r  e  c  e  i  v  e  _  i  m  a  g  e  _  c  l  i  m  a  t  e  .  c           \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Function to receive climate data from IMAGE                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined IMAGE && defined COUPLED

Bool receive_image_climate(Climate *climate,    /**< Climate data */
                           int year,            /**< year (AD) */
                           const Config *config /**< LPJ configuration */
                          )                     /**< returns TRUE on error */
{
  short *vec;
  int i;

  int index;
#ifdef USE_MPI
  int *counts,*offsets;
  int n;
#endif
  float *image_data;
  vec=newvec(short,climate->file_temp_var.n);
  check(vec);
  /* read in data */
  index=year-climate->file_temp_var.firstyear;
  /* IMAGE temperature consists of CRU variability (absolute) and IMAGE temperatures */
  if(fseek(climate->file_temp_var.file,
         (long)(index*climate->file_temp_var.size+climate->file_temp_var.offset),SEEK_SET))
  {
    fprintf(stderr,"ERROR190: seeking temperature variability of year %d in receive_image_climate().\n",year);
    fflush(stderr);
    free(vec);
    return TRUE;
  }
  if(fread(vec,sizeof(short),climate->file_temp_var.n,climate->file_temp_var.file)!=climate->file_temp_var.n)
  {
    fprintf(stderr,"ERROR190: reading temperature variability of year %d in receive_image_climate().\n",year);
    fflush(stderr);
    free(vec);
    return TRUE;
  }
  if(climate->file_temp_var.swap)
    for(i=0;i<climate->file_temp_var.n;i++)
#ifdef IMAGE_CLIM_AVG
      climate->data.temp[i]=0; /* setting varibility to 0 */
#else
      climate->data.temp[i]=swapshort(vec[i])*0.1; /* temp variability in deg C *10 */
#endif
  else
    for(i=0;i<climate->file_temp_var.n;i++)
#ifdef IMAGE_CLIM_AVG
      climate->data.temp[i]=0; /* setting varibility to 0 */
#else
      climate->data.temp[i]=vec[i]*0.1; /* temp variability in deg C *10 */
#endif
  if(climate->data.temp[0]<-70 || climate->data.temp[0]>70){
    fprintf(stderr,"tempvar incorrect %g\n",climate->data.temp[0]);
    fflush(stderr);
  }
    
  image_data = newvec(float,climate->file_temp_var.n);
  check(image_data);
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  n=(climate->file_temp.isdaily) ? NDAYYEAR : NMONTH;
  getcounts(counts,offsets,config->nall,n,config->ntask);
  mpi_read_socket(config->in,image_data,MPI_FLOAT,n*config->nall,counts,
                 offsets,config->rank,config->comm);
#else
#ifdef DEBUG_IMAGE
  printf("getting temperature data, size should be %d\n",climate->file_temp_var.n);
  fflush(stdout);
#endif
  readfloat_socket(config->in,image_data,climate->file_temp_var.n);
#endif

  /* adding IMAGE temperature to absolute temperature variability */
  for(i=0;i<climate->file_temp_var.n;i++)
  {
    climate->data.temp[i]+=(Real)image_data[i];
#ifdef DEBUG_IMAGE
    if((i>=1*12&&i<2*12) || (i>=67032*12 && i<67033*12))
    {
      printf("IMAGE temp in pix %d in month %d: %g\n",(int)i/12,i%12+1,image_data[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data);

  /* IMAGE precipitation consists of CRU variability (%) and IMAGE prec */
  index=year-climate->file_prec_var.firstyear;
  if(fseek(climate->file_prec_var.file,
        (long)(index*climate->file_prec_var.size+climate->file_prec_var.offset),SEEK_SET))
  {
    fprintf(stderr,"ERROR189: seeking precipitation variability of year %d in receive_image_climate().\n",year);
    fflush(stderr);
    free(vec);
    return TRUE;
  }
  if(fread(vec,sizeof(short),climate->file_prec_var.n,climate->file_prec_var.file)!=climate->file_prec_var.n)
  {
    fprintf(stderr,"ERROR189: reading precipitation variability of year %d in receive_image_climate().\n",year);
    fflush(stderr);
    free(vec);
    return TRUE;
  }
  if(climate->file_prec_var.swap)
    for(i=0;i<climate->file_prec_var.n;i++)
#ifdef IMAGE_CLIM_AVG
      climate->data.prec[i]=1; /* setting relative varibility to 1 */
#else
      climate->data.prec[i]=swapshort(vec[i])*0.01; /* prec variability in percent */
#endif
  else
    for(i=0;i<climate->file_prec_var.n;i++)
#ifdef IMAGE_CLIM_AVG
      climate->data.prec[i]=1; /* setting relative varibility to 1 */
#else
      climate->data.prec[i]=vec[i]*0.01; /* prec variability in percent */
#endif
  if(climate->data.prec[0]<0 || climate->data.prec[0]>1000)
  {
    fprintf(stderr,"ERROR174: precvar incorrect %g\n",climate->data.prec[0]);
    fflush(stderr);
  }
   
  image_data = newvec(float,climate->file_prec_var.n);
  check(image_data);
#ifdef USE_MPI
  mpi_read_socket(config->in,image_data,MPI_FLOAT,n*config->nall,counts,
                 offsets,config->rank,config->comm);
#else
#ifdef DEBUG_IMAGE
  printf("getting precip data, size should be %d\n",climate->file_prec_var.n);
  fflush(stdout);
#endif
  readfloat_socket(config->in,image_data,climate->file_prec_var.n);
#endif
 
  for(i=0;i<climate->file_prec_var.n;i++)
  {
    /* IMAGE precipitation data are supplied as mm/day monthly averages */
    climate->data.prec[i]*=(Real)image_data[i]*ndaymonth[i % NMONTH];
#ifdef DEBUG_IMAGE
    if((i>=1*12&&i<2*12) || (i>=67032*12 && i<67033*12))
    {
      printf("IMAGE prec in pix %d in month %d: %g\n",(int)i/12,i%12+1,image_data[i]);
      fflush(stdout);
    }
#endif
  }

  free(vec);
  free(image_data);
  
  image_data = newvec(float,climate->file_cloud.n);
  check(image_data);
#ifdef USE_MPI
  mpi_read_socket(config->in,image_data,MPI_FLOAT,n*config->nall,counts,
                 offsets,config->rank,config->comm);
#else
#ifdef DEBUG_IMAGE
  printf("getting cloudiness data, size should be %d\n",climate->file_cloud.n);
  fflush(stdout);
#endif
  readfloat_socket(config->in,image_data,climate->file_cloud.n);
#endif

  /* assigning IMAGE cloudiness */
  for(i=0;i<climate->file_cloud.n;i++)
  {
    climate->data.sun[i]=100-(Real)image_data[i]*100.0;
#ifdef DEBUG_IMAGE
    if((i>=1*12&&i<2*12) || (i>=67032*12 && i<67033*12))
    {
      printf("IMAGE cloud in pix %d in month %d: %g\n",(int)i/12,i%12+1,image_data[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data);

  image_data = newvec(float,climate->file_wet.n);
  check(image_data);
#ifdef USE_MPI
  mpi_read_socket(config->in,image_data,MPI_FLOAT,n*config->nall,counts,
                 offsets,config->rank,config->comm);
  free(counts);
  free(offsets);
#else
#ifdef DEBUG_IMAGE
  printf("getting wet day data, size should be %d\n",climate->file_wet.n);
  fflush(stdout);
#endif
  readfloat_socket(config->in,image_data,climate->file_wet.n);
#endif

  /* assigning IMAGE number of wet days */
  if(israndomprec(climate))
    for(i=0;i<climate->file_wet.n;i++)
    {
      climate->data.wet[i]=(Real)image_data[i]*ndaymonth[i % NMONTH];
#ifdef DEBUG_IMAGE
      if((i>=1*12&&i<2*12) || (i>=67032*12 && i<67033*12))
      {
        printf("IMAGE wet in pix %d in month %d: %g\n",(int)i/12,i%12+1,image_data[i]);
        fflush(stdout);
      }
#endif
    }
  free(image_data);

  return FALSE;
} /* of 'receive_image_climate' */

#endif
