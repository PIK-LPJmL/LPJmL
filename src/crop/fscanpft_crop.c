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
  if(fscanreal(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read CFT '%s' in %s().\n",pft,__FUNCTION__); \
    return TRUE; \
  }
#define fscanpftint(verb,file,var,pft,name) \
  if(fscanint(file,var,name,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR110: Cannot read CFT '%s' in %s().\n",pft,__FUNCTION__); \
    return TRUE; \
  }
#define fscanpftlimit(verb,file,var,pft,name) \
  if(fscanlimit(file,var,verb)) \
  { \
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read limit '%s' of CFT '%s' in %s().\n",name,pft,__FUNCTION__); \
    return TRUE; \
  }

#define fscancropdate2(verb,file,var,pft,name)\
  if(fscancropdate(file,var,verb))\
  {\
    if(verb)\
    fprintf(stderr,"ERROR112: Cannot read '%s' of CFT '%s' in %s().\n",name,pft,__FUNCTION__); \
    return TRUE; \
  }

static Bool fscancropdate(FILE *file,Initdate *initdate,Verbosity verb)
{
  if(fscanint(file,&initdate->sdatenh,"date northern hemisphere",verb))
    return TRUE;
  if(fscanint(file,&initdate->sdatesh,"date southern hemisphere",verb))
    return TRUE;
  return FALSE;
} /* of 'fscancropdate' */

#define fscancropphys2(verb,file,var,pft,name)\
  if(fscancropphys(file,var,verb))\
  {\
    if(verb)\
    fprintf(stderr,"ERROR114: Cannot read cropphys '%s' of CFT '%s' in %s().\n",name,pft,__FUNCTION__); \
    return TRUE; \
  }

static Bool fscancropphys(FILE *file,Cropphys *phys,Verbosity verb)
{
  if(fscanreal(file,&phys->root,"root",verb))
    return TRUE;
  if(fscanreal(file,&phys->so,"so",verb))
    return TRUE;
  if(fscanreal(file,&phys->pool,"pool",verb))
    return TRUE;
  if(phys->root<=0 || phys->so<=0 || phys->pool<=0)
    return TRUE;
  return FALSE;
} /* of 'fscancropphys' */

Bool fscanpft_crop(FILE *file,           /**< file pointer */
                   Pftpar *pft,          /**< Pointer to Pftpar array */
                   Verbosity verb        /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                      /** \return TRUE on error */
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
  pft->sla=2e-4*pow(10,2.25-0.4*log(pft->longevity*12)/log(10))/CCpDM;   //"A photothermal model of leaf area index for greenhouse crops Xu etal.  "
  if(crop->calcmethod_sdate<0 ||  crop->calcmethod_sdate>MULTICROP)
  {
    if(verb)
      fprintf(stderr,"ERROR201: Invalid value %d for calcmethod_sdate of CFT '%s'.\n",
              crop->calcmethod_sdate,pft->name);
    return TRUE;
  }
  fscancropdate2(verb,file,&crop->initdate,pft->name,"init date");
  fscanpftint(verb,file,&crop->hlimit,pft->name,
              "max length of crop cycle");
  fscanpftint(verb,file,&crop->fallow_days,pft->name,
              "fallow period after harvest");
  fscanpftreal(verb,file,&crop->temp_fall,pft->name,
               "temperature threshold for fall");
  fscanpftreal(verb,file,&crop->temp_spring,pft->name,
               "temperature threshold for spring");
  fscanpftreal(verb,file,&crop->temp_vern,pft->name,
               "temperature threshold for vernalization");
  fscanpftlimit(verb,file,&crop->trg,pft->name,"vernalization temperature");
  fscanpftreal(verb,file,&crop->pvd,pft->name,"number of vernalising days");
  fscanpftreal(verb,file,&crop->psens,pft->name,
               "photoperiod sensitivity");
  fscanpftreal(verb,file,&crop->pb,pft->name,"photoperiod basal");
  fscanpftreal(verb,file,&crop->ps,pft->name,"photoperiod saturated");
  fscanpftlimit(verb,file,&crop->phuw,pft->name,
                "potential heat units winter");
  fscanpftlimit(verb,file,&crop->phus,pft->name,
                "potential heat units summer");
  fscanpftreal(verb,file,&crop->phu_par,pft->name,
               "parameter for determining the variability of phu");
  fscanpftlimit(verb,file,&crop->basetemp,pft->name,"base temperature");
  fscanpftreal(verb,file,&crop->fphuc,pft->name,
               "growing season to the first point");
  fscanpftreal(verb,file,&crop->flaimaxc,pft->name,
               "plant maximal LAI to the first point");
  fscanpftreal(verb,file,&crop->fphuk,pft->name,
               "growing season to the second point");
  fscanpftreal(verb,file,&crop->flaimaxk,pft->name,
               "plant maximal LAI to the second point");
  fscanpftreal(verb,file,&crop->fphusen,pft->name,"growing period");
  fscanpftreal(verb,file,&crop->flaimaxharvest,pft->name,
               "maximal LAI at harvest");
  fscanpftreal(verb,file,&crop->laimax,pft->name,"maximal LAI");
  fscanpftreal(verb,file,&crop->laimin,pft->name,"minimal LAI");
  fscanpftreal(verb,file,&crop->hiopt,pft->name,"optimum harvest index");
  fscanpftreal(verb,file,&crop->himin,pft->name,"minimum harvest index");
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
