/**************************************************************************************/
/**                                                                                \n**/
/**               c  h  e  c  k  b  a  s  e  t  e  m  p  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions compares base temp values from input file with valuse defined    \n**/
/**     in the PFT parameter file                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool checkbasetemp(const Limit basetemp[], /**< array of base temp limits to check */
                   int size,               /**< size of base temp array */
                   int npft,               /**< number of natural PFTs */
                   const Config *config    /**< LPjmL configuration */
                  )                        /** \return TRUE on failed check */
{
  const Pftcroppar *croppar;
  int cft;
  /* check for identical size */
  if(size!=config->crop_phumap_size)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR267: Size of basetemp array=%d in crop PHU file '%s' is not %d.\n",
              size,config->crop_phu_filename.name,config->crop_phumap_size);
    return TRUE;
  }
  /* check for indentical values */
  for(cft=0;cft<size;cft++)
  {
    croppar=config->pftpar[npft+config->crop_phumap[cft]].data;
    if(basetemp[cft].low!=croppar->basetemp.low)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR268: Low limit of basetemp=%g for '%s' in crop PHU file '%s' is not %g in PFT JSON file.\n",
                basetemp[cft].low,config->pftpar[npft+config->crop_phumap[cft]].name,
                config->crop_phu_filename.name,croppar->basetemp.low);
      return TRUE;
    }
    if(basetemp[cft].high!=croppar->basetemp.high)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR268: High limit of basetemp=%g for '%s' in crop PHU file '%s' is not %g in PFT JSON file.\n",
                basetemp[cft].high,config->pftpar[npft+config->crop_phumap[cft]].name,
                config->crop_phu_filename.name,croppar->basetemp.high);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'checkbasetemp' */
