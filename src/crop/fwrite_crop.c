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

Bool fwrite_crop(FILE *file,    /**< pointer to file data */
                 const Pft *pft /**< pointer to PFT data */
                )               /** \return TRUE on error */
{
  const Pftcrop *crop;
  crop=pft->data;
  writereal(file,"nlimit",pft->nlimit);
  writeint(file,"wtype",crop->wtype);
  writeint(file,"growingdays",crop->growingdays);
  writereal(file,"pvd",crop->pvd);
  writereal(file,"phu",crop->phu);
  writereal(file,"basetemp",crop->basetemp);
  writeint(file,"senescence",crop->senescence);
  writeint(file,"senescence0",crop->senescence0);
  writereal(file,"husum",crop->husum);
  writereal(file,"vdsum",crop->vdsum);
  writereal(file,"fphu",crop->fphu);
  writestruct(file,"ind");
  writestocks(file,"leaf",&crop->ind.leaf);
  writestocks(file,"root",&crop->ind.root);
  writestocks(file,"pool",&crop->ind.pool);
  writestocks(file,"so",&crop->ind.so);
  writeendstruct(file);
  writereal(file,"flaimax",crop->flaimax);
  writereal(file,"lai",crop->lai);
  writereal(file,"lai000",crop->lai000);
  writereal(file,"laimax_adjusted",crop->laimax_adjusted);
  writereal(file,"lai_nppdeficit",crop->lai_nppdeficit);
  writereal(file,"demandsum",crop->demandsum);
  writereal(file,"ndemandsum",crop->ndemandsum);
  writereal(file,"nuptakesum",crop->nuptakesum);
  writereal(file,"nfertilizer",crop->nfertilizer);
  writereal(file,"nmanure",crop->nmanure);
  writereal(file,"vscal_sum",crop->vscal_sum);
  if(crop->sh!=NULL)
  {
    writestruct(file,"sh");
    writereal(file,"petsum",crop->sh->petsum);
    writereal(file,"evapsum",crop->sh->evapsum);
    writereal(file,"transpsum",crop->sh->transpsum);
    writereal(file,"intercsum",crop->sh->intercsum);
    writereal(file,"precsum",crop->sh->precsum);
    writereal(file,"sradsum",crop->sh->sradsum);
    writereal(file,"irrig_apply",crop->sh->irrig_apply);
    writereal(file,"tempsum",crop->sh->tempsum);
    writereal(file,"nirsum",crop->sh->nirsum);
    writereal(file,"lgp",crop->sh->lgp);
    writereal(file,"runoffsum",crop->sh->runoffsum);
    writereal(file,"n2o_denitsum",crop->sh->n2o_denitsum);
    writereal(file,"n2o_nitsum",crop->sh->n2o_nitsum);
    writereal(file,"n2_emissum",crop->sh->n2_emissum);
    writereal(file,"leachingsum",crop->sh->leachingsum);
    writereal(file,"c_emissum",crop->sh->c_emissum);
    writereal(file,"nfertsum",crop->sh->nfertsum);
    writeint(file,"sdate",crop->sh->sdate);
    writeint(file,"sowing_year",crop->sh->sowing_year);
    writeendstruct(file);
  }
  return writereal(file,"supplysum",crop->supplysum);
} /* of 'fwrite_crop' */
