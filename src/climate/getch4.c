/***************************************************************************/
/**                                                                       **/
/**                      g  e  t  c  h  4  .  c                           **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function get atmospheric CH4 concentration for specified year.    **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change:  17.01.2009                                          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

Bool getch4(const Climate *climate, /**< Pointer to climate data */
		    Real *pch4,             /** atmospheric CH4 (ppm) */
            int year                /**< year (AD) */
)                        /** \return TRUE on error */
{
  if (year<-1300000)
    year = -130000;
  year -= climate->ch4.firstyear;
  if (year >= climate->ch4.nyear)
	    return TRUE;
  *pch4=(year<0) ? climate->ch4.data[0] : climate->ch4.data[year];
  return FALSE;
} /* of 'getch4' */
