/**************************************************************************************/
/**                                                                                \n**/
/**        r  e  c  e  i  v  e    _  t  o  k  e  n  _  c  o  p  a  n  .  c         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function receives token from socket                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/
#include "lpj.h"

Bool receive_token_copan(Socket *socket, /**< pointer to open socket */
                         Token *token,   /**< token received */
                         int *index      /**< index received */
                        )                /** \return TRUE on error */
{
#ifdef DEBUG_COPAN
  printf("Receiving token");
  fflush(stdout);
#endif
  readint_socket(socket,(int *)token,1);
  if(*token<0 || *token>PUT_INIT_DATA)
  {
    fprintf(stderr,"Invalid token %d.\n",(int)*token);
    return TRUE;
  }
#ifdef DEBUG_COPAN
  printf(", token %s received.\n",token_names[*token]);
  fflush(stdout);
#endif
  if(*token==FAIL_DATA)
  {
    fprintf(stderr,"LPJmL stopped with error.\n");
    return TRUE;
  }
  if(*token!=END_DATA && *token!=GET_STATUS) 
  {
#ifdef DEBUG_COPAN
    printf("Receiving index");
    fflush(stdout);
#endif
    readint_socket(socket,index,1);
#ifdef DEBUG_COPAN
    printf(", index %d received.\n",*index);
    fflush(stdout);
#endif
  }
  return FALSE;
} /* of 'receive_token_copan' */
