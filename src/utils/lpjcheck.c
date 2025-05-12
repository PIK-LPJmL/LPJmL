/**************************************************************************************/
/**                                                                                \n**/
/**                 l  p  j  c  h  e  c  k  .  c                                   \n**/
/**                                                                                \n**/
/**     Program checks syntax of LPJ configuration files and tests on              \n**/
/**     existence of input files and output directories.                           \n**/
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

#define NTYPES 3 /* number of PFT types: grass, tree, crop */
#define USAGE "\nUsage: %s [-h] [-v] [-q] [-nocheck] [-ofiles] [-param] [-vv]\n"\
              "       [-couple hostname[:port]] [-pedantic]\n"\
              "       [-outpath dir] [-inpath dir] [-restartpath dir]\n"\
              "       [-nopp] [-pp cmd] [[-Dmacro[=value]] [-Idir] ...] filename\n"
#define LPJ_USAGE USAGE  "\nTry \"%s --help\" for more information.\n"

int main(int argc,char **argv)
{
  /* Create array of functions, uses the typedef of Pfttype in config.h */
  Pfttype scanfcn[NTYPES]=
  {
    {name_grass,fscanpft_grass},
    {name_tree,fscanpft_tree},
    {name_crop,fscanpft_crop}
  };
  Config config;         /* LPJ configuration */
  int rc=0;              /* return code of program */
  Bool isout,check;
  const char *progname;
  const char *title[4];
  String line,line2;
  FILE *file;
  initconfig(&config);
  isout=check=TRUE;
  progname=strippath(argv[0]);
  if(argc>1)
  {
    if(!strcmp(argv[1],"-q") || !strcmp(argv[1],"--quiet")) /* checks for '-q' flag */
    {
      argc--; /* adjust command line options */
      argv++;
      isout=FALSE; /* no output */
    }
    else if(!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version"))
    {
      puts(getversion());
      return EXIT_SUCCESS;
    }
    else if(!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help"))
    {
      file=popen("more","w");
      if(file==NULL)
        file=stdout;
      fprintf(file,"     ");
      rc=fprintf(file,"%s (" __DATE__ ") Help",
              progname);
      fprintf(file,"\n     ");
      frepeatch(file,'=',rc);
      fprintf(file,"\n\nCheck syntax of LPJmL version %s configuration (*.cjson) files\n",getversion());
      fprintf(file,USAGE,progname);
      fprintf(file,"\nArguments:\n"
             "-h,--help           print this help text\n"
             "-v,--version        print LPJmL version\n"
             "-q,--quiet          print error messsages only\n"
             "-vv                 verbosely print the actual values during reading of the\n"
             "                    configuration files\n"
             "-pedantic           stops on warnings\n"
             "-ofiles             list only all available output variables\n"
             "-param              print LPJ parameter\n"
             "-nopp               disable preprocessing\n"
             "-pp cmd             set preprocessor program. Default is '" cpp_cmd "'\n"
             "-couple host[:port] set host and port where coupled model is running\n"
             "-outpath dir        directory appended to output filenames\n"
             "-inpath dir         directory appended to input filenames\n"
             "-restartpath dir    directory appended to restart filename\n"
             "-Dmacro[=value]     define macro for preprocessor of configuration file\n"
             "-Idir               directory to search for include files\n"
             "filename            configuration filename\n\n"
             "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n");
      if(file!=stdout)
        pclose(file);
      return EXIT_SUCCESS;
    }
  }
  if(argc>1 && !strcmp(argv[1],"-nocheck"))
  {
    argc--; /* adjust command line options */
    argv++;
    check=FALSE; /* no checking */
  }
  if(isout)
  {
    snprintf(line,78-10,
             "%s (" __DATE__ ")",progname);
    snprintf(line2,78-10,"Checking configuration file for LPJmL version %s",getversion());
    title[0]=line;
    title[1]=line2;
    title[2]="(C) Potsdam Institute for Climate Impact Research (PIK),";
    title[3]="see COPYRIGHT file";
    banner(title,4,78);
  }

  if(readconfig(&config,scanfcn,NTYPES,NOUT,&argc,&argv,LPJ_USAGE))
  {
    fail(READ_CONFIG_ERR,FALSE,"Cannot process configuration file");
  }
  else
  {
    if(argc)
      fputs("WARNING018: Arguments listed after configuration filename, will be ignored.\n",stderr);
    if(config.ofiles)
    {
      fprintoutputvar(stdout,config.outnames,NOUT,config.npft[GRASS]+config.npft[TREE],config.npft[CROP],&config);
      return EXIT_SUCCESS;
    }

    if(isout)
    {
      /* print LPJ configuration on stdout if '-q' option is not set */
      printconfig(config.npft[GRASS]+config.npft[TREE],
                  config.npft[CROP],&config);
    }
    if(config.nall>0 && config.n_out)
    {
      config.nall=config.total=config.ngridcell;
      printf("Estimated disk usage for output: ");
      printintf((int)(outputfilesize(&config)/(1024*1024)));
      printf(" MByte\n");
    }
    if(check)
      rc=(filesexist(config,isout)) ? EXIT_FAILURE : EXIT_SUCCESS;
    else
      rc=EXIT_SUCCESS;
  }
  freeconfig(&config);
  return rc;
} /* of 'main' */
