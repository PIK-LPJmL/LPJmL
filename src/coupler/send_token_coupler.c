/**************************************************************************************/
/**                                                                                \n**/
/**               s  e  n  d  _  t  o  k  e  n  _  c  o  u  p  l  e  r  .  c       \n**/
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

char *token_names[]={"GET_DATA","PUT_DATA","GET_DATA_SIZE","PUT_DATA_SIZE",
                     "END_DATA","GET_STATUS","FAIL_DATA","PUT_INIT_DATA"};

Bool send_token_coupler(Token token,         /**< Token (GET_DATA,PUT_DATA, ...) */
                        int index,           /**< index for in- or output stream */
                        const Config *config /**< LPJ configuration */
                       )                     /** \return TRUE on error */
{
  Bool rc;
  if(token<0 || token>PUT_INIT_DATA)
  {
    fprintf(stderr,"ERROR310: Invalid token %d.\n",(int)token);
    return TRUE;
  }
#ifdef DEBUG_COUPLER
  printf("Token %s, index %d sending",token_names[token],index);
  fflush(stdout);
#endif
  writeint_socket(config->socket,&token,1);
#if COUPLER_VERSION == 4
  if(token!=END_DATA && token!=GET_STATUS)
#else
  if(token!=END_DATA && token!=FAIL_DATA && token!=GET_STATUS)
#endif
    rc=writeint_socket(config->socket,&index,1);
  else
    rc=FALSE;
#ifdef DEBUG_COPAN
  printf(", done.\n");
  fflush(stdout);
#endif
  return rc;
} /* of 'send_token_coupler' */
