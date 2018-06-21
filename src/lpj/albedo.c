/**************************************************************************************/
/**                                                                                \n**/
/**                    a  l  b  e  d  o  .  c                                      \n**/
/**                                                                                \n**/
/**     Computation of albedo as described by Bob Gallimore (in an e-mail)         \n**/
/**     and used by him in the coupling of FOAM to LPJ.                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define c_albAntarctica   0.7
#define c_roughness       0.06 /* Roughness height of vegetation below the canopy. Source: FOAM/LPJ */
#define c_albedo_lake     0.06 /* albedo of lakes, or water ,Taken from LAD*/
#define Tfrozen1          0.0  /* freezing temperatrue of lake water*/
#define Tfrozen2         -2.0  /* freezing temperature of sea water*/
#define drydayreduction    -0.011 /* reduction value of dry days albedo with cold water (Snow aging) */
#define meltdayreduction1  -0.013 /* reduction value of melting days albedo with cold water */
#define meltdayreduction2  -0.17  /* reduction value of melting days albedo 
                                     with warm water*/
#define c_albedo_lake_frozen_min  0.38 /* minimal albedo of a frozen lake */
#define c_albedo_lake_frozen_max  0.83 /* maximum albedo of a frozen lake */
#define c_albedo_bare_soil  0.31 /* albedo of the bare soil as used by Gascoin et al 2009 */
#define c_albedo_wet_soil  0.15  /* see above */
#define decay_alb_moist  12.7    /* Describes how fast the moisture dependence is */

static Real albedo_stand(const Stand *stand)
{
  int p;
  Real frsg;  /* Snow cover of the ground below the canopy (0-1) */
  Real HS;    /* Height of the Snow (m) */
  Real albstot;/* Sum of all albs over all pfts */
  Real fbare;  /* Fraction bare soil */
  const Pft *pft;
  Real test;
  Real moisture;
  Real VolWatercontent;

  if (stand->soil.par->type == ICE /* ICE */)
  {
    if (stand->cell->coord.lat<-60)
      return c_albAntarctica;
    return c_albsnow;
  }

  VolWatercontent = stand->soil.w[0]*stand->soil.par->whc[0];
  moisture = c_albedo_wet_soil + c_albedo_bare_soil*exp(-decay_alb_moist*VolWatercontent);/*gives the moisture dependence of the bare soil*/

  HS = c_watertosnow * (stand->soil.snowpack/1000.0); /* mm -> m */
  frsg = HS / (HS+0.5*c_roughness); /* If HS is large, frsg will be close to 1. For small values of HS, frsg will be less than 1, because then a significant amount of the snow is captured by the canopy */
  fbare = 0.0;
  albstot = 0.0;

  foreachpft(pft,p,&stand->pftlist)
  {
    albstot += pft->par->albedo(pft,HS,frsg); /* call PFT-specific albedo function */
    fbare += pft->fpc;
  } /* pft loop */
  fbare = max((1-fbare),0.0);

  test = albstot + fbare * (frsg * c_albsnow + (1-frsg) * (c_albsoil+moisture));

#ifndef NDEBUG
  if (test > 1 || test < 0 || isnan(test))
  {
    printf("WARNING albedo out of bounds: %g fbare=%g albstot=%g\n", test, fbare, albstot);
    foreachpft(pft,p,&stand->pftlist)
    {
      printf("pft %s fpc %g phen %g\n", pft->par->name, pft->fpc, pft->phen);
    }
  }
#endif

  return  test;

} /* end of albedo_stand() */

/** albedo of lakes is computed following
 * Albedo models for snow and ice on a freshwater lake
 * Heather E. Henneman, Heinz G. Stefan
 * St. Anthony Falls Laboratory, Department of Civil Engineering, University of Minnesota, Minneapolis, MN 55414, USA
 * Received 19 November 1997; accepted 15 January 1999
 */
