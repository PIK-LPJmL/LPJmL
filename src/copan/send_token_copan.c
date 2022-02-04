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

#ifdef DEBUG_COPAN
static char *token_names[]={"GET_DATA","PUT_DATA","GET_DATA_SIZE","PUT_DATA_SIZE",
                            "END_DATA","GET_STATUS","FAIL_DATA"};
#endif

Bool send_token_copan(int token,           /**< Token (GET_DATA,PUT_DATA, ...) */
                      int index,           /**< index for in- or output stream */
                      const Config *config /**< LPJ configuration */
                     )                     /** \return TRUE on error */
{
  Bool rc;
  if(token<0 || token>FAIL_DATA)
  {
    fprintf(stderr,"ERROR310: Invalid token %d.\n",token);
    return  TRUE;
  }
#ifdef DEBUG_COPAN
  printf("Token %s, index %d sending",token_names[token],index);
  fflush(stdout);
#endif
  writeint_socket(config->socket,&token,1);
  rc=writeint_socket(config->socket,&index,1);
#ifdef DEBUG_COPAN
  printf(", done.\n");
  fflush(stdout);
#endif
  return rc;
} /* of 'send_token_copan' */
