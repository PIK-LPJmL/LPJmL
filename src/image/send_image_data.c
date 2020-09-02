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

#if defined IMAGE && defined COUPLED

Bool send_image_data(const Config *config,   /**< Grid configuration */
                     const Cell grid[],      /**< LPJ grid */
                     const Climate *climate, /**< Climate data */
                     int npft,               /**< number of natural PFTs */
                     int ncft                /**< number of crop PFTs */
                    )                        /**< return TRUE on error */
{
  int m;
  float agrfrac, natfrac, wpfrac,*agrfrac_image, *natfrac_image,*wpfrac_image;
  Bool rc;
  float *nep_image;
  Biomass *biomass_image,*biomass_image_nat,*biomass_image_wp,*biomass_image_agr;
  int *biome_image;
  float *npp_image,*fire_image,*fireemission_deforest_image;
  float *fire_image_nat,*fire_image_wp,*fireemission_deforest_image_agr;

  float **yields;
  float *adischarge;
  Mirrig_to_image *monthirrig;
  Mirrig_to_image *monthevapotr;
  Mirrig_to_image *monthpetim;
  float *nppgrass_image;
  float *npp_image_nat,*npp_image_wp,*npp_image_agr,*nep_image_nat,*nep_image_wp,*nep_image_agr;
#ifdef SENDSEP
  float *harvest_agric_image,*harvest_biofuel_image;
  float *harvest_timber_image,*product_turnover_fast_image;
  float *product_turnover_slow_image,*trad_biofuel_image, *rh_image;
  float *rh_image_nat,*rh_image_wp,*rh_image_agr;
  float *harvest_agric_image_agr,*harvest_biofuel_image_agr;
  float *harvest_timber_image_nat,*harvest_timber_image_wp,*trad_biofuel_image_nat,*trad_biofuel_image_wp;
#endif
  int p,s,cell,i;
  int ncrops;
  const Stand *stand;
  const Pft *pft;
  const Pfttree *tree;
  const Pftgrass *grass;
  const Pftcrop *crop;
#ifdef USE_MPI
  int *counts,*offsets;
#endif
  ncrops = 2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE);
  yields=newmatrix(float,config->ngridcell,ncrops);
  check(yields);  
  monthirrig=newvec(Mirrig_to_image,config->ngridcell);
  check(monthirrig);  
  monthevapotr = newvec(Mirrig_to_image, config->ngridcell);
  check(monthevapotr);
  monthpetim = newvec(Mirrig_to_image, config->ngridcell);
  check(monthpetim);
  adischarge=newvec(float,config->ngridcell);
  check(adischarge);
  nep_image = newvec(float,config->ngridcell);
  check(nep_image);
  nep_image_nat = newvec(float,config->ngridcell);
  check(nep_image_nat);
  nep_image_wp = newvec(float, config->ngridcell);
  check(nep_image_wp);
  nep_image_agr = newvec(float,config->ngridcell);
  check(nep_image_agr);
  biomass_image = newvec(Biomass,config->ngridcell);
  check(biomass_image);
  biomass_image_nat = newvec(Biomass,config->ngridcell);
  check(biomass_image_nat);
  biomass_image_wp = newvec(Biomass, config->ngridcell);
  check(biomass_image_wp);
  biomass_image_agr = newvec(Biomass,config->ngridcell);
  check(biomass_image_agr);
  biome_image = newvec(int,config->ngridcell);
  check(biome_image);
  npp_image = newvec(float,config->ngridcell);
  check(npp_image);
  npp_image_nat = newvec(float,config->ngridcell);
  check(npp_image_nat);
  npp_image_wp = newvec(float, config->ngridcell);
  check(npp_image_wp);
  npp_image_agr = newvec(float,config->ngridcell);
  check(npp_image_agr);
  fire_image = newvec(float,config->ngridcell);
  check(fire_image);
  fire_image_nat = newvec(float,config->ngridcell);
  check(fire_image_nat);
  fire_image_wp = newvec(float, config->ngridcell);
  check(fire_image_wp);
  fireemission_deforest_image = newvec(float,config->ngridcell);
  check(fireemission_deforest_image);
  fireemission_deforest_image_agr = newvec(float,config->ngridcell);
  check(fireemission_deforest_image_agr);
  nppgrass_image = newvec(float,config->ngridcell);
  check(nppgrass_image);
  agrfrac_image = newvec(float,config->ngridcell);
  check(agrfrac_image);
  natfrac_image = newvec(float,config->ngridcell);
  check(natfrac_image);
  wpfrac_image = newvec(float, config->ngridcell);
  check(wpfrac_image);
