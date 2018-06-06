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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define UNDEF (-1)

#define fscanreal2(verb,file,var,name)\
  if(fscanreal(file,var,name,verb))\
  {\
    return 0;\
  }
#define fscanint2(verb,file,var,name) \
  if(fscanint(file,var,name,verb))\
  {\
    return 0; \
  }

Real soildepth[NSOILLAYER];
Real layerbound[NSOILLAYER];
Real midlayer[NSOILLAYER];
Real logmidlayer[NSOILLAYER];   /*log10(midlayer[l]/midlayer[NSOILLAYER-2]), for vertical soc profile*/
Real fbd_fac[NFUELCLASS];

unsigned int fscansoilpar(FILE *file,        /**< file  pointer */
                          Soilpar **soilpar, /**< Pointer to Soilpar array */
                          Verbosity verb     /**< verbosity level (NO_ERR,ERR,VERB) */
                         )                   /** \return number of elements in array */
{
  unsigned int nsoil,n,id;
  int l;
  String s;
  Soilpar *soil;
  if (verb>=VERB) puts("// soil parameters");
  for(l=0;l<NSOILLAYER;l++)
    if(fscanreal(file,soildepth+l,"soildepth",verb))
      return 0;
  for(l=0;l<NSOILLAYER;l++)
  {
    if(fscanreal(file,layerbound+l,"layerbound",verb))
      return 0;
    midlayer[l]=l>0?(layerbound[l-1]+soildepth[l]/2.):soildepth[l]/2.;
  }
  foreachsoillayer(l)
  {
    logmidlayer[l]=log10(midlayer[l]/midlayer[NSOILLAYER-2]);
  }
  for(l=0;l<NFUELCLASS;l++)
    if(fscanreal(file,fbd_fac+l,"fbd_fac",verb))
      return 0;
  if(fscanuint(file,&nsoil,"nsoil",verb))
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
    if(fscanuint(file,&id,"soiltype",verb))
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
    if(fscanstring(file,s,verb!=NO_ERR))
    {
      if(verb)
        readstringerr("name");
      return 0;
    }
    if (verb>=VERB) printf("SOIL_NAME %s\n", s);
    soil->name=strdup(s);
    check(soil->name);
    soil->type=id;
    fscanreal2(verb,file,&soil->Ks,"Ks");
    fscanreal2(verb,file,&soil->Sf,"Sf");
    fscanreal2(verb,file,&soil->wpwp,"w_pwp");
    fscanreal2(verb,file,&soil->wfc,"w_fc");
    if(soil->wfc>1)
    {
      if(verb)
        fprintf(stderr,"ERROR215: wfc=%g>1 in line %d of '%s' for soil type %s\n",
                soil->wfc,getlinecount(),getfilename(),soil->name);
      return 0;
    }
    if(soil->wfc-soil->wpwp<0)
    {
      if(verb)
        fprintf(stderr,"ERROR213: whc=%g<0 in line %d of '%s' for soil type %s, wfc=%g, wpwp=%g\n",
                soil->wfc-soil->wpwp,getlinecount(),getfilename(),soil->name,soil->wfc,soil->wpwp);
      return 0;
    }
    fscanreal2(verb,file,&soil->wsat,"w_sat");
    if(soil->wsat<=0 || soil->wsat>1)
    {
      if(verb)
        fprintf(stderr,"ERROR220: wsat=%g in line %d of '%s' not in (0,1] for soil type %s\n",
                soil->wsat,getlinecount(),getfilename(),soil->name);
      return 0;
    }
    if(soil->wsat<=soil->wfc)
    {
      if(verb)
        fprintf(stderr,"ERROR216: wsat=%g <= wfc=%g in line %d of '%s' for soil type %s\n",
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
    fscanint2(verb,file,&soil->hsg,"hsg");
    if(soil->hsg<1 || soil->hsg>NHSG)
    {
      if(verb)
        fprintf(stderr,"ERROR199: Invalid value=%d for hsg in soil '%s'.\n",
                soil->hsg,soil->name);
      return 0;
    }
    fscanreal2(verb,file,&soil->tdiff_0,"tdiff_0");
    fscanreal2(verb,file,&soil->tdiff_15,"tdiff_15");
    fscanreal2(verb,file,&soil->tdiff_100,"tdiff_100");
    fscanreal2(verb,file,&soil->tcond_pwp,"cond_pwp");
    fscanreal2(verb,file,&soil->tcond_100,"cond_100");
    fscanreal2(verb,file,&soil->tcond_100_ice,"cond_100_ice");

  } /* of 'for(n=0;...)' */
  return n;
} /* of 'fscansoilpar' */
