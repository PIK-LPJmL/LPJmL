/**************************************************************************************/
/**                                                                                \n**/
/**                  c  l  o  s  e  _  i  m  a  g  e  .  c                         \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Closes connection to IMAGE model                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef IMAGE

void close_image(const Config *config /**< LPJmL configuration */
                )
{
  if(config->rank==0)
  {
    if(config->in!=NULL)
      close_socket(config->in);
    if(config->out!=NULL)
    close_socket(config->out);
  }
} /* of 'close_image' */

#endif
