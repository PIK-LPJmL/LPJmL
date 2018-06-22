/**************************************************************************************/
/**                                                                                \n**/
/**              f  p  r  i  n  t  i  n  c  l  u  d  e  s  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints file names include by the LPJ configuration file           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef _WIN32              /* are we on a Windows machine? */
#define cpp_cmd "cl /E /nologo"  /* C preprocessor command for Windows */
#else
#define cpp_cmd "cpp"  /* C preprocessor command for Unix */
#endif

static int findname(List *list,const char *s)
{
  int i;
  for(i=getlistlen(list)-1;i>=0;i--)
    if(!strcmp(list->data[i],s))
      return i;
  return NOT_FOUND;
} /* of 'findname' */

void fprintincludes(FILE *out,                 /**< pointer to text file */
                    const char *dflt_filename, /**< default name of configuration file */
                    int argc,                  /**< number of arguments */
                    char **argv                /**< argument vector  */
                   )

{
  char *cmd,*lpjpath,*lpjinc,*filter,*env_options;
  char **options;
  const char *filename;
  Bool iscpp;
  String token;
  const char *incfilename;
  FILE *file;
  int i,len,dcount;
  List *list;
  filter=getenv(LPJPREP);
  if(filter==NULL)
  {
    filter=cpp_cmd;
    iscpp=TRUE;
  }
  else
    iscpp=FALSE;
  env_options=getenv(LPJOPTIONS);
  options=newvec(char *,(env_options==NULL) ? argc : argc+1);
  check(options);
  dcount=0;
  len=1;
  /* parse command line arguments */
  for(i=1;i<argc;i++)
  {
    if(argv[i][0]=='-') /* check whether option is given */
    {
      if(argv[i][1]=='D' || argv[i][1]=='I')
      {
        /* only '-D' (Define) or '-I' (Include) options are allowed */
        options[dcount++]=argv[i];
        len+=strlen(argv[i])+1;
      }
      else if(!strcmp(argv[i],"-pp"))
      {
        if(i==argc-1)
        {
          free(options);
          return;
        }
        filter=argv[++i];
        iscpp=FALSE;
      }
      else if(!strcmp(argv[i],"-outpath"))
        i++;
      else if(!strcmp(argv[i],"-inpath"))
        i++;
      else if(!strcmp(argv[i],"-restartpath"))
        i++;
      else if(!strcmp(argv[i],"-output"))
        i++;
#ifdef IMAGE
      else if(!strcmp(argv[i],"-wait"))
        i++;
      else if(!strcmp(argv[i],"-image"))
        i++;
#endif
    }
    else
      break;
  }
  lpjpath=getenv(LPJCONFIG);
  if(lpjpath==NULL)
    filename=(i==argc)  ? dflt_filename : argv[i];
  else
    filename=(i==argc)  ? lpjpath : argv[i];
  lpjpath=getenv(LPJROOT);
  if(lpjpath==NULL || !iscpp) /* Is LPJROOT environment variable defined? */
  { /* no */
    lpjpath=NULL;
  }
  else
  { /* yes, include LPJROOT directory in search path for includes */
    lpjinc=malloc(strlen(lpjpath)+3);
    options[dcount++]=strcat(strcpy(lpjinc,"-I"),lpjpath);
    len+=strlen(lpjinc)+1;
  }
  if(env_options!=NULL)
  {
    options[dcount++]=env_options;
    len+=strlen(env_options)+1;
  }
  len+=strlen(filter);
  cmd=malloc(strlen(filename)+len+1);
  strcat(strcpy(cmd,filter)," ");
  /* concatenate options for cpp command */
  for(i=0;i<dcount;i++)
    strcat(strcat(cmd,options[i])," ");
  strcat(cmd,filename);
  if(lpjpath!=NULL)
    free(lpjinc);
  if(getfilesize(filename)==-1)
  {
    free(options);
    return;
  }
  file=popen(cmd,"r"); /* open pipe, output of cpp goes to file */
  free(cmd);
  if(file==NULL)
  {
    free(options);
    return;
  }
  initscan(filename);
  free(options);
  list=newlist();
  while(!fscantoken(file,token))
  {
    incfilename=getfilename();
    if(findname(list,incfilename)==NOT_FOUND)
    {
       addlistitem(list,strdup(incfilename));
       fprintf(out,"%s\n",incfilename);
    }
  }
  for(i=0;i<getlistlen(list);i++)
    free(list->data[i]);
  freelist(list); 
  pclose(file);
} /* of 'fprintincludes' */
