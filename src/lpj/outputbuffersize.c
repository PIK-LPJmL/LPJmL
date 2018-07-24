/**************************************************************************************/
/**                                                                                \n**/
/**      o  u  t  p  u  t  b  u  f  f  e  r  s  i  z  e  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates memory size for output buffers.                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

long long outputfilesize(int npft,      /**< number of natural pfts */
                         int ncft,      /**< number of crop pfts */
                         const Config *config /**< LPJ configuration */
                        )               /** \return size of buffer in bytes */
{
  long long sum,size;
  int i;
  size=(long long)config->count*(config->lastyear-config->firstyear+1);
  sum=0;
  for(i=0;i<config->n_out;i++)
    if(config->outputvars[i].id>=D_LAI && config->outputvars[i].id<=D_PET)
      sum+=size*NDAYYEAR*sizeof(float);
    else
      switch(config->outputvars[i].id)
      {
        case FIREC: case FLUX_FIREWOOD: case MAXTHAW_DEPTH: case FIREF:
        case FLUX_ESTABC: case FLUX_ESTABN: case HARVESTC: case HARVESTN:
        case VEGC: case VEGN: case MG_VEGC: case AGB: case AGB_TREE: case ABURNTAREA:
        case SOILC: case SOILN: case MG_SOILC: case LITC: case LITN: case MG_LITC: case APREC:
        case INPUT_LAKE: case PROD_TURNOVER: case DEFOREST_EMIS:
        case SOILN_SLOW: case SOILNO3: case SOILNH4:
        case AFRAC_WD_UNSUST: case ACONV_LOSS_EVAP: case ACONV_LOSS_DRAIN: case SOILC_SLOW: case AWATERUSE_HIL:
        case NEGC_FLUXES: case NEGN_FLUXES:
          sum+=size*sizeof(float);
          break;
        case SOILC_LAYER: case SOILN_LAYER: case SOILNO3_LAYER: case SOILNH4_LAYER:
            sum+=size*LASTLAYER*sizeof(float);
            break;
        case ADISCHARGE:
          sum+=(long long)config->ngridcell*(config->lastyear-config->firstyear+1)*sizeof(float);
          break;
        case SDATE: case HDATE: case SDATE2: case HDATE2:
        case SYEAR: case SYEAR2:
          sum+=size*ncft*2*sizeof(short);
          break;
        case CFT_IRRIG_EVENTS:
          sum+=size*(ncft+NGRASS+NBIOMASSTYPE)*2*sizeof(short);
          break;
        case PFT_NUPTAKE: case PFT_NDEMAND:
        case PFT_VEGC: case PFT_VEGN: case PFT_CLEAF: case PFT_NLEAF:
        case PFT_CROOT: case PFT_NROOT: case PFT_CSAPW: case PFT_NSAPW:
        case PFT_CHAWO: case PFT_NHAWO:
        case PFT_NPP: case PFT_GCGP: case PFT_LAIMAX: case PFT_NLIMIT:
          sum+=sizeof(float)*size*((npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
          break;
        case GROWING_PERIOD: case GROWING_PERIOD2:
        case CFT_TEMP:case CFT_PREC:
        case CFT_SRAD: case CFT_ABOVEGBMC: case CFT_ABOVEGBMN:
        case CFT_TEMP2:case CFT_PREC2: 
        case CFT_SRAD2:case CFT_ABOVEGBMC2: case CFT_ABOVEGBMN2:
        case CFT_PET: case CFT_PET2:
          sum+=sizeof(float)*size*(ncft+NGRASS)*2;
          break;
        case PFT_HARVESTC: case PFT_HARVESTN: case PFT_RHARVESTC: case PFT_RHARVESTN: case CFT_CONSUMP_WATER_G:
        case CFT_CONSUMP_WATER_B: case CFTFRAC: case CFT_TRANSP: case CFT_TRANSP_B: 
        case CFT_EVAP: case CFT_EVAP_B: case CFT_NIR: case CFT_NIR2:
        case CFT_INTERC: case CFT_INTERC_B: case CFT_RETURN_FLOW_B: case LUC_IMAGE: 
        case CFT_FPAR: case CFT_AIRRIG: case CFT_CONV_LOSS_EVAP: case CFT_CONV_LOSS_DRAIN:
        case CFT_INTERC2: case PFT_HARVESTC2: case PFT_HARVESTN2: case PFT_RHARVESTC2: case PFT_RHARVESTN2: case CFT_TRANSP2: 
        case CFTFRAC2: case CFT_EVAP2: case CFT_AIRRIG2:
          sum+=sizeof(float)*size*(ncft+NGRASS+NBIOMASSTYPE)*2;
          break;
        case FPC:
          sum+=sizeof(float)*size*((npft-config->nbiomass)+1);
          break;
        case MNPP: case MRH: case MRUNOFF: case MDISCHARGE:
        case MTRANSP: case MTRANSP_B: case MEVAP: case MEVAP_B: case MINTERC: case MINTERC_B: case MPET:
        case MSWC1: case MSWC2: case MIRRIG: case MWD_UNSUST: case MUNMET_DEMAND:
        case MRETURN_FLOW_B: case MEVAP_LAKE: case MGPP: case MEVAP_RES:
        case MPREC_RES: case MFIREC: case MNFIRE: case MFIREDI:
        case MFIREEMISSION_CO2: case MBURNTAREA: case MSOILTEMP1: case MSOILTEMP2:
        case MSOILTEMP3: case MSOILTEMP4: case MSOILTEMP5: case MSOILTEMP6:
        case MTEMP_IMAGE: case MPREC_IMAGE: case MSUN_IMAGE: case MWET_IMAGE: 
        case MPREC: case MRAIN: case MSNOWF: case MMELT: case MSWE: case MSNOWRUNOFF:
        case MRUNOFF_SURF: case MRUNOFF_LAT: case MSEEPAGE: case MIRRIG_RW:
        case MSWC3: case MSWC4: case MSWC5: case MWATERAMOUNT:
        case MFAPAR: case MALBEDO:
        case MPHEN_TMIN: case MPHEN_TMAX: case MPHEN_LIGHT: case MPHEN_WATER: case MWSCAL:
        case MNUPTAKE: case MLEACHING: case MN2O_DENIT: case MN2O_NIT:
        case MN2_EMIS: case MBNF:
        case MRES_CAP: case MGCONS_RF: case MGCONS_IRR: case MBCONS_IRR:
        case MN_MINERALIZATION: case MN_IMMO: case MN_VOLATILIZATION:
        case MWD_LOCAL: case MWD_NEIGHB: case MWD_RES: case MWD_RETURN:
        case MCONV_LOSS_EVAP: case MCONV_LOSS_DRAIN: case MRES_DEMAND: case MTARGET_RELEASE: case MRES_STORAGE: case MSTOR_RETURN:
        case MROOTMOIST: case MLAKEVOL: case MLAKETEMP:
        case MFIREEMISSION_CO: case MFIREEMISSION_CH4: case MFIREEMISSION_VOC:
        case MFIREEMISSION_TPM: case MFIREEMISSION_NOX:
          sum+=sizeof(float)*NMONTH*size;
          break;
        case MSOILTEMP: case MSWC:
          sum+=sizeof(float)*NMONTH*size*NSOILLAYER;
          break;
      } /* of 'switch' */
  return sum;
} /* of 'outputbuffersize' */
