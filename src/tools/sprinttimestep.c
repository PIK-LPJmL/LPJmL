/**************************************************************************************/
/**                                                                                \n**/
/**                   s  p  r  i  n  t  t  i  m  e  s  t  e  p  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints timee step into string                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "types.h"

char *sprinttimestep(String s,    /**< string time step is printed */
                     int timestep /**< time step (ANNUAL,MONTHLY,DAILY) */
                    )             /** \return pointer to string */
{
  char timenames[]={'Y','M','D'};
  if(timestep<0)
  {
    s[0]=timenames[-2-timestep];
    s[1]='\0';
  }
  else
    snprintf(s,STRING_LEN,"%d",timestep);
  return s;
} /* of 'sprinttimestep' */
