/**************************************************************************************/
/**                                                                                \n**/
/**         u  p  d  a  t  e  _  n  e  s  t  e  r  o  v  .v                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function updates nesterov index                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define nesterovindex(tmin,tmax) ((tmax)*(tmax-((tmin)-4)))

void update_nesterov(Cell *cell, /**< pointer to cell */
                     const Dailyclimate *climate /**< daily climate data */
                    )
{

  if (climate->prec >= 3.0 || (climate->tmin - 4.0) <= 0) 
    cell->ignition.nesterov_accum=0;
  else
    cell->ignition.nesterov_accum += nesterovindex(climate->tmin,climate->tmax);
} /* of 'update_nesterov' */
