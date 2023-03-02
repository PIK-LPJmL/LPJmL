/**************************************************************************************/
/**                                                                                \n**/
/**                 l  p  j  f  i  l  e  s  .  c                                   \n**/
/**                                                                                \n**/
/**     Program prints LPJML input/output files                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "crop.h"

#define LPJFILES_VERSION "1.0.003"
#define NTYPES 3 /* number of PFT types: grass, tree, crop */

#define USAGE "Usage: %s [-h] [-noinput] [-nooutput] [-outpath dir] [-inpath dir] [-restartpath dir]\n"\
              "       [[-Dmacro[=value]] [-Idir] ...] filename\n"

int main(int argc,char **argv)
{
  /* Create array of functions, uses the typedef of (*Fscanpftparfcn) in pft.h */
  Pfttype scanfcn[NTYPES]=
  {
    {name_grass,fscanpft_grass},
    {name_tree,fscanpft_tree},
    {name_crop,fscanpft_crop}
  };
  Config config;         /* LPJ configuration */
  int rc;                /* return code of program */
  const char *progname;
  int argc_save,iarg;
  char **argv_save;
  Bool input;
  Bool output;
  FILE *file;
  initconfig(&config);
  progname=strippath(argv[0]);
  if(argc>1)
  {
    if(!strcmp(argv[1],"-h"))
    {
      file=popen("more","w");
      if(file==NULL)
        file=stdout;
      fputs("     ",file);
      rc=fprintf(file,"%s Version " LPJFILES_VERSION " (" __DATE__ ") Help",
              progname);
      fputs("\n     ",file);
      frepeatch(file,'=',rc);
      fputs("\n\nPrint input/output files of LPJmL " LPJ_VERSION "\n\n",file);
      fprintf(file,USAGE,progname);
      fprintf(file,"\nArguments:\n"
             "-h               print this help text\n"
             "-noinput         does not list input data files\n"
             "-nooutput        does not list output files\n"
             "-pp cmd          set preprocessor program. Default is 'cpp -P'\n"
             "-outpath dir     directory appended to output filenames\n"
             "-inpath dir      directory appended to input filenames\n"
             "-restartpath dir directory appended to restart filename\n"
             "-Dmacro[=value]  define macro for preprocessor of configuration file\n"
             "-Idir            directory to search for include files\n"
             "filename         configuration filename\n\n"
             "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n");
      if(file!=stdout)
        pclose(file);
      return EXIT_SUCCESS;
    }
  }
  input=output=TRUE; /* no input files listed */
  for(iarg=1;iarg<argc;iarg++)
  {
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-noinput"))
        input=FALSE;
      else if(!strcmp(argv[iarg],"-nooutput"))
        output=FALSE;
      else
        break;
    }
    else
     break;
  }
  argc-=iarg-1;
  argv+=iarg-1;
  argc_save=argc;
  argv_save=argv;
  if(readconfig(&config,scanfcn,NTYPES,NOUT,&argc,&argv,USAGE))
  {
    fputs("Syntax error found in configuration file.\n",stderr);
    return EXIT_FAILURE;
  }
  else
  {
    printincludes(argc_save,argv_save);
    printfiles(input,output,&config);
  }
  return EXIT_SUCCESS;
} /* of 'main' */
