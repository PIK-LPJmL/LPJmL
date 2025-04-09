/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  _  i  r  r  i  g  a  t  i  o  n  .  c                   \n**/
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

Bool fwrite_irrigation(Bstruct file,                /**< pointer to binary file */
                       const char *name,            /**< name of object */
                       const Irrigation *irrigation /**< irrigation pointer */
                      )                             /** \return TRUE on error */
{
  bstruct_writestruct(file,name);
  bstruct_writebool(file,"irrigation",irrigation->irrigation);
  bstruct_writeint(file,"pft_id",irrigation->pft_id);
  if(irrigation->irrigation)
  {
    bstruct_writebool(file,"irrig_event",irrigation->irrig_event);
    bstruct_writeint(file,"irrig_system",irrigation->irrig_system);
    bstruct_writereal(file,"ec",irrigation->ec);
    bstruct_writereal(file,"conv_evap",irrigation->conv_evap);
    bstruct_writereal(file,"net_irrig_amount",irrigation->net_irrig_amount);
    bstruct_writereal(file,"dist_irrig_amount",irrigation->dist_irrig_amount);
    bstruct_writereal(file,"irrig_amount",irrigation->irrig_amount);
    bstruct_writereal(file,"irrig_stor",irrigation->irrig_stor);
  }
  return bstruct_writeendstruct(file);
} /* of 'fwrite_irrigation' */
