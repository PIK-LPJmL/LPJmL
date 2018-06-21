/**************************************************************************************/
/**                                                                                \n**/
/**                             l  i  g  h  t  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates competition for light among PFTs                       \n**/
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
#include "grass.h"

void light(Stand *stand,        /**< Pointer to stand */
           int ntypes,          /**< number of PFT classes */
           const Real fpc_inc[] /**< FPC increment for each established PFT */
          )

{
  int p,i;
  Real ntree;        /* no of tree PFTs currently present */
  Real *fpc_total;   /* total grid FPC for PFTs */
  Real fpc_inc_tree; /* this years total FPC increment for tree PFTs */
  Real excess;       /* tree FPC or grass cover to be reduced */
  Real fpc_all;
  Real f,g,h;
  Pft *pft;
  /* Calculate total woody FPC, FPC increment and grass cover (= crown area) */

  fpc_inc_tree=0.0;
  ntree=0;
  fpc_all=0;
  f=g=h=epsilon;
  fpc_total=newvec(Real,ntypes);
  check(fpc_total);
  fpc_sum(fpc_total,ntypes,&stand->pftlist);

  foreachpft(pft,p,&stand->pftlist)
    if(istree(pft))
    {
      fpc_inc_tree+=fpc_inc[p];
      ntree++;
    }
  foreachpft(pft,p,&stand->pftlist)
  {
    switch(getpftpar(pft,type))
    {
      case TREE:
        if (fpc_total[TREE]>FPC_TREE_MAX)
        {
          if(ntree)
            f=(fpc_total[TREE]-FPC_TREE_MAX)/ntree;
          if(fpc_inc_tree>epsilon)
            g=(fpc_total[TREE]-FPC_TREE_MAX)/fpc_inc_tree;
          excess =(fpc_inc_tree>0.0) ?  g*fpc_inc[p] : f;

          /* Reduce individual density (and thereby gridcell-level biomass)*/
          /* so that total tree FPC reduced to 'fpc_tree_max'*/
          /* changed by Werner von Bloh to avoid FPE */

          light_tree(&stand->soil.litter,pft,excess);
        }
        break;

      case GRASS:
        if(fpc_total[GRASS]>(1.0-min(fpc_total[TREE],FPC_TREE_MAX)))
        {
          h=(min(fpc_total[TREE],FPC_TREE_MAX)+fpc_total[GRASS]-1.0)/fpc_total[GRASS];
          excess=h*pft->fpc;
          light_grass(&stand->soil.litter,pft,excess,fpc_total[GRASS]);
        }
        break;
    } /* of 'switch' */
  }  /* of 'foreachpft' */

  fpc_all=fpc_sum(fpc_total,ntypes,&stand->pftlist);
  i=0;
  while(fpc_all>1 && i<50){
    foreachpft(pft,p,&stand->pftlist)
    {
      if(pft->par->type==GRASS)
      {
          h=(min(fpc_total[TREE],FPC_TREE_MAX)+fpc_total[GRASS]-1.0)/fpc_total[GRASS];
          excess=h*pft->fpc;
          light_grass(&stand->soil.litter,pft,excess,fpc_total[GRASS]);
      }
    }
    fpc_all=fpc_sum(fpc_total,ntypes,&stand->pftlist)-epsilon;
    i++;
  }
  free(fpc_total);
} /* of 'light' */
