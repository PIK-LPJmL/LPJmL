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

void fprint_grassland(FILE *file,        /**< pointer to text file */
                      const Stand *stand /**< pointer to stand */
                     )
{
  static char *mode[]={"undefined","grazing","recovery"};
  const Grassland *grassland;
  grassland=stand->data;
  fprint_irrigation(file,&grassland->irrigation);
  fprintf(file,"Nr. of lsus ext:\t%g\n"
          "Nr. of lsus int:\t%g\n"
          "grazing days:\t%d\n"
          "recovery_day:\t%d\n"
          "paddocks:\t%d\n"
          "rotation mode:\t%s\n",
          grassland->nr_of_lsus_ext,
          grassland->nr_of_lsus_int,
          grassland->rotation.grazing_days,
          grassland->rotation.recovery_days,
          grassland->rotation.paddocks,
          mode[grassland->rotation.rotation_mode]);
} /* of 'fprint_grassland' */
