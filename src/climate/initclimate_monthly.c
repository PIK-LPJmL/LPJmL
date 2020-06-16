/**************************************************************************************/
/**                                                                                \n**/
/**     i  n  i  t  c  l  i  m  a  t  e  _  m  o  n  t  h  l  y  .  c              \n**/
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

void initclimate_monthly(const Climate *climate, /**< Pointer to climate data */
                         Climbuf *climbuf,       /**< pointer to climate buffer */
                         int cell,               /**< cell index */
                         int month               /**< month (0..11) */
                        )                        /** \return void */
{
  if(!isdaily(climate->file_prec) && israndomprec(climate))
    prdaily(climbuf->dval_prec,ndaymonth[month],
            (getcellprec(climate,cell))[month],
            (getcellwet(climate,cell))[month]);
  climbuf->mtemp=climbuf->mprec=0;
} /* of 'initclimate_monthly' */
