/**************************************************************************************/
/**                                                                                \n**/
/**          s  e  n  d  _  i  m  a  g  e  _  d  a  t  a  .  c                     \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     sends data to IMAGE via socket, annually                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "tree.h"
#include "grass.h"
#include "crop.h"

#define NBPOOLS (sizeof(Biomass)/sizeof(float))

#ifdef IMAGE

Bool send_image_data(const Cell grid[],      /**< LPJ grid */
                     const Climate *climate, /**< Climate data */
                     int npft,               /**< number of natural PFTs */
                     const Config *config    /**< LPJmL configuration */
                    )                        /** \return TRUE on error */
{
  Bool rc;
  float *nep_image;
  Biomass *biomass_image;
  int *biome_image;
  float *npp_image,*fire_image,*fireemission_deforest_image;
#ifdef SENDSEP
  float *harvest_agric_image,*harvest_biofuel_image;
  float *harvest_timber_image,*product_turnover_fast_image;
  float *product_turnover_slow_image,*trad_biofuel_image, *rh_image;
#endif
  int p,s,cell,i;
  const Stand *stand;
  const Pft *pft;
  const Pfttree *tree;
  const Pftgrass *grass;
  const Pftcrop *crop;
#ifdef USE_MPI
  int *counts,*offsets;
#endif
  nep_image = newvec(float,config->ngridcell);
  check(nep_image);
  biomass_image = newvec(Biomass,config->ngridcell);
  check(biomass_image);
  biome_image = newvec(int,config->ngridcell);
  check(biome_image);
  npp_image = newvec(float,config->ngridcell);
  check(npp_image);
  fire_image = newvec(float,config->ngridcell);
  check(fire_image);
  fireemission_deforest_image = newvec(float,config->ngridcell);
  check(fireemission_deforest_image);
#ifdef SENDSEP
  harvest_agric_image = newvec(float,config->ngridcell);
  check(harvest_agric_image);
  harvest_biofuel_image = newvec(float,config->ngridcell);
  check(harvest_biofuel_image);
  product_turnover_fast_image = newvec(float,config->ngridcell);
  check(product_turnover_fast_image);
  product_turnover_slow_image = newvec(float,config->ngridcell);
  check(product_turnover_slow_image);
  harvest_timber_image = newvec(float,config->ngridcell);
  check(harvest_timber_image);
  rh_image = newvec(float,config->ngridcell);
  check(rh_image);
  trad_biofuel_image = newvec(float,config->ngridcell);
  check(trad_biofuel_image);
#endif
  /* init data fields */
#ifdef DEBUG_IMAGE
  printf("prepare sending data\n");
  fflush(stdout);
#endif
  for(cell=0;cell<config->ngridcell;cell++)
  {
    nep_image[cell]=npp_image[cell]=
    fire_image[cell]=fireemission_deforest_image[cell]=0.0;
    biome_image[cell]=DESERT;
    biomass_image[cell].stems=biomass_image[cell].roots=
                                biomass_image[cell].leaves=
                                biomass_image[cell].humus=
                                biomass_image[cell].charcoal=
                                biomass_image[cell].branches=
                                biomass_image[cell].litter=
                                biomass_image[cell].product_fast=
                                biomass_image[cell].product_slow=0.0;
#ifdef SENDSEP
  harvest_agric_image[cell]=harvest_biofuel_image[cell]=
    harvest_timber_image[cell]=product_turnover_fast_image[cell]=
    product_turnover_slow_image[cell]=
    rh_image[cell]=trad_biofuel_image[cell]=0.0;
#endif
  }
  /* assign data */
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
#ifdef DEBUG_IMAGE
      if(isnan(grid[cell].balance.nep))
      {
        printf("NEP isnan in cell %d\n",cell);
        fflush(stdout);
      }
      if(isnan(grid[cell].output.flux_estab))
      {
        printf("flux estab isnan in cell %d\n",cell);
        fflush(stdout);
      }
      if(isnan(grid[cell].output.flux_harvest))
      {
        printf("flux harvest isnan in cell %d\n",cell);
        fflush(stdout);
      }
      if(isnan(grid[cell].output.prod_turnover))
      {
        printf("prod_turnover isnan in cell %d\n",cell);
        fflush(stdout);
      }
#endif
#ifdef SENDSEP
      nep_image[cell]=(float)(grid[cell].balance.nep+grid[cell].output.flux_estab);
      harvest_agric_image[cell]=(float)grid[cell].output.flux_harvest;
      harvest_biofuel_image[cell]=(float)grid[cell].ml.image_data->biomass_yield_annual;
      harvest_timber_image[cell]=(float)grid[cell].output.timber_harvest;
      product_turnover_fast_image[cell]=(float)grid[cell].ml.image_data->prod_turn_fast;
      product_turnover_slow_image[cell]=(float)grid[cell].ml.image_data->prod_turn_slow;
      trad_biofuel_image[cell]=(float)grid[cell].output.trad_biofuel;
      rh_image[cell]=(float)grid[cell].ml.image_data->arh;
#else
      nep_image[cell]=(float)(grid[cell].balance.nep+grid[cell].output.flux_estab-grid[cell].output.flux_harvest)
        -grid[cell].balance.biomass_yield;
#endif
      /* timber harvest is computed in IMAGE based on LPJmL carbon pools, 
         not the extraction is a carbon flux to the atmosphere but the product-pool 
         turnover, which is also computed in IMAGE*/
      /* traditional biofuel emissions are to be overwritten in IMAGE, so DO sent here */
      /* -grid[cell].output.timber_harvest*/ 
      /*-grid[cell].output.prod_turnover);*/
      fire_image[cell]=(float)grid[cell].output.firec;
      npp_image[cell]=(float)(grid[cell].ml.image_data->anpp+grid[cell].output.flux_estab);
      fireemission_deforest_image[cell]=(float)grid[cell].output.deforest_emissions;
#ifdef DEBUG_IMAGE
      if(grid[cell].coord.lon>-2.5 && grid[cell].coord.lon<-2.0 && grid[cell].coord.lat>48.0 && grid[cell].coord.lat<48.5)
      {
        printf("sending cell %d (%g/%g): NPP %g NEP %g fire %g deforest_emissions %g biome %d\n",
               cell,grid[cell].coord.lon,grid[cell].coord.lat,npp_image[cell],nep_image[cell],fire_image[cell],fireemission_deforest_image[cell],
            biome_image[cell]);
        fflush(stdout);
      }
#endif
      foreachstand(stand,s,grid[cell].standlist)
      {
        if(stand->type->landusetype==NATURAL)
        {
          biome_image[cell]=biome_classification(avgtemp(climate,cell),stand,npft);
        } /* if NATURAL */
        foreachpft(pft,p,&stand->pftlist)
          switch(pft->par->type)
          {
            case TREE:
              tree=pft->data;
              /* average grid cell biomass pools.
               * Beware.
               * Do not use for productivity assessments! */
              /* assume 2/3 of sapwood to be aboveground */
              biomass_image[cell].stems+=(float)
                ((tree->ind.heartwood+tree->ind.sapwood*0.66667)*0.84)
                *(float)stand->frac*(float)pft->nind;
              biomass_image[cell].branches+=(float)
                ((tree->ind.heartwood+tree->ind.sapwood*0.66667)*0.16)
                *(float)stand->frac*(float)pft->nind;
              biomass_image[cell].leaves+=(float)
                tree->ind.leaf*(float)stand->frac*(float)pft->nind;
              biomass_image[cell].roots+=(float)
                (tree->ind.root+tree->ind.sapwood*0.33333)*(float)stand->frac*(float)pft->nind;
              break;
            case GRASS:
              grass=pft->data;
              biomass_image[cell].leaves+=(float)
                grass->ind.leaf*(float)stand->frac*(float)pft->nind;
              biomass_image[cell].roots+=(float)
                grass->ind.root*(float)stand->frac*(float)pft->nind;
              break;
            case CROP:
              crop=pft->data;
              biomass_image[cell].leaves+=(float)
                (crop->ind.leaf+crop->ind.so+crop->ind.pool)*(float)stand->frac*(float)pft->nind;
              biomass_image[cell].roots+=(float)
                crop->ind.root*(float)stand->frac*(float)pft->nind;
              break;
          } /* of switch */
        biomass_image[cell].litter+=(float)littersum(&stand->soil.litter)*
                                    (float)stand->frac;
        for(i=0;i<LASTLAYER;i++){
          biomass_image[cell].humus+=(float)
            stand->soil.cpool[i].fast*(float)stand->frac;
          biomass_image[cell].charcoal+=(float)
            stand->soil.cpool[i].slow*(float)stand->frac;
        }
      } /* for each stand */
      biomass_image[cell].product_fast=(float)grid[cell].ml.image_data->timber.fast;
      biomass_image[cell].product_slow=(float)grid[cell].ml.image_data->timber.slow;
#ifdef DEBUG_IMAGE
      if(grid[cell].coord.lon>-2.5 && grid[cell].coord.lon<-2.0 && grid[cell].coord.lat>48.0 && grid[cell].coord.lat<48.5){
        printf("sending biomass branches %g charcoal %g humus %g leaves %g litter %g roots %g stems %g\n",
          biomass_image[cell].branches,biomass_image[cell].charcoal,biomass_image[cell].humus,
          biomass_image[cell].leaves,biomass_image[cell].litter,biomass_image[cell].roots,biomass_image[cell].stems);
        fflush(stdout);
      }
#endif
    } /* if skip */
  }
  /* send data */
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,NBPOOLS,config->ntask);
  mpi_write_socket(config->out,biomass_image,MPI_FLOAT,
                   config->nall*NBPOOLS,counts,offsets,config->rank,config->comm);
  getcounts(counts,offsets,config->nall,1,config->ntask);
  mpi_write_socket(config->out,biome_image,MPI_INT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,nep_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,npp_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
#ifdef SENDSEP
  mpi_write_socket(config->out,rh_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_agric_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_biofuel_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_timber_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
#endif
  mpi_write_socket(config->out,fire_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  rc=mpi_write_socket(config->out,fireemission_deforest_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
#ifdef SENDSEP
  mpi_write_socket(config->out,product_turnover_fast_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,product_turnover_slow_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  rc=mpi_write_socket(config->out,trad_biofuel_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
#endif
  free(counts);
  free(offsets);
#else
#ifdef DEBUG_IMAGE
  printf("sending data\n");
  printf("biomass pools[1] %g %g %g %g %g %g %g\n",biomass_image[1].branches,
    biomass_image[1].charcoal,biomass_image[1].humus,biomass_image[1].leaves,
    biomass_image[1].litter,biomass_image[1].roots,biomass_image[1].stems);
  printf("biome[1] %d\n",biome_image[1]);
  printf("nep[1] %g\n",nep_image[1]);
  printf("npp[1] %g\n",npp_image[1]);
  printf("fire[1] %g\n",fire_image[1]);
  printf("fireem[1] %g\n",fireemission_deforest_image[1]);
  fflush(stdout);
  /*printf("biomass pools[67032] %g %g %g %g %g %g %g\n",biomass_image[67032].branches,
    biomass_image[67032].charcoal,biomass_image[67032].humus,biomass_image[67032].leaves,
    biomass_image[67032].litter,biomass_image[67032].roots,biomass_image[67032].stems);
  printf("biome[67032] %d\n",biome_image[67032]);
  printf("nep[67032] %g\n",nep_image[67032]);
  printf("npp[67032] %g\n",npp_image[67032]);
  printf("fire[67032] %g\n",fire_image[67032]);
  printf("fireem[67032] %g\n",fireemission_deforest_image[67032]);*/
  fflush(stdout);
#endif
  writefloat_socket(config->out, biomass_image, config->ngridcell*NBPOOLS);
  writeint_socket(config->out, biome_image, config->ngridcell);
  writefloat_socket(config->out, nep_image, config->ngridcell);
  writefloat_socket(config->out, npp_image, config->ngridcell);
#ifdef SENDSEP
  writefloat_socket(config->out, rh_image, config->ngridcell);
  writefloat_socket(config->out, harvest_agric_image, config->ngridcell);
  writefloat_socket(config->out, harvest_biofuel_image, config->ngridcell);
  writefloat_socket(config->out, harvest_timber_image, config->ngridcell);
#endif
  writefloat_socket(config->out, fire_image, config->ngridcell);
  rc=writefloat_socket(config->out, fireemission_deforest_image, config->ngridcell);
#ifdef SENDSEP
  writefloat_socket(config->out, product_turnover_fast_image, config->ngridcell);
  writefloat_socket(config->out, product_turnover_slow_image, config->ngridcell);
  rc=writefloat_socket(config->out, trad_biofuel_image, config->ngridcell);
#endif

#ifdef DEBUG_IMAGE
  printf("done sending data\n");
  fflush(stdout);
#endif
#endif
  free(biomass_image);
  free(biome_image);
  free(nep_image);
  free(npp_image);
  free(fire_image);
  free(fireemission_deforest_image);
  return rc;
} /* of 'send_image_data' */

#endif
