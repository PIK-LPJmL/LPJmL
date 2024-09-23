/**************************************************************************************/
/**                                                                                \n**/
/**               f  p  r  i  n  t  _  c  r  o  p  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes crop-specific data in ASCII format to file                 \n**/
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

void fprint_crop(FILE *file,    /**< pointer to text file */
                 const Pft *pft /**< pointer to PFT data to print */
                )
{
  const Pftcrop *crop;
  crop=pft->data;
  fprintf(file,"Wtyp:\t\t%s crop\n",(crop->wtype) ? "Winter" : "Summer");
  fprintf(file,"Growing days:\t%d\n",crop->growingdays);
  fprintf(file,"Pvd:\t\t%g\n",crop->pvd);
  fprintf(file,"Phu:\t\t%g\n",crop->phu);
  fprintf(file,"Basetemp:\t%g\n",crop->basetemp);
  fprintf(file,"Senescence:\t%d\n",crop->senescence);
  fprintf(file,"Senescence0:\t%d\n",crop->senescence0);
  fprintf(file,"Husum:\t\t%g\n",crop->husum);
  fprintf(file,"Vdsum:\t\t%g\n",crop->vdsum);
  fprintf(file,"Fphu:\t\t%g\n",crop->fphu);
  fprintf(file,"Ind:\t\t");
  fprintcropphys2(file,crop->ind,pft->nind);
  fprintf(file,"\nLAImax:\t%g\n",crop->flaimax);
  fprintf(file,"LAI:\t\t%g\n",crop->lai);
  fprintf(file,"LAI000:\t\t%g\n",crop->lai000);
  fprintf(file,"LAImax_adjusted:\t%g\n",crop->laimax_adjusted);
  fprintf(file,"Demandsum:\t%g\n",crop->demandsum);
  fprintf(file,"Ndemandsum:\t%g\n",crop->ndemandsum);
  fprintf(file,"Nuptakesum:\t%g\n",crop->nuptakesum);
  fprintf(file,"Nfertilizer:\t%g\n",crop->nfertilizer);
  fprintf(file,"Vscal_sum:\t%g\n",crop->vscal_sum);
  fprintf(file,"Supplysum:\t%g\n",crop->supplysum);
  if(crop->sh!=NULL)
  {
    fprintf(file,"Petsum:\t%g\n",crop->sh->petsum);
    fprintf(file,"Evapsum:\t%g\n",crop->sh->evapsum);
    fprintf(file,"Transpsum:\t%g\n",crop->sh->transpsum);
    fprintf(file,"Intercsum:\t%g\n",crop->sh->intercsum);
    fprintf(file,"Precsum:\t%g\n",crop->sh->precsum);
    fprintf(file,"Sradsum:\t%g\n",crop->sh->sradsum);
    fprintf(file,"Irrig_apply:\t%g\n",crop->sh->irrig_apply);
    fprintf(file,"Tempsum:\t%g\n",crop->sh->tempsum);
    fprintf(file,"Nirsum:\t%g\n",crop->sh->nirsum);
    fprintf(file,"LGP:\t%g\n",crop->sh->lgp);
    fprintf(file,"Runoffsum:\t%g\n",crop->sh->runoffsum);
    fprintf(file,"N2O_denitsum:\t%g\n",crop->sh->n2o_denitsum);
    fprintf(file,"N2O_nitsum:\t%g\n",crop->sh->n2o_nitsum);
    fprintf(file,"N2_emissum:\t%g\n",crop->sh->n2_emissum);
    fprintf(file,"Leachingsum:\t%g\n",crop->sh->leachingsum);
    fprintf(file,"C_emissum:\t%g\n",crop->sh->c_emissum);
    fprintf(file,"Sdate:\t\t%d\n",crop->sh->sdate);
    fprintf(file,"Sowing year:\t%d\n",crop->sh->sowing_year);
  }
} /* of 'fprint_crop' */
