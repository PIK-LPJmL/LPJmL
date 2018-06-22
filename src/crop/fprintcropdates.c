/**************************************************************************************/
/**                                                                                \n**/
/**     f  p  r  i  n  t  _ c  r  o  p  d  a  t  e  s  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints crop sowing dates                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintcropdates(FILE *file, /**< pointer to text file */
                     const Cropdates cropdates[], /**< array of crop dates */
                     const Pftpar pftpar[],       /**< crop PFT parameter array */
                     int ncft /**< number of crop dates */
                    )
{
  int cft;
  fputs("Cropdates:\n"
        "Crop                fall    last        spring  last          vern   last        fallow fallow\n"
        "                    sdate20 update_fall sdate20 update_spring date20 update_vern        irrig\n"
        "------------------- ------- ----------- ------- ------------- ------ ----------- ------ ------\n",file);
  for(cft=0;cft<ncft;cft++)
    fprintf(file,"%-19s %7d %11d %7d %13d %6d %11d %6d %6d\n",
            pftpar[cft].name,
            cropdates[cft].fall_sdate20,cropdates[cft].last_update_fall,
            cropdates[cft].spring_sdate20,cropdates[cft].last_update_spring,
            cropdates[cft].vern_date20,cropdates[cft].last_update_vern,
            cropdates[cft].fallow,cropdates[cft].fallow_irrig);
  fputs("------------------- ------- ----------- ------- ------------- ------ ----------- ------ ------\n",file);
} /* of 'fprintcropdates' */
