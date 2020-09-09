/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  f  t  _  t  r  e  e  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads tree-specific parameter from text file                      \n**/
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

#define fscanreal2(verb,file,var,pft,name) \
  if(fscanreal(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read float '%s' for PFT '%s'.\n",name,pft); \
    return TRUE; \
  }
#define fscanint2(verb,file,var,pft,name) \
  if(fscanint(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read int '%s' for PFT '%s'.\n",name,pft); \
    return TRUE; \
  }
#define fscantreephys2(verb,file,var,pft,name)\
  if(fscantreephyspar(file,var,name,verb))\
  {\
    if(verb)\
    fprintf(stderr,"ERROR111: Cannot read '%s' for PFT '%s'.\n",name,pft); \
    return TRUE; \
  }

#define fscanratio2(verb,file,var,pft,name)\
  if(fscanratio(file,var,name,verb))\
  {\
    if(verb)\
    fprintf(stderr,"ERROR111: Cannot read '%s' for PFT '%s'.\n",name,pft); \
    return TRUE; \
  }


static Bool fscantreephyspar(LPJfile *file,Treephyspar *phys,const char *name,
                             Verbosity verb)
{
  LPJfile s;
  if(fscanstruct(file,&s,name,verb))
    return TRUE;
  if(fscanreal(&s,&phys->leaf,"leaf",FALSE,verb))
    return TRUE;
  if(fscanreal(&s,&phys->sapwood,"sapwood",FALSE,verb))
    return TRUE;
  if(fscanreal(&s,&phys->root,"root",FALSE,verb))
    return TRUE;
  if(phys->leaf<=0 || phys->sapwood<=0 || phys->root<=0)
  {
    if(verb)
      fprintf(stderr,"ERROR235: Tree parameter '%s'=(%g,%g,%g) must be greater than zero.\n",name,phys->leaf,phys->sapwood,phys->root);
    return TRUE;
  }
  return FALSE;
} /* of 'fscantreephyspar' */

static Bool fscanratio(LPJfile *file,Treeratio *ratio,const char *name,
                       Verbosity verb)
{
  LPJfile s;
  if(fscanstruct(file,&s,name,verb))
    return TRUE;
  if(fscanreal(&s,&ratio->sapwood,"sapwood",FALSE,verb))
    return TRUE;
  if(fscanreal(&s,&ratio->root,"root",FALSE,verb))
    return TRUE;
  if(ratio->sapwood<=0 || ratio->root<=0)
  {
    if(verb)
      fprintf(stderr,"ERROR235: Tree ratios=(%g,%g) must be greater than zero.\n",ratio->sapwood,ratio->root);
    return TRUE;
  }
  return FALSE;
} /* of 'fscanratio' */

char *leaftype[]={"broadleaved","needleleaved","any leaved"};

Bool fscanpft_tree(LPJfile *file, /**< pointer to LPJ file */
                   Pftpar *pft,   /**< Pointer to Pftpar array */
                   Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
                  )               /** \return TRUE on error */
{
  Real stemdiam,height_sapl,wood_sapl;
  Pfttreepar *tree;
  pft->newpft=new_tree;
  pft->npp=npp_tree;
  /*pft->fpc=fpc_tree; */
  pft->fpar=fpar_tree;
  pft->alphaa_manage=alphaa_tree;
  pft->leaf_phenology=phenology_tree;
  pft->fwrite=fwrite_tree;
  pft->mix_veg=mix_veg_tree;
  pft->fprint=fprint_tree;
  pft->fread=fread_tree;
  pft->litter_update=litter_update_tree;
  pft->annual=annual_tree;
  pft->establishment=establishment_tree;
  pft->init=init_tree;
  pft->fire=fire_tree;
  pft->lai=lai_tree;
  pft->actual_lai=actual_lai_tree;
  pft->free=free_tree;
  pft->vegc_sum=vegc_sum_tree;
  pft->vegn_sum=vegn_sum_tree;
  pft->adjust=adjust_tree;
  pft->reduce=reduce_tree;
  pft->fprintpar=fprintpar_tree;
  pft->livefuel_consumption=livefuel_consum_tree;
  pft->turnover_monthly=turnover_monthly_tree;
  pft->turnover_daily=turnover_daily_tree;
  pft->albedo_pft=albedo_tree;
  pft->nuptake=nuptake_tree;
  pft->ndemand=ndemand_tree;
  pft->vmaxlimit=vmaxlimit_tree;
  pft->agb=agb_tree;
  tree=new(Pfttreepar);
  if(tree==NULL)
  {
    printallocerr("tree");
    return TRUE;
  }
  pft->data=tree;
  if(fscankeywords(file,&tree->leaftype,"leaftype",leaftype,3,FALSE,verb))
  {
    if(verb)
      fprintf(stderr,"ERROR201: Invalid value for leaf type of PFT '%s'.\n",
              pft->name);
    return TRUE;
  }
  fscantreephys2(verb,file,&tree->turnover,pft->name,"turnover");
  if(iskeydefined(file,"sla"))
  {
    fscanreal2(verb,file,&pft->sla,pft->name,"sla");
  }
  else
  {
    if(tree->leaftype==BROADLEAVED)
      pft->sla=2e-4*pow(10,2.20-0.4*log10(pft->longevity*12))/CCpDM;
    else
      pft->sla=2e-4*pow(10,2.08-0.4*log10(pft->longevity*12))/CCpDM;
  }
  tree->turnover.root=1.0/tree->turnover.root;
  tree->turnover.sapwood=1.0/tree->turnover.sapwood;
  tree->turnover.leaf=1.0/tree->turnover.leaf;
  fscantreephys2(verb,file,&tree->nc_ratio,pft->name,"cn_ratio");
  tree->nc_ratio.leaf=1/tree->nc_ratio.leaf;
  tree->nc_ratio.sapwood=1/tree->nc_ratio.sapwood;
  tree->nc_ratio.root=1/tree->nc_ratio.root;
  fscanratio2(verb,file,&tree->ratio,pft->name,"ratio");
  fscanreal2(verb,file,&tree->crownarea_max,pft->name,"crownarea_max");
  fscanreal2(verb,file,&wood_sapl,pft->name,"wood_sapl");
  if(pft->phenology==SUMMERGREEN)
  {
    fscanreal2(verb,file,&tree->aphen_min,pft->name,"aphen_min");
    fscanreal2(verb,file,&tree->aphen_max,pft->name,"aphen_max");
  }
  fscanreal2(verb,file,&tree->reprod_cost,pft->name,"reprod_cost");
  fscanreal2(verb,file,&tree->allom1,pft->name,"allom1");
  fscanreal2(verb,file,&tree->allom2,pft->name,"allom2");
  fscanreal2(verb,file,&tree->allom3,pft->name,"allom3");
  fscanreal2(verb,file,&tree->allom4,pft->name,"allom4");
  fscanreal2(verb,file,&tree->height_max,pft->name,"height_max");
  fscanreal2(verb,file,&tree->scorchheight_f_param,pft->name,"scorchheight_f_param");
  fscanreal2(verb,file,&tree->crownlength,pft->name,"crownlength");
  fscanreal2(verb,file,&tree->barkthick_par1,pft->name,"barkthick_par1");
  fscanreal2(verb,file,&tree->barkthick_par2,pft->name,"barkthick_par2");
  fscanreal2(verb,file,&tree->crown_mort_rck,pft->name,"crown_mort_rck");
  fscanreal2(verb,file,&tree->crown_mort_p,pft->name,"crown_mort_p");
  fscanreal2(verb,file,&tree->k_latosa,pft->name,"k_latosa");
  if(fscanrealarray(file,tree->fuelfrac,NFUELCLASS,"fuelfraction",verb))
  {
    if(verb)
      fprintf(stderr,"ERROR112: Cannot read 'fuelfraction' of PFT '%s'.\n",pft->name);
    return TRUE;
  }
  fscanreal2(verb,file,&tree->k_est,pft->name,"k_est");
  if(pft->cultivation_type!=NONE)
  {
    fscanint2(verb,file,&tree->rotation,pft->name,"rotation");
    fscanint2(verb,file,&tree->max_rotation_length,pft->name,"max_rotation_length");
  }
#if defined IMAGE || defined INCLUDEWP
  if(pft->cultivation_type==WP)
  {
    fscanreal2(verb,file,&tree->P_init,pft->name,"P_init");
  }
#endif
   tree->sapl.leaf.carbon=pow(pft->lai_sapl*tree->allom1*pow(wood_sapl,reinickerp)*
                  pow(4*pft->sla/M_PI/tree->k_latosa,reinickerp*0.5)/pft->sla,
                  2/(2-reinickerp));
  stemdiam=wood_sapl*sqrt(4*tree->sapl.leaf.carbon*pft->sla/M_PI/tree->k_latosa);
  height_sapl=tree->allom2*pow(stemdiam,tree->allom3);
  tree->sapl.sapwood.carbon=wooddens*height_sapl*tree->sapl.leaf.carbon*pft->sla/tree->k_latosa;
  tree->sapl.heartwood.carbon=(wood_sapl-1)*tree->sapl.sapwood.carbon;
  tree->sapl.root.carbon=(1.0/pft->lmro_ratio)*tree->sapl.leaf.carbon;
  tree->sapling_C=phys_sum_tree(tree->sapl)*tree->k_est;
  tree->sapl.leaf.nitrogen=tree->sapl.leaf.carbon*tree->nc_ratio.leaf;
  tree->sapl.heartwood.nitrogen=tree->sapl.heartwood.carbon*tree->nc_ratio.sapwood;
  tree->sapl.sapwood.nitrogen=tree->sapl.sapwood.carbon*tree->nc_ratio.sapwood;
  tree->sapl.root.nitrogen=tree->sapl.root.carbon*tree->nc_ratio.root;
  return FALSE;
} /* of 'fscanpft_tree' */
