/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  f  t  _  g  r  a  s  s  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads grass-specific parameter from text file                     \n**/
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

#define fscanreal2(verb,file,var,pft,name) \
  if(fscanreal(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read float '%s' for PFT '%s'.\n",name,pft); \
    return TRUE; \
  }

#define fscangrassphys2(verb,file,var,pft,name) \
  if(fscangrassphys(file,var,name,verb))\
  { \
    if(verb)\
    fprintf(stderr,"ERROR111: Cannot read '%s' for PFT '%s'.\n",name,pft); \
    return TRUE; \
  }


static Bool fscangrassphys(LPJfile *file,Grassphys *phys,const char *name,Verbosity verb)
{
  LPJfile item;
  if(fscanstruct(file,&item,name,verb))
    return TRUE;
  if(fscanreal(&item,&phys->leaf,"leaf",FALSE,verb))
    return TRUE;
  if(fscanreal(&item,&phys->root,"root",FALSE,verb))
    return TRUE;
  if(phys->leaf<=0 ||  phys->root<=0)
    return TRUE;
  return FALSE;
} /* of 'fscangrassphys' */

Bool fscanpft_grass(LPJfile *file, /**< pointer to LPJ file */
                    Pftpar *pft,   /**< Pointer to Pftpar array */
                    Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
                   )               /** \return TRUE on error  */
{
  Pftgrasspar *grass;
  pft->newpft=new_grass;
  pft->npp=npp_grass;
  /*pft->fpc=fpc_grass; */
  pft->fpar=fpar_grass;
  pft->alphaa_manage=alphaa_grass;
  pft->leaf_phenology=phenology_grass;
  pft->fwrite=fwrite_grass;
  pft->mix_veg=mix_veg_grass;
  pft->fire=fire_grass;
  pft->fprint=fprint_grass;
  pft->fread=fread_grass;
  pft->litter_update=litter_update_grass;
  pft->annual=annual_grass;
  pft->establishment=establishment_grass;
  pft->reduce=reduce_grass;
  pft->actual_lai=actual_lai_grass;
  pft->init=init_grass;
  pft->free=free_grass;
  pft->vegc_sum=vegc_sum_grass;
  pft->fprintpar=fprintpar_grass;
  pft->livefuel_consumption=livefuel_consum_grass;
  pft->turnover_monthly=turnover_monthly_grass;
  pft->turnover_daily=turnover_daily_grass;
  pft->albedo_pft=albedo_grass;
  pft->agb=agb_grass;
  grass=new(Pftgrasspar);
  check(grass);
  pft->data=grass;
  pft->data=grass;
  if(iskeydefined(file,"sla"))
  {
    fscanreal2(verb,file,&pft->sla,pft->name,"sla");
  }
  else
    pft->sla=2e-4*pow(10,2.25-0.4*log(pft->longevity*12)/log(10))/CCpDM;
  fscangrassphys2(verb,file,&grass->turnover,pft->name,"turnover");
  grass->turnover.leaf=1.0/grass->turnover.leaf;
  grass->turnover.root=1.0/grass->turnover.root;
  fscangrassphys2(verb,file,&grass->cn_ratio,pft->name,"cn_ratio");
  fscanreal2(verb,file,&grass->reprod_cost,pft->name,"reprod_cost");
  grass->cn_ratio.leaf=pft->respcoeff*param.k/grass->cn_ratio.leaf;
  grass->cn_ratio.root=pft->respcoeff*param.k/grass->cn_ratio.root;
  grass->sapl.leaf=pft->lai_sapl/pft->sla;
  grass->sapl.root=(1.0/pft->lmro_ratio)*grass->sapl.leaf;
  grass->sapling_C=phys_sum_grass(grass->sapl);

  return FALSE;
} /* of 'fscanpft_grass' */
