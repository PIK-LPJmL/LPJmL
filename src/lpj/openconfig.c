/**************************************************************************************/
/**                                                                                \n**/
/**                   o  p  e  n  c  o  n  f  i  g  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens LPJ configuration file                                      \n**/
/**     Input is prepocessed by cpp or by program defined in the                   \n**/
/**     LPJPREP environment variable                                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return NULL; }

FILE *openconfig(Config *config,      /**< configuration struct */
                 int *argc,           /**< pointer to the number of arguments */
                 char ***argv,        /**< pointer to the argument vector  */
                 const char *usage    /**< usage information string or NULL */
                )                     /** \return file pointer of open file or NULL */

{
  char *lpjpath,*lpjinc,*env_options,*pos;
  char **options;
  char *endptr;
  Bool iscpp;
  FILE *file;
  int i,len,dcount;
  config->nopp=FALSE;
  config->cmd=NULL;
  config->filter=getenv(LPJPREP);
  if(config->filter==NULL)
  {
    config->filter=cpp_cmd;
    iscpp=TRUE;
  }
  else
    iscpp=FALSE;
  pos=getenv(LPJOUTPUT);
  if(pos==NULL)
    config->outputdir=NULL;
  else
  {
    config->outputdir=strdup(pos);
    checkptr(config->outputdir);
  }
  pos=getenv(LPJINPUT);
  if(pos==NULL)
    config->inputdir=NULL;
  else
  {
    config->inputdir=strdup(pos);
    checkptr(config->inputdir);
  }
  pos=getenv(LPJRESTART);
  if(pos==NULL)
    config->restartdir=NULL;
  else
  {
    config->restartdir=strdup(pos);
    checkptr(config->restartdir);
  }
  config->param_out=FALSE;
  config->pedantic=FALSE;
  config->ofiles=FALSE;
  config->scan_verbose=ERR; /* NO_ERR would suppress also error messages */
  pos=getenv(LPJWAIT);
  if(pos!=NULL)
  {
    config->wait=strtol(pos,&endptr,10);
    if(*endptr!='\0')
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR193: Invalid number '%s' for wait in environment variable.\n",pos);
      return NULL;
    }
    if(config->wait<0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR193: Invalid number %d for wait in environment variable, must be >=0.\n",config->wait);
      return NULL;
    }
  }
  else
    config->wait=DEFAULT_WAIT;

#if defined IMAGE && defined COUPLED
  config->image_inport=DEFAULT_IMAGE_INPORT;
  config->image_outport=DEFAULT_IMAGE_OUTPORT;
  config->image_host=getenv(LPJIMAGE);
  if(config->image_host==NULL)
    config->image_host=DEFAULT_IMAGE_HOST;
  else
  {
    pos=strchr(config->image_host,':');
    if(pos!=NULL)
    {
      *pos='\0';
       config->image_inport=strtol(pos+1,&endptr,10);
       if(pos+1==endptr || config->image_inport<1 || config->image_inport>USHRT_MAX)
       {
         if(isroot(*config))
           fprintf(stderr,"ERROR193: Invalid number %d for IMAGE input port.\n",
                   config->image_inport);
         return NULL;
       }
       if(*endptr==',')
       {
         pos=endptr;
         config->image_outport=strtol(endptr+1,&endptr,10);
         if(pos+1==endptr || *endptr!='\0' || config->image_outport<1 
                          || config->image_outport>USHRT_MAX)
         {
           if(isroot(*config))
             fprintf(stderr,
                     "ERROR194: Invalid number %d for IMAGE output port.\n",
                     config->image_outport);
           return NULL;
         }
       }
    }
  }
#else
  config->coupler_port=DEFAULT_COUPLER_PORT;
  config->coupled_host=getenv(LPJCOUPLEDHOST);
  if(config->coupled_host==NULL)
    config->coupled_host=DEFAULT_COUPLED_HOST;
  else
  {
    pos=strchr(config->coupled_host,':');
    if(pos!=NULL)
    {
      *pos='\0';
       config->coupler_port=strtol(pos+1,&endptr,10);
       if(pos+1==endptr || config->coupler_port<1 || config->coupler_port>USHRT_MAX)
       {
         if(isroot(*config))
           fprintf(stderr,"ERROR193: Invalid number %d for coupled port.\n",
                   config->coupler_port);
         return NULL;
       }
    }
  }
