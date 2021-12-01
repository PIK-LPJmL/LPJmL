/**************************************************************************************/
/**                                                                                \n**/
/**                  o  p  e  n  _  c  o  p  a  n  .  c                            \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Opens connection to COPAN model using TDT compliant socket                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool open_copan(Config *config /**< LPJmL configuration */
               )               /** \return TRUE on error */
{
  int rev=1;
  if(isroot(*config))
  {
    /* Establish the TDT connection */
    printf("Connecting to COPAN model...\n");
    fflush(stdout);
    config->in=opentdt_socket(config->copan_inport,config->wait_copan);
    if(config->in==NULL)
      return TRUE;
    config->out=connecttdt_socket(config->copan_host,config->copan_outport);
    if(config->out==NULL)
    {
      close_socket(config->in);
      config->in=NULL;
      return TRUE;
    }
    writeint_socket(config->out, &rev,1);
#ifdef DEBUG_COPAN
    printf("open_copan %d \n",IRevision);
#endif
  }
  return FALSE;
} /* of 'open_copan' */
