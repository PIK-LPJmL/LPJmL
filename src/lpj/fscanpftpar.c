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
  if(fscanreal(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read PFT '%s' in %s().\n",pft,__FUNCTION__); \
    return NULL; \
  }
#define fscanpftint(verb,file,var,pft,name) \
  if(fscanint(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read PFT '%s' in %s().\n",pft,__FUNCTION__); \
    return NULL; \
  }
#define fscanpftlimit(verb,file,var,pft,name) \
  if(fscanlimit(file,var,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read limit '%s' of PFT '%s' in %s().\n",name,pft,__FUNCTION__); \
    return NULL; \
  }
#define fscanpftphenpar(verb,file,var,pft,name) \
  if(fscanphenparam(file,var,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read phenology param '%s' of PFT '%s' in %s().\n",name,pft,__FUNCTION__); \
    return NULL; \
  }
#define fscanpftemissionfactor(verb,file,var,pft,name) \
  if(fscanemissionfactor(file,var,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR128: Cannot read emission factor '%s' of PFT '%s' in %s().\n",name,pft,__FUNCTION__); \
    return NULL; \
  }

int *fscanpftpar(FILE  *file,         /**< File pointer to text file */
                 Pftpar **pftpar,     /**< Pointer to PFT parameter array */
                 const Fscanpftparfcn scanfcn[], /**< array of PFT-specific scan
                                                    functions */
                 int ntypes,          /**< number of PFT classes */
                 Verbosity verb       /**< verbosity level (NO_ERR,ERR,VERB) */
                )                     /** \return array of size ntypes or NULL */
{
  int *npft,n,id,l,count;
  String s;
  Pftpar *pft;
  Real totalroots;
  Bool isbiomass;
  if (verb>=VERB) puts("// PFT parameters");
  /* Read total number of defined PFTs */
  if(fscanint(file,&count,"npft",verb))
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
    /* Read pft->id, defined in pftpar.h */
    if(fscanint(file,&id,"id",verb))
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
    if(fscanstring(file,s,verb!=NO_ERR))
    {
      if(verb)
        readstringerr("name");
      return NULL;
    }
    if(verb>=VERB)
      printf("PFT_NAME %s\n",s);
    pft->name=strdup(s); /* store PFT name */
    check(pft->name);

    /* Read pft->type, defined in pftpar.h */
    fscanpftint(verb,file,&pft->type,pft->name,"type");
    fscanpftint(verb,file,&pft->cultivation_type,pft->name,"cultivation type");
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
    for(l=0;l<NHSG;l++)
      fscanpftreal(verb,file,pft->cn+l,pft->name,"cn");
    fscanpftreal(verb,file,&pft->beta_root,pft->name,"beta_root");
    totalroots=1 - pow(pft->beta_root,layerbound[BOTTOMLAYER-1]/10);
    pft->rootdist[0]=(1 - pow(pft->beta_root,layerbound[0]/10))/totalroots;
    for(l=1;l<BOTTOMLAYER;l++)
      pft->rootdist[l]=(pow(pft->beta_root,layerbound[l-1]/10) - pow(pft->beta_root,layerbound[l]/10))/totalroots;
    fscanpftreal(verb,file,&pft->minwscal,pft->name,"minwscal");
    fscanpftreal(verb,file,&pft->gmin,pft->name,"gmin");
    fscanpftreal(verb,file,&pft->respcoeff,pft->name,
                 "respcoeff");
    fscanpftreal(verb,file,&pft->nmax,pft->name,"nmax");
    fscanpftreal(verb,file,&pft->resist,pft->name,"resist");
    fscanpftreal(verb,file,&pft->longevity,pft->name,
                 "longevity");
    fscanpftreal(verb,file,&pft->lmro_ratio,pft->name,
                 "lmro_ratio");
    fscanpftreal(verb,file,&pft->ramp,pft->name,"ramp");
    pft->ramp=1/pft->ramp; /* store reciprocal to speed up calculations */
    fscanpftreal(verb,file,&pft->lai_sapl,pft->name,"lai_sapl");
    fscanpftreal(verb,file,&pft->gdd5min,pft->name,"gdd5min");
    fscanpftreal(verb,file,&pft->twmax,pft->name,"twmax");
    fscanpftreal(verb,file,&pft->twmax_daily,pft->name,"twmax_daily");
    fscanpftreal(verb,file,&pft->gddbase,pft->name,"gddbase");
    fscanpftreal(verb,file,&pft->min_temprange,pft->name,
                 "min_temprange");
    fscanpftreal(verb,file,&pft->emax,pft->name,"emax");
    fscanpftreal(verb,file,&pft->intc,pft->name,"intc");
    fscanpftreal(verb,file,&pft->alphaa,pft->name,"alphaa");
    fscanpftreal(verb,file,&pft->albedo_leaf,pft->name,"albedo_leaf");
    fscanpftreal(verb,file,&pft->albedo_stem,pft->name,"albedo_stem");
    fscanpftreal(verb,file,&pft->albedo_litter,pft->name,"albedo_litter");
    fscanpftreal(verb,file,&pft->snowcanopyfrac,pft->name,"snowcanopyfrac");
    fscanpftreal(verb,file,&pft->lightextcoeff,pft->name,"lightextcoeff");

    /* read new phenology parameters */
    fscanpftphenpar(verb,file,&pft->tmin,pft->name,"tmin");
    fscanpftphenpar(verb,file,&pft->tmax,pft->name,"tmax");
    fscanpftphenpar(verb,file,&pft->light,pft->name,"light");
    fscanpftphenpar(verb,file,&pft->wscal,pft->name,"wscal");
    fscanpftreal(verb,file,&pft->mort_max,pft->name,"mort_max");

    fscanpftint(verb,file,&pft->phenology,pft->name,"phenology");
    if(pft->phenology<0 || pft->phenology>CROPGREEN)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value %d for phenology of PFT '%s' in line %d of '%s'.\n",pft->phenology,pft->name,getlinecount(),getfilename());
      return NULL;
    }
    fscanpftint(verb,file,&pft->path,pft->name,"path");
    if(pft->path<0 || pft->path>C4)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value %d for path of PFT '%s' in line %d of '%s'.\n",pft->path,pft->name,getlinecount(),getfilename());
      return NULL;
    }
    fscanpftlimit(verb,file,&pft->temp_co2,pft->name,"temp_co2");
    fscanpftlimit(verb,file,&pft->temp_photos,pft->name,
                  "temp_photos");
    /* store precalculated k1, k2, k3 values */
    pft->k1=2*log(1/0.99-1)/(pft->temp_co2.low-pft->temp_photos.low);
    pft->k2=(pft->temp_co2.low+pft->temp_photos.low)*0.5;
    pft->k3=log(0.99/0.01)/(pft->temp_co2.high-pft->temp_photos.high);

    fscanpftlimit(verb,file,&pft->temp,pft->name,"temp");
    fscanpftreal(verb,file,&pft->soc_k,pft->name,"soc_k");
    fscanpftreal(verb,file,&pft->alpha_fuelp,pft->name,"alpha_fuelp");
    fscanpftreal(verb,file,&pft->fuelbulkdensity,pft->name,"fuel bulk density");
    fscanpftemissionfactor(verb,file,&pft->emissionfactor,
                           pft->name,"emission factor");
    fscanpftreal(verb,file,&pft->aprec_min,pft->name,
                 "aprec_min");
    fscanpftreal(verb,file,&pft->flam,pft->name,"flam");
    fscanpftreal(verb,file,&pft->k_litter10.leaf,pft->name,
                 "k_litter10_leaf");
    fscanpftreal(verb,file,&pft->k_litter10.wood,pft->name,
                 "k_litter10_wood");
    fscanpftreal(verb,file,&pft->k_litter10.q10_wood,pft->name,
                 "k_litter10_q10_wood");
    fscanpftreal(verb,file,&pft->windspeed,pft->name,
                 "windspeed dampening");
    fscanpftreal(verb,file,&pft->roughness,pft->name,
                 "roughness length");
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
    if(scanfcn[pft->type](file,pft,verb))
      return NULL;
  }
  return npft;
} /* of 'fscanpftpar' */
