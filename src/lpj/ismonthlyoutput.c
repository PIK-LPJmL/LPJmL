/**************************************************************************************/
/**                                                                                \n**/
/**          i  s  m  o  n  t  h  l  y  o  u  t  p  u  t  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines whether output is monthly data                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool ismonthlyoutput(int index /**< Output index */
                    )          /** \return output is monthy output (TRUE/FALSE) */
{
  switch(index)
  {
    case MNPP: case MRH: case MRUNOFF: case MDISCHARGE:
    case MTRANSP: case MTRANSP_B: case MWATERAMOUNT: case MROOTMOIST:
    case MEVAP: case MEVAP_B: case MINTERC: case MINTERC_B: case MPET: case MSWC1: case MSWC2: case MIRRIG:
    case MRETURN_FLOW_B: case MEVAP_LAKE: case MGPP: case MEVAP_RES:
    case MPREC_RES: case MFIREC: case MNFIRE: case MFIREDI:
    case MFIREEMISSION_CO2: case MBURNTAREA: case MTEMP_IMAGE: case MPREC_IMAGE: case MSUN_IMAGE:
    case MWET_IMAGE: case MSOILTEMP1: case MSOILTEMP2: case MSOILTEMP3:
    case MSOILTEMP4: case MSOILTEMP5: case MSOILTEMP6: case MWD_UNSUST: case MUNMET_DEMAND:
    case MPREC: case MRAIN: case MSNOWF: case MMELT: case MSWE: case MSNOWRUNOFF:
    case MRUNOFF_SURF: case MRUNOFF_LAT: case MSEEPAGE: case MIRRIG_RW:
    case MFAPAR: case MALBEDO:
    case MPHEN_TMIN: case MPHEN_TMAX: case MPHEN_LIGHT: case MPHEN_WATER: case MWSCAL:
    case MSWC3: case MSWC4: case MSWC5: case MWD_LOCAL: case MWD_NEIGHB: case MWD_RES:
    case MWD_RETURN: case MCONV_LOSS_EVAP: case MCONV_LOSS_DRAIN: case MRES_STORAGE: case MRES_DEMAND: case MSTOR_RETURN: case MLAKEVOL: case MLAKETEMP:
    case MTARGET_RELEASE: case MRES_CAP: case MGCONS_RF: case MGCONS_IRR: case MBCONS_IRR:
    case MFIREEMISSION_CO: case MFIREEMISSION_CH4: case MFIREEMISSION_VOC:
    case MFIREEMISSION_TPM: case MFIREEMISSION_NOX: case MSOILTEMP: case MSWC:
      return TRUE;
    default:
      return FALSE;
  }
} /* of 'ismonthlyoutput' */
