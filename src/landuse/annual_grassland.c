/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  g  r  a  s  s  l  a  n  d  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for grassland stand                                                   \n**/
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
#include "agriculture.h"
#include "grassland.h"

Bool annual_grassland(Stand *stand,         /**< Pointer to stand */
                      int npft,             /**< number of natural pfts */
                      int UNUSED(ncft),     /**< number of crop PFTs */
                      Real UNUSED(popdens), /**< population density (capita/km2) */
                      int year,             /**< simulation year (AD) */
                      Bool isdaily,         /**< daily temperature data? */
                      Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                      const Config *config  /**< LPJ configuration */
                     )                      /** \return stand has to be killed */
{
  int p;
  Bool *present;
  Pft *pft;
  Real fpc_inc;
  Real acflux_estab;
  int n_est=0;
  Real fpc_total,*fpc_type;
  Irrigation *irrigation;

  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);
  present=newvec(Bool,npft);
  check(present);
  acflux_estab=0;
  for(p=0;p<npft;p++)
    present[p]=FALSE;

  irrigation=stand->data;

  foreachpft(pft,p,&stand->pftlist)
  {
#ifdef DEBUG2
    printf("PFT:%s fpc=%g\n",pft->par->name,pft->fpc);
    printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
           pft->bm_inc,vegc_sum(pft),soilcarbon(&stand->soil));
#endif

    present[pft->par->id]=TRUE;
    if(annual_grass(stand,pft,&fpc_inc,isdaily))
    {
      /* PFT killed, delete from list of established PFTs */
      litter_update_grass(&stand->soil.litter,pft,pft->nind);
      delpft(&stand->pftlist,p);
      p--; /* adjust loop variable */
    }

  } /* of foreachpft */

  /* separate calculation of grass FPC after all grass PFTs have been updated */
  foreachpft(pft,p,&stand->pftlist)
    fpc_grass(pft);

#ifdef DEBUG2
  printf("Number of updated pft: %d\n",stand->pftlist.n);
#endif

  for(p=0;p<npft;p++)
  {
    if(config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE &&
       establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf))
    {
      if(!present[p])
       addpft(stand,config->pftpar+p,year,0);
      n_est++;
    }
  }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  foreachpft(pft,p,&stand->pftlist)
   if(establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf))
    acflux_estab+=establishment_grass(pft,fpc_total,fpc_type[pft->par->type],n_est);

  stand->cell->output.flux_estab+=acflux_estab*stand->frac;
  stand->cell->output.dcflux-=acflux_estab*stand->frac;

  stand->cell->output.soil_storage+=(irrigation->irrig_stor+irrigation->irrig_amount)*stand->frac*stand->cell->coord.area;

  free(present);
  free(fpc_type);
  return FALSE;
} /* of 'annual_grassland' */
