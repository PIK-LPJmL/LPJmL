/**************************************************************************************/
/**                                                                                \n**/
/**             f  r  e  a  d  c  e  l  l  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads cell data from restart file                                 \n**/
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
#define readreal(file,name,value) if(bstruct_readreal(file,name,value)) return TRUE

Bool freadcell(Bstruct file,           /**< pointer to restart file */
               Cell *cell,             /**< Pointer to cell */
               int npft,               /**< number of natural PFTs */
               int ncft,               /**< number of crop PFTs */
               const Soilpar *soilpar, /**< pointer to soil parameter */
               const Standtype standtype[], /**< array of stand types */
               int nstand,             /**< number of stand types */
               Config *config          /**< LPJ configuration */
              )                        /** \return TRUE on error */
{
  int i;
  if(bstruct_readbeginstruct(file,NULL))
    return TRUE;
  if(bstruct_readbool(file,"skip",&cell->skip))
    return TRUE;
  if(freadseed(file,"seed",cell->seed))
  {
    fprintf(stderr,"ERROR254: Cannot read random seed.\n");
    return TRUE;
  }
  readreal(file,"dmass_lake",&cell->discharge.dmass_lake);
  if(config->river_routing)
  {
    if(config->river_routing_restart)
    {
      readreal(file,"dmass_gw",&cell->discharge.dmass_gw); // groundwater mass
      readreal(file,"dfout",&cell->discharge.dfout);
      readreal(file,"dmass_river",&cell->discharge.dmass_river);
      readreal(file,"dmass_sum",&cell->discharge.dmass_sum);
      cell->discharge.withdrawal=cell->discharge.withdrawal_gw=0;
#ifdef COUPLING_WITH_FMS
      readreal(file,"laketemp",&cell->laketemp);
#endif
      cell->discharge.queue=freadqueue(file,"queue");
      if(cell->discharge.queue==NULL)
      {
        fprintf(stderr,"ERROR254: Cannot read queue data.\n");
        return TRUE;
      }
      if(bstruct_readbool(file,"dam",&cell->ml.dam))
        return TRUE;
      if(cell->ml.dam)
      {
        if(freadresdata(file,"resdata",cell))
        {
          fprintf(stderr,"ERROR254: Cannot read reservoir data.\n");
          return TRUE;
        }
      }
    }
    else
    {
      cell->discharge.withdrawal=cell->discharge.withdrawal_gw=0;
#ifdef COUPLING_WITH_FMS
      cell->laketemp=0;
#endif
      cell->discharge.dfout=cell->discharge.dmass_river=cell->discharge.dmass_sum=cell->lateral_water=cell->NO3_lateral=0.0;
      cell->ml.dam=FALSE;
      cell->discharge.queue=NULL;
    }
  }
  if(!cell->skip)
  {
    /* cell has valid soilcode */
    readreal(file,"lateral_water",&cell->lateral_water);
    readreal(file,"NO3_lateral",&cell->NO3_lateral);
    if(freadstocksarray(file,"estab_storage_tree",cell->balance.estab_storage_tree,2))
      return TRUE;
    if(freadstocksarray(file,"estab_storage_grass",cell->balance.estab_storage_grass,2))
      return TRUE;
    if(freadignition(file,"ignition",&cell->ignition))
    {
      fprintf(stderr,"ERROR254: Cannot read ignition data.\n");
      return TRUE;
    }
    readreal(file,"excess_water",&cell->balance.excess_water);

    readreal(file,"waterdeficit",&cell->discharge.waterdeficit);
    cell->gdd=newgdd(npft);
    checkptr(cell->gdd);
    if(bstruct_readrealarray(file,"gdd",cell->gdd,npft))
    {
      fprintf(stderr,"ERROR254: Cannot read GDD data.\n");
      return TRUE;
    }
    /* read stand list */
    cell->standlist=freadstandlist(file,"standlist",cell,config->pftpar,npft+ncft,soilpar,
                                   standtype,nstand,config->separate_harvests);
    if(cell->standlist==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read stand list.\n");
      return TRUE;
    }
    readreal(file,"cropfrac_rf",&cell->ml.cropfrac_rf);
    readreal(file,"cropfrac_wl",&cell->ml.cropfrac_wl);
    readreal(file,"cropfrac_ir",&cell->ml.cropfrac_ir);
    cell->discharge.dmass_gw=(cell->ground_st+cell->ground_st_am)*cell->coord.area;;
    if(freadclimbuf(file,"climbuf",&cell->climbuf,ncft))
    {
      fprintf(stderr,"ERROR254: Cannot read climbuf data.\n");
      return TRUE;
    }
    cell->ml.cropdates=freadcropdates(file,"cropdates",ncft);
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
        if(bstruct_readintarray(file,"sdate_fixed",cell->ml.sdate_fixed,2*ncft))
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
        if(bstruct_readrealarray(file,"crop_phu_fixed",cell->ml.crop_phu_fixed,2*ncft))
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
    if(bstruct_readintarray(file,"sowing_month",cell->ml.sowing_month,2*ncft))
    {
      fprintf(stderr,"ERROR254: Cannot read sowing month data.\n");
      return TRUE;
    }
    cell->ml.gs=newvec(int,2*ncft);
    checkptr(cell->ml.gs);
    if(bstruct_readintarray(file,"gs",cell->ml.gs,2*ncft))
    {
      fprintf(stderr,"ERROR254: Cannot read gs data.\n");
      return TRUE;
    }
    if (freadhydrotope(file,"hydrotope", &cell->hydrotopes))
    {
      fprintf(stderr,"ERROR254: Cannot read hydrotope data.\n");
      return TRUE;
    }
    if(cell->ml.landfrac!=NULL && config->landuse_restart)
    {
      if(freadlandfrac(file,"landfrac",cell->ml.landfrac,ncft,config->nagtree))
      {
        fprintf(stderr,"ERROR254: Cannot read landfrac data.\n");
        return TRUE;
      }
#ifndef IMAGE
      if(freadpool(file,"product",&cell->ml.product))
      {
        fprintf(stderr,"ERROR254: Cannot read product pool.\n");
        return TRUE;
      }
#endif
    }
    if(cell->ml.fertilizer_nr!=NULL && config->landuse_restart)
    {
      if(freadlandfrac(file,"fertilizer_nr",cell->ml.fertilizer_nr,ncft,config->nagtree))
      {
        fprintf(stderr,"ERROR254: Cannot read fertilizer data.\n");
        return TRUE;
      }
    }
    if(config->ischeckpoint && config->n_out)
    {
      if(freadoutputdata(file,"outputdata",&cell->output,config))
      {
        fprintf(stderr,"ERROR254: Cannot read output data.\n");
        return TRUE;
      }
    }
  }
  return bstruct_readendstruct(file,NULL);
} /* of 'freadcell' */
