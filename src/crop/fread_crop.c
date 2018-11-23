/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  a  d  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads crop-specific variables from binary file                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

Bool fread_crop(FILE *file, /**< file pointer */
                Pft *pft,   /**< PFT data to be read */
                Bool swap   /**< if true data is in different byte order */
               )            /** \return TRUE on error */
{
  Pftcrop *crop;
  crop=new(Pftcrop);
  pft->data=crop;
  if(crop==NULL)
  {
    printallocerr("crop");
    return TRUE;
  }
  freadint1(&crop->wtype,swap,file);
  freadint1(&crop->growingdays,swap,file);
  freadreal1(&crop->pvd,swap,file);
  freadreal1(&crop->phu,swap,file);
  freadreal1(&crop->basetemp,swap,file);
  freadint1(&crop->senescence,swap,file);
  freadint1(&crop->senescence0,swap,file);
  freadreal1(&crop->husum,swap,file);
  freadreal1(&crop->vdsum,swap,file);
  freadreal1(&crop->fphu,swap,file);
  freadreal((Real *)&crop->ind,sizeof(Cropphys2)/sizeof(Real),swap,file);
  freadreal1(&crop->flaimax,swap,file);
  freadreal1(&crop->lai,swap,file);
  freadreal1(&crop->lai000,swap,file);
  freadreal1(&crop->laimax_adjusted,swap,file);
  freadreal1(&crop->lai_nppdeficit,swap,file);
  freadreal1(&crop->demandsum,swap,file);
#ifdef DOUBLE_HARVEST
  freadreal1(&crop->supplysum,swap,file);
  freadreal1(&crop->petsum,swap,file);
  freadreal1(&crop->evapsum,swap,file);
  freadreal1(&crop->transpsum,swap,file);
  freadreal1(&crop->intercsum,swap,file);
  freadreal1(&crop->precsum,swap,file);
  freadreal1(&crop->sradsum,swap,file);
  freadreal1(&crop->pirrww,swap,file);
  freadreal1(&crop->tempsum,swap,file);
  freadreal1(&crop->nirsum,swap,file);
  freadreal1(&crop->lgp,swap,file);
  freadint1(&crop->sdate,swap,file);
  return freadint1(&crop->sowing_year,swap,file)!=1;
#else
  return freadreal1(&crop->supplysum,swap,file)!=1;
#endif
} /* of 'fread_crop' */
