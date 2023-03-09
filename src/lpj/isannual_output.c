/**************************************************************************************/
/**                                                                                \n**/
/**      i  s  n  a  n  n  u  a  l  _  o  u  t  p  u  t  .  c                      \n**/
/**                                                                                \n**/
/** Function determines whether output is annual only output                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool isannual_output(int index /**< index for output file */
                    )          /** \return output is annual only output (TRUE/FALSE) */
{
  switch(index)
  {
    case CFTFRAC: case SDATE : case SDATE2 : case HDATE : case HDATE2:
    case SYEAR: case SYEAR2:
      return TRUE;
    default:
      return FALSE;
  }
} /* of 'isannual_output' */
