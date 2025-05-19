/**************************************************************************************/
/**                                                                                \n**/
/**             f  w  r  i  t  e  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes crop specific variables into binary file                   \n**/
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

Bool fwrite_crop(Bstruct file,  /**< pointer to file data */
                 const Pft *pft /**< pointer to PFT data */
                )               /** \return TRUE on error */
{
  const Pftcrop *crop;
  crop=pft->data;
  bstruct_writereal(file,"nlimit",pft->nlimit);
  bstruct_writeint(file,"wtype",crop->wtype);
  bstruct_writeint(file,"growingdays",crop->growingdays);
  bstruct_writereal(file,"pvd",crop->pvd);
  bstruct_writereal(file,"phu",crop->phu);
  bstruct_writereal(file,"basetemp",crop->basetemp);
  bstruct_writeint(file,"senescence",crop->senescence);
  bstruct_writeint(file,"senescence0",crop->senescence0);
  bstruct_writereal(file,"husum",crop->husum);
  bstruct_writereal(file,"vdsum",crop->vdsum);
  bstruct_writereal(file,"fphu",crop->fphu);
  bstruct_writebeginstruct(file,"ind");
  fwritestocks(file,"leaf",&crop->ind.leaf);
  fwritestocks(file,"root",&crop->ind.root);
  fwritestocks(file,"pool",&crop->ind.pool);
  fwritestocks(file,"so",&crop->ind.so);
  bstruct_writeendstruct(file);
  bstruct_writereal(file,"flaimax",crop->flaimax);
  bstruct_writereal(file,"lai",crop->lai);
  bstruct_writereal(file,"lai000",crop->lai000);
  bstruct_writereal(file,"laimax_adjusted",crop->laimax_adjusted);
  bstruct_writereal(file,"lai_nppdeficit",crop->lai_nppdeficit);
  bstruct_writereal(file,"demandsum",crop->demandsum);
  bstruct_writereal(file,"ndemandsum",crop->ndemandsum);
  bstruct_writereal(file,"nuptakesum",crop->nuptakesum);
  bstruct_writereal(file,"nfertilizer",crop->nfertilizer);
  bstruct_writereal(file,"nmanure",crop->nmanure);
  bstruct_writereal(file,"vscal_sum",crop->vscal_sum);
  if(crop->sh!=NULL)
  {
    bstruct_writebeginstruct(file,"sh");
    bstruct_writereal(file,"petsum",crop->sh->petsum);
    bstruct_writereal(file,"evapsum",crop->sh->evapsum);
    bstruct_writereal(file,"transpsum",crop->sh->transpsum);
    bstruct_writereal(file,"intercsum",crop->sh->intercsum);
    bstruct_writereal(file,"precsum",crop->sh->precsum);
    bstruct_writereal(file,"sradsum",crop->sh->sradsum);
    bstruct_writereal(file,"irrig_apply",crop->sh->irrig_apply);
    bstruct_writereal(file,"tempsum",crop->sh->tempsum);
    bstruct_writereal(file,"nirsum",crop->sh->nirsum);
    bstruct_writereal(file,"lgp",crop->sh->lgp);
    bstruct_writereal(file,"runoffsum",crop->sh->runoffsum);
    bstruct_writereal(file,"n2o_denitsum",crop->sh->n2o_denitsum);
    bstruct_writereal(file,"n2o_nitsum",crop->sh->n2o_nitsum);
    bstruct_writereal(file,"n2_emissum",crop->sh->n2_emissum);
    bstruct_writereal(file,"leachingsum",crop->sh->leachingsum);
    bstruct_writereal(file,"c_emissum",crop->sh->c_emissum);
    bstruct_writereal(file,"nfertsum",crop->sh->nfertsum);
    bstruct_writeint(file,"sdate",crop->sh->sdate);
    bstruct_writeint(file,"sowing_year",crop->sh->sowing_year);
    bstruct_writeendstruct(file);
  }
  return bstruct_writereal(file,"supplysum",crop->supplysum);
} /* of 'fwrite_crop' */
