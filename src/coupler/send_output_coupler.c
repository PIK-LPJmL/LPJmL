/**************************************************************************************/
/**                                                                                \n**/
/**            s  e  n  d  _  o  u  t  p  u  t  _  c  o  u  p  l  e  r  .  c       \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online                                      \n**/
/**     Send output stream header to coupled model                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void send_output_coupler(int index,           /**< index of output stream */
                         int year,            /**< year */
                         int step,            /**< time step within year */
                         const Config *config /**< LPJmL configuration */
                        )
{
  if(isroot(*config))
  {
    send_token_coupler(PUT_DATA,index,config);
    writeint_socket(config->socket,&year,1);
#if COUPLER_VERSION == 4
    writeint_socket(config->socket,&step,1);
#endif
  }
} /* of 'send_output_coupler' */
