/**************************************************************************************/
/**                                                                                \n**/
/**        r  e  c  e  i  v  e  _  i  m  a  g  e  _  l  u  c  .  c                 \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     function to read in image land-use data                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef IMAGE

#define NIMAGECROPS (sizeof(Image_landuse)/sizeof(float))

static void reducelandfrac(Cell *cell,Real cropsum,int ncft)
{
  int i,j;
  for(i=0;i<2;i++)
  {
    for(j=0;j<ncft;j++)
      if(cell->ml.landfrac[i].crop[j]>=cropsum-1)
      {
#ifdef SAFE
        printf("reducing landfrac.crop[%d] from %f to %f in cell %g/%g, cropsum %g\n",j,
               cell->ml.landfrac[i].crop[j],
               cell->ml.landfrac[i].crop[j]-cropsum+1., cell->coord.lon,cell->coord.lat,
               cropsum);

#endif

        cell->ml.landfrac[i].crop[j]-=cropsum-1;
        cropsum=1;
        return;
      }
      else
      {
#ifdef SAFE
        printf("reducing landfrac.crop[%d] from %f to 0 in cell %g/%g, cropsum %g\n",j,
               cell->ml.landfrac[i].crop[j],cell->coord.lon,cell->coord.lat,
               cropsum);
#endif
        cropsum-=cell->ml.landfrac[i].crop[j];
        cell->ml.landfrac[i].crop[j]=0;
      }
    for(j=0;j<NGRASS;j++)
      if(cell->ml.landfrac[i].grass[j]>=cropsum-1)
      {
#ifdef SAFE
        printf("reducing landfrac.grass[%d] from %f to %f in cell %g/%g, cropsum %g\n",j,
               cell->ml.landfrac[i].grass[j],
               cell->ml.landfrac[i].grass[j]-cropsum+1., cell->coord.lon,cell->coord.lat,
               cropsum);
#endif

        cell->ml.landfrac[i].grass[j]-=cropsum-1;
        cropsum=1;
        return;
      }
      else
      {
#ifdef SAFE
        printf("reducing landfrac.grass[%d] from %f to 0 in cell %g/%g, cropsum %g\n",j,
               cell->ml.landfrac[i].grass[j], cell->coord.lon,cell->coord.lat,
               cropsum);
#endif
        cropsum-=cell->ml.landfrac[i].grass[j];
        cell->ml.landfrac[i].grass[j]=0;
      }
  }
}

Bool receive_image_luc(Cell *grid,          /* LPJ grid */
                       int npft,            /* number of natural PFTs */
                       int ncft,            /* number of crop PFTs */
                       const Config *config /* Grid configuration */
                      )                     /* returns TRUE on error */
{
  int i;
  Real cropsum;
#ifdef DEBUG_IMAGE
  float sum;
#endif
  Image_landuse *image_landuse;
  float *image_data;
  int *image_data_int;
#ifdef USE_MPI
  int *counts,*offsets;
#endif

  /* get timber harvest shares from IMAGE */
  image_data = newvec(float,config->ngridcell);
  check(image_data);
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,1,config->ntask);
  if(mpi_read_socket(config->in,image_data,MPI_FLOAT,config->nall,counts,
                     offsets,config->rank,config->comm))
  {
    free(offsets);
    free(counts);
    free(image_data);
    return TRUE;
  }
#else
#ifdef DEBUG_IMAGE
  printf("getting timber harvest shares\n");
  fflush(stdout);
#endif
  if(readfloat_socket(config->in, image_data, config->ngridcell))
  {
    free(image_data);
    return TRUE;
  }
#endif
#ifdef DEBUG_IMAGE
  printf("assigning timber harvest shares\n");
  fflush(stdout);
