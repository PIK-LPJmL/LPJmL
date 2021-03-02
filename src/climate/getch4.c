/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  c  h  4  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function get atmospheric CH4 concentration for specified year.             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool getch4(const Climate *climate, /**< Pointer to climate data */
            Real *pch4,             /**< atmospheric CH4 (ppm) */
            int year                /**< year (AD) */
           )                        /** \return TRUE on error */
{
  if (year<-1300000)
    year = -1300000;
  year -= climate->ch4.firstyear;
  if (year >= climate->ch4.nyear)
    return TRUE;
  *pch4=(year<0) ? climate->ch4.data[0] : climate->ch4.data[year];
  return FALSE;
} /* of 'getch4' */
