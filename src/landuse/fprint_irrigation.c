/**************************************************************************************/
/**                                                                                \n**/
/**      f  p  r  i  n  t  _  i  r  r  i  g  a  t  i  o  n  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints irrigation data of stand                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprint_irrigation(FILE *file,                  /**< pointer to text file */
                       const Irrigation *irrigation /**< pointer to irrigation */
                      )
{
  const char *irrig[]={"no","surface","sprinkler","drip"};
  fprintf(file,"Irrigation:\t%s\n",bool2str(irrigation->irrigation));
  if(irrigation->irrigation)
    fprintf(file,"Irrigation event today:\t%s\n"
            "Irrigation system:\t%s\n"
            "conveyance efficiency:\t%g\n"
            "conveyance evaporation:\t%g\n"
            "net irrig amount:\t%g\n"
            "distribution irrig amount:\t%g\n"
            "irrig amount:\t%g\n"
            "irrig stor:\t%g\n",
            bool2str(irrigation->irrig_event),irrig[irrigation->irrig_system],
            irrigation->ec,irrigation->conv_evap,
            irrigation->net_irrig_amount,irrigation->dist_irrig_amount,
            irrigation->irrig_amount,irrigation->irrig_stor);
} /* of 'fprint_irrigation' */
