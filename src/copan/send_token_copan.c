/**************************************************************************************/
/**                                                                                \n**/
/**               s  e  n  d  _  t  o  k  e  n  _  c  o  p  a  n  .  c             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes token into socket                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void send_token_copan(int token,           /**< Token (GET_DATA,PUT_DATA, ...) */
                      int index,           /**< index for in- or output stream */
                      const Config *config /**< LPJ configuration */
                     )
{
  writeint_socket(config->socket,&token,1);
  writeint_socket(config->socket,&index,1);
} /* of 'send_token_copan' */
