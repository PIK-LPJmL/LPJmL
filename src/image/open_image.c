/**************************************************************************************/
/**                                                                                \n**/
/**                  o  p  e  n  _  i  m  a  g  e  .  c                            \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Opens connection to IMAGE model using TDT compliant socket                 \n**/
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

Bool open_image(Config *config /**< LPJmL configuration */
               )               /** \return TRUE on error */
{
  if(config->rank==0)
  {
    /* Establish the TDT connection */
    printf("Connecting IMAGE model...  image_inport %d, wait %d host %s outport %d\n",config->image_inport,config->wait_image,config->image_host,config->image_outport);
    fflush(stdout);
    config->in=opentdt_socket(config->image_inport,config->wait_image);
    if(config->in==NULL)
      return TRUE;
#ifdef DEBUG_IMAGE
    printf("opened INTERFACE_to_LPJ\n");

    printf("opening LPJ_to_INTERFACE image_host %s outport %d\n",config->image_host,config->image_outport);
#endif
    config->out=connecttdt_socket(config->image_host,
                                       config->image_outport);
    if(config->out==NULL)
    {
      close_socket(config->in);
      config->in=NULL;
      return TRUE;
    }
#ifdef DEBUG_IMAGE
    printf("opened LPJ_to_INTERFACE\n");
    printf("All connections to IMAGE are established.\n");
    fflush(stdout);
#endif
  }
  return FALSE;
} /* of 'open_image' */

#endif
