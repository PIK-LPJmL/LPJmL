/**************************************************************************************/
/**                                                                                \n**/
/**                  f  e  r  t  i  l  i  z  e  _  t  r  e  e  .  c                \n**/
/**                                                                                \n**/
/**     Function applies fertilizer and manure for trees depending on phenology    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fertilize_tree(Stand *stand,        /**< pointer to stand */
                    Real fertil,         /**< fertilizer (gN/m2) */
                    Real manure,         /**< manure (gN/m2) */
                    const Config *config /**< LPJmL configuration */
                   )
{
  Pft *pft;
  Pfttree *tree;
  Output *output;
  int p;
  output=&stand->cell->output;
  /* Loop over PFTs for applying fertilizer */
  foreachpft(pft,p,&stand->pftlist)
  {
    if(istree(pft))
    {
      tree = pft->data;
      if(fertil>0)
      {
        /* Apply fertilizer depending on how much there is (currently always) and split parameters */
        /* First fertilizer application,
           Assuming 10 on-leaves days, as early-season proxy */
        if (pft->aphen > 10 && tree->nfertilizer < epsilon)
        {
          /* The index for ag_tree in ml.fertilizer_nr.ag_tree structure (fertil)
             takes PFT id (according to pft.js or similar),
             subtracts the number of all non-crop PFTs (npft),
             adds the number of ag_trees (config->nagtree),
             so that the first element in ag_tree fertilizer vector (indexed as 0)
             is read for the first ag_tree in pftpar list */
          stand->soil.NO3[0] += fertil * param.nfert_no3_frac * param.nfert_split_frac;
          stand->soil.NH4[0] += fertil * (1 - param.nfert_no3_frac) * param.nfert_split_frac;
          stand->cell->balance.n_influx += fertil * param.nfert_split_frac * stand->frac;
          getoutput(output, NFERT_AGR, config) += fertil * param.nfert_split_frac * pft->stand->frac;
          /* Store remainder of fertilizer for second application */
          tree->nfertilizer = fertil * (1 - param.nfert_split_frac);
        }
        /* Second fertilizer application */
        else if (pft->aphen > 30 && tree->nfertilizer > epsilon)
        {
          fertil = tree->nfertilizer;
          stand->soil.NO3[0] += fertil * param.nfert_no3_frac;
          stand->soil.NH4[0] += fertil * (1 - param.nfert_no3_frac);
          stand->cell->balance.n_influx += fertil * stand->frac;
          getoutput(output, NFERT_AGR, config) += fertil * pft->stand->frac;
          tree->nfertilizer = 0;
        }
      }

      if(manure>0)
      {
        /* Apply manure depending on how much there is (currently always) and split parameters */

        /* First manure application,
           Assuming 10 on-leaves days, as early-season proxy */
        if (pft->aphen > 10 && tree->nmanure < epsilon)
        {
          stand->soil.NO3[0] += manure * param.nfert_no3_frac * param.nfert_split_frac;
          stand->soil.NH4[0] += manure * (1 - param.nfert_no3_frac) * param.nfert_split_frac;
          stand->cell->balance.n_influx += manure * param.nfert_split_frac * stand->frac;
          getoutput(output, NFERT_AGR, config) += manure * param.nfert_split_frac * pft->stand->frac;
          /* Store remainder of manure for second application */
          tree->nmanure = manure * (1 - param.nfert_split_frac);
        }
        /* Second manure application */
        else if (pft->aphen > 30 && tree->nmanure > epsilon)
        {
          manure = tree->nmanure;
          stand->soil.NO3[0] += manure * param.nfert_no3_frac;
          stand->soil.NH4[0] += manure * (1 - param.nfert_no3_frac);
          stand->cell->balance.n_influx += manure * stand->frac;
          getoutput(output, NFERT_AGR, config) += manure * pft->stand->frac;
          tree->nmanure = 0;
        }
      }
    }
  } /* of foreachpft() */
} /* of 'fertilize_tree' */