#endif

  env_options=getenv(LPJOPTIONS);
  options=newvec(char *,(env_options==NULL) ? *argc : *argc+1);
  if(options==NULL)
  {
    printallocerr("options");
    return NULL;
  }
  dcount=0;
  len=1;
  /* parse command line arguments */
  for(i=1;i<*argc;i++)
  {
    if((*argv)[i][0]=='-') /* check whether option is given */
    {
      if((*argv)[i][1]=='D' || (*argv)[i][1]=='I')
      {
        /* only '-D' (Define) or '-I' (Include) options are allowed */
        options[dcount++]=(*argv)[i];
        len+=strlen((*argv)[i])+1;
      }
      else if(!strcmp((*argv)[i],"-wait"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,"ERROR164: Argument missing for '-wait' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        else
        {
          config->wait=strtol((*argv)[++i],&endptr,10);
          if(*endptr!='\0')
          {
            if(isroot(*config))
              fprintf(stderr,"ERROR193: Invalid number '%s' for wait in option.\n",(*argv)[i]);
            free(options);
            return NULL;
          }
          if(config->wait<0)
          {
            if(isroot(*config))
              fprintf(stderr,"ERROR193: Invalid number %d for wait in option, must be >=0.\n",config->wait);
            free(options);
            return NULL;
          }
        }
      }
#if defined IMAGE && defined COUPLED
      else if(!strcmp((*argv)[i],"-image"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,"ERROR164: Argument missing for '-image' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        else
        {
           config->image_host=(*argv)[++i];
           pos=strchr(config->image_host,':');
           if(pos!=NULL)
           {
             *pos='\0';
             config->image_inport=strtol(pos+1,&endptr,10);
             if(pos+1==endptr || config->image_inport<1 
                              || config->image_inport>USHRT_MAX)
             {
               if(isroot(*config))
                 fprintf(stderr,
                         "ERROR193: Invalid number %d for IMAGE input port.\n",
                         config->image_inport);
               free(options);
               return NULL;
             }
             if(*endptr==',')
             {
               pos=endptr;
               config->image_outport=strtol(endptr+1,&endptr,10);
               if(endptr==pos+1 || *endptr!='\0' || config->image_outport<1  
                                || config->image_outport>USHRT_MAX)
               {
                 if(isroot(*config))
                   fprintf(stderr,"ERROR194: Invalid number %d for IMAGE output port.\n",
                           config->image_outport);
                 free(options);
                 return NULL;
               }
             }
           }
        }
      }
#else
      else if(!strcmp((*argv)[i],"-couple"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,"ERROR164: Argument missing for '-couple' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        else
        {
           config->coupled_host=(*argv)[++i];
           pos=strchr(config->coupled_host,':');
           if(pos!=NULL)
           {
             *pos='\0';
             config->coupler_port=strtol(pos+1,&endptr,10);
             if(pos+1==endptr || config->coupler_port<1
                              || config->coupler_port>USHRT_MAX)
             {
               if(isroot(*config))
                 fprintf(stderr,
                         "ERROR193: Invalid number %d for coupled port.\n",
                         config->coupler_port);
               free(options);
               return NULL;
             }
           }
        }
      }
#endif
      else if(!strcmp((*argv)[i],"-nopp"))
        config->nopp=TRUE;
      else if(!strcmp((*argv)[i],"-pp"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,"ERROR164: Argument missing for '-pp' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        config->filter=(*argv)[++i];
        iscpp=FALSE;
      }
      else if(!strcmp((*argv)[i],"-param"))
        config->param_out=TRUE;
      else if(!strcmp((*argv)[i],"-pedantic"))
        config->pedantic=TRUE;
      else if(!strcmp((*argv)[i],"-ofiles"))
        config->ofiles=TRUE;
      else if(!strcmp((*argv)[i],"-vv"))
        config->scan_verbose=VERB;
      else if(!strcmp((*argv)[i],"-inpath"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,
                    "ERROR164: Argument missing for '-inpath' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        else
        {
          free(config->inputdir);
          config->inputdir=strdup((*argv)[++i]);
          checkptr(config->inputdir);
        }
      }
      else if(!strcmp((*argv)[i],"-outpath"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,
                    "ERROR164: Argument missing for '-outpath' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        else
        {
          free(config->outputdir);
          config->outputdir=strdup((*argv)[++i]);
          checkptr(config->outputdir);
        }
      }
      else if(!strcmp((*argv)[i],"-restartpath"))
      {
        if(i==*argc-1)
        {
          if(isroot(*config))
          {
            fprintf(stderr,"ERROR164: Argument missing for '-restartpath' option.\n");
            if(usage!=NULL)
              fprintf(stderr,usage,(*argv)[0]);
          }
          free(options);
          return NULL;
        }
        else
        {
          free(config->restartdir);
          config->restartdir=strdup((*argv)[++i]);
          checkptr(config->restartdir);
        }
      }
#ifdef WITH_FPE
      else if(!strcmp((*argv)[i],"-fpe"))
        /* enable floating point exceptions, core file will be generated */
        enablefpe();
#endif
      else
      {
        if(isroot(*config))
        {
          fprintf(stderr,"ERROR162: Invalid option '%s'.\n",(*argv)[i]);
          if(usage!=NULL)
            fprintf(stderr,usage,(*argv)[0]);
        }
        free(options);
        return NULL;
      }
    }
    else
      break;
  }
  if(i==*argc)
  {
    if(isroot(*config))
    {
      fprintf(stderr,"ERROR164: Configuration filename missing.\n");
      if(usage!=NULL)
        fprintf(stderr,usage,(*argv)[0]);
    }
    free(options);
    return NULL;
  }
  config->filename=(*argv)[i++];
  /* check whether config file exists */
  if(getfilesize(config->filename)==-1)
  {
    if(isroot(*config))
      printfopenerr(config->filename);
    free(options);
    return NULL;
  }
  else if(isdir(config->filename))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR241: File '%s' is a directory, must be a file.\n",config->filename);
    free(options);
    return NULL;
  }
  else if(getfilesize(config->filename)==0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR242: File '%s' is empty.\n",config->filename);
    free(options);
    return NULL;
  }
  if(config->nopp)
    file=fopen(config->filename,"r");
  else
  {
    /* adjust argc and argv */
    *argv+=i;
    *argc-=i;
    if(env_options!=NULL)
    {
      options[dcount++]=env_options;
      len+=strlen(env_options)+1;
    }
    lpjpath=getenv(LPJROOT);
    if(lpjpath==NULL || !iscpp) /* Is LPJROOT environment variable defined? */
    { /* no */
      if(isroot(*config))
        fprintf(stderr,"WARNING019: Environment variable '%s' not set.\n",
                LPJROOT);
      lpjpath=NULL;
    }
    else
    { /* yes, include LPJROOT directory in search path for includes */
      lpjinc=malloc(strlen(lpjpath)+3);
      checkptr(lpjinc);
      options[dcount++]=strcat(strcpy(lpjinc,"-I"),lpjpath);
      len+=strlen(lpjinc)+1;
    }
    len+=strlen(config->filter);
    if(config->rank!=0)
#ifdef _WIN32
      len+=strlen(" 2>nul:");
#else
      len+=strlen(" 2>/dev/null");
#endif
    config->cmd=malloc(strlen(config->filename)+len+1);
    checkptr(config->cmd);
    strcat(strcpy(config->cmd,config->filter)," ");
    /* concatenate options for cpp command */
    for(i=0;i<dcount;i++)
      strcat(strcat(config->cmd,options[i])," ");
    strcat(config->cmd,config->filename);
    if(config->rank!=0)
#ifdef _WIN32
      strcat(config->cmd," 2>nul:"); /* only task 0 sends error messages */
#else
      strcat(config->cmd," 2>/dev/null"); /* only task 0 sends error messages */
#endif
    if(lpjpath!=NULL)
      free(lpjinc);
    file=popen(config->cmd,"r"); /* open pipe, output of cpp goes to file */
  }
  if(file==NULL)
  {
    if(isroot(*config))
      printfopenerr(config->filename);
    free(options);
    return NULL;
  }
  initscan(config->filename);
  if(isroot(*config))
  {
    len=printf("Reading configuration from '%s'",config->filename);
    if(!config->nopp && (dcount>1 || (dcount && lpjpath==NULL)))
    {
      len+=printf(" with options ");
      if(lpjpath!=NULL)
        dcount--;
      for(i=0;i<dcount;i++)
      {
        if(i)
          len+=printf(",");
        if(strlen(options[i])+len+2>78)
        {
          len=0;
          printf("\n");
        }
        len+=printf("'%s'",options[i]);
      }
    }
    printf(".\n");
  }
  free(options);
  return file;
} /* of 'openconfig' */
