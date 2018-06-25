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
#ifdef IMAGE
#define IMAGE_OPT "       [-image host[:inport[,outport]]] [-wait time]\n"
#else
#define IMAGE_OPT ""
#endif
#ifdef USE_MPI
#define USAGE "\nUsage: %s [-h] [-l] [-v] [-vv] [-param] [-pp cmd] " FPE_OPT "\n" IMAGE_OPT\
                "       [-output {gather|mpi2|socket=hostname[:port]}]\n"\
                "       [-outpath dir] [-inpath dir] [-restartpath dir]\n"\
                "       [[-Dmacro[=value]] [-Idir] ...] [filename]\n"
#else
#define USAGE "\nUsage: %s [-h] [-l] [-v] [-vv] [-param] [-pp cmd] " FPE_OPT "\n" IMAGE_OPT\
                "       [-output {write|socket=hostname[:port]}]\n"\
                "       [-outpath dir] [-inpath dir] [-restartpath dir]\n"\
                "       [[-Dmacro[=value]] [-Idir] ...] [filename]\n"
#endif

char *lpj_usage=USAGE;

void help(const char *progname, /**< program filename */
          const char *filename  /**< default LPJmL configuration filename */
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
  count=fprintf(file,"%s C Version " LPJ_VERSION " (%s) Help",
                progname,getbuilddate());
  fprintf(file,"\n    ");
  frepeatch(file,'=',count);
  fprintf(file,"\n\nDynamic global vegetation model with managed land\n");
#ifdef IMAGE
  fputs("and IMAGE coupler\n",file);
#endif
  fprintf(file,lpj_usage,progname);
  fprintf(file,"\nArguments:\n"
          "-h               print this help text\n"
          "-l               print license file\n"
          "-v               print version, compiler and compile flags\n"
          "-vv              verbosely print the actual values during reading of the\n"
          "                 configuration files\n"
          "-param           print LPJmL parameter\n"
          "-pp cmd          set preprocessor program. Default is 'cpp'\n"
#ifdef WITH_FPE
          "-fpe             enable floating point exceptions\n"
#endif
#ifdef IMAGE
          "-image host[:inport[,outport]] set host where IMAGE model is running\n"
          "-wait time       time to wait for connection to IMAGE model (sec)\n"
#endif
#ifdef USE_MPI
          "-output method   output method. Must be gather, mpi2, socket\n"
          "                 Default is gather.\n"
#else
          "-output method   output method. Must be write or socket. Default is write\n"
#endif
          "-outpath dir     directory appended to output filenames\n"
          "-inpath dir      directory appended to input filenames\n"
          "-restartpath dir directory appended to restart filename\n"
          "-Dmacro[=value]  define macro for preprocessor of configuration file\n"
          "-Idir            directory to search for include files\n"
          "filename         configuration filename. Default is '%s'\n\n"
          "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",filename);
  if(file!=stdout)
    pclose(file);
} /* of 'help' */
