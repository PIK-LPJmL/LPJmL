/**************************************************************************************/
/**                                                                                \n**/
/**                   r  e  a  d  c  o  n  f  i  g  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads LPJ configuration file                                      \n**/
/**     Input is prepocessed by cpp                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readconfig(Config *config,        /**< LPJ configuration */
                const char *filename,  /**< Default configuration filename */
                Fscanpftparfcn scanfcn[], /**< array of PFT-specific scan
                                             functions */
                int ntypes,            /**< Number of PFT classes */
                int nout,              /**< Maximum number of output files */
                int *argc,             /**< pointer to the number of arguments */
                char ***argv,          /**< pointer to the argument vector */
                const char *usage      /**< usage information or NULL */
               )                       /** \return TRUE on error */
{
  FILE *file;
  String s;
  Verbosity verbosity;
  config->arglist=catstrvec(*argv,*argc); /* store command line in arglist */
  file=openconfig(config,filename,argc,argv,usage);
  if(file==NULL)
    return TRUE;
  verbosity=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(fscanstring(file,s,verbosity!=NO_ERR))
  {
    if(verbosity)
      fputs("ERROR121: Cannot read simulation name.\n",stderr);
    closeconfig(file);
    return TRUE;
  }
  if(verbosity>=VERB)
    printf("SIM_NAME %s\n",s);
  config->sim_name=strdup(s);
  if(config->sim_name==NULL)
  {
    printallocerr("sim_name");
    return TRUE;
  }
  if(fscanint(file,&config->sim_id,"simulation id",verbosity))
  {
    closeconfig(file);
    return TRUE;
  }
#ifdef IMAGE
  if(config->sim_id!=LPJML && config->sim_id!=LPJ && config->sim_id!=LPJML_IMAGE)
  {
    if(verbosity)
      fprintf(stderr,"ERROR123: Invalid simulation type in line %d of '%s', must be 'LPJML' or 'LPJ' or 'LPJML_IMAGE'.\n",getlinecount(),getfilename());
    closeconfig(file);
    return TRUE;
  }
#else
  if(config->sim_id!=LPJML && config->sim_id!=LPJ && config->sim_id!=LPJML_FMS)
  {
    if(verbosity)
    {
      if(config->sim_id==LPJML_IMAGE)
        fputs("ERROR219: LPJmL has to be compiled with '-DIMAGE' for simulation type 'LPJML_IMAGE'.\n",stderr);
      else
        fprintf(stderr,"ERROR123: Invalid simulation type in line %d of '%s', must be 'LPJML', 'LPJML_FMS' or 'LPJ'.\n",getlinecount(),getfilename());
    }
    closeconfig(file);
    return TRUE;
  }
#endif
  /* Read LPJ configuration */
  if(fscanconfig(config,file,scanfcn,ntypes,nout))
  {
    closeconfig(file);
    return TRUE;
  }
  closeconfig(file);
  return FALSE;
} /* of 'readconfig' */
