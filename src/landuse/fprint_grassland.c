/**************************************************************************************/
/**                                                                                \n**/
/**      f  p  r  i  n  t  _  g  r  a  s  s  l  a  n  d  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints grassland data of stand                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

void fprint_grassland(FILE *file,          /**< pointer to text file */
                      const Stand *stand,  /**< pointer to stand */
                      const Pftpar *pftpar /**< PFT parameter array */
                     )
{
  static char *mode[]={"undefined","grazing","recovery"};
  const Grassland *grassland;
  grassland=stand->data;
  fprint_irrigation(file,&grassland->irrigation,pftpar);
  fprintf(file,"deficit LSU NE:\t%g\n"
          "deficit LSU MP:\t%g\n"
          "grazing days:\t%d\n"
          "recovery_day:\t%d\n"
          "paddocks:\t%d\n"
          "rotation mode:\t%s\n",
          grassland->deficit_lsu_ne,
          grassland->deficit_lsu_mp,
          grassland->rotation.grazing_days,
          grassland->rotation.recovery_days,
          grassland->rotation.paddocks,
          mode[grassland->rotation.mode]);
} /* of 'fprint_grassland' */
