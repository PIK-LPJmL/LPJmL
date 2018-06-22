/**************************************************************************************/
/**                                                                                \n**/
/**          r  o  u  g  h  n  e  s  s  l  e  n  g  t  h  .  c                     \n**/
/**                                                                                \n**/
/**     Computation of the roughneslength (m) as needed by Climber-LPJ             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/*! TODO: lake fraction */

#define zb 100.0      /* height where the roughness is sensible (m) [in accordance with Max Planck GCM]. */
                      /* minimal boundary layer thickness in SPEEDY */

#define z0bare  0.005 /* smooth surface roughness length (m) */

/*******************************************************************************************************************************************/
/**	Daveport-Wieringa roughness length classification [table 4-1, Meterologie for scientists and engineers, p78] 	        */
/**	-------------------------------------------------								        */
/**	z0(m)  | Classification	Landscape										        */
/**	0.0002 | sea	      | sea, paved areas, snow-covered flat plain, tide flat, smooth desert			        */
/**	0.005  | smooth	      |	beaches, pack ice, morass, snow-covered fields						        */
/**	0.03   | open	      |	grass prairy or farm fields, tundra, airports, heather					        */
/**	0.1    | roughly open |	cultivated area with low crops & occasional obstacles (single bushes)			        */
/**	0.25   | rough	      |	high crops, crops of various heights, scattered obstacles such as trees or hedgerows, vineyards	*/
/**	0.5    | very rough   |	mixed farm fields and forest clumps, orchards, scattered buildings				*/
/**	1.0    | closed	      |	regular coverage with large size obstacles with open spaces roughly equal to obstacle heights, 	*/
/**	       |	      	  |	suburban houses, villages, mature forests					        */
/**	>2     | chaotic      |	centers of large towns and cities, irregular forests with scattered clearings			*/
/*******************************************************************************************************************************************/

/*! specific roughness length parameter for each PFT */
/*! TODO: fill in real values */
/* Even better might be to put the roughness parameters also into the official LPJ
 * data structures and config files.
 */
/* The PFT class type (TREE, GRASS, etc) is assigned in the input file par/pft.par .
 * Those definitions must match this table, especially in order!
 * Negative entries (-TREE) mean that the roughness parameter for that PFT must be obtained
 * somehow else (specifically: calculated from tree heights).
 * NOTE: most values in this table (and the code below) are taken from the LPJ version that
 * was coupled to the SPEEDY atmosphere model (see reference in comments for the code below).
 */
/* NOTE: ATTENTION: The current LPJ configuration has some PFT/CFT entries which the
 * LPJ_SPEEDY setup did not have.
 * For those entries, values were just guessed - with no real scientific reasoning. S.P. 23.4.12
 */

/* Roughness Length: must be calculated from plant height and plant density.
 * For tree-class PFTs / stands, a height is contained in the PFT-specific data,
 * and roughness length is calculated from that height.
 * For non-tree-PFTs a PFT-specific roughness length is taken from a table.
 * Here, the roughness length for the cell is computed as in Strengers et al., JoC 30(13) 2010, SI 1
 * (Interaction between SPEEDY and LPJmL), S1.7 .
 *$$z^{cell}_{0m} =
 *  \frac{z_b}
 *       {e^{\sqrt{\frac{\displaystyle{1}}
 *                      {\displaystyle{\sum^{nrPFT}_{i=1}\frac{PFT_i}
 *                                                            {\left[\ln\left(\frac{z_b}
 *                                                                                 {z^i_{0m}}
 *                                                                      \right)
 *                                                             \right]^2
 * }}}}}}$$

 * An alternate calculation of the whole-grid-cell roughness length was added for
 * the project LUCID (Land-Use and Climate, Identification of robust impacts).
 * http://www.ileaps.org/index.php?option=com_content&task=view&id=99
 * http://www.agu.org/journals/gl/gl0914/2009GL039076/
 * http://dx.doi.org/10.1175/JCLI-D-11-00338.1
 * That calculation is commented-out below.
 */

Real roughnesslength(const Standlist standlist /**< stand list */
                    )  /** \return roughness length (m) */
{
  int p,s;
  Real z0all = 0.0; 	/* roughness length of entire gridcell */
  Real z0i;		/* PFT specific roughness length [grass=0.03, tropical=2.0, other forests=1.0] */
  Real baregrid = 1.0;  /* fraction bare of the grid */
  const Pft *pft;
  const Stand *stand;
  Real test;
  /*Real sum=0;*/ /* for LUCID */

  foreachstand(stand,s,standlist)
  {
    foreachpft(pft,p,&stand->pftlist)
    {
      Real z0temp;
      z0i=pft->par->roughness;
      /* if (istree(pft)) {
        const Pfttree *tree = pft->data;
        const Real height = tree->height;
         z0i = TODO: function of height ?;
        
        z0i = pft->par->roughness; 
      } else {
        z0i = pft->par->roughness;
      } */
      z0temp=log(zb/z0i);
      z0all+=pft->fpc*stand->frac/(z0temp*z0temp);
      baregrid-=(pft->fpc*stand->frac);
      /* sum+=pft->fpc*stand->frac*log(10.0/z0i); */
    }
  }
  z0all += baregrid/((log(zb/z0bare)*log(zb/z0bare)));
  /* sum+=baregrid*log(10.0/z0bare); */
  /* *z0eff = 10.0/exp(sum); */
  test = zb/(exp(sqrt(1/z0all)));

#ifndef NDEBUG
  /*if (*z0eff > 2 || *z0eff < 0) {
   *  printf("WARNING z0eff=%g is out of bounds!\n",*z0eff);
   *  printf("exp(sum)=%g\n",exp(sum));
   *  sum=0.0;
   *  foreachstand(stand,s,standlist) {
   *   foreachpft(pft,p,&stand->pftlist) {
   *     z0i=pft->par->rl;
   *     sum+=pft->fpc*log(10.0/z0i);
   *     printf("sum=%g fpc=%g standfrac=%g (%g) z0i=%g \n",sum,pft->fpc,stand->frac,pft->fpc*stand->frac,z0i);
   *   }
   *  }
   * }
   */
  if (test>2 || test<0)
  {
    printf("WARNING roughnesslength out of bounds %g\n",test);
    foreachstand(stand,s,standlist)
    {
      foreachpft(pft,p,&stand->pftlist)
      {
        printf("stand %d LUT %s frac %g pft %s rl %g z0temp %g z0all %g fpc %g\n",
               s,stand->type->name,stand->frac,pft->par->name,
               /*pft->par->rl*/ pft->par->roughness,
               /*log(zb/pft->par->rl)*/ log(zb/pft->par->roughness),
               z0all,pft->fpc);
      }
    }
  }
#endif /* NDEBUG */

  return test;
} /* of roughnesslength() */
