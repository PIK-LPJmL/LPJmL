/**************************************************************************************/
/**                                                                                \n**/
/**                     c  m  p  m  a  p  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function compares to maps for identity                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool cmpmap(const Map *map1, /**< pointer to first map */
            const Map *map2  /**< pointer to second map */
           )                 /** \return TRUE if maps are identical */
{
  float *value1,*value2;
  int i;
  if(map1->isfloat!=map2->isfloat)
    return FALSE;
  if(getlistlen(map1->list)!=getlistlen(map2->list))
    return FALSE;
  if(map1->isfloat)
  {
    for(i=0;i<getlistlen(map1->list);i++)
    {
      value1=getlistitem(map1->list,i);
      value2=getlistitem(map2->list,i);
      if(*value1!=*value2)
        return FALSE;
    }
  }
  else
    for(i=0;i<getlistlen(map1->list);i++)
      if(strcmp(getlistitem(map1->list,i),getlistitem(map2->list,i)))
        return FALSE;
  return TRUE;
} /* of 'cmpmap' */