#ifdef SENDSEP
  harvest_agric_image = newvec(float,config->ngridcell);
  check(harvest_agric_image);
  harvest_agric_image_agr = newvec(float,config->ngridcell);
  check(harvest_agric_image_agr);
  harvest_biofuel_image = newvec(float,config->ngridcell);
  check(harvest_biofuel_image);
  harvest_biofuel_image_agr = newvec(float,config->ngridcell);
  check(harvest_biofuel_image_agr);
  product_turnover_fast_image = newvec(float,config->ngridcell);
  check(product_turnover_fast_image);
  product_turnover_slow_image = newvec(float,config->ngridcell);
  check(product_turnover_slow_image);
  harvest_timber_image = newvec(float,config->ngridcell);
  check(harvest_timber_image);
  harvest_timber_image_nat = newvec(float,config->ngridcell);
  check(harvest_timber_image_nat);
  harvest_timber_image_wp = newvec(float, config->ngridcell);
  check(harvest_timber_image_wp);
  rh_image = newvec(float,config->ngridcell);
  check(rh_image);
  rh_image_nat = newvec(float,config->ngridcell);
  check(rh_image_nat);
  rh_image_wp = newvec(float,config->ngridcell);
  check(rh_image_wp);
  rh_image_agr = newvec(float,config->ngridcell);
  check(rh_image_agr);
  trad_biofuel_image = newvec(float,config->ngridcell);
  check(trad_biofuel_image);
  trad_biofuel_image_nat = newvec(float,config->ngridcell);
  check(trad_biofuel_image_nat);
  trad_biofuel_image_wp = newvec(float, config->ngridcell);
  check(trad_biofuel_image_wp);
#endif
  /* init data fields */
#ifdef DEBUG_IMAGE
  printf("prepare sending data: initialization\n");
  fflush(stdout);
#endif
  for(cell=0;cell<config->ngridcell;cell++)
  {
  /* allocating memory to list of yields_image pointers */
  /* VORtech: a list of pointers to vectors written to the socket is not correctly handled by
     the TDT library, as a workaround we defined the struct cropyield_to_image */
  /*    yields_image[cell]=newvec(float,ncrops);  here we could check if it is more
                                                reasonable to send only yield for the 19
                                                crops known in IMAGE -- c.f. also our
                                                discussion on what crops to force where:
                                                If temperate roots are not grown in the tropics and
                                                vice versa, we probably also don't need
                                                to send the information for these separately.
                                                So far, however, I use 32 as the number of crops in
                                                pft_harvest */
/*    check(yields_image[cell]);*/
/*    for(i=0;i<ncrops;i++) yields_image[cell][i] = 0.0;*/
    /* initializing to have zeros in skipped cells */
    for(i=0;i<ncrops;i++)
      yields[cell][i]  = 0.0;
    for(m=0;m<NMONTH;m++)
    {
      monthirrig[cell][m] = 0.0;
      monthevapotr[cell][m] = 0.0;
      monthpetim[cell][m] = 0.0;
    }     
    adischarge[cell]=0.0;
    nep_image[cell]=npp_image[cell]=
    fire_image[cell]=fireemission_deforest_image[cell]=0.0;
    npp_image_nat[cell]=npp_image_agr[cell]=nep_image_nat[cell]=nep_image_agr[cell]=0.0;
    fire_image_nat[cell]=fireemission_deforest_image_agr[cell]=0.0;
    npp_image_wp[cell]=npp_image_agr[cell]=nep_image_wp[cell]=nep_image_agr[cell]=0.0;
    fire_image_wp[cell]=fireemission_deforest_image_agr[cell]=0.0;
    biome_image[cell]=HOT_DESERT;
    biomass_image[cell].stems=biomass_image[cell].roots=
    biomass_image[cell].leaves=
    biomass_image[cell].humus=
    biomass_image[cell].charcoal=
    biomass_image[cell].branches=
    biomass_image[cell].litter=
    biomass_image[cell].product_fast=
    biomass_image[cell].product_slow=
    nppgrass_image[cell]=0.0;
    biomass_image_nat[cell].stems=biomass_image_nat[cell].roots=
    biomass_image_nat[cell].leaves=
    biomass_image_nat[cell].humus=
    biomass_image_nat[cell].charcoal=
    biomass_image_nat[cell].branches=
    biomass_image_nat[cell].litter=
    biomass_image_nat[cell].product_fast=
    biomass_image_nat[cell].product_slow=0.0;
    biomass_image_wp[cell].stems=biomass_image_wp[cell].roots=
    biomass_image_wp[cell].leaves=
    biomass_image_wp[cell].humus=
    biomass_image_wp[cell].charcoal=
    biomass_image_wp[cell].branches=
    biomass_image_wp[cell].litter=
    biomass_image_wp[cell].product_fast=
    biomass_image_wp[cell].product_slow=0.0;
    biomass_image_agr[cell].stems=biomass_image_agr[cell].roots=
    biomass_image_agr[cell].leaves=
    biomass_image_agr[cell].humus=
    biomass_image_agr[cell].charcoal=
    biomass_image_agr[cell].branches=
    biomass_image_agr[cell].litter=
    biomass_image_agr[cell].product_fast=
    biomass_image_agr[cell].product_slow=0.0;
    agrfrac_image[cell]=natfrac_image[cell]=wpfrac_image[cell]=0.0;
#ifdef SENDSEP
    harvest_agric_image[cell]=harvest_biofuel_image[cell]=
    harvest_timber_image[cell]=product_turnover_fast_image[cell]=
    product_turnover_slow_image[cell]=
    rh_image[cell]=trad_biofuel_image[cell]=0.0;
    harvest_agric_image_agr[cell]=harvest_biofuel_image_agr[cell]=
    harvest_timber_image_nat[cell]=trad_biofuel_image_nat[cell]=0.0;
    harvest_timber_image_wp[cell]=trad_biofuel_image_wp[cell]=0.0;
#endif
  } // cells
