/**************************************************************************************/
/**                                                                                \n**/
/**              i  n  i  t  _  i  r  r  i  g  a  t  i  o  n  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes irrigation data                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void init_irrigation(Irrigation *irrigation)
{
  irrigation->irrigation=FALSE;
  irrigation->irrig_event=FALSE;
  irrigation->irrig_system=NOIRRIG;
  irrigation->ec=1;
  irrigation->conv_evap=irrigation->net_irrig_amount=irrigation->dist_irrig_amount=irrigation->irrig_amount=irrigation->irrig_stor=0.0;
} /* of 'init_irrigation' */
