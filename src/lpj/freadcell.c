/**************************************************************************************/
/**                                                                                \n**/
/**             f  r  e  a  d  c  e  l  l  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads cell data from file                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}
#define readreal2(file,name,value,swap) if(readreal(file,name,value,swap)) return TRUE

Bool freadcell(FILE *file,             /**< File pointer to restart file */
               Cell *cell,             /**< Pointer to cell */
               int npft,               /**< number of natural PFTs */
               int ncft,               /**< number of crop PFTs */
               const Soilpar *soilpar, /**< pointer to soil parameter */
               const Standtype standtype[], /**< array of stand types */
               int nstand,             /**< number of stand types */
               Bool swap,              /**< Byte order has to be changed (TRUE/FALSE) */
               Config *config          /**< LPJ configuration */
              )                        /** \return TRUE on error */
{
  int i;
  if(readstruct(file,NULL,swap))
    return TRUE;
  if(readbool(file,"skip",&cell->skip,swap))
    return TRUE;
  if(readseed(file,"seed",cell->seed,swap))
    return TRUE;
  readreal2(file,"dmass_lake",&cell->discharge.dmass_lake,swap);
  if(config->river_routing)
  {
#ifdef IMAGE
    readreal2(file,"dmass_gw",&cell->discharge.dmass_gw,swap); // groundwater mass
#endif
    readreal2(file,"dfout",&cell->discharge.dfout,swap);
    readreal2(file,"dmass_river",&cell->discharge.dmass_river,swap);
    readreal2(file,"dmass_sum",&cell->discharge.dmass_sum,swap);
#ifdef COUPLING_WITH_FMS
    readreal2(file,"laketemp",&cell->laketemp,swap);
#endif
    cell->discharge.queue=freadqueue(file,"queue",swap);
    if(cell->discharge.queue==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read queue data.\n");
      return TRUE;
    }
    if(readbool(file,"dam",&cell->ml.dam,swap))
      return TRUE;
    if(cell->ml.dam)
    {
      if(freadresdata(file,"resdata",cell,swap))
      {
        fprintf(stderr,"ERROR254: Cannot read reservoir data.\n");
        return TRUE;
      }
    }
  } 
  if(!cell->skip)
  {
    if(readstocks(file,"estab_storage_tree_rf",cell->balance.estab_storage_tree,swap))
      return TRUE;
    if(readstocks(file,"estab_storage_tree_ir",cell->balance.estab_storage_tree+1,swap))
      return TRUE;
    if(readstocks(file,"estab_storage_grass_rf",cell->balance.estab_storage_grass,swap))
      return TRUE;
    if(readstocks(file,"estab_storage_grass_ir",cell->balance.estab_storage_grass+1,swap))
      return TRUE;
    if(freadignition(file,"ignition",&cell->ignition,swap))
    {
      fprintf(stderr,"ERROR254: Cannot read ignition data.\n");
      return TRUE;
    }
    readreal2(file,"excess_water",&cell->balance.excess_water,swap);

    /* cell has valid soilcode */
    readreal2(file,"waterdeficit",&cell->discharge.waterdeficit,swap);
    cell->gdd=newgdd(npft);
    checkptr(cell->gdd);
    if(readrealarray(file,"gdd",cell->gdd,npft,swap))
    {
      fprintf(stderr,"ERROR254: Cannot read GDD data.\n");
      return TRUE;
    }
    /* read stand list */
    cell->standlist=freadstandlist(file,"standlist",cell,config->pftpar,npft+ncft,soilpar,
                                   standtype,nstand,config->separate_harvests,swap);
    if(cell->standlist==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read stand list.\n");
      return TRUE;
    }
    readreal2(file,"cropfrac_rf",&cell->ml.cropfrac_rf,swap);
    readreal2(file,"cropfrac_ir",&cell->ml.cropfrac_ir,swap);
    if(freadclimbuf(file,"climbuf",&cell->climbuf,ncft,swap))
    {
      fprintf(stderr,"ERROR254: Cannot read climbuf data.\n");
      return TRUE;
    }
    cell->ml.cropdates=freadcropdates(file,"cropdates",ncft,swap);
    if(cell->ml.cropdates==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read crop dates.\n");
      return TRUE;
    }
    if(config->sdate_option>NO_FIXED_SDATE)
    {
      cell->ml.sdate_fixed=newvec(int,2*ncft);
      checkptr(cell->ml.sdate_fixed);
      if(config->sdate_option_restart>NO_FIXED_SDATE)
      {
        if(readintarray(file,"sdate_fixed",cell->ml.sdate_fixed,2*ncft,swap))
        {
          fprintf(stderr,"ERROR254: Cannot read sowing date data.\n");
          return TRUE;
        }
      }
      else
        for(i=0; i<2*ncft; i++)
          cell->ml.sdate_fixed[i]=0;
    }
    else
    {
      cell->ml.sdate_fixed=NULL;
    }
    if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
    {
      cell->ml.crop_phu_fixed=newvec(Real,2*ncft);
      checkptr(cell->ml.crop_phu_fixed);
      if(config->crop_phu_option_restart)
      {
        if(readrealarray(file,"crop_phu_fixed",cell->ml.crop_phu_fixed,2*ncft,swap))
        {
          fprintf(stderr,"ERROR254: Cannot read PHU data.\n");
          return TRUE;
        }
      }
      else
        for(i=0; i<2*ncft; i++)
          cell->ml.crop_phu_fixed[i]=0;
    }
    else
    {
      cell->ml.crop_phu_fixed=NULL;
    }
    cell->ml.sowing_month=newvec(int,2*ncft);
    checkptr(cell->ml.sowing_month);
    if(readintarray(file,"sowing_month",cell->ml.sowing_month,2*ncft,swap))
      return TRUE;
    cell->ml.gs=newvec(int,2*ncft);
    checkptr(cell->ml.gs);
    if(readintarray(file,"gs",cell->ml.gs,2*ncft,swap))
    {
      fprintf(stderr,"ERROR254: Cannot read gs data.\n");
      return TRUE;
    }
    if(cell->ml.landfrac!=NULL && config->landuse_restart)
    {
      if(freadlandfrac(file,"landfrac",cell->ml.landfrac,ncft,config->nagtree,swap))
      {
        fprintf(stderr,"ERROR254: Cannot read landfrac data.\n");
        return TRUE;
      }
#ifndef IMAGE
      if(readstruct(file,"product",swap))
        return TRUE;
      if(readstocks(file,"slow",&cell->ml.product.slow,swap))
        return TRUE;
      if(readstocks(file,"fast",&cell->ml.product.fast,swap))
        return TRUE;
      if(readendstruct(file))
        return TRUE;
#endif
    }
    if(cell->ml.fertilizer_nr!=NULL && config->landuse_restart)
    {
      if(freadlandfrac(file,"fertilizer_nr",cell->ml.fertilizer_nr,ncft,config->nagtree,swap))
      {
        fprintf(stderr,"ERROR254: Cannot read fertilizer data.\n");
        return TRUE;
      }
    }
    if(config->ischeckpoint && config->n_out)
    {
      if(freadoutputdata(file,"outputdata",&cell->output,swap,config))
      {
        fprintf(stderr,"ERROR254: Cannot read output data.\n");
        return TRUE;
      }
    }
  }
  return readendstruct(file);;
} /* of 'freadcell' */
