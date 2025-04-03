/**************************************************************************************/
/**                                                                                \n**/
/**        f  r  e  a  d  _  i  r  r  i  g  a  t  i  o  n  .  c                    \n**/
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

Bool fread_irrigation(FILE *file,             /**< pointer to binary file */
                      const char *name,       /**< name of object */
                      Irrigation *irrigation, /**< irrigation pointer */
                      Bool swap               /**< byte order has to be changed */
                     )                        /** \return TRUE on error */
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readbool(file,"irrigation",&irrigation->irrigation,swap))
    return TRUE;
  if(readint(file,"pft_id",&irrigation->pft_id,swap))
    return TRUE;
  if(irrigation->irrigation)
  {
    if(readbool(file,"irrig_event",&irrigation->irrig_event,swap))
      return TRUE;
    if(readint(file,"irrig_system",(int *)(&irrigation->irrig_system),swap))
      return TRUE;
    if(readreal(file,"ec",&irrigation->ec,swap))
      return TRUE;
    if(readreal(file,"conv_evap",&irrigation->conv_evap,swap))
      return TRUE;
    if(readreal(file,"net_irrig_amount",&irrigation->net_irrig_amount,swap))
      return TRUE;
    if(readreal(file,"dist_irrig_amount",&irrigation->dist_irrig_amount,swap))
      return TRUE;
    if(readreal(file,"irrig_amount",&irrigation->irrig_amount,swap))
      return TRUE;
    if(readreal(file,"irrig_stor",&irrigation->irrig_stor,swap))
      return TRUE;
  }
  else
  {
    irrigation->irrig_event=FALSE;
    irrigation->irrig_system=NOIRRIG;
    irrigation->ec=1;
    irrigation->conv_evap=irrigation->net_irrig_amount=irrigation->dist_irrig_amount=irrigation->irrig_amount=irrigation->irrig_stor=0.0;
  }
  return readendstruct(file);
} /* of 'fread_irrigation' */
