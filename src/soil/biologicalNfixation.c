/**************************************************************************************/
/**                                                                                \n**/
/**     b  i  o  l  o  g  i  c  a  l  N  f  i  x  a  t  i  o  n  .  c              \n**/
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
#include "grass.h"
#include "tree.h"

Real biologicalnfixation(const Stand *stand /**< pointer to stand */
                        )                   /** \return N fixation rate (gN/m2/d) */                
{
  const Pft *pft;
  const Pftgrass *grass;
  const Pfttree *tree;
  //const Pftcrop *crop;
  int p;
  Real bnf=0;

  foreachpft(pft,p,&stand->pftlist)
    switch(pft->par->type)
    {
      case TREE:
        tree=pft->data;
        if(tree->ind.root.carbon>param.min_c_bnf)
          bnf+=max(0,(0.0234*stand->cell->climbuf.aetp_mean-0.172)/10/NDAYYEAR)*pft->fpc;
        break;
      case GRASS:
        grass=pft->data;
        if(grass->ind.root.carbon>param.min_c_bnf)
          bnf+=max(0,(0.0234*stand->cell->climbuf.aetp_mean-0.172)/10/NDAYYEAR)*pft->fpc;
        break;
        // N fixation for crops now in n_uptake_crop.c
      /*case CROP:
        if(pft->par->id==PULSES || pft->par->id==OIL_CROPS_SOYBEAN)
        {
          crop=pft->data;
          if(crop->ind.root.carbon>param.min_c_bnf)
            bnf+=max(0,(0.0234*stand->cell->climbuf.aetp_mean-0.172)/10/NDAYYEAR)*pft->fpc;
        }
        break;
        */
    } /* of switch */
  return bnf;
} /* of 'biologicalnfixation' */

/* based on Cleveland, 1999                                               */
/* allow only for natural vegetation and grassland and pulses and soybean */
/* allow only if minimum root biomass is available.                       */
/* first approximation to avoid BNF in areas with no vegetation           */
/* later extend to Fisher et al. 2010 approach                            */
