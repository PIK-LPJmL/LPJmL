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

#if defined IMAGE && defined COUPLED

void setoutput_image(Cell *cell,int ncft,const Config *config)
{
  int cft;
  for(cft=0;cft<ncft;cft++)
  {
    getoutputindex(&cell->output,LUC_IMAGE,cft,config)=cell->ml.landfrac[0].crop[cft];
    getoutputindex(&cell->output,LUC_IMAGE,cft+getnirrig(ncft,config),config)=cell->ml.landfrac[1].crop[cft];
  }
  getoutputindex(&cell->output,LUC_IMAGE,rothers(ncft),config)=cell->ml.landfrac[0].grass[0];
  getoutputindex(&cell->output,LUC_IMAGE,rmgrass(ncft),config)=cell->ml.landfrac[0].grass[1];
  getoutputindex(&cell->output,LUC_IMAGE,rbgrass(ncft),config)=cell->ml.landfrac[0].biomass_grass;
  getoutputindex(&cell->output,LUC_IMAGE,rbtree(ncft),config)=cell->ml.landfrac[0].biomass_tree;
  getoutputindex(&cell->output,LUC_IMAGE,rwp(ncft),config)=cell->ml.landfrac[0].woodplantation;
  getoutputindex(&cell->output,LUC_IMAGE,rothers(ncft)+getnirrig(ncft,config),config)=cell->ml.landfrac[1].grass[0];
  getoutputindex(&cell->output,LUC_IMAGE,rmgrass(ncft)+getnirrig(ncft,config),config)=cell->ml.landfrac[1].grass[1];
  getoutputindex(&cell->output,LUC_IMAGE,rbgrass(ncft)+getnirrig(ncft,config),config)=cell->ml.landfrac[1].biomass_grass;
  getoutputindex(&cell->output,LUC_IMAGE,rbtree(ncft)+getnirrig(ncft,config),config)=cell->ml.landfrac[1].biomass_tree;
  getoutputindex(&cell->output,LUC_IMAGE,rwp(ncft)+getnirrig(ncft,config),config)=cell->ml.landfrac[1].woodplantation;
} /* of 'setoutput_image' */

#endif
