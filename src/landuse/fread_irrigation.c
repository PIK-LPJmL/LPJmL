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

Bool fread_irrigation(Bstruct file,          /**< pointer to binary file */
                      const char *name,      /**< name of object */
                      Irrigation *irrigation /**< irrigation pointer */
                     )                       /** \return TRUE on error */
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(bstruct_readbool(file,"irrigation",&irrigation->irrigation))
    return TRUE;
  if(bstruct_readint(file,"pft_id",&irrigation->pft_id))
    return TRUE;
  if(irrigation->irrigation)
  {
    if(bstruct_readbool(file,"irrig_event",&irrigation->irrig_event))
      return TRUE;
    if(bstruct_readint(file,"irrig_system",(int *)(&irrigation->irrig_system)))
      return TRUE;
    if(bstruct_readreal(file,"ec",&irrigation->ec))
      return TRUE;
    if(bstruct_readreal(file,"conv_evap",&irrigation->conv_evap))
      return TRUE;
    if(bstruct_readreal(file,"net_irrig_amount",&irrigation->net_irrig_amount))
      return TRUE;
    if(bstruct_readreal(file,"dist_irrig_amount",&irrigation->dist_irrig_amount))
      return TRUE;
    if(bstruct_readreal(file,"irrig_amount",&irrigation->irrig_amount))
      return TRUE;
    if(bstruct_readreal(file,"irrig_stor",&irrigation->irrig_stor))
      return TRUE;
  }
  else
  {
    irrigation->irrig_event=FALSE;
    irrigation->irrig_system=NOIRRIG;
    irrigation->ec=1;
    irrigation->conv_evap=irrigation->net_irrig_amount=irrigation->dist_irrig_amount=irrigation->irrig_amount=irrigation->irrig_stor=0.0;
  }
  return bstruct_readendstruct(file);
} /* of 'fread_irrigation' */
