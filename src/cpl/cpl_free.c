/****************************************************************************************/
/**                                                                                  \n**/
/**                    c  p  l  _  f  r  e  e  .  c                                  \n**/
/**                                                                                  \n**/
/**     Function deallocates cpl data                                                \n**/
/**                                                                                  \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "cpl.h"

void cpl_free(Cpl *cpl)
{
  if(cpl!=NULL)
  {
#ifdef USE_MPI
    free(cpl->inlen);
    free(cpl->indisp);
    free(cpl->outlen);
    free(cpl->outdisp);
#endif
    free(cpl->index);
    free(cpl->src_index);
    free(cpl->coord);
    free(cpl);
  }
} /* of 'cpl_free' */
