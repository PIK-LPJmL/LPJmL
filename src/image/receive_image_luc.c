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

#if defined IMAGE && defined COUPLED

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
} /* of 'reducelandfrac' */

Bool receive_image_luc(Cell *grid,          /**< LPJ grid */
                       int npft,            /**< number of natural PFTs */
                       int ncft,            /**< number of crop PFTs */
                       const Config *config /**< Grid configuration */
                      )                     /** \return TRUE on error */
{
  int i;
  Real cropsum;
#ifdef DEBUG_IMAGE_CELL
  int j;
#endif
#ifdef DEBUG_IMAGE
  float sum;
#endif
  Image_landuse *image_landuse;
  float *image_data;
  Takeaway *image_takeaway;
  int *image_data_int;
#ifdef USE_MPI
  int *counts,*offsets;
  MPI_Datatype datatype;
#endif

  /* get timber harvest shares from IMAGE */
  image_data = newvec(float,config->ngridcell);
  check(image_data);
  /*printf("getting timber harvest shares mutliarray, %d (DYNgridreal)\n",
  config->ngridcell);*/
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
      /*if(image_data[i]>0) printf("cell %d th %g\n",i,grid[i].image_data->timber_frac);*/
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
  /* get timber harvest for woodplantattion shares from IMAGE */
  image_data = newvec(float,config->ngridcell);
  check(image_data);
#ifdef DEBUG_IMAGE
  printf("getting timber harvest for woodplantation shares\n");
  fflush(stdout);
#endif
  if(readfloat_socket(config->in, image_data, config->ngridcell))
  {
    free(image_data);
    return TRUE;
  }

#ifdef DEBUG_IMAGE
  printf("assigning timber harvest for woodplantations shares\n");
  fflush(stdout);
#endif
  for(i=0;i<config->ngridcell;i++)
  {
    if(!grid[i].skip)
    {
      grid[i].ml.image_data->timber_frac_wp=(Real)image_data[i];
    }
#ifdef DEBUG_IMAGE
    if(grid[i].coord.lon>-2.5 && grid[i].coord.lon<-2.0 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)/*(i==1 || i==67032)*/
    {
      printf("cell %d %g/%g\n",i,grid[i].coord.lon,grid[i].coord.lat);
      fflush(stdout);
      printf("th[%d]:%g %g\n",i,grid[i].ml.image_data->timber_frac_wp,image_data[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data);

  /* get water consumption from IMAGE */
  image_data = newvec(float, config->ngridcell);
  check(image_data);
#ifdef DEBUG_IMAGE
  printf("getting water consumption\n");
  fflush(stdout);
#endif
  if (readfloat_socket(config->in, image_data, config->ngridcell))
  {
    free(image_data);
    return TRUE;
  }

#ifdef DEBUG_IMAGE
  printf("assigning water consumption\n");
  fflush(stdout);
#endif
  for (i = 0; i<config->ngridcell; i++)
  {
    if (!grid[i].skip)
    {
      grid[i].ml.image_data->totwatcons = (Real)image_data[i];
      grid[i].discharge.wateruse = grid[i].ml.image_data->totwatcons;//HB->check of er nog unit conversie nodig is.
      grid[i].discharge.wateruse = grid[i].discharge.wateruse * 1000;
      getoutput(&grid[i].output,WATERUSECONS,config) = grid[i].discharge.wateruse*NDAYYEAR;
    }
#ifdef DEBUG_IMAGE
    if (grid[i].coord.lon>5.0 && grid[i].coord.lon<5.5 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)/*(i==1 || i==67032)*/
    {
      printf("cell %d %g/%g\n", i, grid[i].coord.lon, grid[i].coord.lat);
      fflush(stdout);
      printf("th[%d]:%g %g\n", i, grid[i].ml.image_data->totwatcons, image_data[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data);

  /* get water demand from IMAGE */
  image_data = newvec(float, config->ngridcell);
  check(image_data);
#ifdef DEBUG_IMAGE
  printf("getting water demand\n");
  fflush(stdout);
#endif
  if (readfloat_socket(config->in, image_data, config->ngridcell))
  {
    free(image_data);
    return TRUE;
  }

#ifdef DEBUG_IMAGE
  printf("assigning water waterdemand\n");
  fflush(stdout);
#endif
  for (i = 0; i<config->ngridcell; i++)
  {
    if (!grid[i].skip)
    {
      grid[i].ml.image_data->totwatdem = (Real)image_data[i];
      grid[i].discharge.wateruse_wd = grid[i].ml.image_data->totwatdem;
      grid[i].discharge.wateruse_wd = grid[i].discharge.wateruse_wd * 1000;
      getoutput(&grid[i].output,WATERUSEDEM,config) = grid[i].discharge.wateruse_wd*NDAYYEAR;
    }
#ifdef DEBUG_IMAGE
    if (grid[i].coord.lon>5.0 && grid[i].coord.lon<5.5 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)/*(i==1 || i==67032)*/
    {
      printf("cell %d %g/%g\n", i, grid[i].coord.lon, grid[i].coord.lat);
      fflush(stdout);
      printf("th[%d]:%g %g\n", i, grid[i].ml.image_data->totwatdem, image_data[i]);
      fflush(stdout);
    }
#endif
  }
  free(image_data);

  // get takeaway fractions from IMAGE (4 values 1D array)
  image_takeaway = newvec(Takeaway,config->ngridcell);
  check(image_takeaway);
#ifdef USE_MPI
  /* The MPI part has to not been tested so far */
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,1,config->ntask);
  MPI_Type_contiguous(NIMAGETREEPARTS,MPI_FLOAT,&datatype);
  MPI_Type_commit(&datatype);

  if(mpi_read_socket(config->in,image_takeaway,datatype,config->nall,counts,
                     offsets,config->rank,config->comm))
  {
    free(offsets);
    free(counts);
    free(image_takeaway);
    MPI_Type_free(&datatype);
    return TRUE;
  }
  MPI_Type_free(&datatype);
#else
#ifdef DEBUG_IMAGE
  printf("getting takeaway fractions\n");
  fflush(stdout);
#endif
  if(readfloat_socket(config->in,(float *)image_takeaway,
                      config->ngridcell*NIMAGETREEPARTS))
  {
    free(image_takeaway);
    return TRUE;
  }
#endif
#ifdef DEBUG_IMAGE
  printf("assigning takeaway fractions \n");
  fflush(stdout);
#endif
  for(i=0;i<config->ngridcell;i++)
  {
    if(!grid[i].skip)
    {
      grid[i].ml.image_data->takeaway[0]=(Real)image_takeaway[i].stems;
      grid[i].ml.image_data->takeaway[1]=(Real)image_takeaway[i].branches;
      grid[i].ml.image_data->takeaway[2]=(Real)image_takeaway[i].leaves;
      grid[i].ml.image_data->takeaway[3]=(Real)image_takeaway[i].roots;

#ifdef DEBUG_IMAGE
      if(i==3794 || i==4696 || i==850 || i==4582)
      {
        printf("takeaway cell %d %g/%g/%g/%g\n",i,grid[i].ml.image_data->takeaway[0],grid[i].ml.image_data->takeaway[1],grid[i].ml.image_data->takeaway[2],grid[i].ml.image_data->takeaway[3]);
        fflush(stdout);
      }
#endif
    }
#ifdef DEBUG_IMAGE_CELL
    if(image_takeaway[i].stems>0)
    {
      printf("cell %d %g/%g\n",i,grid[i].coord.lon,grid[i].coord.lat);
      fflush(stdout);
      printf("takeaway: th[%d]:%g %g\n",i,grid[i].ml.image_data->takeaway[0],image_takeaway[i].stems);
      fflush(stdout);
      for (j=0;j<4;j++)
        printf("takeaway2:%d %.6g\n", i, grid[i].ml.image_data->takeaway[j]);
    }
#endif
  }
  free(image_takeaway);
  
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
      /*if(image_data_int[i]>0) printf("cell %d th %g fb %g\n",i,grid[i].image_data->timber_frac,grid[i].image_data->fburnt);*/
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
  /*printf("getting crop shares multiarray %d (DYNgridreal)\n",
  config->ngridcell*NIMAGECROPS);*/
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
    if(grid[i].coord.lon>-2.5 && grid[i].coord.lon<-2.0 && grid[i].coord.lat>48.0 && grid[i].coord.lat<48.5)/*(i==1 || i==67032)*/
    /*if(i==2017)*/
    sum=0;
    if(!grid[i].skip)
    {
       sum = image_landuse[i].pasture +
          image_landuse[i].wheat_rf +
          image_landuse[i].rice_rf +
          image_landuse[i].maize_rf +
          image_landuse[i].trop_cereals_rf +
          image_landuse[i].oth_temp_cereals_rf +
          image_landuse[i].pulses_rf +
          image_landuse[i].soybeans_rf +
          image_landuse[i].temp_oilcrops_rf +
          image_landuse[i].trop_oilcrops_rf +
          image_landuse[i].temp_roots_tub_rf +
          image_landuse[i].trop_roots_tub_rf +
          image_landuse[i].suger_cain_rf +
          image_landuse[i].oil_palmfruit_rf +
          image_landuse[i].veg_fruits_rf +
          image_landuse[i].oth_non_rf +
          image_landuse[i].plant_fibres_rf +
          image_landuse[i].grains_rf +
          image_landuse[i].oilcrops_bf +
          image_landuse[i].sugercrops_bf +
          image_landuse[i].woody_bf +
          image_landuse[i].non_woody_bf +
          image_landuse[i].wheat_ir +
          image_landuse[i].rice_ir +
          image_landuse[i].maize_ir +
          image_landuse[i].trop_cereals_ir +
          image_landuse[i].oth_temp_cereals_ir +
          image_landuse[i].pulses_ir +
          image_landuse[i].soybeans_ir +
          image_landuse[i].temp_oilcrops_ir +
          image_landuse[i].trop_oilcrops_ir +
          image_landuse[i].temp_roots_tub_ir +
          image_landuse[i].trop_roots_tub_ir +
          image_landuse[i].suger_cain_ir +
          image_landuse[i].oil_palmfruit_ir +
          image_landuse[i].veg_fruits_ir +
          image_landuse[i].oth_non_ir +
          image_landuse[i].plant_fibres_ir +
          image_landuse[i].forest_plant;
       /*if(grid[i].image_data->fburnt>0) {
        printf("cell %d th %g fb %g\n",i,grid[i].image_data->timber_frac,grid[i].image_data->fburnt);

        if((grid[i].cropfrac_ir+grid[i].cropfrac_rf)<0.998-epsilon){
          if(sum>0) printf("EXPANSION! last year cropland %g this year %g\n",grid[i].cropfrac_ir+grid[i].cropfrac_rf,sum);
          else if(sum==0) printf("no cropland still, last year %g this year %g\n",grid[i].cropfrac_ir+grid[i].cropfrac_rf,sum);
        }
      }
      else
      {
        if((grid[i].cropfrac_ir+grid[i].cropfrac_rf)<0.998-epsilon)
        {
          if(sum>0)
            printf("EXPANSION but fburnt is %g! last year cropland %g this year %g\n",grid[i].image_data->fburnt,grid[i].cropfrac_ir+grid[i].cropfrac_rf,sum);
        *//*  else if(sum==0 && (grid[i].cropfrac_ir+grid[i].cropfrac_rf)>0) printf("no cropland still, last year %g this year %g\n",grid[i].cropfrac_ir+grid[i].cropfrac_rf,sum); */ /*
        } 
      }*/
    }

  /*if((grid[i].coord.lon>102.2 && grid[i].coord.lon<102.3 && grid[i].coord.lat>28.7 && grid[i].coord.lat<28.8) ||
     (grid[i].coord.lon>-82.3 && grid[i].coord.lon<-82.2 && grid[i].coord.lat>28.2 && grid[i].coord.lat<28.3) ||
     (grid[i].coord.lon>102.7 && grid[i].coord.lon<102.8 && grid[i].coord.lat>30.7 && grid[i].coord.lat<30.8))
  {
      printf("cell %d %g/%g\n",i,grid[i].coord.lon,grid[i].coord.lat);
      printf("cell %d th %g fb %g\n",i,grid[i].image_data->timber_frac,grid[i].image_data->fburnt);
      fflush(stdout);
      printf("cropshares: %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g\n",
        image_landuse[i].irrig_maize,
        image_landuse[i].irrig_oil_crops,
        image_landuse[i].irrig_pulses,
        image_landuse[i].irrig_rice,
        image_landuse[i].irrig_roots_tubers,
        image_landuse[i].irrig_temp_cereals,
        image_landuse[i].irrig_trop_cereals,
        image_landuse[i].maize,
        image_landuse[i].maize_feed,
        image_landuse[i].non_woody_biofuels,
        image_landuse[i].oil_crops,
        image_landuse[i].pasture,
        image_landuse[i].pulses,
        image_landuse[i].rice,
        image_landuse[i].roots_tubers,
        image_landuse[i].sugar_cane,
        image_landuse[i].temp_cereals,
        image_landuse[i].trop_cereals,
        image_landuse[i].woody_biofuels);
      fflush(stdout);
    }*/
#endif
    if(!grid[i].skip)
    {
#ifdef DEBUG_IMAGE_CELL
      printf("doing cell %d\n",i);
      fflush(stdout);
#endif
      /* 0 wheat rainfed */
      grid[i].ml.landfrac[0].crop[TEMPERATE_CEREALS]= (Real)image_landuse[i].wheat_rf + (Real)image_landuse[i].oth_temp_cereals_rf;
      /* 1 rice rainfed */
      grid[i].ml.landfrac[0].crop[RICE]=(Real)image_landuse[i].rice_rf;
      /* 2 maize rainfed = rf maize + rf grains */
      grid[i].ml.landfrac[0].crop[MAIZE] = (Real)image_landuse[i].maize_rf + (Real)image_landuse[i].grains_rf;
      /* 3 tropical cereals rainfed*/
      grid[i].ml.landfrac[0].crop[TROPICAL_CEREALS]= (Real)image_landuse[i].trop_cereals_rf;;
      /* 4 pulses rainfed */
      grid[i].ml.landfrac[0].crop[PULSES]=(Real)image_landuse[i].pulses_rf;
      /* 5 temperate roots rainfed = temp_roots_tub_rf + 0.5 * sugercrops_bf */
      grid[i].ml.landfrac[0].crop[TEMPERATE_ROOTS]= (Real)image_landuse[i].temp_roots_tub_rf + (Real)image_landuse[i].sugercrops_bf * 0.5;
      /* 6 tropical roots rainfed */
      grid[i].ml.landfrac[0].crop[TROPICAL_ROOTS]= (Real)image_landuse[i].trop_roots_tub_rf;
      /* 7 sunflower rainfed*/
      grid[i].ml.landfrac[0].crop[OIL_CROPS_SUNFLOWER]= (Real)image_landuse[i].temp_oilcrops_rf * 0.5;
      /* 8 soybean  rainfed*/
      grid[i].ml.landfrac[0].crop[OIL_CROPS_SOYBEAN] = (Real)image_landuse[i].soybeans_rf;
      /* 9 groundnut  rainfed*/
      grid[i].ml.landfrac[0].crop[OIL_CROPS_GROUNDNUT] = (Real)image_landuse[i].trop_oilcrops_rf + (Real)image_landuse[i].oilcrops_bf * 0.5;
      /* 10 rapeseed  rainfed*/
      grid[i].ml.landfrac[0].crop[OIL_CROPS_RAPESEED]= (Real)image_landuse[i].temp_oilcrops_rf * 0.5 + (Real)image_landuse[i].oilcrops_bf *0.5;
      /* 11 suger cane rainfed */
      grid[i].ml.landfrac[0].crop[SUGARCANE]= (Real)image_landuse[i].suger_cain_rf + (Real)image_landuse[i].sugercrops_bf * 0.5;
      /* 12 grass(0) = others rainfed  */
      grid[i].ml.landfrac[0].grass[0]= (Real)image_landuse[i].oil_palmfruit_rf + (Real)image_landuse[i].veg_fruits_rf + (Real)image_landuse[i].oth_non_rf + (Real)image_landuse[i].plant_fibres_rf;
      /* 13 grass(1) = grassland managed rainfed */
      grid[i].ml.landfrac[0].grass[1]=(Real)image_landuse[i].pasture;
      /* 14 non bioenergy trees rainfed */
      grid[i].ml.landfrac[0].biomass_grass= (Real)image_landuse[i].non_woody_bf;
      /* 15 bioenergy trees rainfed */
      grid[i].ml.landfrac[0].biomass_tree=(Real)image_landuse[i].woody_bf;
      /* 16 forest plantations rainfed */
      grid[i].ml.landfrac[0].woodplantation=(Real)image_landuse[i].forest_plant;

#ifdef DEBUG_IMAGE_CELL
      printf("done with rainfed in %d\n",i);
      fflush(stdout);
#endif
      /* irrigated crops */
      /* 17 wheat irrigated */
      grid[i].ml.landfrac[1].crop[TEMPERATE_CEREALS]= (Real)image_landuse[i].wheat_ir + (Real)image_landuse[i].oth_temp_cereals_ir;
      grid[i].ml.irrig_system->crop[TEMPERATE_CEREALS] = grid[i].ml.manage.par->default_irrig_system;
      /* 18 rice irrigated */
      grid[i].ml.landfrac[1].crop[RICE]=(Real)image_landuse[i].rice_ir;
      grid[i].ml.irrig_system->crop[RICE] = grid[i].ml.manage.par->default_irrig_system;
      /* 19 maize irrigated = irr maize */
      grid[i].ml.landfrac[1].crop[MAIZE]=(Real)image_landuse[i].maize_ir;
      grid[i].ml.irrig_system->crop[MAIZE] = grid[i].ml.manage.par->default_irrig_system;
      /* 20 millet irrigated */
      grid[i].ml.landfrac[1].crop[TROPICAL_CEREALS]= (Real)image_landuse[i].trop_cereals_ir;
      grid[i].ml.irrig_system->crop[TROPICAL_CEREALS] = grid[i].ml.manage.par->default_irrig_system;
      /* 21 peas irrigated */
      grid[i].ml.landfrac[1].crop[PULSES]= (Real)image_landuse[i].pulses_ir;
      grid[i].ml.irrig_system->crop[PULSES] = grid[i].ml.manage.par->default_irrig_system;
      /* 22 suger beet irrigated */
      grid[i].ml.landfrac[1].crop[TEMPERATE_ROOTS]= (Real)image_landuse[i].temp_roots_tub_ir;
      grid[i].ml.irrig_system->crop[TEMPERATE_ROOTS] = grid[i].ml.manage.par->default_irrig_system;
      /* 23 cassava */
      grid[i].ml.landfrac[1].crop[TROPICAL_ROOTS]= (Real)image_landuse[i].trop_roots_tub_ir;
      grid[i].ml.irrig_system->crop[TROPICAL_ROOTS] = grid[i].ml.manage.par->default_irrig_system;
      /* 24 sunflower  irrigated*/
      grid[i].ml.landfrac[1].crop[OIL_CROPS_SUNFLOWER]= (Real)image_landuse[i].temp_oilcrops_ir * 0.5;
      grid[i].ml.irrig_system->crop[OIL_CROPS_SUNFLOWER] = grid[i].ml.manage.par->default_irrig_system;
      /* 25 soybean irrigated  */
      grid[i].ml.landfrac[1].crop[OIL_CROPS_GROUNDNUT]= (Real)image_landuse[i].soybeans_ir;
      grid[i].ml.irrig_system->crop[OIL_CROPS_GROUNDNUT] = grid[i].ml.manage.par->default_irrig_system;
      /* 26 groundnut irrigated  */
      grid[i].ml.landfrac[1].crop[OIL_CROPS_RAPESEED]= (Real)image_landuse[i].trop_oilcrops_ir;
      grid[i].ml.irrig_system->crop[OIL_CROPS_RAPESEED] = grid[i].ml.manage.par->default_irrig_system;
      /* 27 rapeseed irrigated  */
      grid[i].ml.landfrac[1].crop[OIL_CROPS_SOYBEAN] = (Real)image_landuse[i].temp_oilcrops_ir * 0.5;
      grid[i].ml.irrig_system->crop[OIL_CROPS_SOYBEAN] = grid[i].ml.manage.par->default_irrig_system;
      /* 28 suger cane irrigation */
      grid[i].ml.landfrac[1].crop[SUGARCANE]= (Real)image_landuse[i].suger_cain_ir;
      grid[i].ml.irrig_system->crop[SUGARCANE] = grid[i].ml.manage.par->default_irrig_system;
      /* 29 grass(0) = ir others irrigated  */
      grid[i].ml.landfrac[1].grass[0]= (Real)image_landuse[i].oil_palmfruit_ir + (Real)image_landuse[i].veg_fruits_ir + (Real)image_landuse[i].oth_non_ir + (Real)image_landuse[i].plant_fibres_ir;
      grid[i].ml.irrig_system->grass[0] = grid[i].ml.manage.par->default_irrig_system;
      /* 30 NO irrigated pasture in IMAGE, so intialize to zero */
      grid[i].ml.landfrac[1].grass[1]=0;
      /* 31 NO irrigated non bioenergy trees, so intialize to zero  */
      grid[i].ml.landfrac[1].biomass_grass=0;
      /* 32 NO irrigated bioenergy trees, so intialize to zero  */
      grid[i].ml.landfrac[1].biomass_tree=0;
      /* 33 No irrigated forest plantations, so intialize to zero */
      grid[i].ml.landfrac[1].woodplantation=0.0;

#ifdef DEBUG_IMAGE_CELL
      printf("done with irrig in %d\n",i);
      fflush(stdout);
#endif

#ifdef DEBUG_IMAGE_CELL
      printf("done with assigning in %d, now reducing too large fractions\n",i);
      fflush(stdout);
#endif
      if(grid[i].ml.image_data->timber_frac>1.0)
      {
#ifdef SAFE
        fprintf(stderr,"WARNING015: timber fraction too large: %f in cell %d (%g/%g) as woody biofuels (%f) have been added.\n",
          grid[i].ml.image_data->timber_frac,i,grid[i].coord.lon,grid[i].coord.lat,
          (Real)image_landuse[i].woody_bf);
        fflush(stderr);
#endif
        grid[i].ml.image_data->timber_frac=1.0;
      }
      /* checking sum of crop fractions */
#ifdef DEBUG_IMAGE_CELL
      printf("done with reducing timber in %d, now reducing too large fractions\n",i);
      fflush(stdout);
#endif
      cropsum=landfrac_sum(grid[i].ml.landfrac,ncft,config->nagtree,FALSE)+landfrac_sum(grid[i].ml.landfrac,ncft,config->nagtree,TRUE);
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
#ifdef DEBUG_IMAGE_CELL
  printf("done with everything, freeing memory\n");
  fflush(stdout);
#endif

  free(image_landuse);


  /* get irrigation efficiency improvement from IMAGE */
  /* irrigation efficiency are defined per IMAGE region and mapped to LPJ countries 
   * therefore no #ifdef USE_MPI directives are present: the full data array must 
   * be received by all nodes. */
  /* remove and use config->ncountries once INTERFACE is updated to match the number of countries in par/manage_laimax_alphaa_all7.par */
  int ncountries = 197;
  image_data = newvec(float,ncountries);
  check(image_data);

#ifdef DEBUG_IMAGE
  printf("getting irrigation efficiency improvement\n");
  fflush(stdout);
#endif
  /* use config->ncountries once INTERFACE is updated to match the number of countries in par/manage_laimax_alphaa_all7.par */
  if(readfloat_socket(config->in, image_data, ncountries))
  {
    free(image_data);
    return TRUE;
  }

#ifdef DEBUG_IMAGE
  printf("done in receive_image_luc()\n");
  fflush(stdout);
#endif

  return FALSE;
} /* of 'receive_image_luc' */

#endif
