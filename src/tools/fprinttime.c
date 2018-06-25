/**************************************************************************************/
/**                                                                                \n**/
/**                   f  p  r  i  n  t  t  i  m  e  .  c                           \n**/
/**                                                                                \n**/
/**     Function prints seconds in format d+hh:mm:ss                               \n**/
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
#include "types.h"

void fprinttime(FILE *file, /**< pointer to text file */
                int s       /**< number of seconds to print */
               )
{
  int h,m,d;
  d=s/(3600*24);
  if(d>0)
  {
    fprintf(file,"%d+",d);
    s-=d*3600*24;
  }
  h=s/3600;
  s-=h*3600;
  m=s/60;
  fprintf(file,"%02d:%02d:%02d",h,m,s-m*60);
} /* of 'fprinttime' */
