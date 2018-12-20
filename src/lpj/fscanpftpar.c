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
#define fscanpftlimit(verb,file,var,pft,name) \
  if(fscanlimit(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read limit '%s' for PFT '%s'.\n",name,pft); \
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

int *fscanpftpar(LPJfile *file,       /**< pointer to LPJ file */
                 Pftpar **pftpar,     /**< Pointer to PFT parameter array */
                 const Fscanpftparfcn scanfcn[], /**< array of PFT-specific scan
                                                    functions */
                 int ntypes,          /**< number of PFT classes */
                 Verbosity verb       /**< verbosity level (NO_ERR,ERR,VERB) */
                )                     /** \return array of size ntypes or NULL */
{
  int *npft,n,id,l,count;
  LPJfile arr,item,subitem;
  String s;
  Pftpar *pft;
  Real totalroots;
  Bool isbiomass;
  if (verb>=VERB) puts("// PFT parameters");
  /* Read total number of defined PFTs */
  if(fscanarray(file,&arr,&count,TRUE,"pftpar",verb))
    return NULL;
  if(count<0 || count>UCHAR_MAX)
  {
    if(verb)
      fprintf(stderr,"ERROR171: Invalid number of PFTs=%d in line %d of '%s'.\n",count,getlinecount(),getfilename());
    return NULL;
  }
  npft=newvec(int,ntypes);
  check(npft);
  for(n=0;n<ntypes;n++)
    npft[n]=0;
  *pftpar=newvec(Pftpar,count);
  check(*pftpar);
  for(n=0;n<count;n++)
    (*pftpar)[n].id=UNDEF;
  isbiomass=FALSE;
  for(n=0;n<count;n++)
  {
    fscanarrayindex(&arr,&item,n,verb);
    /* Read pft->id, defined in pftpar.h */
    if(fscanint(&item,&id,"id",FALSE,verb))
      return NULL;
    if(id<0 || id>=count)
    {
      if(verb)
        fprintf(stderr,"ERROR120: Invalid range of 'id'=%d in line %d of '%s'.\n",
                id,getlinecount(),getfilename());
      return NULL;
    }
    pft=(*pftpar)+id;
    if(pft->id!=UNDEF)
    {
      if(verb)
        fprintf(stderr,
                "ERROR176: PFT id=%d in line %d of '%s' has been already defined.\n",id,getlinecount(),getfilename());
      return NULL;
    }
    pft->id=id;

    /* Read pft->name */
    if(fscanstring(&item,s,"name",FALSE,verb))
    {
      if(verb)
        readstringerr("name");
      return NULL;
    }
    pft->name=strdup(s); /* store PFT name */
    check(pft->name);

    /* Read pft->type, defined in pftpar.h */
    fscanpftint(verb,&item,&pft->type,pft->name,"type");
    fscanpftint(verb,&item,&pft->cultivation_type,pft->name,"cultivation_type");
    if(pft->cultivation_type<0 || pft->cultivation_type>ANNUAL_CROP)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value %d for cultivation type of PFT '%s' in line %d of '%s'.\n",pft->cultivation_type,pft->name,getlinecount(),getfilename());
      return NULL;
    }
    if(isbiomass && pft->cultivation_type==NONE)
    {
      if(verb)
        fprintf(stderr,"ERROR210: Natural PFT '%s' in line %d of '%s' must be put before biomass plantation PFT.\n",pft->name,getlinecount(),getfilename());
      return NULL;
    }
    else if(pft->cultivation_type==BIOMASS)
      isbiomass=TRUE;
    fscanpftrealarray(verb,&item,pft->cn,NHSG,pft->name,"cn");
    fscanpftreal(verb,&item,&pft->beta_root,pft->name,"beta_root");
    totalroots=1 - pow(pft->beta_root,layerbound[BOTTOMLAYER-1]/10);
    pft->rootdist[0]=(1 - pow(pft->beta_root,layerbound[0]/10))/totalroots;
    for(l=1;l<BOTTOMLAYER;l++)
      pft->rootdist[l]=(pow(pft->beta_root,layerbound[l-1]/10) - pow(pft->beta_root,layerbound[l]/10))/totalroots;
    fscanpftreal(verb,&item,&pft->minwscal,pft->name,"minwscal");
    fscanpftreal(verb,&item,&pft->gmin,pft->name,"gmin");
    fscanpftreal(verb,&item,&pft->respcoeff,pft->name,"respcoeff");
    fscanpftreal(verb,&item,&pft->nmax,pft->name,"nmax");
    fscanpftreal(verb,&item,&pft->resist,pft->name,"resist");
    fscanpftreal(verb,&item,&pft->longevity,pft->name,"longevity");
    fscanpftreal(verb,&item,&pft->lmro_ratio,pft->name,"lmro_ratio");
    fscanpftreal(verb,&item,&pft->ramp,pft->name,"ramp");
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
    fscanpftreal(verb,&item,&pft->alphaa,pft->name,"alphaa");
    fscanpftreal(verb,&item,&pft->albedo_leaf,pft->name,"albedo_leaf");
    fscanpftreal(verb,&item,&pft->albedo_stem,pft->name,"albedo_stem");
    fscanpftreal(verb,&item,&pft->albedo_litter,pft->name,"albedo_litter");
    fscanpftreal(verb,&item,&pft->snowcanopyfrac,pft->name,"snowcanopyfrac");
    fscanpftreal(verb,&item,&pft->lightextcoeff,pft->name,"lightextcoeff");

    /* read new phenology parameters */
    fscanpftphenpar(verb,&item,&pft->tmin,pft->name,"tmin");
    fscanpftphenpar(verb,&item,&pft->tmax,pft->name,"tmax");
    fscanpftphenpar(verb,&item,&pft->light,pft->name,"light");
    fscanpftphenpar(verb,&item,&pft->wscal,pft->name,"wscal");
    fscanpftreal(verb,&item,&pft->mort_max,pft->name,"mort_max");

    fscanpftint(verb,&item,&pft->phenology,pft->name,"phenology");
    if(pft->phenology<0 || pft->phenology>CROPGREEN)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value %d for phenology of PFT '%s' in line %d of '%s'.\n",pft->phenology,pft->name,getlinecount(),getfilename());
      return NULL;
    }
    fscanpftint(verb,&item,&pft->path,pft->name,"path");
    if(pft->path<0 || pft->path>C4)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value %d for path of PFT '%s' in line %d of '%s'.\n",pft->path,pft->name,getlinecount(),getfilename());
      return NULL;
    }
    fscanpftlimit(verb,&item,&pft->temp_co2,pft->name,"temp_co2");
    fscanpftlimit(verb,&item,&pft->temp_photos,pft->name,"temp_photos");
    /* store precalculated k1, k2, k3 values */
    pft->k1=2*log(1/0.99-1)/(pft->temp_co2.low-pft->temp_photos.low);
    pft->k2=(pft->temp_co2.low+pft->temp_photos.low)*0.5;
    pft->k3=log(0.99/0.01)/(pft->temp_co2.high-pft->temp_photos.high);

    fscanpftlimit(verb,&item,&pft->temp,pft->name,"temp");
    fscanpftreal(verb,&item,&pft->soc_k,pft->name,"soc_k");
    fscanpftreal(verb,&item,&pft->alpha_fuelp,pft->name,"alpha_fuelp");
    fscanpftreal(verb,&item,&pft->vpd_par,pft->name,"vpd_par");
    fscanpftreal(verb,&item,&pft->fuelbulkdensity,pft->name,"fuelbulkdensity");
    fscanpftemissionfactor(verb,&item,&pft->emissionfactor,
                           pft->name,"emission_factor");
    fscanpftreal(verb,&item,&pft->aprec_min,pft->name,"aprec_min");
    fscanpftreal(verb,&item,&pft->flam,pft->name,"flam");
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
    fscanpftreal(verb,&item,&pft->windspeed,pft->name,"windspeed_dampening");
    fscanpftreal(verb,&item,&pft->roughness,pft->name,
                 "roughness_length");
    pft->k_litter10.leaf/=NDAYYEAR;
    pft->k_litter10.wood/=NDAYYEAR;
    if(pft->type<0 || pft->type>=ntypes)
    {
      if(verb)
        fprintf(stderr,"ERROR116: Invalid PFT class=%d of PFT '%s' in line %d of '%s'.\n",
                pft->type,pft->name,getlinecount(),getfilename());
      return NULL;
    }
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
    if(scanfcn[pft->type](&item,pft,verb))
      return NULL;
  }
  return npft;
} /* of 'fscanpftpar' */
