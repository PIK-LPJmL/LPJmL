/**************************************************************************************/
/**                                                                                \n**/
/**                   b  i  s  e  c  t  .  c                                       \n**/
/**                                                                                \n**/
/**     Finds a zero of a function using the bisectioning algorithm                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"   /* Definition of datatype Real  */
#include "numeric.h"

Real bisect(Real (*fcn)(Real,void *), /**< function */
            Real xlow,  /**< lower bound of interval */
            Real xhigh, /**< upper bound of interval */
            void *data, /**< pointer to additional data for function */
            Real xacc,  /**< accuracy in x */
            Real yacc,  /**< accuracy in y */
            int maxit,  /**< maximum number of iterations */
            int *it     /**< iterations performed */
           )            /** \return position of zero of function */
{
  int i;
  Real ylow,ymid,xmid;
  Real ymin=1e9,xmin;
  ylow=(*fcn)(xlow,data);
  xmin=(xlow+xhigh)*0.5;
  for(i=0;i<maxit;i++)
  {
    xmid=(xlow+xhigh)*0.5;
    if(xhigh-xlow<xacc)
    {
      *it=i;
      return xmid;
    }
    ymid=(*fcn)(xmid,data);
    if(fabs(ymid)<ymin)
    {
      ymin=fabs(ymid);
      xmin=xmid;
    }
    if(fabs(ymid)<yacc)
    {
      *it=i;
      return xmid;
    }
    if(ylow*ymid<=0)
      xhigh=xmid;
    else
    {
      xlow=xmid;
      ylow=ymid;
    } 
  } /* of for */
  *it=i;
  return xmin;
} /* of 'bisect' */
