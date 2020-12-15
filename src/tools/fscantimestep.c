/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  t  i  m  e  s  t  e  p  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads time step from a text file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef USE_JSON
#include <json-c/json.h>
#endif
#include "conf.h"
#include "types.h"

Bool fscantimestep(LPJfile *file, /**< pointer to a LPJ file */
                   int *timestep, /**< time szep (ANNUAL,MONTHLY,DAILY) */
                   Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
                  )               /** \return TRUE on error */
{
  char *time_step[]={"annual","monthly","daily"};
  if(isstring(file,"timestep"))
  {
    if(fscankeywords(file,timestep,"timestep",time_step,3,FALSE,verb))
      return TRUE;
    *timestep=-2-*timestep;
  }
  else
  {
    if(fscanint(file,timestep,"timestep",FALSE,verb))
      return TRUE;
    if(*timestep==1)
      *timestep=ANNUAL;
    else if(*timestep<0 && *timestep!=ANNUAL && *timestep!=MONTHLY && *timestep!=DAILY)
    {
      if(verb)
        fprintf(stderr,"ERROR229: Invalid value %d for time step, muste be >0.\n",
                *timestep);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'timestep' */
