/**************************************************************************************/
/**                                                                                \n**/
/**     r  e  c  e  i  v  e  _  i  m  a  g  e  _  d  a  t  a  .  c                 \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef IMAGE

Bool receive_image_data(Cell *cell,          /**< LPJ grid */
                        int npft,            /**< number of natural PFTs */
                        int ncft,            /**< number of crop PFTS */
                        const Config *config /**< LPJ configuration */
                       )                     /** \return TRUE on error */
{
  if(config->withlanduse!=NO_LANDUSE)
  {
    if(receive_image_luc(cell,npft,ncft,config))
    {
      fprintf(stderr,"ERROR172: Simulation stopped for lack of LUC data from IMAGE\n");
      fflush(stderr);
      return TRUE;
    }
  }
  if(receive_image_productpools(cell,config))
  {
    fprintf(stderr,"ERROR173: Simulation stopped for lack of product pool data from IMAGE\n");
    fflush(stderr);
    return TRUE;
  }
  return FALSE;
} /* of 'receive_image_data' */

#endif
