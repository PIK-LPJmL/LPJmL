/**************************************************************************************/
/**                                                                                \n**/
/**                i  s  a  n  n  u  a  l  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines whether output has a annual or longer time step        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool isannual(int index,          /**< index of output file */
             const Config *config /**< LPJmL configuration */
            )                     /** \return time step is annual or longer */
{
 return (config->outnames[index].timestep==ANNUAL ||  config->outnames[index].timestep>0);
} /* 'isannual' */
