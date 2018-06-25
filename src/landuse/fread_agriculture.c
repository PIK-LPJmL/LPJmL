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
  Byte b;
  Irrigation *irrigation;
  irrigation=new(Irrigation);
  stand->data=irrigation;
  if(irrigation==NULL)
    return TRUE;
  fread(&b,sizeof(b),1,file);
  irrigation->irrigation=b;
  freadint1(&irrigation->irrig_event,swap,file);
  freadint1(&irrigation->irrig_system,swap,file);
  freadreal1(&irrigation->ec,swap,file);
  freadreal1(&irrigation->conv_evap,swap,file);
  freadreal1(&irrigation->net_irrig_amount,swap,file);
  freadreal1(&irrigation->dist_irrig_amount,swap,file);
  freadreal1(&irrigation->irrig_amount,swap,file);
  freadreal1(&irrigation->irrig_stor,swap,file);
  freadint1(&stand->growing_time,swap,file);
  freadint1(&stand->growing_days,swap,file);
  return freadint1(&stand->age,swap,file)!=1;
} /* of 'fread_agriculture' */
