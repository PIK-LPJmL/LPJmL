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

Bool fwrite_irrigation(FILE *file,        /**< pointer to binary file */
                       const Irrigation *irrigation /**< irrigation pointer */
                      )                   /** \return TRUE on error */
{
  Byte b;
  b=(Byte)irrigation->irrigation;
  fwrite(&b,sizeof(b),1,file);
  fwrite1(&irrigation->pft_id,sizeof(int),file);
  if(irrigation->irrigation)
  {
    fwrite1(&irrigation->irrig_event,sizeof(int),file);
    fwrite1((int *)(&irrigation->irrig_system),sizeof(int),file);
    fwrite1(&irrigation->ec,sizeof(Real),file);
    fwrite1(&irrigation->conv_evap,sizeof(Real),file);
    fwrite1(&irrigation->net_irrig_amount,sizeof(Real),file);
    fwrite1(&irrigation->dist_irrig_amount,sizeof(Real),file);
    fwrite1(&irrigation->irrig_amount,sizeof(Real),file);
    fwrite1(&irrigation->irrig_stor,sizeof(Real),file);
  }
  return FALSE;
} /* of 'fwrite_irrigation' */
