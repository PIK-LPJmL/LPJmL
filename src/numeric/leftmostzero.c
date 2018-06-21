/**************************************************************************************/
/**                                                                                \n**/
/**               l  e  f  t  m  o  s  t  z  e  r  o  .  c                         \n**/
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

#include <stdio.h>
#include "types.h"
#include "numeric.h"

#define NSEG 20

Real leftmostzero(Real (*fcn)(Real,void *), /**< function to find leftmost zero */
                  Real x1,                  /**< lower bound of interval */
                  Real x2,                  /**< upper bound of interval */
                  void *data,               /**< pointer to additional data for function */
                  Real xacc,                /**< accuracy in x */
                  Real yacc,                /**< accuracy in y */
                  int maxiter               /**< maximum number of iterations */
                 )                          /** \return position of zero of function */
{
  Real dx,xmid,swap;
  if(x2<x1)
  {
    swap=x1;
    x1=x2;
    x2=swap;
  }

  dx=(x2-x1)/NSEG;
  if((*fcn)(x1,data)<0)
    for(xmid=x1+dx;(*fcn)(xmid,data)<0 && xmid<=x2-dx;xmid+=dx);  
  else
    for(xmid=x1+dx;(*fcn)(xmid,data)>0 && xmid<=x2-dx;xmid+=dx);
  return bisect(fcn,xmid-dx,xmid,data,xacc,yacc,maxiter);
}  /* of 'leftmostzero' */ 
