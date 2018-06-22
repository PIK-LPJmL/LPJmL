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

#ifdef USE_MPI

void failonerror(const Config *config, /**< LPJmL configuration */
                 int rc,               /**< return code (0=no error) */
                 int errorcode,        /**< error code returned from program */
                 const char *msg       /**< error message printed */
                )
{
  int sum;
  String s;
  /* sum up error codes of all tasks and broadcast result */
  MPI_Allreduce(&rc,&sum,1,MPI_INT,MPI_SUM,config->comm);
  if(sum)
  {
    /* error occurred, end program */
    MPI_Finalize(); /* finish MPI */
    if(config->rank==0)
    {
      if(config->ntask==1)
        fail(errorcode,FALSE,msg); /* write error message and exit*/
      if(sum==config->ntask)
        snprintf(s,STRING_LEN,"%s on all tasks",msg);
      else
        snprintf(s,STRING_LEN,"%s on %d tasks",msg,sum);
      fail(errorcode,FALSE,s); /* write error message and exit*/
    }
    else
      exit(errorcode);     /* exit only */
  }
} /* of 'failonerror' */

#endif
