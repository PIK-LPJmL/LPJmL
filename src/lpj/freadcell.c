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
               Bool swap,              /**< Byte order has to be changed (TRUE/FALSE) */
               Config *config          /**< LPJ configuration */
              )                        /** \return TRUE on error */
{
  int i;
  Byte b;
  if(fread(&b,sizeof(b),1,file)!=1)
  {
    fprintf(stderr,"ERROR254: Cannot read skip flag.\n");
    return TRUE;
  }
  cell->skip=b;
  freadseed(file,cell->seed,swap);
  freadreal1(&cell->discharge.dmass_lake,swap,file);
  if(config->river_routing)
  {
#ifdef IMAGE
    freadreal1(&cell->discharge.dmass_gw,swap,file); // groundwater mass
#endif
    freadreal1(&cell->discharge.dfout,swap,file);
    freadreal1(&cell->discharge.dmass_river,swap,file);
    freadreal1(&cell->discharge.dmass_sum,swap,file);
#ifdef COUPLING_WITH_FMS
    freadreal1(&cell->laketemp,swap,file);
#endif
    cell->discharge.queue=freadqueue(file,swap);
    if(cell->discharge.queue==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read queue data.\n");
      return TRUE;
    }
    if(fread(&b,sizeof(b),1,file)!=1)
    {
      fprintf(stderr,"ERROR254: Cannot read dam flag.\n");
      return TRUE;
    }
    cell->ml.dam=b;
    if(cell->ml.dam)
    {
      if(freadresdata(file,cell,swap))
      {
        fprintf(stderr,"ERROR254: Cannot read reservoir data.\n");
        return TRUE;
      }
    }
  } 
  if(!cell->skip)
  {
    freadreal((Real *)cell->balance.estab_storage_tree,2*sizeof(Stocks)/sizeof(Real),swap,file);
    freadreal((Real *)cell->balance.estab_storage_grass,2*sizeof(Stocks)/sizeof(Real),swap,file);
    if(freadignition(file,&cell->ignition,swap))
    {
      fprintf(stderr,"ERROR254: Cannot read ignition data.\n");
      return TRUE;
    }
    freadreal1(&cell->balance.excess_water,swap,file);

    /* cell has valid soilcode */
    freadreal1(&cell->discharge.waterdeficit,swap,file);
    cell->gdd=newgdd(npft);
    checkptr(cell->gdd);
    if(freadreal(cell->gdd,npft,swap,file)!=npft)
    {
      fprintf(stderr,"ERROR254: Cannot read GDD data.\n");
      return TRUE;
    }
    /* read stand list */
    cell->standlist=freadstandlist(file,cell,config->pftpar,npft+ncft,soilpar,
                                   standtype,nstand,config->separate_harvests,swap);
    if(cell->standlist==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read stand list.\n");
      return TRUE;
    }
    freadreal1(&cell->ml.cropfrac_rf,swap,file);
    freadreal1(&cell->ml.cropfrac_ir,swap,file);
    if(freadclimbuf(file,&cell->climbuf,ncft,swap))
    {
      fprintf(stderr,"ERROR254: Cannot read climbuf data.\n");
      return TRUE;
    }
    cell->ml.cropdates=freadcropdates(file,ncft,swap);
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
        if(freadint(cell->ml.sdate_fixed,2*ncft,swap,file)!=2*ncft)
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
      if(config->sdate_option_restart)
        fseek(file,sizeof(int)*2*ncft,SEEK_CUR);
    }
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
    {
      cell->ml.crop_phu_fixed=newvec(Real,2*ncft);
      checkptr(cell->ml.crop_phu_fixed);
      if(config->crop_option_restart)
      {
        if(freadreal(cell->ml.crop_phu_fixed,2*ncft,swap,file)!=2*ncft)
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
      if(config->crop_option_restart)
        fseek(file,sizeof(Real)*2*ncft,SEEK_CUR);
    }
    cell->ml.sowing_month=newvec(int,2*ncft);
    checkptr(cell->ml.sowing_month);
    freadint(cell->ml.sowing_month,2*ncft,swap,file);
    cell->ml.gs=newvec(int,2*ncft);
    checkptr(cell->ml.gs);
    if(freadint(cell->ml.gs,2*ncft,swap,file)!=2*ncft)
    {
      fprintf(stderr,"ERROR254: Cannot read gs data.\n");
      return TRUE;
    }
    if(cell->ml.landfrac!=NULL && config->landuse_restart)
    {
      if(freadlandfrac(file,cell->ml.landfrac,ncft,config->nagtree,swap))
      {
        fprintf(stderr,"ERROR254: Cannot read landfrac data.\n");
        return TRUE;
      }
#ifndef IMAGE
      freadreal((Real *)&cell->ml.product,sizeof(Pool)/sizeof(Real),swap,file);
#endif
    }
    if(cell->ml.fertilizer_nr!=NULL && config->landuse_restart)
    {
      if(freadlandfrac(file,cell->ml.fertilizer_nr,ncft,config->nagtree,swap))
      {
        fprintf(stderr,"ERROR254: Cannot read fertilizer data.\n");
        return TRUE;
      }
    }
    if(config->ischeckpoint && config->n_out)
    {
      if(freadoutputdata(file,&cell->output,swap,config))
      {
        fprintf(stderr,"ERROR254: Cannot read output data.\n");
        return TRUE;
      }
    }
  }
  return FALSE;
} /* of 'freadcell' */