#endif
  for(i=0;i<config->ngridcell;i++)
  {
    if(!grid[i].skip)
    {
      grid[i].ml.image_data->timber_frac=(Real)image_data[i];
    }
#ifdef DEBUG_IMAGE
    if(grid[i].coord.lon>-2.5 && grid[i].coord.lon<-2.0 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)/*(i==1 || i==67032)*/
    {
      printf("cell %d %g/%g\n",i,grid[i].coord.lon,grid[i].coord.lat);
      fflush(stdout);
      printf("th[%d]:%g %g\n",i,grid[i].ml.image_data->timber_frac,image_data[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data);

  /* get timber burnt shares from IMAGE */
  image_data_int = newvec(int,config->ngridcell);
  check(image_data_int);
#ifdef USE_MPI
  if(mpi_read_socket(config->in,image_data_int,MPI_INT,config->nall,counts,
                     offsets,config->rank,config->comm))
  {
    free(offsets);
    free(counts);
    free(image_data_int);
    return TRUE;
  }
#else
#ifdef DEBUG_IMAGE
  printf("getting timber burnt shares\n");
  fflush(stdout);
#endif
  if(readint_socket(config->in,image_data_int,config->ngridcell))
  {
    free(image_data_int);
    return TRUE;
  }
#endif
#ifdef DEBUG_IMAGE
  printf("assigning timber burnt shares\n");
  fflush(stdout);
#endif
  for(i=0;i<config->ngridcell;i++)
  {
    if(!grid[i].skip)
    {
      grid[i].ml.image_data->fburnt=(Real)image_data_int[i];
    }
#ifdef DEBUG_IMAGE
    if(grid[i].coord.lon>-2.5 && grid[i].coord.lon<-2.0 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)/*(i==1 || i==67032)*/
    {
      printf("cell %d %g/%g\n",i,grid[i].coord.lon,grid[i].coord.lat);
      fflush(stdout);
      printf("fb[%d]:%g %d\n",i,grid[i].ml.image_data->fburnt,image_data_int[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data_int);

  /* get land-use shares from IMAGE */
  image_landuse = newvec(Image_landuse,config->ngridcell);
  check(image_landuse);
#ifdef USE_MPI
  getcounts(counts,offsets,config->nall,NIMAGECROPS,config->ntask);
  if(mpi_read_socket(config->in,image_landuse,MPI_FLOAT,config->nall*NIMAGECROPS,
                     counts,offsets,config->rank,config->comm))
  {
    free(offsets);
    free(counts);
    free(image_landuse);
    return TRUE;
  }
  free(offsets);
  free(counts);
#else
#ifdef DEBUG_IMAGE
  printf("getting crop shares\n");
  fflush(stdout);
#endif
  if(readfloat_socket(config->in,(float *)image_landuse,config->ngridcell*NIMAGECROPS))
  {
    free(image_landuse);
    return TRUE;
  }
#endif
#ifdef DEBUG_IMAGE
  printf("assigning IMAGE crop groups to LPJmL CFTs\n");
  fflush(stdout);
#endif
  for(i=0;i<config->ngridcell;i++)
  {
#ifdef DEBUG_IMAGE
    if(grid[i].coord.lon>-2.5 && grid[i].coord.lon<-2.0 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)
    sum=0;
    if(!grid[i].skip)
    {
      sum=image_landuse[i].irrig_maize+
        image_landuse[i].irrig_oil_crops+
        image_landuse[i].irrig_pulses+
        image_landuse[i].irrig_rice+
        image_landuse[i].irrig_roots_tubers+
        image_landuse[i].irrig_temp_cereals+
        image_landuse[i].irrig_trop_cereals+
        image_landuse[i].maize+
        image_landuse[i].maize_feed+
        image_landuse[i].non_woody_biofuels+
        image_landuse[i].oil_crops+
        image_landuse[i].pasture+
        image_landuse[i].pulses+
        image_landuse[i].rice+
        image_landuse[i].roots_tubers+
        image_landuse[i].sugar_cane+
        image_landuse[i].temp_cereals+
        image_landuse[i].trop_cereals+
        image_landuse[i].woody_biofuels;
    }
#endif
    if(!grid[i].skip)
    {
#ifdef DEBUG_IMAGE
      printf("doing cell %d\n",i);
      fflush(stdout);
#endif
      grid[i].ml.landfrac[0].crop[TEMPERATE_CEREALS-npft]=(Real)image_landuse[i].temp_cereals;
      grid[i].ml.landfrac[0].crop[RICE-npft]=(Real)image_landuse[i].rice;
      grid[i].ml.landfrac[0].crop[MAIZE-npft]=(Real)image_landuse[i].maize+
        (Real)image_landuse[i].maize_feed;
      grid[i].ml.landfrac[0].biomass_grass=(Real)image_landuse[i].non_woody_biofuels;
      grid[i].ml.landfrac[0].crop[SUGARCANE-npft]=(Real)image_landuse[i].sugar_cane;
      grid[i].ml.landfrac[0].crop[TROPICAL_CEREALS-npft]=(Real)image_landuse[i].trop_cereals;
      grid[i].ml.landfrac[0].crop[PULSES-npft]=(Real)image_landuse[i].pulses;
      if(grid[i].coord.lat>30.0 || grid[i].coord.lat<-30.0)
      {
        grid[i].ml.landfrac[0].crop[TEMPERATE_ROOTS-npft]=(Real)image_landuse[i].roots_tubers;
        grid[i].ml.landfrac[0].crop[TROPICAL_ROOTS-npft]=0;
      }
      else
      {
        grid[i].ml.landfrac[0].crop[TEMPERATE_ROOTS-npft]=0;
        grid[i].ml.landfrac[0].crop[TROPICAL_ROOTS-npft]=(Real)image_landuse[i].roots_tubers;
      }
      if(grid[i].coord.lat>30.0 || grid[i].coord.lat<-30.0)
      {
        /* sunflower */
        grid[i].ml.landfrac[0].crop[OIL_CROPS_SUNFLOWER-npft]=(Real)image_landuse[i].oil_crops*0.5;
        /* rapeseed */
        grid[i].ml.landfrac[0].crop[OIL_CROPS_RAPESEED-npft]=(Real)image_landuse[i].oil_crops*0.5;
        /* soybean */
        grid[i].ml.landfrac[0].crop[OIL_CROPS_SOYBEAN-npft]=0;
        /* groundnut */
        grid[i].ml.landfrac[0].crop[OIL_CROPS_GROUNDNUT-npft]=0;
      }
      else
      {
        grid[i].ml.landfrac[0].crop[OIL_CROPS_SUNFLOWER-npft]=0;
        grid[i].ml.landfrac[0].crop[OIL_CROPS_RAPESEED-npft]=0;
        grid[i].ml.landfrac[0].crop[OIL_CROPS_SOYBEAN-npft]=(Real)image_landuse[i].oil_crops*0.5;
        grid[i].ml.landfrac[0].crop[OIL_CROPS_GROUNDNUT-npft]=(Real)image_landuse[i].oil_crops*0.5;
      }
      grid[i].ml.landfrac[0].grass[0]=(Real)image_landuse[i].pasture;
      /* no "others" */
      grid[i].ml.landfrac[0].grass[1]=0;
      /* no bioenergy trees so far */
      grid[i].ml.landfrac[0].biomass_tree=0;
#ifdef DEBUG_IMAGE
      printf("done with rainfed in %d\n",i);
      fflush(stdout);
#endif
      /* irrigated crops */
      grid[i].ml.landfrac[1].crop[TEMPERATE_CEREALS-npft]=(Real)image_landuse[i].irrig_temp_cereals;
      grid[i].ml.landfrac[1].crop[RICE-npft]=(Real)image_landuse[i].irrig_rice;
      grid[i].ml.landfrac[1].crop[MAIZE-npft]=(Real)image_landuse[i].irrig_maize;
      grid[i].ml.landfrac[1].crop[TROPICAL_CEREALS-npft]=(Real)image_landuse[i].irrig_trop_cereals;
      grid[i].ml.landfrac[1].crop[PULSES-npft]=(Real)image_landuse[i].irrig_pulses;
      if(grid[i].coord.lat>30.0 || grid[i].coord.lat<-30.0)
      {
        grid[i].ml.landfrac[1].crop[TEMPERATE_ROOTS-npft]=(Real)image_landuse[i].irrig_roots_tubers;
        grid[i].ml.landfrac[1].crop[TROPICAL_ROOTS-npft]=0;
      }
      else
      {
        grid[i].ml.landfrac[1].crop[TEMPERATE_ROOTS-npft]=0;
        grid[i].ml.landfrac[1].crop[TROPICAL_ROOTS-npft]=(Real)image_landuse[i].irrig_roots_tubers;
      }
      if(grid[i].coord.lat>30.0 || grid[i].coord.lat<-30.0)
      {
        grid[i].ml.landfrac[1].crop[OIL_CROPS_SUNFLOWER-npft]=(Real)image_landuse[i].irrig_oil_crops*0.5;
        grid[i].ml.landfrac[1].crop[OIL_CROPS_SOYBEAN-npft]=(Real)image_landuse[i].irrig_oil_crops*0.5;
        grid[i].ml.landfrac[1].crop[OIL_CROPS_GROUNDNUT-npft]=0;
        grid[i].ml.landfrac[1].crop[OIL_CROPS_RAPESEED-npft]=0;
      }
      else
      {
        grid[i].ml.landfrac[1].crop[OIL_CROPS_SUNFLOWER-npft]=0;
        grid[i].ml.landfrac[1].crop[OIL_CROPS_SOYBEAN-npft]=0;
        grid[i].ml.landfrac[1].crop[OIL_CROPS_GROUNDNUT-npft]=(Real)image_landuse[i].irrig_oil_crops*0.5;
        grid[i].ml.landfrac[1].crop[OIL_CROPS_RAPESEED-npft]=(Real)image_landuse[i].irrig_oil_crops*0.5;
      }
      /* no irrigated pasture */
      grid[i].ml.landfrac[1].grass[0]=0.0;
      /* no "others" */
      grid[i].ml.landfrac[1].grass[1]=0;
#ifdef DEBUG_IMAGE
      printf("done with irrig19 in %d\n",i);
      fflush(stdout);
#endif
      /* woody biofuels treated as timber harvest */
      grid[i].ml.landfrac[0].biomass_tree=(Real)image_landuse[i].woody_biofuels;
#ifdef DEBUG_IMAGE
      printf("done with assigning in %d, now reducing too large fractions\n",i);
      fflush(stdout);
#endif
      if(grid[i].ml.image_data->timber_frac>1.0)
      {
#ifdef SAFE
        fprintf(stderr,"WARNING015: timber fraction too large: %f in cell %d (%g/%g) as woody biofuels (%f) have been added.\n",
          grid[i].ml.image_data->timber_frac,i,grid[i].coord.lon,grid[i].coord.lat,
          (Real)image_landuse[i].woody_biofuels);
        fflush(stderr);
#endif
        grid[i].ml.image_data->timber_frac=1.0;
      }
      /* checking sum of crop fractions */
#ifdef DEBUG_IMAGE
      printf("done with reducing timber in %d, now reducing too large fractions\n",i);
      fflush(stdout);
#endif
      cropsum=landfrac_sum(grid[i].ml.landfrac,ncft,FALSE)+landfrac_sum(grid[i].ml.landfrac,ncft,TRUE);
      if(cropsum>(1.0+epsilon))
      {
#ifdef SAFE
        fprintf(stderr,"WARNING016: cropfractions are too large: %f in pix %d (%g/%g)\n",cropsum,i,
                grid[i].coord.lon,grid[i].coord.lat);
        fflush(stderr);
#endif
        reducelandfrac(grid+i,cropsum,ncft);
      }
    }/* if skip */
  }
#ifdef DEBUG_IMAGE
  printf("done with everything, freeing memory\n");
  fflush(stdout);
#endif

  free(image_landuse);
#ifdef DEBUG_IMAGE
  printf("done in receive_image_luc()\n");
  fflush(stdout);
#endif

  return FALSE;
} /* of 'receive_image_luc' */

#endif
