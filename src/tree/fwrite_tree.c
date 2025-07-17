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

static Bool fwritetreephys2(Bstruct file,const char *name,const Treephys2 *tree)
{
  bstruct_writebeginstruct(file,name);
  fwritestocks(file,"leaf",&tree->leaf);
  fwritestocks(file,"sapwood",&tree->sapwood);
  fwritestocks(file,"heartwood",&tree->heartwood);
  fwritestocks(file,"root",&tree->root);
  fwritestocks(file,"debt",&tree->debt);
  return bstruct_writeendstruct(file);
}

static Bool fwritetreeturn(Bstruct file,const char *name,const Treeturn *tree)
{
  bstruct_writebeginstruct(file,name);
  fwritestocks(file,"leaf",&tree->leaf);
  fwritestocks(file,"root",&tree->root);
  return bstruct_writeendstruct(file);
}

static Bool fwritetreephyspar(Bstruct file,const char *name,const Treephyspar *tree)
{
  bstruct_writebeginstruct(file,name);
  bstruct_writereal(file,"leaf",tree->leaf);
  bstruct_writereal(file,"sapwood",tree->sapwood);
  bstruct_writereal(file,"root",tree->root);
  return bstruct_writeendstruct(file);
}

Bool fwrite_tree(Bstruct file,    /**< pointer to binary file */
                 const Pft *pft /**< pointer to tree PFT */
                )               /** \return TRUE on error */
{
  const Pfttree *tree;
  tree=pft->data;
  bstruct_writereal(file,"height",tree->height);
  bstruct_writereal(file,"crownarea",tree->crownarea);
  bstruct_writereal(file,"barkthickness",tree->barkthickness);
  bstruct_writereal(file,"gddtw",tree->gddtw);
  bstruct_writereal(file,"aphen_raingreen",tree->aphen_raingreen);
  bstruct_writebool(file,"isphen",tree->isphen);
  fwritetreeturn(file,"turn",&tree->turn);
  fwritetreeturn(file,"turn_litt",&tree->turn_litt);
  bstruct_writereal(file,"turn_nbminc",tree->turn_nbminc);
  fwritetreephys2(file,"ind",&tree->ind);
  if(pft->par->cultivation_type==ANNUAL_TREE)
  {
    fwritestocks(file,"fruit",&tree->fruit);
    bstruct_writeint(file,"boll_age",tree->boll_age);
  }
  bstruct_writereal(file,"excess_carbon",tree->excess_carbon);
  bstruct_writereal(file,"nfertilizer",tree->nfertilizer);
  bstruct_writereal(file,"nmanure",tree->nmanure);
  bstruct_writeint(file,"nfert_event",tree->nfert_event);
  fwritetreephyspar(file,"falloc",&tree->falloc);
  return FALSE;
} /* of 'fwrite_tree' */