#ifdef DEBUG_IMAGE
  printf("prepare sending data: assigning values\n");
  fflush(stdout);
#endif
  /* assign data */
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
        for(i=0;i<ncrops;i++)
          yields[cell][i] = (float)grid[cell].output.pft_harvest[i].harvest;
        

      for(m=0;m<NMONTH;m++)
      {
        monthirrig[cell][m]  = (float)grid[cell].ml.image_data->mirrwatdem[m];
        monthevapotr[cell][m] = (float)grid[cell].ml.image_data->mevapotr[m];
        monthpetim[cell][m] = (float)grid[cell].ml.image_data->mpetim[m];
      }
      if (config->river_routing) {
        adischarge[cell] = (float)(grid[cell].output.ydischarge*1e-9);
      }
#ifdef DEBUG_IMAGE_CELL        
      if (grid[cell].output.pft_harvest[11].harvest > 0){
        printf("pft_harvest.sugarcane = %d, %g\n", cell, grid[cell].output.pft_harvest[11].harvest);
        printf("yields sugarcane = %d %g\n", cell, yields[cell][11]);
      }
      fflush(stdout);
#endif
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
      nep_image_nat[cell]=(float)(grid[cell].npp_nat+grid[cell].flux_estab_nat-grid[cell].rh_nat);
      nep_image_wp[cell]=(float)(grid[cell].npp_wp+grid[cell].flux_estab_wp-grid[cell].rh_wp);
      harvest_agric_image[cell]=(float)grid[cell].output.flux_harvest;
      harvest_biofuel_image[cell]=(float)grid[cell].ml.image_data->biomass_yield_annual;
      harvest_timber_image[cell]=(float)grid[cell].output.timber_harvest;
      product_turnover_fast_image[cell]=(float)grid[cell].ml.image_data->prod_turn_fast;
      product_turnover_slow_image[cell]=(float)grid[cell].ml.image_data->prod_turn_slow;
      trad_biofuel_image[cell]=(float)grid[cell].output.trad_biofuel;
      rh_image[cell]=(float)grid[cell].ml.image_data->arh;
      rh_image_nat[cell]=(float)(grid[cell].rh_nat);
      rh_image_wp[cell]=(float)(grid[cell].rh_wp);
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
      npp_image_nat[cell]=(float)(grid[cell].npp_nat+grid[cell].flux_estab_nat);
      npp_image_wp[cell]=(float)(grid[cell].npp_wp+grid[cell].flux_estab_wp);
      fireemission_deforest_image[cell]=(float)grid[cell].output.deforest_emissions;
      /*printf("sending pix %d trad_biof %g deforest_emiss %g\n",
        cell,trad_biofuel_image[cell],fireemission_deforest_image[cell]);*/
      /* pft_npp voor rainfed managed grass */
      nppgrass_image[cell]=(float)grid[cell].output.pft_npp[(npft-config->nbiomass)+rmgrass(ncft)];
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
          biome_image[cell]=biome_classification(avgtemp(climate,cell),npp_image[cell],stand,npft);
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
          biomass_image[cell].humus+=(float)stand->soil.cpool[i].fast*(float)stand->frac;
          biomass_image[cell].charcoal+=(float)stand->soil.cpool[i].slow*(float)stand->frac;
        }
      } /* for each stand */
      biomass_image[cell].product_fast=(float)grid[cell].ml.image_data->timber.fast;
      biomass_image[cell].product_slow=(float)grid[cell].ml.image_data->timber.slow;
      /*if(product_turnover_fast_image[cell]>0){
        printf("cell %d turn_fast %g\n",cell,product_turnover_fast_image[cell]);
        printf("cell %d fast %g\n",cell,biomass_image[cell].product_fast);
      }
      if(product_turnover_slow_image[cell]>0) {
        printf("cell %d turn_slow %g\n",cell,product_turnover_slow_image[cell]);
        printf("cell %d slow %g\n",cell,biomass_image[cell].product_slow);
      }*/