Real albedo(Cell *cell, /**< Pointer to cell */
            Real temp,  /**< temperature (deg C) */
            Real prec   /**< precipitattion (mm) */
           )            /** \return albedo (0..1) */
{
  const Stand *stand;
  int s;
  Real a;


#ifdef COUPLING_WITH_FMS

  if (cell->discharge.next >= -1) { /* albedo for all land cells including the caspian sea cells . Land cells are always cells with a discharge information distinct of -9 */
    if (cell->laketemp <= Tfrozen1) { /* lake has to be frozen to accumulate snow */ 
      if (temp < Tfrozen1) {
        if (prec > 0) { /* snowy day with frozen lake */
          cell->albedo_lake=c_albedo_lake_frozen_max;
          cell->day_after_snowfall=0;
        } else { /* cold dry day with frozen lake */
          if (cell->albedo_lake>c_albedo_lake_frozen_min) {
            cell->day_after_snowfall+=1;
            cell->albedo_lake=c_albedo_lake_frozen_max+drydayreduction*cell->day_after_snowfall;
          } else {
            cell->albedo_lake=c_albedo_lake_frozen_min;
          }
        }
      } else { /* (temp > Tfrozen1) i.e. warm day with frozen lake */
        if (cell->albedo_lake > c_albedo_lake) { /*above lake albedo*/
          cell->albedo_lake+=meltdayreduction1;
        } else { /* smallest albedo possible for lakes */
          cell->albedo_lake=c_albedo_lake;
        }
      }
    } else {
      if (cell->albedo_lake>c_albedo_lake) { /*above lake albedo*/
        cell->albedo_lake+=meltdayreduction2;
      } else { /*smallest albedo possible for lakes*/
        cell->albedo_lake=c_albedo_lake;
      }
    }
      
    a = cell->lakefrac*cell->albedo_lake; /* computing the albedo of lakes */
      
    foreachstand(stand,s,cell->standlist) {
      a+=albedo_stand(stand)*stand->frac;
    }
  } else if (cell->coord.lat<-60) { /*compute the albedo of antarctica*/
    cell->albedo_lake=c_albAntarctica;
    a=cell->albedo_lake;
  } else { /*computing the albedo of real ocean cells that are on lpj grid in land lad coars resolution not resolved, DDM30 contains a discharge information of -9(only other information
            for discharge, and hence an indecator for ocean cells*/
    if (cell->laketemp <= Tfrozen2) { /*ocean has to be frozen, wich happens at -2degC in difference to fresh water*/
      if (temp < Tfrozen1) {
        if (prec > 0) { /*snowy day with frozen ocean*/
          cell->albedo_lake=c_albedo_lake_frozen_max;
          cell->day_after_snowfall=0;
        } else { /*cold dry day with frozen ocean*/
          if (cell->albedo_lake>c_albedo_lake_frozen_min) {
            cell->day_after_snowfall+=1;
            cell->albedo_lake=c_albedo_lake_frozen_max+drydayreduction*cell->day_after_snowfall;
          } else {
            cell->albedo_lake=c_albedo_lake_frozen_min;
          }
        }
      } else { /* (temp>=Tfrozen1) i.e. warm day with frozen ocean*/
        if (cell->albedo_lake>c_albedo_lake) { /*above lake albedo*/
          cell->albedo_lake += meltdayreduction1;
        } else { /*smallest albedo possible for lakes*/
          cell->albedo_lake = c_albedo_lake;
        }
      }
    } else {
      if (cell->albedo_lake>c_albedo_lake){ /*above lake albedo*/
        cell->albedo_lake+=meltdayreduction2;
      } else { /*smallest albedo possible for lakes*/
        cell->albedo_lake = c_albedo_lake;
      }
    }
    /*a=cell->lakefrac*cell->albedo_lake; computing the albedo of lakes here lakefrac of a cell is always one*/
    a = 1.0*cell->albedo_lake;
  }
#else
  a = cell->lakefrac*c_albedo_lake ;
  foreachstand(stand,s,cell->standlist) {
    a+=albedo_stand(stand)*stand->frac;
  }
#endif
  
  return a;
} /* of 'albedo' */
