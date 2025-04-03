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

static Bool freadtreephys2(FILE *file,const char *name,Treephys2 *tree,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readstocks(file,"leaf",&tree->leaf,swap))
    return TRUE;
  if(readstocks(file,"sapwood",&tree->sapwood,swap))
    return TRUE;
  if(readstocks(file,"heartwood",&tree->heartwood,swap))
    return TRUE;
  if(readstocks(file,"root",&tree->root,swap))
    return TRUE;
  if(readstocks(file,"debt",&tree->debt,swap))
    return TRUE;
  return readendstruct(file);
}

static Bool freadtreeturn(FILE *file,const char *name,Treeturn *tree,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readstocks(file,"leaf",&tree->leaf,swap))
    return TRUE;
  if(readstocks(file,"root",&tree->root,swap))
    return TRUE;
  return readendstruct(file);
}

static Bool freadtreephyspar(FILE *file,const char *name,Treephyspar *tree,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readreal(file,"leaf",&tree->leaf,swap))
    return TRUE;
  if(readreal(file,"sapwood",&tree->sapwood,swap))
    return TRUE;
  if(readreal(file,"root",&tree->root,swap))
    return TRUE;
  return readendstruct(file);
}

Bool fread_tree(FILE *file, /**< pointer to binary file */
                Pft *pft,   /**< pointer to PFT read */
                Bool UNUSED(separate_harvests),
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
  if(readreal(file,"height",&tree->height,swap))
    return TRUE;
  if(readreal(file,"crownarea",&tree->crownarea,swap))
    return TRUE;
  if(readreal(file,"barkthickness",&tree->barkthickness,swap))
    return TRUE;
  if(readreal(file,"gddtw",&tree->gddtw,swap))
    return TRUE;
  if(readreal(file,"aphen_raingreen",&tree->aphen_raingreen,swap))
    return TRUE;
  if(readbool(file,"isphen",&tree->isphen,swap))
    return TRUE;
  if(freadtreeturn(file,"turn",&tree->turn,swap))
    return TRUE;
  if(freadtreeturn(file,"turn_litt",&tree->turn_litt,swap))
    return TRUE;
  if(readreal(file,"turn_nbminc",&tree->turn_nbminc,swap))
    return TRUE;
  if(freadtreephys2(file,"ind",&tree->ind,swap))
    return TRUE;
  if(pft->par->cultivation_type==ANNUAL_TREE)
  {
    if(readstocks(file,"fruit",&tree->fruit,swap))
      return TRUE;
    if(readint(file,"boll_age",&tree->boll_age,swap))
      return TRUE;
  }
  else
    tree->fruit.carbon=tree->fruit.nitrogen=0;
  if(readreal(file,"excess_carbon",&tree->excess_carbon,swap))
    return TRUE;
  if(readreal(file,"nfertilizer",&tree->nfertilizer,swap))
    return TRUE;
  if(readreal(file,"nmanure",&tree->nmanure,swap))
    return TRUE;
  if(readint(file,"nfert_event",&tree->nfert_event,swap))
    return TRUE;
  return freadtreephyspar(file,"falloc",&tree->falloc,swap);
} /* of 'fread_tree' */
