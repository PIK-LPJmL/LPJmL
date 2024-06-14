/**************************************************************************************/
/**                                                                                \n**/
/**                c  r  e  a  t  e  c  o  n  f  i  g  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function write processed LPJ configuration file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void createconfig(const Config *config)
{
  char *cmd;
  int rc;
  if(config->json_filename!=NULL)
  {
    cmd=malloc(strlen(config->cmd)+strlen(config->json_filename)+6);
    if(cmd==NULL)
    {
      printallocerr("cmd");
      return;
    }
    cmd=strcat(strcat(strcpy(cmd,config->cmd)," -P >"),config->json_filename);
    if(rc=system(cmd))
      fprintf(stderr,"ERROR263: Cannot write '%s', rc=%d.\n",
              config->json_filename,rc);
    free(cmd);
  }
} /* of 'createconfig' */
