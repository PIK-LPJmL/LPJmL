/**************************************************************************************/
/**                                                                                \n**/
/**              i  s  e  r  r  o  r  .  c                                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     The parallel version checks the return code on all tasks                   \n**/
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

Bool iserror(int rc,              /**< return code (0=no error) */
             const Config *config /**< LPJ configuration */
            )             /** \return TRUE if one or more tasks have errors */
{
  int sum;
  /* sum up error codes of all tasks and broadcast result */
  MPI_Allreduce(&rc,&sum,1,MPI_INT,MPI_SUM,config->comm);
  return sum!=0;
} /* of 'iserror' */

#endif
