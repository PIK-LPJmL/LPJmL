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

static Bool fwritetreephys2(FILE *file,const char *name,const Treephys2 *tree)
{
  writestruct(file,name);
  writestocks(file,"leaf",&tree->leaf);
  writestocks(file,"sapwood",&tree->sapwood);
  writestocks(file,"heartwood",&tree->heartwood);
  writestocks(file,"root",&tree->root);
  writestocks(file,"debt",&tree->debt);
  return writeendstruct(file);
}

static Bool fwritetreeturn(FILE *file,const char *name,const Treeturn *tree)
{
  writestruct(file,name);
  writestocks(file,"leaf",&tree->leaf);
  writestocks(file,"root",&tree->root);
  return writeendstruct(file);
}

static Bool fwritetreephyspar(FILE *file,const char *name,const Treephyspar *tree)
{
  writestruct(file,name);
  writereal(file,"leaf",tree->leaf);
  writereal(file,"sapwood",tree->sapwood);
  writereal(file,"root",tree->root);
  return writeendstruct(file);
}

Bool fwrite_tree(FILE *file,    /**< pointer to binary file */
                 const Pft *pft /**< pointer to tree PFT */
                )               /** \return TRUE on error */
{
  const Pfttree *tree;
  tree=pft->data;
  writereal(file,"height",tree->height);
  writereal(file,"crownarea",tree->crownarea);
  writereal(file,"barkthickness",tree->barkthickness);
  writereal(file,"gddtw",tree->gddtw);
  writereal(file,"aphen_raingreen",tree->aphen_raingreen);
  writebool(file,"isphen",tree->isphen);
  fwritetreeturn(file,"turn",&tree->turn);
  fwritetreeturn(file,"turn_litt",&tree->turn_litt);
  writereal(file,"turn_nbminc",tree->turn_nbminc);
  fwritetreephys2(file,"ind",&tree->ind);
  if(pft->par->cultivation_type==ANNUAL_TREE)
  {
    writestocks(file,"fruit",&tree->fruit);
    writeint(file,"boll_age",tree->boll_age);
  }
  writereal(file,"excess_carbon",tree->excess_carbon);
  writereal(file,"nfertilizer",tree->nfertilizer);
  writereal(file,"nmanure",tree->nmanure);
  writeint(file,"nfert_event",tree->nfert_event);
  fwritetreephyspar(file,"falloc",&tree->falloc);
  return FALSE;
} /* of 'fwrite_tree' */
