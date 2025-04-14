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

static Bool fwritetrait(Bstruct file,const char *name,const Trait *trait)
{
  bstruct_writestruct(file,name);
  fwritestocks(file,"leaf",&trait->leaf);
  fwritestocksarray(file,"wood",trait->wood,NFUELCLASS);
  return bstruct_writeendstruct(file);
} /* of ' fwritetrait' */

Bool fwritelitter(Bstruct file,        /**< pointer to restart file */
                  const char *name,    /**< name of object */
                  const Litter *litter /**< pointer to litter data written */
                 )                     /** \return TRUE on error */
{
  int l;
  bstruct_writerealarray(file,"avg_fbd",litter->avg_fbd,NFUELCLASS+1);
  bstruct_writearray(file,name,litter->n);
  for(l=0;l<litter->n;l++)
  {
    bstruct_writestruct(file,NULL);
    bstruct_writeint(file,"pft_id",litter->item[l].pft->id);
    fwritetrait(file,"agtop",&litter->item[l].agtop);
    fwritetrait(file,"agsub",&litter->item[l].agsub);
    fwritestocks(file,"bg",&litter->item[l].bg);
    bstruct_writeendstruct(file);
  }
  bstruct_writeendarray(file);
  bstruct_writereal(file,"agtop_wcap",litter->agtop_wcap);
  bstruct_writereal(file,"agtop_moist",litter->agtop_moist);
  bstruct_writereal(file,"agtop_cover",litter->agtop_cover);
  return bstruct_writereal(file,"agtop_temp",litter->agtop_temp);
} /* of 'fwritelitter' */
