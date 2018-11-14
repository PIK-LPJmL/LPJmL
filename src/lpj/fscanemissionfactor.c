/**************************************************************************************/
/**                                                                                \n**/
/**     f  s  c  a  n  e  m  i  s  s  i  o  n  f  a  c  t  o  r  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads emission factors from configuration file                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanemissionfactor(LPJfile *file,Tracegas *emissionfactor,const char *key,Verbosity verb)
{
  LPJfile f;
  if(fscanstruct(file,&f,key,verb))
    return TRUE;
  if(fscanreal(&f,&emissionfactor->co2,"co2",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&emissionfactor->co,"co",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&emissionfactor->ch4,"ch4",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&emissionfactor->voc,"voc",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&emissionfactor->tpm,"tpm",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&emissionfactor->nox,"nox",FALSE,verb))
    return TRUE;
  emissionfactor->co2*=1e-3;
  emissionfactor->co*=1e-3;
  emissionfactor->ch4*=1e-3;
  emissionfactor->voc*=1e-3;
  emissionfactor->tpm*=1e-3;
  emissionfactor->nox*=1e-3;
  return FALSE;
} /* of 'fscanemissionfactor' */
