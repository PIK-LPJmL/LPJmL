/**************************************************************************************/
/**                                                                                \n**/
/**             k  i  l  l  s  t  a  n  d  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for killed stands                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"

void killstand(Cell *cell,            /**< cell pointer */
               const Pftpar pftpar[], /**< PFT parameter array */
               int npft,              /**< number of natural PFTs */
               Bool intercrop,        /**< intercropping possible */
               int year               /**< simulation year (AD) */
              )
{
  Stand *stand;
  int s;
  Irrigation *data;
  Bool irrig;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==KILL)
    {
      if(stand->data!=NULL)
      {
        data=stand->data;
        irrig=data->irrigation;
      }
      else
        irrig=FALSE;
      if(setaside(cell,stand,pftpar,intercrop,npft,irrig,year))
      {
        delstand(cell->standlist,s);
        s--; /* stand has been killed, adjust stand index */
      }
    }
} /* of 'killstand' */
