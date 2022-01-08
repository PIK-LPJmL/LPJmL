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
                    int day,             /**< day (1..365) */
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

      /* Reset the count of fertilization event when a new phenology cycle starts
       * (see condition for aphen=0 in phenology_gsi()) */
      if ((pft->stand->cell->coord.lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) ||
          (pft->stand->cell->coord.lat<0.0 && day==COLDEST_DAY_SHEMISPHERE))
      {
        tree->nfert_event = 0;
      }

      /* Apply fertilizer depending on how much there is (currently always) and split parameters */

      /* Start fertilization only if a full phenology cycle begins, otherwise
       * in the first rotation year (esp. SH) is aphen=0 twice and trees will be
       * fertilized in the first half of the year (incomplete phenology cycle) */
      if(fertil>0 &&
         ((pft->stand->cell->coord.lat>=0.0 && day>=COLDEST_DAY_NHEMISPHERE) ||
          (pft->stand->cell->coord.lat<0.0 && day>=COLDEST_DAY_SHEMISPHERE)))
      {
        /* First fertilizer application,
           Assuming aphen > 10, as early-season proxy */
        if (pft->aphen > 10 && tree->nfert_event == 0)
        {
          stand->soil.NO3[0] += fertil * param.nfert_no3_frac * param.nfert_split_frac;
          stand->soil.NH4[0] += fertil * (1 - param.nfert_no3_frac) * param.nfert_split_frac;
          stand->cell->balance.n_influx += fertil * param.nfert_split_frac * stand->frac;
          getoutput(output, NFERT_AGR, config) += fertil * param.nfert_split_frac * pft->stand->frac;
          /* Store remainder of fertilizer for second application */
          tree->nfertilizer = fertil * (1 - param.nfert_split_frac);
          tree->nfert_event++;
        }
        /* Second fertilizer application */
        else if (pft->aphen > 30 && tree->nfert_event == 1)
        {
          fertil = tree->nfertilizer;
          stand->soil.NO3[0] += fertil * param.nfert_no3_frac;
          stand->soil.NH4[0] += fertil * (1 - param.nfert_no3_frac);
          stand->cell->balance.n_influx += fertil * stand->frac;
          getoutput(output, NFERT_AGR, config) += fertil * pft->stand->frac;
          tree->nfertilizer = 0;
          tree->nfert_event++;
        }
      }

      if(manure>0)
      {
        /* Apply manure depending on how much there is (currently always) and split parameters */

        /* First manure application,
           Assuming aphen > 10, as early-season proxy */
        if (pft->aphen > 10 && tree->nfert_event == 0)
        {
          stand->soil.NO3[0] += manure * param.nfert_no3_frac * param.nfert_split_frac;
          stand->soil.NH4[0] += manure * (1 - param.nfert_no3_frac) * param.nfert_split_frac;
          stand->cell->balance.n_influx += manure * param.nfert_split_frac * stand->frac;
          getoutput(output, NMANURE_AGR, config) += manure * param.nfert_split_frac * pft->stand->frac;
          /* Store remainder of manure for second application */
          tree->nmanure = manure * (1 - param.nfert_split_frac);
          tree->nfert_event++;
        }
        /* Second manure application */
        else if (pft->aphen > 30 && tree->nfert_event == 1)
        {
          manure = tree->nmanure;
          stand->soil.NO3[0] += manure * param.nfert_no3_frac;
          stand->soil.NH4[0] += manure * (1 - param.nfert_no3_frac);
          stand->cell->balance.n_influx += manure * stand->frac;
          getoutput(output, NMANURE_AGR, config) += manure * pft->stand->frac;
          tree->nmanure = 0;
          tree->nfert_event++;
        }
      }
    }
  } /* of foreachpft() */
} /* of 'fertilize_tree' */
