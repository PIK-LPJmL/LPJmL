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

Bool freadcell(FILE *file,             /**< File pointer to binary file */
               Cell *cell,             /**< Pointer to cell */
               int npft,               /**< number of natural PFTs */
               int ncft,               /**< number of crop PFTs */
               const Soilpar *soilpar, /**< pointer to soil parameter */
               const Standtype standtype[], /**< array of stand types */
               int nstand,             /**< number of stand types */
               Bool swap, /**< Byte order has to be changed (TRUE/FALSE) */
               const Config *config    /**< LPJ configuration */
              )                        /** \return TRUE on error */
{
  int i;
  Byte b;
  if(fread(&b,sizeof(b),1,file)!=1)
    return TRUE;
  cell->skip=b;
  if(config->river_routing)
  {
    freadreal1(&cell->discharge.dmass_lake,swap,file);
    freadreal1(&cell->discharge.dfout,swap,file);
    freadreal1(&cell->discharge.dmass_river,swap,file);
    freadreal1(&cell->discharge.dmass_sum,swap,file);
#ifdef COUPLING_WITH_FMS
    freadreal1(&cell->laketemp,swap,file);
#endif
    cell->discharge.queue=freadqueue(file,swap);
    if(cell->discharge.queue==NULL)
      return TRUE;
    if(fread(&b,sizeof(b),1,file)!=1)
      return TRUE;
    cell->ml.dam=b;
    if(cell->ml.dam)
    {
      if(freadresdata(file,cell,swap))
        return TRUE;
    }
  } 
  if(!cell->skip)
  {
    freadreal((Real *)cell->balance.estab_storage_tree,2*sizeof(Stocks)/sizeof(Real),swap,file);
    freadreal((Real *)cell->balance.estab_storage_grass,2*sizeof(Stocks)/sizeof(Real),swap,file);
    freadreal1(&cell->balance.excess_water,swap,file);

    /* cell has valid soilcode */
    freadreal1(&cell->discharge.waterdeficit,swap,file);
    cell->gdd=newgdd(npft);
    checkptr(cell->gdd);
    initgdd(cell->gdd,npft);
    /* read stand list */
    cell->standlist=freadstandlist(file,cell,config->pftpar,npft+ncft,soilpar,
      standtype,nstand,swap);
    if(cell->standlist==NULL)
      return TRUE;
    freadreal1(&cell->ml.cropfrac_rf,swap,file);
    freadreal1(&cell->ml.cropfrac_ir,swap,file);
    freadreal1(&cell->ml.abandon_rf,swap,file);
    freadreal1(&cell->ml.abandon_ir,swap,file);
    freadclimbuf(file,&cell->climbuf,swap);
    cell->ml.cropdates=freadcropdates(file,ncft,swap);
    if(cell->ml.cropdates==NULL)
      return TRUE;
    if(config->sdate_option>NO_FIXED_SDATE)
    {
      cell->ml.sdate_fixed=newvec(int,2*ncft);
      checkptr(cell->ml.sdate_fixed);
      if(config->sdate_option_restart>NO_FIXED_SDATE)
        freadint(cell->ml.sdate_fixed,2*ncft,swap,file);
      else
        for(i=0; i<2*ncft; i++)
          cell->ml.sdate_fixed[i]=0;
    }
    else
      cell->ml.sdate_fixed=NULL;
    if(config->crop_phu_option>SEMISTATIC_CROP_PHU)
    {
      cell->ml.crop_phu_fixed=newvec(Real,2*ncft);
      checkptr(cell->ml.crop_phu_fixed);
      if(config->sdate_option_restart>SEMISTATIC_CROP_PHU)
        freadreal(cell->ml.crop_phu_fixed,2*ncft,swap,file);
      else
        for(i=0; i<2*ncft; i++)
          cell->ml.crop_phu_fixed[i]=0;
    }
    else
      cell->ml.crop_phu_fixed=NULL;
    cell->ml.sowing_month=newvec(int,2*ncft);
    checkptr(cell->ml.sowing_month);
    freadint(cell->ml.sowing_month,2*ncft,swap,file);
    cell->ml.gs=newvec(int,2*ncft);
    checkptr(cell->ml.gs);
    if(freadint(cell->ml.gs,2*ncft,swap,file)!=2*ncft)
      return TRUE;
    if(cell->ml.landfrac!=NULL && config->landuse_restart)
      freadlandfrac(file,cell->ml.landfrac,ncft,swap);
    if(cell->ml.fertilizer_nr!=NULL && config->landuse_restart)
      freadlandfrac(file,cell->ml.fertilizer_nr,ncft,swap);
  }
  return FALSE;
} /* of 'freadcell' */
