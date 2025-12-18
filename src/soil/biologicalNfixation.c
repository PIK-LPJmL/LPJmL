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

Real biologicalnfixation(const Stand *stand, /**< pointer to stand */
                        int npft,           /**< number of natural PFTs */
                        int ncft,           /**< number of crop PFTs */
                        const Config *config  /**< LPJmL configuration */
                       )                   /** \return N fixation rate (gN/m2/d) */
{
  const Pft *pft;
  const Pftgrass *grass;
  const Pfttree *tree;
  Irrigation *data;
  int p, nnat, nirrig;
   nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);
  Real bnf = 0;
  Real pft_bnf = 0;

  foreachpft(pft,p,&stand->pftlist)
  {
    switch(pft->par->type)
    {
      case TREE:
        tree=pft->data;
        if(tree->ind.root.carbon>param.min_c_bnf &&
            ((tree->ind.leaf.nitrogen+pft->bm_inc.nitrogen/pft->nind*tree->falloc.leaf)/(tree->ind.leaf.carbon+pft->bm_inc.carbon/pft->nind*tree->falloc.leaf)<(pft->par->ncleaf.high)))
          pft_bnf=max(0,(0.0234*stand->cell->climbuf.aetp_mean-0.172)/10/NDAYYEAR)*pft->fpc;
        break;
      case GRASS:
        grass=pft->data;
        if(grass->ind.root.carbon>param.min_c_bnf &&
          ((grass->ind.leaf.nitrogen+pft->bm_inc.nitrogen/pft->nind*grass->falloc.leaf)/(grass->ind.leaf.carbon+pft->bm_inc.carbon/pft->nind*grass->falloc.leaf)<(pft->par->ncleaf.high)))
          pft_bnf=max(0,(0.0234*stand->cell->climbuf.aetp_mean-0.172)/10/NDAYYEAR)*pft->fpc;
        break;
        // N fixation for crops now in n_uptake_crop.c
      /*case CROP:
        if(pft->par->id==PEAS || pft->par->id==SOYBEAN)
        {
          crop=pft->data;
          if(crop->ind.root.carbon>param.min_c_bnf)
            bnf+=max(0,(0.0234*stand->cell->climbuf.aetp_mean-0.172)/10/NDAYYEAR)*pft->fpc;
        }
        break;
        */
    } /* of switch */
    switch(pft->stand->type->landusetype)
    {
    case NATURAL: case SETASIDE_RF: case SETASIDE_IR:
      if(config->pft_output_scaled)
         getoutputindex(&pft->stand->cell->output,PFT_BNF,pft->par->id,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,pft->par->id,config)+=pft_bnf;
    break;
    case BIOMASS_TREE:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbtree(ncft)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbtree(ncft)+data->irrigation*nirrig,config)+=pft_bnf; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
    case AGRICULTURE_TREE:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=pft_bnf; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
    case WOODPLANTATION:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rwp(ncft)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rwp(ncft)+data->irrigation*nirrig,config)+=pft_bnf; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
    case BIOMASS_GRASS:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=pft_bnf; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
    break;
    case AGRICULTURE_GRASS:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=pft_bnf; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
    case GRASSLAND:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=pft_bnf;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
    break;
    case OTHERS:
      data=pft->stand->data;
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=pft_bnf*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=pft_bnf;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
    break;
      default:
        /* do nothing */
       break;
    } /* of switch() */
  
    bnf += pft_bnf;
  } /* of foreachpft */
  return bnf;
} /* of 'biologicalnfixation' */

/* based on Cleveland, 1999                                               */
/* allow only for natural vegetation and grassland and pulses and soybean */
/* allow only if minimum root biomass is available.                       */
/* first approximation to avoid BNF in areas with no vegetation           */
/* later extend to Fisher et al. 2010 approach                            */
