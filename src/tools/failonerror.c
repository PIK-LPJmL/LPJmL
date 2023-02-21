/**************************************************************************************/
/**                                                                                \n**/
/**              f  a  i  l  o  n  e  r  r  o  r  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes out error message and terminates program                   \n**/
/**     The parallel version checks the return code on all tasks and               \n**/
/**     prints error message only on task zero.                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void failonerror(const Config *config, /**< LPJmL configuration */
                 int rc,               /**< return code (0=no error) */
                 int errorcode,        /**< error code returned from program */
                 const char *msg       /**< error message printed */
                )
{
  int sum;
  /* sum up error codes of all tasks and broadcast result */
#ifdef USE_MPI
  MPI_Allreduce(&rc,&sum,1,MPI_INT,MPI_SUM,config->comm);
#else
  sum=rc;
#endif
  if(sum)
  {
    /* error occurred, end program */
#ifdef USE_MPI
    MPI_Finalize(); /* finish MPI */
#endif
    if(config->rank==0)
    {
      if(config->ntask==1)
        fprintf(stderr,"ERROR%03d: %s, program terminated unsuccessfully.\n",errorcode,msg);
      else if(sum==config->ntask)
        fprintf(stderr,"ERROR%03d: %s on all tasks, program terminated unsuccessfully.\n",errorcode,msg);
      else
        fprintf(stderr,"ERROR%03d: %s on %d tasks, program terminated unsuccessfully.\n",errorcode,msg,sum);
    }
    if(config->coupled_model!=NULL)
      close_coupler(errorcode,config);
    exit(errorcode);     /* exit */
  }
} /* of 'failonerror' */
