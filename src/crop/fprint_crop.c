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
  fprintf(file,"Supplysum:\t%g\n",crop->supplysum);
} /* of 'fprint_crop' */
