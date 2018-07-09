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
  if(fscanreal(file,var,name,verb))\
  {\
    if(verb) \
      fprintf(stderr,"ERROR110: Cannot read real '%s' for soil type '%s'.\n",name,soil);\
    return 0;\
  }
#define fscanint2(verb,file,var,soil,name) \
  if(fscanint(file,var,name,verb))\
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

unsigned int fscansoilpar(LPJfile *file,     /**< pointer to LPJ file */
                          Soilpar **soilpar, /**< Pointer to Soilpar array */
                          Verbosity verb     /**< verbosity level (NO_ERR,ERR,VERB) */
                         )                   /** \return number of elements in array */
{
  LPJfile arr,item;
  unsigned int nsoil,n,id;
  int l;
  String s;
  Soilpar *soil;
  if (verb>=VERB) puts("// soil parameters");
  if(fscanrealarray(file,soildepth,NSOILLAYER,"soildepth",verb))
    return 0;
  if(fscanrealarray(file,layerbound,NSOILLAYER,"layerbound",verb))
    return 0;
  for(l=0;l<NSOILLAYER;l++)
    midlayer[l]=l>0?(layerbound[l-1]+soildepth[l]/2.):soildepth[l]/2.;
  foreachsoillayer(l)
  {
    logmidlayer[l]=log10(midlayer[l]/midlayer[NSOILLAYER-2]);
  }
  if(fscanrealarray(file,fbd_fac,NFUELCLASS,"fbd_fac",verb))
    return 0;
  if(fscanarray(file,&arr,&nsoil,TRUE,"soilpar",verb))
    return 0;
  if(nsoil<1)
  {
    if(verb)
      fprintf(stderr,"ERROR170: Invalid value for number of soil types=%u in line %d of '%s'\n",
              nsoil,getlinecount(),getfilename());
    return 0;
  }
  *soilpar=newvec(Soilpar,nsoil);
  check(*soilpar);
  for(n=0;n<nsoil;n++)
    (*soilpar)[n].type=UNDEF;
  for(n=0;n<nsoil;n++)
  {
    fscanarrayindex(&arr,&item,n,verb);
    if(fscanuint(&item,&id,"id",verb))
      return 0;
    if(id>=nsoil)
    {
      if(verb)
        fprintf(stderr,"ERROR115: Invalid range of soil type=%u in line %d of '%s' in fscansoilpar(), valid range is [0,%u].\n",id,getlinecount(),getfilename(),nsoil-1);
      return 0;
    }
    soil=(*soilpar)+id;
    if(soil->type!=UNDEF)
    {
      if(verb)
        fprintf(stderr,"ERROR177: Soil type=%u in line %d of '%s' has been already defined in fscansoilpar().\n",id,getlinecount(),getfilename());
      return 0;
    }
    if(fscanstring(&item,s,"name",verb))
    {
      if(verb)
        readstringerr("name");
      return 0;
    }
    soil->name=strdup(s);
    check(soil->name);
    soil->type=id;
    fscanreal2(verb,&item,&soil->Ks,soil->name,"Ks");
    fscanreal2(verb,&item,&soil->Sf,soil->name,"Sf");
    fscanreal2(verb,&item,&soil->wpwp,soil->name,"w_pwp");
    fscanreal2(verb,&item,&soil->wfc,soil->name,"w_fc");
    if(soil->wfc>1)
    {
      if(verb)
        fprintf(stderr,"ERROR215: wfc=%g>1 in line %d of '%s' for soil type '%s'.\n",
                soil->wfc,getlinecount(),getfilename(),soil->name);
      return 0;
    }
    if(soil->wfc-soil->wpwp<0)
    {
      if(verb)
        fprintf(stderr,"ERROR213: whc=%g<0 in line %d of '%s' for soil type '%s', wfc=%g, wpwp=%g\n",
                soil->wfc-soil->wpwp,getlinecount(),getfilename(),soil->name,soil->wfc,soil->wpwp);
      return 0;
    }
    fscanreal2(verb,&item,&soil->wsat,soil->name,"w_sat");
    if(soil->wsat<=0 || soil->wsat>1)
    {
      if(verb)
        fprintf(stderr,"ERROR220: wsat=%g in line %d of '%s' not in (0,1] for soil type '%s'.\n",
                soil->wsat,getlinecount(),getfilename(),soil->name);
      return 0;
    }
    if(soil->wsat<=soil->wfc)
    {
      if(verb)
        fprintf(stderr,"ERROR216: wsat=%g <= wfc=%g in line %d of '%s' for soil type '%s'.\n",
                soil->wsat,soil->wfc,getlinecount(),getfilename(),soil->name);
      return 0;
    }
    soil->beta_soil=-2.655/log10(soil->wfc/soil->wsat);
    soil->whcs_all=0.0;
    for(l=0;l<LASTLAYER;l++)
    {
      soil->whc[l]=soil->wfc-soil->wpwp;
      soil->whcs[l]=soil->whc[l]*soildepth[l];
      soil->wpwps[l]=soil->wpwp*soildepth[l];
      soil->wsats[l]=soil->wsat*soildepth[l];
      soil->bulkdens[l]=(1-soil->wsat)*MINERALDENS;
      if(soil->type==ROCK)
        soil->k_dry[l]=8.8;
      else            //Johansen assumptions
        soil->k_dry[l]=(0.135*soil->bulkdens[l]+64.7)/
               (MINERALDENS-0.947*soil->bulkdens[l]);
    }
    /*assume last layer is bedrock in 6-layer version */
    soil->whc[BOTTOMLAYER]=0.002;/*0.006 wsats - 0.002 whc - 0.001 wpwps = 0.003 for free water */
    soil->whcs[BOTTOMLAYER]=soil->whc[BOTTOMLAYER]*soildepth[BOTTOMLAYER];
    soil->wpwps[BOTTOMLAYER]=0.001*soildepth[BOTTOMLAYER];
    soil->wsats[BOTTOMLAYER]=0.006*soildepth[BOTTOMLAYER];
    soil->bulkdens[BOTTOMLAYER]=(1-soil->wsats[BOTTOMLAYER]/soildepth[BOTTOMLAYER])*MINERALDENS;
    soil->k_dry[BOTTOMLAYER]=0.039*pow(soil->wsats[BOTTOMLAYER]/soildepth[BOTTOMLAYER],-2.2);
    fscanint2(verb,&item,&soil->hsg,soil->name,"hsg");
    if(soil->hsg<1 || soil->hsg>NHSG)
    {
      if(verb)
        fprintf(stderr,"ERROR199: Invalid value=%d for hsg in soil '%s'.\n",
                soil->hsg,soil->name);
      return 0;
    }
    fscanreal2(verb,&item,&soil->tdiff_0,soil->name,"tdiff_0");
    fscanreal2(verb,&item,&soil->tdiff_15,soil->name,"tdiff_15");
    fscanreal2(verb,&item,&soil->tdiff_100,soil->name,"tdiff_100");
    fscanreal2(verb,&item,&soil->tcond_pwp,soil->name,"cond_pwp");
    fscanreal2(verb,&item,&soil->tcond_100,soil->name,"cond_100");
    fscanreal2(verb,&item,&soil->tcond_100_ice,soil->name,"cond_100_ice");
    fscanreal2(verb,&item,&soil->denit_rate,soil->name,"denit_rate");
    fscanreal2(verb,&item,&soil->a_denit,soil->name,"a_denit");
    fscanreal2(verb,&item,&soil->b_denit,soil->name,"b_denit");
    fscanreal2(verb,&item,&soil->anion_excl,soil->name,"anion_excl");
    fscanreal2(verb,&item,&soil->nitvol_factor_temp,soil->name,"nitvol_temp_factor");
    fscanreal2(verb,&item,&soil->vol_cation_exchange,soil->name,"vol_cation_exchange");
    fscanreal2(verb,&item,&soil->denit_water_threshold,soil->name,"denit_water_threshold");
    fscanreal2(verb,&item,&soil->a_nit,soil->name,"a_nit");
    fscanreal2(verb,&item,&soil->b_nit,soil->name,"b_nit");
    fscanreal2(verb,&item,&soil->c_nit,soil->name,"c_nit");
    fscanreal2(verb,&item,&soil->d_nit,soil->name,"d_nit");
    soil->z_nit=soil->d_nit*(soil->b_nit-soil->a_nit)/(soil->a_nit-soil->c_nit);
    soil->n_nit=soil->a_nit-soil->b_nit;
    soil->m_nit=soil->a_nit-soil->c_nit;
    //fscanreal2(verb,&item,&soil->C_corr,soil->name,"C_corr");
    //fscanreal2(verb,&item,&soil->bd,soil->name,"bd");
    fscanreal2(verb,&item,&soil->cn_ratio,soil->name,"cn_ratio");

  } /* of 'for(n=0;...)' */
  return n;
} /* of 'fscansoilpar' */
