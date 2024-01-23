/**************************************************************************************/
/**                                                                                \n**/
/**        i  n  i  t  o  u  t  p  u  t  _  m  o  n  t  h  l  y  .  c              \n**/
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

void initoutput_monthly(Output *output /**< Output data */
                       )
{
  int l;

  output->mnpp=output->mgpp=output->mrh=output->mtransp=output->mtransp_b=
  output->mrunoff=output->mpet=output->mwateramount=
  output->mdischarge=output->mevap=output->mevap_b=output->minterc=output->minterc_b=output->mirrig=output->mwd_unsustainable=
  output->mevap_lake=output->mreturn_flow_b=output->mevap_res=output->munmet_demand=
  output->mprec_res=output->mnfire=output->mfireemission.co2=
  output->mfireemission.co=output->mfireemission.ch4=output->mfireemission.voc=
  output->mfireemission.tpm= output->mfireemission.nox=
  output->mburntarea=output->mlakevol=output->mlaketemp=
  output->mfirec=output->mfiredi=output->mtemp_image=output->mprec_image=
  output->msun_image=output->mwet_image=output->mres_storage=
  output->mres_demand=output->mtarget_release=output->mres_cap=
  output->mwd_local=output->mwd_neighb=output->mwd_res=output->mwd_return=
  output->mconv_loss_evap=output->mconv_loss_drain=output->mprec=output->mrain=output->msnowf=output->mmelt=output->msnowrunoff=output->mswe=output->mstor_return=0.0;
  output->mrunoff_lat=output->mrunoff_surf=output->mseepage=output->mrootmoist=0.0;
  output->mgcons_rf=output->mgcons_irr=output->mbcons_irr=output->mirrig_rw=0.0;
  output->mfapar=output->malbedo=0.0;
  output->mphen_tmin = output->mphen_tmax = output->mphen_light = output->mphen_water = output->mwscal = 0.0;

  for(l=0;l<NSOILLAYER;l++)
    output->mswc[l]=output->msoiltemp[l]=0;
} /* of 'initoutput_monthly' */
