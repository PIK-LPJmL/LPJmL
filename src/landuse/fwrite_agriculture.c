/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  _  a  g  r  i  c  u  l  t  u  r  e  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes irrigation data of stand                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"

Bool fwrite_agriculture(FILE *file,        /**< pointer to binary file */
                        const Stand *stand /**< stand pointer */
                       )                   /** \return TRUE on error */
{
  Irrigation *irrigation;
  irrigation=stand->data;
  fwrite_irrigation(file,irrigation);
  fwrite1(&stand->growing_days,sizeof(int),file);
  return FALSE;
} /* of 'fwrite_agriculture' */
