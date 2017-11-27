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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanemissionfactor(FILE *file,Tracegas *emissionfactor,Verbosity verb)
{
  if(fscanreal(file,&emissionfactor->co2,"emiss. factor co2",verb))
    return TRUE;
  if(fscanreal(file,&emissionfactor->co,"emiss. factor co",verb))
    return TRUE;
  if(fscanreal(file,&emissionfactor->ch4,"emiss. factor ch4",verb))
    return TRUE;
  if(fscanreal(file,&emissionfactor->voc,"emiss. factor voc",verb))
    return TRUE;
  if(fscanreal(file,&emissionfactor->tpm,"emiss. factor tpm",verb))
    return TRUE;
  if(fscanreal(file,&emissionfactor->nox,"emiss. factor nox",verb))
    return TRUE;
  emissionfactor->co2*=1e-3;
  emissionfactor->co*=1e-3;
  emissionfactor->ch4*=1e-3;
  emissionfactor->voc*=1e-3;
  emissionfactor->tpm*=1e-3;
  emissionfactor->nox*=1e-3;
  return FALSE;
} /* of 'fscanemissionfactor' */