#ifdef DEBUG_IMAGE
      if(grid[cell].coord.lon>-2.5 && grid[cell].coord.lon<-2.0 && grid[cell].coord.lat>48.0 && grid[cell].coord.lat<48.5){
        printf("sending biomass branches %g charcoal %g humus %g leaves %g litter %g roots %g stems %g\n",
          biomass_image[cell].branches,biomass_image[cell].charcoal,biomass_image[cell].humus,
          biomass_image[cell].leaves,biomass_image[cell].litter,biomass_image[cell].roots,biomass_image[cell].stems);
        fflush(stdout);
      }
#endif
      /* NATURAL variable biomass_image_nat with 9 biomass pools with ONLY natural stand */
      natfrac=0.0;
      foreachstand(stand,s,grid[cell].standlist)
      {
        if(stand->type->landusetype==NATURAL)
        {
          foreachpft(pft,p,&stand->pftlist)
            switch(pft->par->type)
            {
              case TREE:
                tree=pft->data;
                /* average grid cell biomass pools.
                * Beware.
                * Do not use for productivity assessments! */
                /* assume 2/3 of sapwood to be aboveground */
                biomass_image_nat[cell].stems+=(float)
                  ((tree->ind.heartwood+tree->ind.sapwood*0.66667)*0.84)
                  *(float)stand->frac*(float)pft->nind;
                biomass_image_nat[cell].branches+=(float)
                  ((tree->ind.heartwood+tree->ind.sapwood*0.66667)*0.16)
                  *(float)stand->frac*(float)pft->nind;
                biomass_image_nat[cell].leaves+=(float)
                  tree->ind.leaf*(float)stand->frac*(float)pft->nind;
                biomass_image_nat[cell].roots+=(float)
                   (tree->ind.root+tree->ind.sapwood*0.33333)*(float)stand->frac*(float)pft->nind;
                break;
              case GRASS:
                grass=pft->data;
                biomass_image_nat[cell].leaves+=(float)
                  grass->ind.leaf*(float)stand->frac*(float)pft->nind;
                biomass_image_nat[cell].roots+=(float)
                  grass->ind.root*(float)stand->frac*(float)pft->nind;
                break;
            } /* of switch */
          biomass_image_nat[cell].litter+=(float)littersum(&stand->soil.litter)*(float)stand->frac;
          for (i=0; i<LASTLAYER; i++){
            biomass_image_nat[cell].humus+=(float)stand->soil.cpool[i].fast*(float)stand->frac;
            biomass_image_nat[cell].charcoal+=(float)stand->soil.cpool[i].slow*(float)stand->frac;
          }
          natfrac+=(float)stand->frac;
        }
      } /* for each stand */
      biomass_image_nat[cell].product_fast=(float)grid[cell].ml.image_data->timber.fast;
      biomass_image_nat[cell].product_slow=(float)grid[cell].ml.image_data->timber.slow;
      natfrac_image[cell]=natfrac;

      /* WOODPLANTATIONS variable biomass_image_wp with 9 biomass pools with ONLY woodplantation stand */
      wpfrac=0.0;
      foreachstand(stand,s,grid[cell].standlist)
      {
        if (stand->type->landusetype==WOODPLANTATION)
        {
          foreachpft(pft,p,&stand->pftlist)
            switch (pft->par->type)
            {
              case TREE:
                tree = pft->data;
                /* average grid cell biomass pools.
                * Beware.
                * Do not use for productivity assessments! */
                /* assume 2/3 of sapwood to be aboveground */
                biomass_image_wp[cell].stems += (float)
                   ((tree->ind.heartwood + tree->ind.sapwood*0.66667)*0.84)
                   *(float)stand->frac*(float)pft->nind;
                biomass_image_wp[cell].branches += (float)
                   ((tree->ind.heartwood + tree->ind.sapwood*0.66667)*0.16)
                      *(float)stand->frac*(float)pft->nind;
                biomass_image_wp[cell].leaves += (float)
                   tree->ind.leaf*(float)stand->frac*(float)pft->nind;
                biomass_image_wp[cell].roots += (float)
                    (tree->ind.root + tree->ind.sapwood*0.33333)*(float)stand->frac*(float)pft->nind;
                break;
              case GRASS:
                grass = pft->data;
                biomass_image_wp[cell].leaves += (float)grass->ind.leaf*(float)stand->frac*(float)pft->nind;
                biomass_image_wp[cell].roots += (float)grass->ind.root*(float)stand->frac*(float)pft->nind;
                break;
            } /* of switch */
          biomass_image_wp[cell].litter+=(float)littersum(&stand->soil.litter)*(float)stand->frac;
          for (i = 0; i<LASTLAYER; i++){
            biomass_image_wp[cell].humus+=(float)stand->soil.cpool[i].fast*(float)stand->frac;
            biomass_image_wp[cell].charcoal+=(float)stand->soil.cpool[i].slow*(float)stand->frac;
          }
          wpfrac += (float)stand->frac;
        }
      } /* for each stand */
      biomass_image_wp[cell].product_fast=(float)grid[cell].ml.image_data->timber.fast;
      biomass_image_wp[cell].product_slow=(float)grid[cell].ml.image_data->timber.slow;
      wpfrac_image[cell]=wpfrac;

      /* Added new variable biomass_image_agr with 9 biomass pools for all NON-natural stand */
      agrfrac=0.0;
      foreachstand(stand,s,grid[cell].standlist)
      {
        if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WOODPLANTATION)
        {
          foreachpft(pft,p,&stand->pftlist)
          switch(pft->par->type)
          {
            case TREE:
              tree=pft->data;
              /* average grid cell biomass pools.
              * Beware.
              * Do not use for productivity assessments! */
              /* assume 2/3 of sapwood to be aboveground */
              biomass_image_agr[cell].stems+=(float)
                ((tree->ind.heartwood+tree->ind.sapwood*0.66667)*0.84)
                *(float)stand->frac*(float)pft->nind;
              biomass_image_agr[cell].branches+=(float)
                ((tree->ind.heartwood+tree->ind.sapwood*0.66667)*0.16)
                *(float)stand->frac*(float)pft->nind;
              biomass_image_agr[cell].leaves+=(float)
                tree->ind.leaf*(float)stand->frac*(float)pft->nind;
              biomass_image_agr[cell].roots+=(float)
                (tree->ind.root+tree->ind.sapwood*0.33333)*(float)stand->frac*(float)pft->nind;
              break;
            case GRASS:
              grass=pft->data;
              biomass_image_agr[cell].leaves+=(float)
                grass->ind.leaf*(float)stand->frac*(float)pft->nind;
              biomass_image_agr[cell].roots+=(float)
                grass->ind.root*(float)stand->frac*(float)pft->nind;
              break;
            case CROP:
              crop=pft->data;
              biomass_image_agr[cell].leaves+=(float)
                (crop->ind.leaf+crop->ind.so+crop->ind.pool)*(float)stand->frac*(float)pft->nind;
              biomass_image_agr[cell].roots+=(float)
                crop->ind.root*(float)stand->frac*(float)pft->nind;
              break;
          } /* of switch */
          biomass_image_agr[cell].litter+=(float)littersum(&stand->soil.litter)*
            (float)stand->frac;
          for(i=0;i<LASTLAYER;i++){
            biomass_image_agr[cell].humus+=(float)stand->soil.cpool[i].fast*(float)stand->frac;
            biomass_image_agr[cell].charcoal+=(float)stand->soil.cpool[i].slow*(float)stand->frac;
          }
        agrfrac+=(float)stand->frac;
        }
      } /* for each stand */
      biomass_image_agr[cell].product_fast=(float)grid[cell].ml.image_data->timber.fast;
      biomass_image_agr[cell].product_slow=(float)grid[cell].ml.image_data->timber.slow;
      agrfrac_image[cell]=agrfrac;

      // scale with total agr fraction (all but natural fraction)
      if(agrfrac>0.02)
      {
        biomass_image_agr[cell].stems/=agrfrac;
        biomass_image_agr[cell].roots/=agrfrac;
        biomass_image_agr[cell].leaves/=agrfrac;
        biomass_image_agr[cell].humus/=agrfrac;
        biomass_image_agr[cell].charcoal/=agrfrac;
        biomass_image_agr[cell].branches/=agrfrac;
        biomass_image_agr[cell].litter/=agrfrac;

        npp_image_agr[cell]=(npp_image[cell]-npp_image_nat[cell]-npp_image_wp[cell])/agrfrac;
        nep_image_agr[cell]=(nep_image[cell]-nep_image_nat[cell]-nep_image_wp[cell])/agrfrac;
#ifdef SENDSEP
        rh_image_agr[cell]=(rh_image[cell]-rh_image_nat[cell]- rh_image_wp[cell])/agrfrac;
        harvest_agric_image_agr[cell]=harvest_agric_image[cell]/agrfrac;
        harvest_biofuel_image_agr[cell]=harvest_biofuel_image[cell]/agrfrac;
#endif
        fireemission_deforest_image_agr[cell]=fireemission_deforest_image[cell]/agrfrac;
      }
      else
      {
        biomass_image_agr[cell].stems=biomass_image_agr[cell].roots=biomass_image_agr[cell].leaves=
        biomass_image_agr[cell].humus=biomass_image_agr[cell].charcoal=biomass_image_agr[cell].branches=
        biomass_image_agr[cell].litter=npp_image_agr[cell]=nep_image_agr[cell]=fireemission_deforest_image_agr[cell]=0.0;
#ifdef SENDSEP
        rh_image_agr[cell]=harvest_agric_image_agr[cell]=harvest_biofuel_image_agr[cell]=0.0;
#endif
      }

      if(natfrac>0.02)
      {
        biomass_image_nat[cell].stems/=natfrac;
        biomass_image_nat[cell].roots/=natfrac;
        biomass_image_nat[cell].leaves/=natfrac;
        biomass_image_nat[cell].humus/=natfrac;
        biomass_image_nat[cell].charcoal/=natfrac;
        biomass_image_nat[cell].branches/=natfrac;
        biomass_image_nat[cell].litter/=natfrac;
        fire_image_nat[cell]=fire_image[cell]/natfrac;
        npp_image_nat[cell]/=natfrac;
        nep_image_nat[cell]/=natfrac;
#ifdef SENDSEP
        harvest_timber_image_nat[cell]=harvest_timber_image[cell]/(natfrac+wpfrac);
        trad_biofuel_image_nat[cell]=trad_biofuel_image[cell]/(natfrac+wpfrac);
        rh_image_nat[cell]/=natfrac;
#endif
      }
      else
      {
        biomass_image_nat[cell].stems=biomass_image_nat[cell].roots=biomass_image_nat[cell].leaves=
        biomass_image_nat[cell].humus=biomass_image_nat[cell].charcoal=biomass_image_nat[cell].branches=
        biomass_image_nat[cell].litter=npp_image_nat[cell]=nep_image_nat[cell]=0.0;
#ifdef SENDSEP
        rh_image_nat[cell]=fire_image_nat[cell]=harvest_timber_image_nat[cell]=trad_biofuel_image_nat[cell]=0.0;
#endif
      }
      if(wpfrac>0.02)
      {
        biomass_image_wp[cell].stems/=wpfrac;
        biomass_image_wp[cell].roots/=wpfrac;
        biomass_image_wp[cell].leaves/=wpfrac;
        biomass_image_wp[cell].humus/=wpfrac;
        biomass_image_wp[cell].charcoal/=wpfrac;
        biomass_image_wp[cell].branches/=wpfrac;
        biomass_image_wp[cell].litter/=wpfrac;
        fire_image_wp[cell]=0.0; /* fire_image[cell] / wpfrac; */
        npp_image_wp[cell]/=wpfrac;
        nep_image_wp[cell]/=wpfrac;
#ifdef SENDSEP
        rh_image_wp[cell]/=wpfrac;
        harvest_timber_image_wp[cell]=harvest_timber_image[cell]/(natfrac+wpfrac);
        trad_biofuel_image_wp[cell]=trad_biofuel_image[cell]/(natfrac+wpfrac);
#endif
      }
      else
      {
        biomass_image_wp[cell].stems=biomass_image_wp[cell].roots=biomass_image_wp[cell].leaves=
          biomass_image_wp[cell].humus=biomass_image_wp[cell].charcoal=biomass_image_wp[cell].branches=
          biomass_image_wp[cell].litter=npp_image_wp[cell]=nep_image_wp[cell]=0.0;
#ifdef SENDSEP
        rh_image_wp[cell]=fire_image_wp[cell]=harvest_timber_image_wp[cell]=trad_biofuel_image_wp[cell]=0.0;
#endif
      }

         
      /* Set C-balance variables to 0 if the biome is ICE, formerly done in IMAGE -> interface_image.f */
      if(biome_image[cell]==IJS)
      {
        npp_image[cell]=npp_image_nat[cell]=npp_image_wp[cell]=npp_image_agr[cell]=0.0;
        nep_image[cell]=nep_image_nat[cell]=nep_image_wp[cell]=nep_image_agr[cell]=0.0;
        fire_image_nat[cell]=fire_image_wp[cell]=fire_image[cell]=0.0;
#ifdef SENDSEP
        harvest_agric_image_agr[cell]=harvest_agric_image[cell]=0.0;
        harvest_biofuel_image_agr[cell]=harvest_biofuel_image[cell]=0.0;
        rh_image[cell]=rh_image_nat[cell]=rh_image_wp[cell]=rh_image_agr[cell]=0.0;
#endif
        fireemission_deforest_image_agr[cell]=fireemission_deforest_image[cell]=0.0;
        biomass_image[cell].stems=biomass_image[cell].roots=biomass_image[cell].leaves=
        biomass_image[cell].humus=biomass_image[cell].charcoal=biomass_image[cell].branches=
        biomass_image[cell].litter=biomass_image[cell].product_fast=biomass_image[cell].product_slow=0.0;
        biomass_image_nat[cell].stems=biomass_image_nat[cell].roots=biomass_image_nat[cell].leaves=
        biomass_image_nat[cell].humus=biomass_image_nat[cell].charcoal=biomass_image_nat[cell].branches=
        biomass_image_nat[cell].litter=biomass_image_nat[cell].product_fast=biomass_image_nat[cell].product_slow=0.0;
        biomass_image_wp[cell].stems=biomass_image_wp[cell].roots=biomass_image_wp[cell].leaves=
        biomass_image_wp[cell].humus=biomass_image_wp[cell].charcoal=biomass_image_wp[cell].branches=
        biomass_image_wp[cell].litter=biomass_image_wp[cell].product_fast=biomass_image_wp[cell].product_slow=0.0;
        biomass_image_agr[cell].stems=biomass_image_agr[cell].roots=biomass_image_agr[cell].leaves=
        biomass_image_agr[cell].humus=biomass_image_agr[cell].charcoal=biomass_image_agr[cell].branches=
        biomass_image_agr[cell].litter=biomass_image_agr[cell].product_fast=biomass_image_agr[cell].product_slow=0.0;
      }
    } /* if skip */
  } //cells

  /* send data */
