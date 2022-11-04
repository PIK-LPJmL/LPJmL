/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  _  t  r  e  e  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function writes tree specific PFT variables into restart file                  \n**/
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

Bool fwrite_tree(FILE *file,    /**< pointer to binary file */
                 const Pft *pft /**< pointer to tree PFT */
                )               /** \return TRUE on error */
{
  const Pfttree *tree;
  tree=pft->data;
  fwrite1(&tree->height,sizeof(Real),file);
  fwrite1(&tree->crownarea,sizeof(Real),file);
  fwrite1(&tree->barkthickness,sizeof(Real),file);
  fwrite1(&tree->gddtw,sizeof(Real),file);
  fwrite1(&tree->aphen_raingreen,sizeof(Real),file);
  fwrite1(&tree->isphen,sizeof(int),file);
  fwrite1(&tree->turn,sizeof(Treeturn),file);
  fwrite1(&tree->turn_litt,sizeof(Treeturn),file);
  fwrite1(&tree->turn_nbminc,sizeof(Real),file);
  fwrite1(&tree->ind,sizeof(Treephys2),file);
  if(pft->par->cultivation_type==ANNUAL_TREE)
  {
    fwrite1(&tree->fruit,sizeof(Stocks),file);
    fwrite1(&tree->boll_age,sizeof(int),file);
  }
  fwrite1(&tree->excess_carbon,sizeof(Real),file);
  fwrite1(&tree->nfertilizer,sizeof(Real),file);
  fwrite1(&tree->nmanure,sizeof(Real),file);
  fwrite1(&tree->nfert_event,sizeof(int),file);
  fwrite1(&tree->falloc,sizeof(Treephyspar),file);
  return FALSE;
} /* of 'fwrite_tree' */
