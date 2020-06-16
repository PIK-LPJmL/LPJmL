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

#if defined IMAGE && defined COUPLED

Bool open_image(Config *config /**< LPJmL configuration */
               )               /** \return TRUE on error */
{
  char Revision_exe[10]  = "0000";
  char Date_exe[80]      = "2020/05";
  char TimeNow_exe[80]   = "2020/05";
  char URL_exe[80]       = "https://gitlab.pik-potsdam.de";

  int *IRevision;
  int *IUrl;
  int len,i;

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
    printf("LPJ.exe version information\n");
    printf("URL repository: %s \n", URL_exe);
    printf("Revision: %s \n",Revision_exe);
    printf("Date: %s \n",Date_exe);
    printf("BuildTime: %s \n",TimeNow_exe);
/* SEND revision_lpj and URL_lpj; should be a string in future*/
    IRevision = newvec(int,1);
    check(IRevision);
    IUrl = newvec(int,80);
    check(IUrl);

/* send revision number to interface */
    IRevision[0] = atoi(Revision_exe);
#ifdef DEBUG_IMAGE
    printf("open_image1 %d \n",IRevision[0]);
#endif
    writeint_socket(config->out, IRevision,1); 
#ifdef DEBUG_IMAGE
    printf("open_image2 %d \n",IRevision[0]);
#endif

  /* send revision url */
#ifdef DEBUG_IMAGE
    printf("open_image3  \n"); 
#endif
    len = (int)strlen (URL_exe);
    for(i=0;i<80;i++) {
        if (i < len) {
           IUrl[i] = URL_exe[i];
        }
        else {
           IUrl[i] = 32;
        }
#ifdef DEBUG_IMAGE
        printf("open_image4 %i %i \n",i, IUrl[i] );  
#endif
    }
#ifdef DEBUG_IMAGE
    printf("open_image5  \n"); 
#endif
    writeint_socket(config->out, IUrl,80); 
#ifdef DEBUG_IMAGE
    printf("open_image6  \n"); 
#endif

    free(IRevision);
    free(IUrl);
/* end SEND revision_lpj and URL_lpj; should be a string in future*/


  }
  return FALSE;
} /* of 'open_image' */

#endif
