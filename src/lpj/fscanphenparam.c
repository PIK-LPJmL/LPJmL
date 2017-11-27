/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  h  e  n  p  a  r  a  m  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads phenology parameter from configuration file                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanphenparam(FILE *file,          /**< pointer to text file */
                    Phen_param *phenpar, /**< on return phenology params read */
                    Verbosity verb       /**< verbosity level (NO_ERR,ERR,VERB) */
                   )                     /** \return TRUE on error */
{
  if(fscanreal(file,&phenpar->sl,"sl param",verb))
    return TRUE;
  if(fscanreal(file,&phenpar->base,"base param",verb))
    return TRUE;
  if(fscanreal(file,&phenpar->tau,"tau param",verb))
    return TRUE;
  return FALSE;
} /* of 'fscanphenparam' */
