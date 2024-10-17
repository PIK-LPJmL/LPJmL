/**************************************************************************************/
/**                                                                                \n**/
/**                         h  e  l  p  .  c                                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints usage information                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef WITH_FPE
#define FPE_OPT "[-fpe]"
#else
#define FPE_OPT ""
#endif
#if defined IMAGE && defined COUPLED
#define IMAGE_OPT "       [-image host[:inport[,outport]]\n"
#else
#define IMAGE_OPT ""
#endif
#define USAGE "\nUsage: %s [-h] [-l] [-v] [-vv] [-pedantic] [-ofiles] [-param] [-nopp] [-pp cmd] " FPE_OPT "\n" IMAGE_OPT\
                "       [-couple host[:port]] [-wait time]\n"\
                "       [-outpath dir] [-inpath dir] [-restartpath dir]\n"\
                "       [[-Dmacro[=value]] [-Idir] ...] filename\n"

char *lpj_usage=USAGE;

void help(const char *progname /**< program filename */
         )
{
  FILE *file;
  int count;
  file=popen("more","w"); /* output is piped thru 'more' */
  if(file==NULL)
  { /* open failed, output to stdout */
    file=stdout;
  }
  fprintf(file,"    ");
  count=fprintf(file,"%s C Version %s (%s) Help",
                progname,getversion(),getbuilddate());
  fprintf(file,"\n    ");
  frepeatch(file,'=',count);
  fprintf(file,"\n\nDynamic global vegetation model with managed land\n");
#if defined IMAGE && defined COUPLED
  fputs("and IMAGE coupler\n",file);
#endif
  fprintf(file,lpj_usage,progname);
  fprintf(file,"\nArguments:\n"
          "-h,--help           print this help text\n"
          "-l,--license        print license file\n"
          "-v,--version        print version, compiler and compile flags\n"
          "-vv                 verbosely print the actual values during reading of the\n"
          "                    configuration files\n"
          "-pedantic           stops on warnings\n"
          "-ofiles             list only all available output variables\n"
          "-param              print LPJmL parameter\n"
          "-nopp               disable preprocessing\n"
          "-pp cmd             set preprocessor program. Default is '" cpp_cmd "'\n"
#ifdef WITH_FPE
          "-fpe                enable floating point exceptions\n"
#endif
#if defined IMAGE && defined COUPLED
          "-image host[:inport[,outport]] set host where IMAGE model is running\n"
#endif
          "-couple host[:port] set host and port where coupled model is running\n"
          "-wait time          time to wait for connection to coupled/IMAGE model (sec)\n"
          "-outpath dir        directory appended to output filenames\n"
          "-inpath dir         directory appended to input filenames\n"
          "-restartpath dir    directory appended to restart filename\n"
          "-Dmacro[=value]     define macro for preprocessor of configuration file\n"
          "-Idir               directory to search for include files\n"
          "filename            configuration filename\n\n"
          "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n");
  if(file!=stdout)
    pclose(file);
} /* of 'help' */
