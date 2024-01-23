/**************************************************************************************/
/**                                                                                \n**/
/**                   b  i  l  i  n  e  a  r  .  c                                 \n**/
/**                                                                                \n**/
/**     Bilinear interpolation function                                            \n**/
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
#include "types.h"
#include "errmsg.h"
#include "numeric.h"

Real bilinear(const Real data[],int ny,int nx,Real y,Real x)
{
  Real f00,f01,f10,f11;
  int ix,iy;
#ifdef SAFE
  if(x<=0 || x>nx-1)
    fail(OUT_OF_RANGE_ERR,FALSE,"Index x=%g out of bounds in bilinear()",x);
  if(y<=0 || y>ny-1)
    fail(OUT_OF_RANGE_ERR,FALSE,"Index y=%g out of bounds in bilinear()",y);
#endif
  ix=(int)x;
  iy=(int)y;
  x-=floor(x);
  y-=floor(y);
  f00=data[iy*nx+ix];
  f01=(y==0) ? 0 : data[(iy+1)*nx+ix];
  f10=(x==0) ? 0 : data[iy*nx+ix+1];
  f11=(x==0 || y==0) ? 0: data[(iy+1)*nx+ix+1];
  return f00*(1-x)*(1-y)+f10*x*(1-y)+f01*(1-x)*y+f11*x*y;
} /* of 'bilinear' */ 
   
