/**************************************************************************************/
/**                                                                                \n**/
/**     m  o  n  t  h  l  y  o  u  t  p  u  t  _  i  m  a  g  e  .  c              \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Set output for Image climate data                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined IMAGE && defined COUPLED

void monthlyoutput_image(Output *output,
                         const Climate *climate, /**< climate data */
                         int cell,               /**< index of cell */
                         int month,              /**< month (0..11) */
                         const Config *config    /**< LPJml configuration */
                        )
{
  getoutput(output,TEMP_IMAGE,config)+=(getcelltemp(climate,cell))[month];
  getoutput(output,PREC_IMAGE,config)+=(getcellprec(climate,cell))[month];
  if(climate->data[0].sun!=NULL)
    getoutput(output,SUN_IMAGE,config)+=(getcellsun(climate,cell))[month];
  if(climate->data[0].wet!=NULL)
    getoutput(output,WET_IMAGE,config)+=(getcellwet(climate,cell))[month];
} /* of 'monthlyoutput_image' */

#endif
