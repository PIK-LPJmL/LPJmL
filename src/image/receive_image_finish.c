/**************************************************************************************/
/**                                                                                \n**/
/**     r  e  c  e  i  v  e  _  i  m  a  g  e  _  f  i  n  i  s  h  .  c           \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Set output for Image land use                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined IMAGE && defined COUPLED

Real receive_image_finish(const Config *config /* Grid configuration */
                      )                     /* returns finsh */
{
  double finish;
#ifdef USE_MPI
  if(config.rank==0)
  {
#endif
#ifdef DEBUG_IMAGE
  printf("Waiting for IMAGE-INTERFACE\n");
  fflush(stdout);
#endif
  readdouble_socket(config->in,&finish,1);
#ifdef DEBUG_IMAGE
  printf("Received finish from IMAGE-INTERFACE %g\n",finish);
  fflush(stdout);
#endif
#ifdef USE_MPI
  }
  MPI_Bcast(&finish,1,MPI_DOUBLE,0,config.comm);
#endif
  return (Real)finish;
} /* of 'receive_image_finish' */

#endif
