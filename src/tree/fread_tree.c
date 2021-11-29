/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  a  d  _  t  r  e  e  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads tree-specific PFT variables from restart file               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "tree.h"

Bool fread_tree(FILE *file, /**< pointer to binary file */
                Pft *pft,   /**< pointer to PFT read */
                Bool UNUSED(double_harvest),
                Bool swap   /**< Byte order has to be changed (TRUE/FALSE) */
               )            /** \return TRUE on error */
{
  Pfttree *tree;
  tree=new(Pfttree);
  pft->data=tree;
  if(tree==NULL)
  {
    printallocerr("tree");
    return TRUE;
  }
  pft->nlimit=0.0;
  freadreal1(&tree->height,swap,file);
  freadreal1(&tree->crownarea,swap,file);
  freadreal1(&tree->barkthickness,swap,file);
  freadreal1(&tree->gddtw,swap,file);
  freadreal1(&tree->aphen_raingreen,swap,file);
  freadint1(&tree->isphen,swap,file);
  freadreal((Real *)&tree->turn,sizeof(Treeturn)/sizeof(Real),swap,file);
  freadreal((Real *)&tree->turn_litt,sizeof(Treeturn)/sizeof(Real),swap,file);
  freadreal1(&tree->turn_nbminc,swap,file);
  freadreal((Real *)&tree->ind,sizeof(Treephys2)/sizeof(Real),swap,file);
  if(pft->par->cultivation_type==ANNUAL_TREE)
  {
    freadreal((Real *)&tree->fruit,sizeof(Stocks)/sizeof(Real),swap,file);
    freadint1(&tree->boll_age,swap,file);
  }
  else
    tree->fruit.carbon=tree->fruit.nitrogen=0;
  freadreal1(&tree->excess_carbon,swap,file);
  freadreal1(&tree->nfertilizer,swap,file);
  freadreal1(&tree->nmanure,swap,file);
  return freadreal((Real *)&tree->falloc,sizeof(Treephyspar)/sizeof(Real),swap,file)!=sizeof(Treephyspar)/sizeof(Real);
} /* of 'fread_tree' */
