/**************************************************************************************/
/**                                                                                \n**/
/**      c  l  o  s  e  o  u  t  p  u  t  _  y  e  a  r  l  y  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function closes yearly output files                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void closeoutput_yearly(Outputfile *output, /**< Output data */
                        const Config *config /**< LPJmL configuration */
                       )
{
 int i;
 if(isroot(*config))
   for(i=0;i<config->n_out;i++)
     if(config->outputvars[i].oneyear && output->files[config->outputvars[i].id].isopen)
       switch(config->outputvars[i].filename.fmt)
       {
         case RAW: case TXT: case CLM:
           fclose(output->files[config->outputvars[i].id].fp.file);
           break;
         case CDF:
           close_netcdf(&output->files[config->outputvars[i].id].fp.cdf);
           break;
       }
} /* of 'closeoutput_yearly' */
