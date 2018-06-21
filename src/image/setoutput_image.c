/**************************************************************************************/
/**                                                                                \n**/
/**           s  e  t  o  u  t  p  u  t  _  i  m  a  g  e  .  c                    \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Set output for Image land use                                              \n**/
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

void setoutput_image(Cell *cell,int ncft)
{
  int cft;
  for(cft=0;cft<ncft;cft++)
  {
    cell->output.cft_luc_image[cft]=cell->ml.landfrac[0].crop[cft];
    cell->output.cft_luc_image[cft+ncft+NGRASS+NBIOMASSTYPE]=cell->ml.landfrac[1].crop[cft];
  }
  cell->output.cft_luc_image[rothers(ncft)]=cell->ml.landfrac[0].grass[0];
  cell->output.cft_luc_image[rmgrass(ncft)]=cell->ml.landfrac[0].grass[1];
  cell->output.cft_luc_image[rbgrass(ncft)]=cell->ml.landfrac[0].biomass_grass;
  cell->output.cft_luc_image[rbtree(ncft)]=cell->ml.landfrac[0].biomass_tree;
  cell->output.cft_luc_image[rothers(ncft)+ncft+NGRASS+NBIOMASSTYPE]=cell->ml.landfrac[1].grass[0];
  cell->output.cft_luc_image[rmgrass(ncft)+ncft+NGRASS+NBIOMASSTYPE]=cell->ml.landfrac[1].grass[1];
  cell->output.cft_luc_image[rbgrass(ncft)+ncft+NGRASS+NBIOMASSTYPE]=cell->ml.landfrac[1].biomass_grass;
  cell->output.cft_luc_image[rbtree(ncft)+ncft+NGRASS+NBIOMASSTYPE]=cell->ml.landfrac[1].biomass_tree;
} /* of 'setoutput_image' */

#endif
