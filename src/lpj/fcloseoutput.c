/**************************************************************************************/
/**                                                                                \n**/
/**               f  c  l  o  s  e  o  u  t  p  u  t  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function closes all open output files. For buffer output method            \n**/
/**     all data stored in buffers are written to disk.                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static int compress(const char *filename,const char *compress_cmd)
{
  int rc;
  char *cmd,*newfile;
  newfile=stripsuffix(filename);
  if(newfile==NULL)
    return TRUE;
  cmd=malloc(strlen(newfile)+strlen(compress_cmd)+2);
  if(cmd==NULL)
  {
    free(newfile);
    return TRUE;
  }
  strcat(strcat(strcpy(cmd,compress_cmd)," "),newfile);
  rc=system(cmd);
  if(rc)
    fprintf(stderr,"ERROR250: Cannot compress output '%s', file not compressed.\n",newfile);
  free(newfile);
  free(cmd);
  return rc;
} /* of 'compress' */

void fcloseoutput(Outputfile *output,  /**< Output file array */
                  const Config *config /**< LPJmL configuration */
                 )
{
  int i;
  for(i=0;i<output->n;i++)
    if(output->files[i].isopen)  /* output file is open? */
    {
#ifdef USE_MPI
      switch(output->method)
      {
        case LPJ_MPI2:
          MPI_File_close(&output->files[i].fp.mpi_file);
          if(output->files[i].compress)
          {
            MPI_Barrier(config->comm);
            if(isroot(*config))
              compress(output->files[i].filename,config->compress_cmd);
          }
          break;
        case LPJ_GATHER:
          if(isroot(*config) && !output->files[i].oneyear)
          {
            switch(output->files[i].fmt)
            {
              case RAW: case TXT:
                fclose(output->files[i].fp.file);
                break;
              case CDF:
                close_netcdf(&output->files[i].fp.cdf);
                break;
            }
            if(output->files[i].compress)
              compress(output->files[i].filename,config->compress_cmd);
          }
          break;
      } /* of switch */
#else
      if(output->method==LPJ_FILES && !output->files[i].oneyear)
      {
         switch(output->files[i].fmt)
         {
           case RAW: case TXT: case CLM:
             fclose(output->files[i].fp.file);
             break;
           case CDF:
             close_netcdf(&output->files[i].fp.cdf);
             break;
         } /* of switch */
         if(output->files[i].compress)
           compress(output->files[i].filename,config->compress_cmd);
      }
#endif
    }
#ifdef USE_MPI
  if(output->method!=LPJ_MPI2)
  {
    free(output->counts);
    free(output->offsets);
  }
#else
  if(output->method==LPJ_SOCKET && output->socket!=NULL)
    close_socket(output->socket);
#endif
  free(output->files);
  freecoordarray(output->index);
  freecoordarray(output->index_all);
  free(output);
} /* of 'fcloseoutput' */
