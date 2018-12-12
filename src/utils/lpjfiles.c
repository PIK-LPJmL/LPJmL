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

#define LPJFILES_VERSION "1.0.002"
#define NTYPES 3 /* number of PFT types: grass, tree, crop */
#ifdef USE_JSON
#define dflt_conf_filename "lpjml.js" /* Default LPJ configuration file */
#else
#define dflt_conf_filename "lpjml.conf" /* Default LPJ configuration file */
#endif
#define USAGE "Usage: %s [-h] [-noinput] [-outpath dir] [-inpath dir] [-restartpath dir]\n"\
              "       [[-Dmacro[=value]] [-Idir] ...] [filename]\n"

int main(int argc,char **argv)
{
  /* Create array of functions, uses the typedef of (*Fscanpftparfcn) in pft.h */
  Fscanpftparfcn scanfcn[NTYPES]={fscanpft_grass,fscanpft_tree,fscanpft_crop};
  Config config;         /* LPJ configuration */
  int rc;                /* return code of program */
  const char *progname;
  int argc_save;
  char **argv_save;
  Bool input;
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
             "-pp cmd          set preprocessor program. Default is 'cpp -P'\n"
             "-outpath dir     directory appended to output filenames\n"
             "-inpath dir      directory appended to input filenames\n"
             "-restartpath dir directory appended to restart filename\n"
             "-Dmacro[=value]  define macro for preprocessor of configuration file\n"
             "-Idir            directory to search for include files\n"
             "filename         configuration filename. Default is '%s'\n\n"
             "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
             dflt_conf_filename);
      if(file!=stdout)
        pclose(file);
      return EXIT_SUCCESS;
    }
  }
  if(argc>1 && !strcmp(argv[1],"-noinput"))
  {
    argc--; /* adjust command line options */
    argv++;
    input=FALSE; /* no input files listed */
  }
  else 
    input=TRUE; /* list input files */
  argc_save=argc;
  argv_save=argv;
  if(readconfig(&config,dflt_conf_filename,scanfcn,NTYPES,NOUT,&argc,&argv,USAGE))
  {
    fputs("Syntax error found in configuration file.\n",stderr);
    return EXIT_FAILURE;
  }
  else
  {
    printincludes(dflt_conf_filename,argc_save,argv_save);
    printfiles(input,&config);
  }
  return EXIT_SUCCESS;
} /* of 'main' */
