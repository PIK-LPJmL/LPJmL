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
                      Irrigation *irrigation, /**< irrigation pointer */
                      Bool swap               /**< byte order has to be changed */
                     )                        /** \return TRUE on error */
{
  Byte b;
  if(fread(&b,sizeof(b),1,file)!=1)
    return TRUE;
  if(b>1)
  {
    fprintf(stderr,"ERROR195: Invalid value %d for irrigation, must be 0 or 1.\n",b);
    return TRUE;
  }
  irrigation->irrigation=b;
  freadint1(&irrigation->pft_id,swap,file);
  if(irrigation->irrigation)
  {
    freadint1(&irrigation->irrig_event,swap,file);
    freadint1((int *)(&irrigation->irrig_system),swap,file);
    freadreal1(&irrigation->ec,swap,file);
    freadreal1(&irrigation->conv_evap,swap,file);
    freadreal1(&irrigation->net_irrig_amount,swap,file);
    freadreal1(&irrigation->dist_irrig_amount,swap,file);
    freadreal1(&irrigation->irrig_amount,swap,file);
    return freadreal1(&irrigation->irrig_stor,swap,file)!=1;
  }
  else
  {
    irrigation->irrig_event=FALSE;
    irrigation->irrig_system=NOIRRIG;
    irrigation->ec=1;
    irrigation->conv_evap=irrigation->net_irrig_amount=irrigation->dist_irrig_amount=irrigation->irrig_amount=irrigation->irrig_stor=0.0;
  }
  return FALSE;
} /* of 'fread_irrigation' */
