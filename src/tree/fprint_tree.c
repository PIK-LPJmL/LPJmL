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

void fprint_tree(FILE *file,       /**< pointer to text file */
                 const Pft *pft,   /**< pointer to tree PFT */
                 int with_nitrogen /**< nitrogen cycle enabled */
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
  fputs("Mass:\t\t",file);
  if(with_nitrogen)
  {
    fprinttreephys2(file,tree->ind,pft->nind);
    fprintf(file,"\nExcess carbon:\t%g (gC/m2)\n",tree->excess_carbon*pft->nind);
    fprintf(file,"falloc:\t\t%g %g %g\n",tree->falloc.leaf,tree->falloc.root,tree->falloc.sapwood);
  }
  else
  {
    fprinttreephys2carbon(file,tree->ind,pft->nind);
    fputc('\n',file);
  }
} /* of 'fprint_tree' */
