/**************************************************************************************/
/**                                                                                \n**/
/**        f  p  r  i  n  t  h  e  a  d  e  r  .  c                                \n**/
/**                                                                                \n**/
/**     Printing file header for LPJ related files.                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintheader(FILE *file, /**< pointer to text file */
                  const Header *header /**< file header to print */
                 )
{
  if(header->datatype>=0 && header->datatype<5)
    fprintf(file,"Type:\t\t%s\n",typenames[header->datatype]);
  else
    fprintf(file,"Type:\t\t%d\n",(int)header->datatype);
  if(header->order>=CELLYEAR && header->order<=CELLSEQ)
    fprintf(file,"Order:\t\t%s\n",ordernames[header->order-1]);
  else
    fprintf(file,"Order:\t\t%d\n",header->order);
  fprintf(file,"First year:\t%6d\n"
          "Last year:\t%6d\n"
          "First cell:\t%6d\n"
          "Number of cells:%6d\n"
          "Number of bands:%6d\n"
          "Number of steps:%6d\n"
          "Time step:      %6d\n"
          "cellsize:\t%10.6f %10.6f\n"
          "conversion factor:\t%g\n",
          header->firstyear,header->firstyear+(header->nyear-1)*header->timestep,
          header->firstcell,header->ncell,header->nbands,header->nstep,header->timestep,header->cellsize_lon,
          header->cellsize_lat,header->scalar);
} /* of 'fprintheader' */
