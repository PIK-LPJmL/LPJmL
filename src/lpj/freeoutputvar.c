/**************************************************************************************/
/**                                                                                \n**/
/**            f  r  e  e  o  u  t  p  u  t  v  a  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freeoutputvar(Variable *var, /**< Variable description array */
                   int nout_max /**< size of variable description array */
                  )
{
  int i;
  if(var!=NULL)
  {
    for(i=0;i<nout_max;i++)
    {
      free(var[i].name);
      free(var[i].var);
      free(var[i].descr);
      free(var[i].unit);
    }
    free(var);
  }
} /* of 'freeoutputvar' */
