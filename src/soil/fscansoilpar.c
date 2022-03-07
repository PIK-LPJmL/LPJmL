/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  s  o  i  l  p  a  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads soil parameter from text file                               \n**/
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

#define fscanreal2(verb,file,var,soil,name)\
  if(fscanreal(file,var,name,FALSE,verb))\
  {\
    if(verb) \
      fprintf(stderr,"ERROR110: Cannot read real '%s' for soil type '%s'.\n",name,soil);\
    return 0;\
  }
#define fscanint2(verb,file,var,soil,name) \
  if(fscanint(file,var,name,FALSE,verb))\
  {\
    if(verb) \
      fprintf(stderr,"ERROR110: Cannot read int '%s' for soil type '%s'.\n",name,soil);\
    return 0;\
  }

Real soildepth[NSOILLAYER];
Real layerbound[NSOILLAYER];
Real midlayer[NSOILLAYER];
Real logmidlayer[NSOILLAYER];   /*log10(midlayer[l]/midlayer[NSOILLAYER-2]), for vertical soc profile*/
Real fbd_fac[NFUELCLASS];

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return 0;}

unsigned int fscansoilpar(LPJfile *file,     /**< pointer to LPJ file */
                          Soilpar **soilpar, /**< Pointer to Soilpar array */
                          int soilpar_option,/**< soil parameter option */
                          int with_nitrogen, /**< nitrogen cycled enabled? */
                          Verbosity verb     /**< verbosity level (NO_ERR,ERR,VERB) */
                         )                   /** \return number of elements in array */
{
  LPJfile arr,item;
  int id;
  int l,nsoil,n;
  String s;
  Soilpar *soil;
  if (verb>=VERB) puts("// soil parameters");
  if(fscanrealarray(file,soildepth,NSOILLAYER,"soildepth",verb))
    return 0;
  /* calculate layerbound and midlayer from soildepth */
  layerbound[0]=soildepth[0];
  midlayer[0]=soildepth[0]*0.5;
  for(l=1;l<NSOILLAYER;l++)
  {
    layerbound[l]=layerbound[l-1]+soildepth[l];
    midlayer[l]=layerbound[l-1]+soildepth[l]*0.5;
  }
  foreachsoillayer(l)
  {
    if(soildepth[l]<=0)
    {
      if(verb)
        fprintf(stderr,"ERROR234: Soil depth of layer %d=%g must be greater than zero.\n",l,soildepth[l]);
      return 0;
    }
    logmidlayer[l]=log10(midlayer[l]/midlayer[NSOILLAYER-2]);
  }
  if(fscanrealarray(file,fbd_fac,NFUELCLASS,"fbd_fac",verb))
    return 0;
  if(fscanarray(file,&arr,&nsoil,TRUE,"soilpar",verb))
    return 0;
  if(nsoil<1)
  {
    if(verb)
      fprintf(stderr,"ERROR170: Invalid value for number of soil types=%d, must be greater than zero.\n",
              nsoil);
    return 0;
  }
  *soilpar=newvec(Soilpar,nsoil);
  checkptr(*soilpar);
  for(n=0;n<nsoil;n++)
    (*soilpar)[n].type=UNDEF;
  for(n=0;n<nsoil;n++)
  {
    fscanarrayindex(&arr,&item,n,verb);
    id=n;
    if(fscanint(&item,&id,"id",TRUE,verb))
      return 0;
    if(id<0 || id>=nsoil)
    {
      if(verb)
        fprintf(stderr,"ERROR115: Invalid range of soil type=%d in fscansoilpar(), valid range is [0,%d].\n",id,nsoil-1);
      return 0;
    }
    soil=(*soilpar)+id;
    if(soil->type!=UNDEF)
    {
      if(verb)
        fprintf(stderr,"ERROR177: Soil type=%d has been already defined in fscansoilpar().\n",id);
      return 0;
    }
    if(fscanstring(&item,s,"name",FALSE,verb))
    {
      if(verb)
        readstringerr("name");
      return 0;
    }
    soil->name=strdup(s);
    checkptr(soil->name);
    soil->type=id;
    fscanreal2(verb,&item,&soil->Ks,soil->name,"Ks");
    fscanreal2(verb,&item,&soil->Sf,soil->name,"Sf");
    if(soilpar_option==PRESCRIBED_SOILPAR)
    {
      fscanreal2(verb,&item,&soil->wpwp,soil->name,"w_pwp");
      fscanreal2(verb,&item,&soil->wfc,soil->name,"w_fc");
      if(soil->wfc<=0 || soil->wfc>1)
      {
        if(verb)
          fprintf(stderr,"ERROR215: wfc=%g not in (0,1] for soil type '%s'.\n",
                  soil->wfc,soil->name);
        return 0;
      }
      if(soil->wfc-soil->wpwp<0)
      {
        if(verb)
          fprintf(stderr,"ERROR213: whc=%g<0 for soil type '%s', wfc=%g, wpwp=%g\n",
                  soil->wfc-soil->wpwp,soil->name,soil->wfc,soil->wpwp);
        return 0;
      }
      fscanreal2(verb,&item,&soil->wsat,soil->name,"w_sat");
      if(soil->wsat<=0 || soil->wsat>1)
      {
        if(verb)
          fprintf(stderr,"ERROR220: wsat=%g not in (0,1] for soil type '%s'.\n",
                  soil->wsat,soil->name);
        return 0;
      }
      if(soil->wsat<=soil->wfc)
      {
        if(verb)
          fprintf(stderr,"ERROR216: wsat=%g <= wfc=%g for soil type '%s'.\n",
                  soil->wsat,soil->wfc,soil->name);
        return 0;
      }
    }
    fscanreal2(verb,&item,&soil->sand,soil->name,"sand");
    fscanreal2(verb,&item,&soil->silt,soil->name,"silt");
    fscanreal2(verb,&item,&soil->clay,soil->name,"clay");
    fscanreal2(verb, &item, &soil->psi_sat, soil->name, "psi_sat");
    if(fabs(soil->sand+soil->silt+soil->clay-1)>epsilon)
    {
      if(verb)
        fprintf(stderr,"ERROR199: Sum of sand+silt+clay=%g is not one in soil '%s'.\n",
                soil->sand+soil->silt+soil->clay,soil->name);
      return 0;
    }
    fscanint2(verb,&item,&soil->hsg,soil->name,"hsg");
    if(soil->hsg<1 || soil->hsg>NHSG)
    {
      if(verb)
        fprintf(stderr,"ERROR199: Invalid value=%d for hsg in soil '%s', must be in [1,%d].\n",
                soil->hsg,soil->name,NHSG);
      return 0;
    }
    fscanreal2(verb,&item,&soil->tdiff_0,soil->name,"tdiff_0");
    fscanreal2(verb,&item,&soil->tdiff_15,soil->name,"tdiff_15");
    fscanreal2(verb,&item,&soil->tdiff_100,soil->name,"tdiff_100");
    fscanreal2(verb,&item,&soil->tcond_pwp,soil->name,"cond_pwp");
    fscanreal2(verb,&item,&soil->tcond_100,soil->name,"cond_100");
    fscanreal2(verb,&item,&soil->tcond_100_ice,soil->name,"cond_100_ice");
    if(with_nitrogen)
    {
      fscanreal2(verb,&item,&soil->anion_excl,soil->name,"anion_excl");
      fscanreal2(verb,&item,&soil->a_nit,soil->name,"a_nit");
      fscanreal2(verb,&item,&soil->b_nit,soil->name,"b_nit");
      fscanreal2(verb,&item,&soil->c_nit,soil->name,"c_nit");
      fscanreal2(verb,&item,&soil->d_nit,soil->name,"d_nit");
      soil->m_nit=soil->a_nit-soil->c_nit;
      if(soil->m_nit==0)
      {
        if(verb)
          fprintf(stderr,"ERROR199: a_nit=%g must not be equal c_nit=%g for soil '%s'.\n",
                  soil->a_nit,soil->c_nit,soil->name);
        return 0;
      }
      soil->z_nit=soil->d_nit*(soil->b_nit-soil->a_nit)/(soil->a_nit-soil->c_nit);
      soil->n_nit=soil->a_nit-soil->b_nit;
      fscanreal2(verb,&item,&soil->cn_ratio,soil->name,"cn_ratio");
      if(soil->cn_ratio<=0)
      {
        if(verb)
          fprintf(stderr,"ERROR199: CN ratio=%g must not be less than zero for soil '%s'.\n",
                  soil->cn_ratio,soil->name);
        return 0;
      }
    }
    fscanreal2(verb, &item, &soil->b, soil->name, "b");
    fscanreal2(verb, &item, &soil->efold, soil->name, "efold");
    fscanreal2(verb, &item, &soil->ctimax, soil->name, "ctimax");
  } /* of 'for(n=0;...)' */
  return n;
} /* of 'fscansoilpar' */
