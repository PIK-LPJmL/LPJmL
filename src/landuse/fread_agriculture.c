/**************************************************************************************/
/**                                                                                \n**/
/**        f  r  e  a  d  _  a  g  r  i  c  u  l  t  u  r  e  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads irrigation data of stand                                    \n**/
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

Bool fread_agriculture(FILE *file,   /**< pointer to binary file */
                       Stand *stand, /**< stand pointer */
                       Bool swap     /**< byte order has to be changed */
                      )              /** \return TRUE on error */
{
  Irrigation *irrigation;
  irrigation=new(Irrigation);
  stand->data=irrigation;
  if(irrigation==NULL)
  {
    printallocerr("agriculture");
    return TRUE;
  }
  if(fread_irrigation(file,irrigation,swap))
    return TRUE;
  return freadint1(&stand->growing_days,swap,file)!=1;
} /* of 'fread_agriculture' */
