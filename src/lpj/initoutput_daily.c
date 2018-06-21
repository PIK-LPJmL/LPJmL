/**************************************************************************************/
/**                                                                                \n**/
/**        i  n  i  t  o  u  t  p  u  t  _  d  a  i  l  y  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes monthly output data to zero                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void initoutput_daily(Daily_outputs *output /**< Daily output data */
                     )
{
  output->cleaf=output->cpool=output->croot=output->cso=output->evap=
    output->fphu=output->froot=output->gpp=output->gresp=
    output->growingday=output->hi=output->himind=output->fhiopt=
    output->husum=output->irrig=output->lai=output->laimaxad=
    output->lainppdeficit=output->npp=output->perc=
    output->phen=output->phu=output->prec=output->pvd=output->rd=
    output->rpool=output->rroot=output->rso=output->trans=
    output->vdsum=output->w0=output->w1=output->wdf=output->wevap=
    output->wscal=output->temp=output->sun=output->par=output->pet=
    output->daylength=output->swe=output->discharge=output->runoff=
    output->rh=output->interc=0.0;
} /* of 'initoutput_daily' */
