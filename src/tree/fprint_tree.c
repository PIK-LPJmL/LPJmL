/**************************************************************************************/
/**                                                                                \n**/
/**                 f  p  r  i  n  t  _  t  r  e  e  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints tree-specific state variables in ASCII file                \n**/
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

void fprint_tree(FILE *file,     /**< pointer to text file */
                 const Pft *pft  /**< pointer to tree PFT */
                )
{
  const Pfttree *tree;
  tree=pft->data;
  fprintf(file,"Height:\t\t%g (m)\n"
               "Crown area:\t%g (m2)\n"
               "Bark thickness:\t%g (cm)\n"
               "isphen:\t\t%s\n"
               "Gddtw:\t\t%g\n",
               tree->height,tree->crownarea,tree->barkthickness,bool2str(tree->isphen),
               tree->gddtw);
  if(pft->par->phenology==RAINGREEN)
    fprintf(file,"Aphen raingreen:\t%g\n",tree->aphen_raingreen);
  fprintf(file,"Turn:\t\t%6.2f %6.2f %6.2f (gC/m2)\n",
          tree->turn.leaf,tree->turn.sapwood,tree->turn.root);
  fputs("Cmass:\t\t",file);
  fprinttreephys2(file,tree->ind,pft->nind);
  fputc('\n',file);
} /* of 'fprint_tree' */
