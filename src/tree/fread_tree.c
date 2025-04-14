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

static Bool freadtreephys2(Bstruct file,const char *name,Treephys2 *tree)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(freadstocks(file,"leaf",&tree->leaf))
    return TRUE;
  if(freadstocks(file,"sapwood",&tree->sapwood))
    return TRUE;
  if(freadstocks(file,"heartwood",&tree->heartwood))
    return TRUE;
  if(freadstocks(file,"root",&tree->root))
    return TRUE;
  if(freadstocks(file,"debt",&tree->debt))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'freadtreephys2' */

static Bool freadtreeturn(Bstruct file,const char *name,Treeturn *tree)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(freadstocks(file,"leaf",&tree->leaf))
    return TRUE;
  if(freadstocks(file,"root",&tree->root))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'freadtreeturn' */

static Bool freadtreephyspar(Bstruct file,const char *name,Treephyspar *tree)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(bstruct_readreal(file,"leaf",&tree->leaf))
    return TRUE;
  if(bstruct_readreal(file,"sapwood",&tree->sapwood))
    return TRUE;
  if(bstruct_readreal(file,"root",&tree->root))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'freadtreephyspar' */

Bool fread_tree(Bstruct file, /**< pointer to binary file */
                Pft *pft,   /**< pointer to PFT read */
                Bool UNUSED(separate_harvests)
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
  if(bstruct_readreal(file,"height",&tree->height))
    return TRUE;
  if(bstruct_readreal(file,"crownarea",&tree->crownarea))
    return TRUE;
  if(bstruct_readreal(file,"barkthickness",&tree->barkthickness))
    return TRUE;
  if(bstruct_readreal(file,"gddtw",&tree->gddtw))
    return TRUE;
  if(bstruct_readreal(file,"aphen_raingreen",&tree->aphen_raingreen))
    return TRUE;
  if(bstruct_readbool(file,"isphen",&tree->isphen))
    return TRUE;
  if(freadtreeturn(file,"turn",&tree->turn))
    return TRUE;
  if(freadtreeturn(file,"turn_litt",&tree->turn_litt))
    return TRUE;
  if(freadtreephys2(file,"ind",&tree->ind))
    return TRUE;
  if(pft->par->cultivation_type==ANNUAL_TREE)
  {
    if(freadstocks(file,"fruit",&tree->fruit))
      return TRUE;
    if(bstruct_readint(file,"boll_age",&tree->boll_age))
      return TRUE;
  }
  else
    tree->fruit.carbon=tree->fruit.nitrogen=0;
  if(bstruct_readreal(file,"excess_carbon",&tree->excess_carbon))
    return TRUE;
  if(bstruct_readreal(file,"nfertilizer",&tree->nfertilizer))
    return TRUE;
  if(bstruct_readreal(file,"nmanure",&tree->nmanure))
    return TRUE;
  if(bstruct_readint(file,"nfert_event",&tree->nfert_event))
    return TRUE;
  return freadtreephyspar(file,"falloc",&tree->falloc);
} /* of 'fread_tree' */
