/**************************************************************************************/
/**                                                                                \n**/
/**                   f  p  r  i  n  t  i  n  t  f  .c                             \n**/
/**                                                                                \n**/
/**     Function prints integer in the format dd,dddd,ddd                          \n**/
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

void fprintintf(FILE *file, /**< pointer to text file */
                int num     /** integer to be print */
               )
{
  int i,count,d;
  if(num<0)
  {
    fprintf(file,"-");
    num=-num;
  }
  d=1;
  count=0;
  for(i=num;i>=1000;i/=1000)
  {
    d*=1000;
    count++;
  }
  fprintf(file,"%d",i);
  for(i=0;i<count;i++)
  {
    num=num % d;
    d/=1000;
    fprintf(file,",%03d",num/d);
  }
} /* of fprintintf' */
