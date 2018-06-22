/**************************************************************************************/
/**                                                                                \n**/
/**        f  p  r  i  n  t  r  e  g  i  o  n  p  a  r  .  c                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintregionpar(FILE *file, /**< pointer to text file */
                     const Regionpar regionpar[], /**< array of region params */
                     int nregions /**< size of array */
                    )
{
  int i;
  fputs("Region                                       bifratio fuelratio woodconsum\n"
        "-------------------------------------------- -------- --------- ----------\n",file);
  for(i=0;i<nregions;i++)
   fprintf(file,"%44s %8.5f %9.5f %10.5f\n",regionpar[i].name,
           regionpar[i].bifratio,regionpar[i].fuelratio,regionpar[i].woodconsum);
  fputs("-------------------------------------------- -------- --------- ----------\n",file);
} /* of 'fprintregionpar' */
