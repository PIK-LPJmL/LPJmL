/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  l  i  t  t  e  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes litter variables into restart file                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static Bool fwritetrait(FILE *file,const char *name,const Trait *trait)
{
  writestruct(file,name);
  writestocks(file,"leaf",&trait->leaf);
  writestocksarray(file,"wood",trait->wood,NFUELCLASS);
  return writeendstruct(file);
} /* of ' fwritetrait' */

Bool fwritelitter(FILE *file,          /**< pointer to restart file */
                  const char *name,    /**< name of object */
                  const Litter *litter /**< pointer to litter data written */
                 )                     /** \return TRUE on error */
{
  int l;
  writerealarray(file,"avg_fbd",litter->avg_fbd,NFUELCLASS+1);
  writearray(file,name,litter->n);
  for(l=0;l<litter->n;l++)
  {
    writestruct(file,NULL);
    writeint(file,"pft_id",litter->item[l].pft->id);
    fwritetrait(file,"agtop",&litter->item[l].agtop);
    fwritetrait(file,"agsub",&litter->item[l].agsub);
    writestocks(file,"bg",&litter->item[l].bg);
    writeendstruct(file);
  }
  writeendarray(file);
  writereal(file,"agtop_wcap",litter->agtop_wcap);
  writereal(file,"agtop_moist",litter->agtop_moist);
  writereal(file,"agtop_cover",litter->agtop_cover);
  return writereal(file,"agtop_temp",litter->agtop_temp);
} /* of 'fwritelitter' */
