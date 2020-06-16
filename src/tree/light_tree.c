/**************************************************************************************/
/**                                                                                \n**/
/**              l  i  g  h  t  _  t  r  e  e  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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

void light_tree(Litter *litter,Pft *pft,Real excess)
{
  
  Real nind_kill;  /* reduction in individual density to reduce tree FPC to
                      permitted maximum (indiv/m2)*/

#ifdef DEBUG3
  printf("light: %g %g %s %g\n",pft->fpc,pft->nind,pft->par->name,excess);
#endif
  if (pft->prescribe_fpc)
    nind_kill=0;
  else
  {
    nind_kill=(excess<1e-20) ? 0 : pft->nind*(excess/pft->fpc); 
    pft->nind-=nind_kill;
    litter_update_tree(litter,pft,nind_kill);
  }
  fpc_tree(pft);

} /* of 'light_tree' */