#ifdef USE_MPI
  /* The MPI part has to not been tested so far */
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,NBPOOLS,config->ntask);
  mpi_write_socket(config->out,biomass_image,MPI_FLOAT,
                   config->nall*NBPOOLS,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,biomass_image_nat,MPI_FLOAT,
                   config->nall*NBPOOLS,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,biomass_image_wp,MPI_FLOAT,
                   config->nall*NBPOOLS,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,biomass_image_agr,MPI_FLOAT,
                   config->nall*NBPOOLS,counts,offsets,config->rank,config->comm);
  getcounts(counts,offsets,config->nall,1,config->ntask);
  mpi_write_socket(config->out,biome_image,MPI_INT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,nep_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,nep_image_nat,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,nep_image_wp,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,nep_image_agr,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,npp_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,npp_image_nat,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,npp_image_wp,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,npp_image_agr,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
#ifdef SENDSEP
  mpi_write_socket(config->out,rh_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,rh_image_nat,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,rh_image_wp,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,rh_image_agr,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_agric_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_agric_image_agr,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_biofuel_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_biofuel_image_agr,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_timber_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_timber_image_nat,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_timber_image_wp,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,harvest_timber_image_wp,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
#endif
  mpi_write_socket(config->out,fire_image,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,fire_image_nat,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,fire_image_wp,MPI_FLOAT,config->nall,
                   counts,offsets,config->rank,config->comm);
  rc=mpi_write_socket(config->out,fireemission_deforest_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  rc=mpi_write_socket(config->out,fireemission_deforest_image_agr,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
#ifdef SENDSEP
  mpi_write_socket(config->out,product_turnover_fast_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,product_turnover_slow_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,trad_biofuel_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,trad_biofuel_image_nat,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  rc=mpi_write_socket(config->out,trad_biofuel_image_wp,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
#endif
/* sending yield data to interface -- needs to be read at the same position! */
  getcounts(counts,offsets,config->nall,ncrops,config->ntask);
  mpi_write_socket(config->out,yields,MPI_FLOAT,
                   config->nall*ncrops,counts,offsets,config->rank,config->comm);
  getcounts(counts,offsets,config->nall,1,config->ntask);
  mpi_write_socket(config->out,adischarge,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,nppgrass_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,natfrac_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,wpfrac_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,agrfrac_image,MPI_FLOAT,
                   config->nall,counts,offsets,config->rank,config->comm);
  getcounts(counts,offsets,config->nall,NMONTH,config->ntask);
  mpi_write_socket(config->out,monthirrig,MPI_FLOAT,
                   config->nall*NMONTH,counts,offsets,config->rank,config->comm);
  mpi_write_socket(config->out,monthevapotr,MPI_FLOAT,
                   config->nall*NMONTH,counts,offsets,config->rank,config->comm);
  rc=mpi_write_socket(config->out,monthpetim,MPI_FLOAT,
                      config->nall*NMONTH,counts,offsets,config->rank,config->comm);

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


#ifdef DEBUG_IMAGE_CELL
  for(cell=0;cell<config->ngridcell;cell++) {
        printf("lpjsend %i %g %i %g %g %g %g %g %g %g %g %g %g %g %g %g %g \n",
           cell, biomass_image[cell].branches, 
           biome_image[cell],nep_image[cell],npp_image[cell],rh_image[cell],
           harvest_agric_image[cell],harvest_biofuel_image[cell], 
           harvest_timber_image[cell], fire_image[cell],fireemission_deforest_image[cell],
           product_turnover_fast_image[cell], product_turnover_slow_image[cell],
           trad_biofuel_image[cell], yields[cell][0], adischarge[cell],nppgrass_image[cell]);

   }
#endif

#endif
   
  writefloat_socket(config->out, biomass_image, config->ngridcell*NBPOOLS);
  writefloat_socket(config->out, biomass_image_nat, config->ngridcell*NBPOOLS);
  writefloat_socket(config->out, biomass_image_wp , config->ngridcell*NBPOOLS);
  writefloat_socket(config->out, biomass_image_agr, config->ngridcell*NBPOOLS);
  writeint_socket(config->out, biome_image, config->ngridcell);
  writefloat_socket(config->out, nep_image, config->ngridcell);
  writefloat_socket(config->out, nep_image_nat, config->ngridcell);
  writefloat_socket(config->out, nep_image_wp , config->ngridcell);
  writefloat_socket(config->out, nep_image_agr, config->ngridcell);
  writefloat_socket(config->out, npp_image, config->ngridcell);
  writefloat_socket(config->out, npp_image_nat, config->ngridcell);
  writefloat_socket(config->out, npp_image_wp , config->ngridcell);
  writefloat_socket(config->out, npp_image_agr, config->ngridcell);

#ifdef SENDSEP
  writefloat_socket(config->out, rh_image, config->ngridcell);
  writefloat_socket(config->out, rh_image_nat, config->ngridcell);
  writefloat_socket(config->out, rh_image_wp , config->ngridcell);
  writefloat_socket(config->out, rh_image_agr, config->ngridcell);
  writefloat_socket(config->out, harvest_agric_image, config->ngridcell);
  writefloat_socket(config->out, harvest_agric_image_agr, config->ngridcell);
  writefloat_socket(config->out, harvest_biofuel_image, config->ngridcell);
  writefloat_socket(config->out, harvest_biofuel_image_agr, config->ngridcell);
  writefloat_socket(config->out, harvest_timber_image, config->ngridcell);
  writefloat_socket(config->out, harvest_timber_image_nat, config->ngridcell);
  writefloat_socket(config->out, harvest_timber_image_wp , config->ngridcell);
#endif
  writefloat_socket(config->out, fire_image, config->ngridcell);
  writefloat_socket(config->out, fire_image_nat, config->ngridcell);
  writefloat_socket(config->out, fire_image_wp , config->ngridcell);
  writefloat_socket(config->out, fireemission_deforest_image, config->ngridcell);
  writefloat_socket(config->out, fireemission_deforest_image_agr, config->ngridcell);
#ifdef SENDSEP
  writefloat_socket(config->out, product_turnover_fast_image, config->ngridcell);
  writefloat_socket(config->out, product_turnover_slow_image, config->ngridcell);
  writefloat_socket(config->out, trad_biofuel_image, config->ngridcell);
  writefloat_socket(config->out, trad_biofuel_image_nat, config->ngridcell);
  writefloat_socket(config->out, trad_biofuel_image_wp, config->ngridcell);
#endif
  /* sending yield data to interface -- needs to be read at the same position! */
  writefloat_socket(config->out,yields, ncrops*config->ngridcell); /*NCROPS should be made generic somewhere*/
  writefloat_socket(config->out, adischarge, config->ngridcell);
  writefloat_socket(config->out,nppgrass_image, config->ngridcell);
  writefloat_socket(config->out, natfrac_image, config->ngridcell);
  writefloat_socket(config->out, wpfrac_image, config->ngridcell);
  writefloat_socket(config->out, agrfrac_image, config->ngridcell);
  writefloat_socket(config->out, monthirrig,  (NMONTH)*config->ngridcell); 
  writefloat_socket(config->out, monthevapotr,(NMONTH)*config->ngridcell);
  rc = writefloat_socket(config->out, monthpetim,  (NMONTH)*config->ngridcell);

#ifdef DEBUG_IMAGE
  printf("done sending data\n");
  fflush(stdout);
#endif
#endif
  free(biomass_image);
  free(biomass_image_nat);
  free(biomass_image_wp);
  free(biomass_image_agr);
  free(biome_image);
  free(nep_image);
  free(nep_image_nat);
  free(nep_image_wp);
  free(nep_image_agr);
  free(npp_image);
  free(npp_image_nat);
  free(npp_image_wp);
  free(npp_image_agr);
  free(fire_image);
  free(fire_image_nat);
  free(fire_image_wp);
  free(fireemission_deforest_image);
  free(fireemission_deforest_image_agr);
  freemat((void **)yields);
  free(monthirrig);
  free(monthevapotr);
  free(monthpetim);
  free(adischarge);
  free(nppgrass_image);
  free(agrfrac_image);
  free(natfrac_image);
  free(wpfrac_image);

#ifdef SENDSEP
  free(harvest_agric_image);
  free(harvest_agric_image_agr);
  free(harvest_biofuel_image);
  free(harvest_biofuel_image_agr);
  free(product_turnover_fast_image);
  free(product_turnover_slow_image);
  free(harvest_timber_image);
  free(harvest_timber_image_nat);
  free(harvest_timber_image_wp);
  free(rh_image);
  free(rh_image_nat);
  free(rh_image_wp);
  free(rh_image_agr);
  free(trad_biofuel_image);
  free(trad_biofuel_image_nat);
  free(trad_biofuel_image_wp);
#endif

#ifdef DEBUG_IMAGE
  printf("done free data\n");
  fflush(stdout);
#endif
  return rc;
} /* of 'send_image_data' */

#endif
