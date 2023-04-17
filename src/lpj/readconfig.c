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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readconfig(Config *config,        /**< LPJ configuration */
                const char *filename,  /**< Default configuration filename */
                Pfttype scanfcn[],     /**< array of PFT-specific scan
                                             functions */
                int ntypes,            /**< Number of PFT classes */
                int nout,              /**< Maximum number of output files */
                int *argc,             /**< pointer to the number of arguments */
                char ***argv,          /**< pointer to the argument vector */
                const char *usage      /**< usage information or NULL */
               )                       /** \return TRUE on error */
{
  FILE *file;
  LPJfile lpjfile;
  String s;
  Verbosity verbosity;
  const char *sim_id[]={"lpj","lpjml","lpjml_image","lpjml_fms"};
  config->arglist=catstrvec(*argv,*argc); /* store command line in arglist */
  config->coupled_model=NULL;
  file=openconfig(config,filename,argc,argv,usage);
  if(file==NULL)
    return TRUE;
  verbosity=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  lpjfile.file.file=file;
  lpjfile.isjson=FALSE;
  if(fscanstring(&lpjfile,s,"sim_name",FALSE,verbosity))
  {
    if(verbosity)
      fputs("ERROR121: Cannot read simulation name.\n",stderr);
    closeconfig(&lpjfile);
    return TRUE;
  }
  if(s[0]=='{') /* check whether file is in JSON format */
  {
#ifdef USE_JSON
    if(parse_json(file,&lpjfile,s,verbosity))
    {
      if(config->nopp)
        fclose(file);
      else
        pclose(file);
      return TRUE;
    }
    if(config->nopp)
      fclose(file);
    else
      pclose(file);
    if(fscanstring(&lpjfile,s,"sim_name",FALSE,verbosity))
    {
      if(verbosity)
        fputs("ERROR121: Cannot read simulation name.\n",stderr);
      closeconfig(&lpjfile);
      return TRUE;
    }
#else
    if(verbosity)
      fputs("ERROR229: JSON format not supported in this version of LPJmL.\n",stderr);
    closeconfig(&lpjfile);
    return TRUE;
#endif
  }
  else
  {
#ifdef USE_JSON
  if(verbosity)
    printf("REMARK001: File format of '%s' is deprecated, please use JSON format instead.\n",config->filename);
#endif
    lpjfile.isjson=FALSE;
  }
  config->sim_name=strdup(s);
  if(config->sim_name==NULL)
  {
    printallocerr("sim_name");
    closeconfig(&lpjfile);
    return TRUE;
  }
  if(iskeydefined(&lpjfile,"version"))
  {
    if(fscanstring(&lpjfile,s,"version",FALSE,verbosity))
    {
      if(verbosity)
        fputs("ERROR121: Cannot read version.\n",stderr);
      closeconfig(&lpjfile);
      return TRUE;
    }
    if(verbosity && strncmp(s,LPJ_VERSION,strlen(s)))
      fprintf(stderr,"WARNING025: LPJ version '%s' does not match '" LPJ_VERSION "'.\n",s);
  }
  /* Read LPJ configuration */
  config->sim_id=LPJML;
  if(fscankeywords(&lpjfile,&config->sim_id,"sim_id",sim_id,4,TRUE,verbosity))
  {
    closeconfig(&lpjfile);
    return TRUE;
  }
#if defined IMAGE && defined COUPLED
  if(config->sim_id!=LPJML && config->sim_id!=LPJ && config->sim_id!=LPJML_IMAGE)
  {
    if(verbosity)
      fprintf(stderr,"ERROR123: Invalid simulation type, must be \"lpjml\" or \"lpj\" or \"lpjml_image\".\n");
    closeconfig(&lpjfile);
    return TRUE;
  }
#else
  if(config->sim_id==LPJML_IMAGE)
  {
    if(verbosity)
      fputs("ERROR219: LPJmL has to be compiled with '-DIMAGE -DCOUPLED' for simulation type \"lpjml_image\".\n",stderr);
    closeconfig(&lpjfile);
    return TRUE;
  }
#endif
  if(fscanconfig(config,&lpjfile,scanfcn,ntypes,nout))
  {
    closeconfig(&lpjfile);
    return TRUE;
  }
  closeconfig(&lpjfile);
  return FALSE;
} /* of 'readconfig' */
