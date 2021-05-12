/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  f  t  p  a  r  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads parameter for all defined PFTs from                        \n**/
/**     configuration file                                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define UNDEF (-1)

#define fscanpftreal(verb,file,var,pft,name) \
  if(fscanreal(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read real '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }
#define fscanpftreal01(verb,file,var,pft,name) \
  if(fscanreal01(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read real '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }
#define fscanpftrealarray(verb,file,var,size,pft,name) \
  if(fscanrealarray(file,var,size,name,verb))\
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read array '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }

#define fscanpftint(verb,file,var,pft,name) \
  if(fscanint(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read int '%s' for PFT '%s').\n",name,pft); \
    return NULL; \
  }
#define fscanpftbool(verb,file,var,pft,name) \
  if(fscanbool(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read boolean '%s' for PFT '%s').\n",name,pft); \
    return NULL; \
  }
#define fscanpftlimit(verb,file,var,pft,name) \
  if(fscanlimit(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read limit '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }
#define fscanpftcnratio(verb,file,var,pft,name) \
  if(fscancnratio(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read C:N ratio '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }
#define fscanpftphenpar(verb,file,var,pft,name) \
  if(fscanphenparam(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read phenology param '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }
#define fscanpftemissionfactor(verb,file,var,pft,name) \
  if(fscanemissionfactor(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR128: Cannot read emission factor '%s' for PFT '%s'.\n",name,pft); \
    return NULL; \
  }

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return 0;}

const char *phenology[]={"evergreen","raingreen","summergreen","any","cropgreen"};
const char *cultivation_type[]={"none","biomass","annual crop","annual tree","wp"};
const char *path[]={"no pathway","C3","C4"};


int *fscanpftpar(LPJfile *file,       /**< pointer to LPJ file */
                 Pftpar **pftpar,     /**< Pointer to PFT parameter array */
                 const Fscanpftparfcn scanfcn[], /**< array of PFT-specific scan
                                                    functions */
                 int ntypes,          /**< number of PFT classes */
                 const Config *config       /**< LPJ configuration */
                )                     /** \return array of size ntypes or NULL */
{
  int *npft,n,l,count;
  LPJfile arr,item,subitem;
  String s;
  Pftpar *pft;
  Real totalroots;
  Cnratio cnratio;
  Bool isbiomass,isagtree,iscrop,iswp;
  Verbosity verb;
  verb=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if (verb>=VERB) puts("// PFT parameters");
  /* Read total number of defined PFTs */
  if(fscanarray(file,&arr,&count,TRUE,"pftpar",verb))
    return NULL;
  if(count<0 || count>UCHAR_MAX)
  {
    if(verb)
      fprintf(stderr,"ERROR171: Invalid number of PFTs=%d.\n",count);
    return NULL;
  }
  npft=newvec(int,ntypes);
  checkptr(npft);
  for(n=0;n<ntypes;n++)
    npft[n]=0;
  *pftpar=newvec(Pftpar,count);
  checkptr(*pftpar);
  for(n=0;n<count;n++)
    (*pftpar)[n].id=UNDEF;
  isbiomass=isagtree=iscrop=iswp=FALSE;
  for(n=0;n<count;n++)
  {
    fscanarrayindex(&arr,&item,n,verb);
    pft=(*pftpar)+n;
    pft->id=n;

    /* Read pft->name */
    if(fscanstring(&item,s,"name",FALSE,verb))
    {
      if(verb)
        readstringerr("name");
      return NULL;
    }
    pft->name=strdup(s); /* store PFT name */
    checkptr(pft->name);

    /* Read pft->type, defined in pftpar.h */
    fscanpftint(verb,&item,&pft->type,pft->name,"type");
    if(pft->type<0 || pft->type>=ntypes)
    {
      if(verb)
        fprintf(stderr,"ERROR116: Invalid PFT class=%d of PFT '%s'.\n",
                pft->type,pft->name);
      return NULL;
    }
    if(fscankeywords(&item,&pft->cultivation_type,"cultivation_type",cultivation_type,5,FALSE,verb))
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value for cultivation type of PFT '%s'.\n",pft->name);
      return NULL;
    }
    if(isbiomass && pft->cultivation_type==NONE)
    {
      if(verb)
        fprintf(stderr,"ERROR210: Natural PFT '%s' must be put before biomass plantation PFT.\n",pft->name);
      return NULL;
    }
    if(isagtree)
    {
      if(pft->cultivation_type==NONE)
      {
        if(verb)
          fprintf(stderr,"ERROR210: Natural PFT '%s' must be put before agriculture tree plantation PFT.\n",pft->name);
        return NULL;
      }
      else if(pft->cultivation_type==BIOMASS)
      {
        if(verb)
          fprintf(stderr,"ERROR210: Biomass plantation PFT '%s' must be put before agriculture tree plantation PFT.\n",pft->name);
        return NULL;
      }
      else if(pft->cultivation_type==WP)
      {
        if(verb)
          fprintf(stderr,"ERROR210: wood plantation PFT '%s' must be put before agriculture tree plantation PFT.\n",pft->name);
        return NULL;
      }
    }
    if(iscrop)
    {
      if(pft->cultivation_type==NONE)
      {
        if(verb)
          fprintf(stderr,"ERROR210: Natural PFT '%s' must be put before crop PFT.\n",pft->name);
        return NULL;
      }
      else if(pft->cultivation_type==BIOMASS)
      {
        if(verb)
          fprintf(stderr,"ERROR210: Biomass plantation PFT '%s' must be put before crop PFT.\n",pft->name);
        return NULL;
      }
      else if(pft->cultivation_type==WP)
      {
        if(verb)
          fprintf(stderr,"ERROR210: wood plantation PFT '%s' must be put before crop PFT.\n",pft->name);
        return NULL;
      }
    }
    if(iswp)
    {
      if(pft->cultivation_type==NONE)
      {
        if(verb)
          fprintf(stderr,"ERROR210: Natural PFT '%s' must be put before wood plantation PFT.\n",pft->name);
        return NULL;
      }
      else if(pft->cultivation_type==BIOMASS)
      {
        if(verb)
          fprintf(stderr,"ERROR210: Biomass plantation PFT '%s' must be put before wood planatation PFT.\n",pft->name);
        return NULL;
      }
    }
    switch(pft->cultivation_type)
    {
      case BIOMASS:
        isbiomass=TRUE;
        break;
      case ANNUAL_TREE:
        isagtree=TRUE;
        break;
      case ANNUAL_CROP:
        iscrop=TRUE;
        break;
      case WP:
        iswp=TRUE;
        break;
    }
    fscanpftrealarray(verb,&item,pft->cn,NHSG,pft->name,"cn");
    fscanpftreal(verb,&item,&pft->beta_root,pft->name,"beta_root");
    totalroots=1 - pow(pft->beta_root,layerbound[BOTTOMLAYER-1]/10);
    pft->rootdist[0]=(1 - pow(pft->beta_root,layerbound[0]/10))/totalroots;
    for(l=1;l<BOTTOMLAYER;l++)
      pft->rootdist[l]=(pow(pft->beta_root,layerbound[l-1]/10) - pow(pft->beta_root,layerbound[l]/10))/totalroots;
    fscanpftreal01(verb,&item,&pft->minwscal,pft->name,"minwscal");
    fscanpftreal(verb,&item,&pft->gmin,pft->name,"gmin");
    fscanpftreal(verb,&item,&pft->respcoeff,pft->name,"respcoeff");
    fscanpftreal(verb,&item,&pft->nmax,pft->name,"nmax");
    fscanpftreal01(verb,&item,&pft->resist,pft->name,"resist");
    fscanpftreal(verb,&item,&pft->longevity,pft->name,"longevity");
    if(pft->longevity<=0)
    {
      if(verb)
        fprintf(stderr,"ERROR234: Parameter 'longevity'=%g must be greater than zero for PFT '%s'.\n",
                pft->longevity,pft->name);
      return NULL;
    }
    fscanpftreal(verb,&item,&pft->lmro_ratio,pft->name,"lmro_ratio");
    if(pft->lmro_ratio<=0)
    {
      if(verb)
        fprintf(stderr,"ERROR234: Parameter 'lmro_ratio'=%g must be greater than zero for PFT '%s'.\n",
                pft->lmro_ratio,pft->name);
      return NULL;
    }
    fscanpftreal(verb,&item,&pft->ramp,pft->name,"ramp");
    if(pft->ramp<=0)
    {
      if(verb)
        fprintf(stderr,"ERROR234: Parameter 'ramp'=%g must be greater than zero for PFT '%s'.\n",
                pft->ramp,pft->name);
      return NULL;
    }
    pft->ramp=1/pft->ramp; /* store reciprocal to speed up calculations */
    fscanpftreal(verb,&item,&pft->lai_sapl,pft->name,"lai_sapl");
    fscanpftreal(verb,&item,&pft->gdd5min,pft->name,"gdd5min");
    fscanpftreal(verb,&item,&pft->twmax,pft->name,"twmax");
    fscanpftreal(verb,&item,&pft->twmax_daily,pft->name,"twmax_daily");
    fscanpftreal(verb,&item,&pft->gddbase,pft->name,"gddbase");
    fscanpftreal(verb,&item,&pft->min_temprange,pft->name,
                 "min_temprange");
    fscanpftreal(verb,&item,&pft->emax,pft->name,"emax");
    fscanpftreal(verb,&item,&pft->intc,pft->name,"intc");
    fscanpftreal01(verb,&item,&pft->alphaa,pft->name,"alphaa");
    fscanpftreal01(verb,&item,&pft->albedo_leaf,pft->name,"albedo_leaf");
    fscanpftreal01(verb,&item,&pft->albedo_stem,pft->name,"albedo_stem");
    fscanpftreal01(verb,&item,&pft->albedo_litter,pft->name,"albedo_litter");
    fscanpftreal01(verb,&item,&pft->snowcanopyfrac,pft->name,"snowcanopyfrac");
    fscanpftreal(verb,&item,&pft->lightextcoeff,pft->name,"lightextcoeff");
    if(config->new_phenology)
    {
      /* read new phenology parameters */
      fscanpftphenpar(verb,&item,&pft->tmin,pft->name,"tmin");
      fscanpftphenpar(verb,&item,&pft->tmax,pft->name,"tmax");
      fscanpftphenpar(verb,&item,&pft->light,pft->name,"light");
      fscanpftphenpar(verb,&item,&pft->wscal,pft->name,"wscal");
    }
    fscanpftreal01(verb,&item,&pft->mort_max,pft->name,"mort_max");

    if(fscankeywords(&item,&pft->phenology,"phenology",phenology,5,FALSE,verb))
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value for phenology of PFT '%s'.\n",pft->name);
      return NULL;
    }
    if(fscankeywords(&item,&pft->path,"path",path,3,FALSE,verb))
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value for path of PFT '%s'.\n",pft->name);
      return NULL;
    }
    fscanpftlimit(verb,&item,&pft->temp_co2,pft->name,"temp_co2");
    fscanpftlimit(verb,&item,&pft->temp_photos,pft->name,"temp_photos");
    fscanpftreal(verb,&item,&pft->b,pft->name,"b");
    /* store precalculated k1, k2, k3 values */
    pft->k1=2*log(1/0.99-1)/(pft->temp_co2.low-pft->temp_photos.low);
    pft->k2=(pft->temp_co2.low+pft->temp_photos.low)*0.5;
    pft->k3=log(0.99/0.01)/(pft->temp_co2.high-pft->temp_photos.high);

    fscanpftlimit(verb,&item,&pft->temp,pft->name,"temp");
    fscanpftreal(verb,&item,&pft->soc_k,pft->name,"soc_k");
    if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    {
      fscanpftreal(verb,&item,&pft->alpha_fuelp,pft->name,"alpha_fuelp");
      if(config->fdi==WVPD_INDEX)
        fscanpftreal(verb,&item,&pft->vpd_par,pft->name,"vpd_par");
      fscanpftemissionfactor(verb,&item,&pft->emissionfactor,
                             pft->name,"emission_factor");
    }
    else
      pft->fuelbulkdensity=0;
    fscanpftreal(verb,&item,&pft->fuelbulkdensity,pft->name,"fuelbulkdensity");
    fscanpftreal(verb,&item,&pft->aprec_min,pft->name,"aprec_min");
    if(config->fire!=NO_FIRE)
    {
      fscanpftreal(verb,&item,&pft->flam,pft->name,"flam");
    }
    if(fscanstruct(&item,&subitem,"k_litter10",verb))
    {
      if(verb)
        fprintf(stderr,"ERROR112: Cannot find k_litter10 in PFT '%s'.\n",pft->name);
      return NULL;
    }
    fscanpftreal(verb,&subitem,&pft->k_litter10.leaf,pft->name,"leaf");
    fscanpftreal(verb,&subitem,&pft->k_litter10.wood,pft->name,"wood");
    fscanpftreal(verb,&item,&pft->k_litter10.q10_wood,pft->name,
                 "k_litter10_q10_wood");
    if(config->with_nitrogen)
    {
      fscanpftbool(verb,&item,&pft->nfixing,pft->name,"nfixing");
      fscanpftreal(verb,&item,&pft->vmax_up,pft->name,"vmax_up");
      fscanpftreal(verb,&item,&pft->kNmin,pft->name,"kNmin");
      fscanpftreal(verb,&item,&pft->KNmin,pft->name,"KNmin");
      fscanpftreal(verb,&item,&pft->knstore,pft->name,"knstore");
      fscanpftreal01(verb,&item,&pft->fn_turnover,pft->name,"fn_turnover");
      fscanpftcnratio(verb,&item,&cnratio,pft->name,"cnratio_leaf");
      if(cnratio.low<=0 || cnratio.high<=0 || cnratio.median<=0)
      {
        if(verb)
          fprintf(stderr,"ERROR235: CN ratio limits=(%g,%g,%g) must be greater than zero for PFT '%s'.\n",
                  cnratio.low,cnratio.median,cnratio.high,pft->name);
        return NULL;
      }
      pft->ncleaf.median=1/cnratio.median;
      pft->ncleaf.low=1/cnratio.high;
      pft->ncleaf.high=1/cnratio.low;
    }
    else
      pft->fn_turnover=0;
    fscanpftreal(verb,&item,&pft->windspeed,pft->name,"windspeed_dampening");
    fscanpftreal(verb,&item,&pft->roughness,pft->name,
                 "roughness_length");
    pft->k_litter10.leaf/=NDAYYEAR;
    pft->k_litter10.wood/=NDAYYEAR;
    npft[pft->type]++;
    /* set default PFT-specific functions */
    pft->init=noinit;
    pft->fire=nofire;
    pft->mix_veg=nomix_veg;
    pft->adjust=noadjust;
    pft->establishment=noestablishment;
    pft->wdf=nowdf;
    pft->turnover_monthly=noturnover_monthly;
    /* Now scan PFT-specific parameters and set specific functions */
    if(scanfcn[pft->type](&item,pft,config))
      return NULL;
  }
  return npft;
} /* of 'fscanpftpar' */
