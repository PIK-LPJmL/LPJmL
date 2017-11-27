/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  a  d  _  t  r  e  e  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "tree.h"

Bool fread_tree(FILE *file,Pft *pft,Bool swap)
{
  Pfttree *tree;
  tree=new(Pfttree);
  pft->data=tree;
  if(tree==NULL)
    return TRUE;
  freadreal1(&tree->height,swap,file);
  freadreal1(&tree->crownarea,swap,file);
  freadreal1(&tree->barkthickness,swap,file);
  freadreal1(&tree->gddtw,swap,file);
  freadreal1(&tree->aphen_raingreen,swap,file);
  freadint1(&tree->isphen,swap,file);
  freadreal((Real *)&tree->turn,sizeof(Treephys)/sizeof(Real),swap,file);
  tree->turn_litt.leaf=tree->turn_litt.root=0;
  return freadreal((Real *)&tree->ind,sizeof(Treephys2)/sizeof(Real),swap,file)!=sizeof(Treephys2)/sizeof(Real);
 /* fpc_tree(pft); */
} /* of 'fread_tree' */
