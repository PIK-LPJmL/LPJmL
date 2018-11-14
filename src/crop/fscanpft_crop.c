/**************************************************************************************/
/**                                                                                \n**/
/**             f  s  c  a  n  p  f  t  _  c  r  o  p  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads crop-specific parameter from text file                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"
#include "grass.h"

#define fscanpftreal(verb,file,var,pft,name) \
  if(fscanreal(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read float '%s' for CFT '%s'.\n",name,pft); \
    return TRUE; \
  }
#define fscanpftint(verb,file,var,pft,name) \
  if(fscanint(file,var,name,FALSE,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read int '%s' for CFT '%s'.\n",name,pft); \
    return TRUE; \
  }
#define fscanpftlimit(verb,file,var,pft,name) \
  if(fscanlimit(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read limit '%s' for CFT '%s'.\n",name,pft); \
    return TRUE; \
  }

#define fscancropdate2(verb,file,var,pft,name)\
  if(fscancropdate(file,var,verb))\
  {\
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read '%s' for CFT '%s'.\n",name,pft); \
    return TRUE; \
  }

static Bool fscancropdate(LPJfile *file,Initdate *initdate,Verbosity verb)
{
  if(fscanint(file,&initdate->sdatenh,"sdatenh",FALSE,verb))
    return TRUE;
  if(fscanint(file,&initdate->sdatesh,"sdatesh",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscancropdate' */

#define fscancropphys2(verb,file,var,pft,name)\
  if(fscancropphys(file,var,name,verb))\
  {\
    if(verb)\
    fprintf(stderr,"ERROR114: Cannot read cropphys '%s' for CFT '%s').\n",name,pft); \
    return TRUE; \
  }

static Bool fscancropphys(LPJfile *file,Cropphys *phys,const char *name,Verbosity verb)
{
  LPJfile item;
  if(fscanstruct(file,&item,name,verb))
    return TRUE;
  if(fscanreal(&item,&phys->root,"root",FALSE,verb))
    return TRUE;
  if(fscanreal(&item,&phys->so,"so",FALSE,verb))
    return TRUE;
  if(fscanreal(&item,&phys->pool,"pool",FALSE,verb))
    return TRUE;
  if(phys->root<=0 || phys->so<=0 || phys->pool<=0)
    return TRUE;
  return FALSE;
} /* of 'fscancropphys' */

Bool fscanpft_crop(LPJfile *file,  /**< pointer to LPJ file */
                   Pftpar *pft,    /**< Pointer to Pftpar array */
                   Verbosity verb  /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                /** \return TRUE on error */
{
  Pftcroppar *crop;
  pft->newpft=new_crop;
  pft->npp=NULL;
  pft->leaf_phenology=NULL;
  pft->fwrite=fwrite_crop;
  pft->fprint=fprint_crop;
  pft->fread=fread_crop;
  pft->litter_update=litter_update_crop;
  pft->actual_lai=actual_lai_crop;
  pft->fpar=fpar_crop;
  pft->alphaa_manage=alphaa_crop;
  pft->free=free_crop;
  pft->vegc_sum=vegc_sum_crop;
  pft->wdf=wdf_crop;
  pft->fprintpar=fprintpar_crop;
  pft->livefuel_consumption=NULL;
  pft->annual=NULL;
  pft->turnover_daily=NULL;
  pft->albedo_pft=albedo_crop;
  pft->agb=agb_crop;
  crop=new(Pftcroppar);
  check(crop);
  pft->data=crop;
  fscanpftint(verb,file,&crop->calcmethod_sdate,pft->name,
              "calcmethod_sdate");
  if(iskeydefined(file,"sla"))
  {
    fscanpftreal(verb,file,&pft->sla,pft->name,"sla");
  }
  else
    pft->sla=2e-4*pow(10,2.25-0.4*log(pft->longevity*12)/log(10))/CCpDM;   //"A photothermal model of leaf area index for greenhouse crops Xu etal.  "
  if(crop->calcmethod_sdate<0 ||  crop->calcmethod_sdate>MULTICROP)
  {
    if(verb)
      fprintf(stderr,"ERROR201: Invalid value %d for calcmethod_sdate of CFT '%s'.\n",
              crop->calcmethod_sdate,pft->name);
    return TRUE;
  }
  fscancropdate2(verb,file,&crop->initdate,pft->name,"init_date");
  fscanpftint(verb,file,&crop->hlimit,pft->name,
              "hlimit");
  fscanpftint(verb,file,&crop->fallow_days,pft->name,
              "fallow_days");
  fscanpftreal(verb,file,&crop->temp_fall,pft->name,
               "temp_fall");
  fscanpftreal(verb,file,&crop->temp_spring,pft->name,
               "temp_spring");
  fscanpftreal(verb,file,&crop->temp_vern,pft->name,
               "temp_vern");
  fscanpftlimit(verb,file,&crop->trg,pft->name,"trg");
  fscanpftreal(verb,file,&crop->pvd,pft->name,"pvd");
  fscanpftreal(verb,file,&crop->psens,pft->name,
               "psens");
  fscanpftreal(verb,file,&crop->pb,pft->name,"pb");
  fscanpftreal(verb,file,&crop->ps,pft->name,"ps");
  fscanpftlimit(verb,file,&crop->phuw,pft->name,
                "phuw");
  fscanpftlimit(verb,file,&crop->phus,pft->name,
                "phus");
  fscanpftreal(verb,file,&crop->phu_par,pft->name,"phu_par");
  fscanpftlimit(verb,file,&crop->basetemp,pft->name,"basetemp");
  fscanpftreal(verb,file,&crop->fphuc,pft->name,
               "fphuc");
  fscanpftreal(verb,file,&crop->flaimaxc,pft->name,"flaimaxc");
  fscanpftreal(verb,file,&crop->fphuk,pft->name,"fphuk");
  fscanpftreal(verb,file,&crop->flaimaxk,pft->name,"flaimaxk");
  fscanpftreal(verb,file,&crop->fphusen,pft->name,"fphusen");
  fscanpftreal(verb,file,&crop->flaimaxharvest,pft->name,"flaimaxharvest");
  fscanpftreal(verb,file,&crop->laimax,pft->name,"laimax");
  fscanpftreal(verb,file,&crop->laimin,pft->name,"laimin");
  fscanpftreal(verb,file,&crop->hiopt,pft->name,"hiopt");
  fscanpftreal(verb,file,&crop->himin,pft->name,"himin");
  fscanpftreal(verb,file,&crop->shapesenescencenorm,pft->name,
               "shapesenescencenorm");
  fscancropphys2(verb,file,&crop->cn_ratio,pft->name,"cn_ratio");
  crop->cn_ratio.root=pft->respcoeff*param.k/crop->cn_ratio.root;
  crop->cn_ratio.so=pft->respcoeff*param.k/crop->cn_ratio.so;
  crop->cn_ratio.pool=pft->respcoeff*param.k/crop->cn_ratio.pool;
  return FALSE;
} /* of 'fscanpft_crop' */

/*
- called in fscanpftpar()
- scans crop specific parameters from pft.par
  -> struct Pftcroppar in crop.h
*/
