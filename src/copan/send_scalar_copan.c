/**************************************************************************************/
/**                                                                                \n**/
/**             s  e  n  d  _  s  c  a  l  a  r  _  c  o  p  a  n  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes global values into socket                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool send_scalar_copan(int index,           /**< index of output stream */
                       const void *data,    /**< data sent */
                       Type type,           /**< datatype of stream */
                       int size,            /**< number of items */
                       int year,            /**< Simulation year (AD) */
                       const Config *config /**< LPJ configuration */
                      )                     /** \return TRUE on error */
{
  int date=0;
  send_token_copan(PUT_DATA,index,config);
  writeint_socket(config->socket,&year,1);
#if COPAN_COUPLER_VERSION == 4
  writeint_socket(config->socket,&date,1);
#endif
  return write_socket(config->socket,data,typesizes[type]*size);
} /* of 'send_scalar_copan' */
