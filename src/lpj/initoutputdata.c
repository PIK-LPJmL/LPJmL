/**************************************************************************************/
/**                                                                                \n**/
/**                i  n  i  t  o  u  t  p  u  t  d  a  t  a  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes output data to zero                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define isinit(index) (config->outnames[index].timestep==timestep)

#define setoutputvar(index,name) if(isinit(index))\
   output->name=0

#define setoutputarray(index,name,n) if(isinit(index))\
  {\
    for(i=0;i<n;i++)\
      output->name[i]=0;\
  }

#define setoutputarrayitem(index,name,item,n) if(isinit(index))\
  {\
    for(i=0;i<n;i++)\
      output->name[i].item=0;\
  }

void initoutputdata(Output *output,      /**< output data */
                    int timestep,        /**< time step (ANNUAL, MONTHLY, DAILY) */
                    int npft,            /**< number of natural PFTs */
                    int ncft,            /**< number of crop PFTs */
                    const Config *config /**< LPJ configuration */
                   )
{
  int i,index;
  setoutputvar(NPP,npp);
  setoutputvar(GPP,gpp);
  setoutputvar(RH,rh);
  setoutputvar(EVAP,evap);
  setoutputvar(INTERC,interc);
  setoutputvar(TRANSP,transp);
  setoutputvar(PET,pet);
  setoutputvar(RUNOFF,runoff);
  setoutputvar(LITFALLC,alittfall.carbon);
  setoutputvar(LITFALLN,alittfall.nitrogen);
  setoutputvar(FIREC,fire.carbon);
  setoutputvar(FIREN,fire.nitrogen);
  setoutputvar(BURNTAREA,burntarea);
  setoutputvar(FLUX_FIREWOOD,flux_firewood.carbon);
  setoutputvar(FLUX_FIREWOOD,flux_firewood.nitrogen);
  setoutputvar(FIREF,firef);
  setoutputvar(FLUX_ESTABC,flux_estab.carbon);
  setoutputvar(FLUX_ESTABN,flux_estab.nitrogen);
  setoutputvar(HARVESTC,flux_harvest.carbon);
  setoutputvar(HARVESTN,flux_harvest.nitrogen);
  setoutputvar(RHARVEST_BURNTC,flux_rharvest_burnt.carbon);
  setoutputvar(RHARVEST_BURNTN,flux_rharvest_burnt.nitrogen);
  setoutputvar(RHARVEST_BURNT_IN_FIELDC,flux_rharvest_burnt_in_field.carbon);
  setoutputvar(RHARVEST_BURNT_IN_FIELDN,flux_rharvest_burnt_in_field.nitrogen);
  setoutputvar(INPUT_LAKE,input_lake);
  setoutputvar(DEFOREST_EMIS,deforest_emissions.carbon);
  setoutputvar(DEFOREST_EMIS,deforest_emissions.nitrogen);
  setoutputvar(TRAD_BIOFUEL,trad_biofuel);
  setoutputvar(FBURN,fburn);
  setoutputvar(FTIMBER,ftimber);
  setoutputvar(TIMBER_HARVESTC,timber_harvest.carbon);
  setoutputvar(TIMBER_HARVESTC,timber_harvest.nitrogen);
#ifdef IMAGE
  setoutputvar(PRODUCT_POOL_FAST,product_pool_fast);
  setoutputvar(PRODUCT_POOL_SLOW,product_pool_slow);
  setoutputvar(PROD_TURNOVER,prod_turnover);
#else
  setoutputvar(PRODUCT_POOL_FAST,product_pool_fast.carbon);
  setoutputvar(PRODUCT_POOL_SLOW,product_pool_slow.carbon);
  setoutputvar(PRODUCT_POOL_FAST,product_pool_fast.nitrogen);
  setoutputvar(PRODUCT_POOL_SLOW,product_pool_slow.nitrogen);
  setoutputvar(PROD_TURNOVER,prod_turnover.carbon);
  setoutputvar(PROD_TURNOVER,prod_turnover.nitrogen);
#endif
  setoutputvar(WATERUSE_HIL,awateruse_hil);
  setoutputvar(NEGC_FLUXES,neg_fluxes.carbon);
  setoutputvar(NEGN_FLUXES,neg_fluxes.nitrogen);
  setoutputvar(MEAN_VEGC_MANGRASS,mean_vegc_mangrass);  
  setoutputvar(VEGC_AVG,vegc_avg);
  setoutputvar(RH_LITTER,mrh_litter);
  setoutputvar(DISCHARGE,mdischarge);
  setoutputvar(ADISCHARGE,adischarge);
  setoutputvar(WATERAMOUNT,mwateramount);
  setoutputvar(TRANSP_B,mtransp_b);
  setoutputvar(EVAP_B,mevap_b);
  setoutputvar(INTERC_B,minterc_b);
  setoutputarray(SWC,mswc,NSOILLAYER);
  setoutputvar(SWC1,mswc2[0]);
  setoutputvar(SWC2,mswc2[1]);
  setoutputvar(SWC3,mswc2[2]);
  setoutputvar(SWC4,mswc2[3]);
  setoutputvar(SWC5,mswc2[4]);
  setoutputvar(ROOTMOIST,mrootmoist);
  setoutputvar(IRRIG,irrig);
  setoutputvar(RETURN_FLOW_B,mreturn_flow_b);
  setoutputvar(EVAP_LAKE,mevap_lake);
  setoutputvar(EVAP_RES,mevap_res);
  setoutputvar(PREC_RES,mprec_res);
  setoutputvar(NFIRE,mnfire);
  setoutputvar(FIREDI,mfiredi);
  setoutputvar(FIREEMISSION_CO2,mfireemission.co2);
  setoutputvar(FIREEMISSION_CO,mfireemission.co);
  setoutputvar(FIREEMISSION_CH4,mfireemission.ch4);
  setoutputvar(FIREEMISSION_VOC,mfireemission.voc);
  setoutputvar(FIREEMISSION_TPM,mfireemission.tpm);
  setoutputvar(FIREEMISSION_NOX,mfireemission.nox);
  setoutputvar(TEMP_IMAGE,mtemp_image);
  setoutputvar(PREC_IMAGE,mprec_image);
  setoutputvar(SUN_IMAGE,msun_image);
  setoutputvar(WET_IMAGE,mwet_image);
  setoutputarray(SOILTEMP,msoiltemp,NSOILLAYER);
  setoutputvar(SOILTEMP1,msoiltemp2[0]);
  setoutputvar(SOILTEMP2,msoiltemp2[1]);
  setoutputvar(SOILTEMP3,msoiltemp2[2]);
  setoutputvar(SOILTEMP4,msoiltemp2[3]);
  setoutputvar(SOILTEMP5,msoiltemp2[4]);
  setoutputvar(SOILTEMP6,msoiltemp2[5]);
  setoutputvar(NUPTAKE,mn_uptake);
  setoutputvar(LEACHING,mn_leaching);
  setoutputvar(N2O_DENIT,mn2o_denit);
  setoutputvar(N2O_NIT,mn2o_nit);
  setoutputvar(N2_EMIS,mn2_emissions);
  setoutputvar(BNF,mbnf);
  setoutputvar(N_MINERALIZATION,mn_mineralization);
  setoutputvar(N_VOLATILIZATION,mn_volatilization);
  setoutputvar(N_IMMO,mn_immo);
  setoutputvar(RES_STORAGE,mres_storage);
  setoutputvar(RES_DEMAND,mres_demand);
  setoutputvar(TARGET_RELEASE,mtarget_release);
  setoutputvar(RES_CAP,mres_cap);
  setoutputvar(WD_UNSUST,mwd_unsustainable);
  setoutputvar(UNMET_DEMAND,munmet_demand);
  setoutputvar(WD_LOCAL,mwd_local);
  setoutputvar(WD_NEIGHB,mwd_neighb);
  setoutputvar(WD_RES,mwd_res);
  setoutputvar(WD_RETURN,mwd_return);
  setoutputvar(CONV_LOSS_EVAP,mconv_loss_evap);
  setoutputvar(CONV_LOSS_DRAIN,mconv_loss_drain);
  setoutputvar(STOR_RETURN,mstor_return);
  setoutputvar(PREC,prec);
  setoutputvar(RAIN,mrain);
  setoutputvar(SNOWF,msnowf);
  setoutputvar(MELT,mmelt);
  setoutputvar(SWE,mswe);
  setoutputvar(SNOWRUNOFF,msnowrunoff);
  setoutputvar(RUNOFF_SURF,mrunoff_surf);
  setoutputvar(RUNOFF_LAT,mrunoff_lat);
  setoutputvar(SEEPAGE,mseepage);
  setoutputvar(FAPAR,fapar);
  setoutputvar(ALBEDO,malbedo);
  setoutputvar(PHEN_TMIN,mphen_tmin);
  setoutputvar(PHEN_TMAX,mphen_tmax);
  setoutputvar(PHEN_LIGHT,mphen_light);
  setoutputvar(PHEN_WATER,mphen_water);
  setoutputvar(WSCAL,mwscal);
  setoutputvar(GCONS_RF,mgcons_rf);
  setoutputvar(GCONS_IRR,mgcons_irr);
  setoutputvar(BCONS_IRR,mbcons_irr);
  setoutputvar(IRRIG_RW,mirrig_rw);
  setoutputvar(LAKEVOL,mlakevol);
  setoutputvar(LAKETEMP,mlaketemp);
  setoutputarray(SDATE,sdate,2*ncft);
  setoutputarray(HDATE,hdate,2*ncft);
  setoutputarray(PFT_NPP,pft_npp,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  setoutputarray(PFT_NUPTAKE,pft_nuptake,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  setoutputarray(PFT_NDEMAND,pft_ndemand,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  setoutputarray(PFT_GCGP,pft_gcgp,npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2);
  setoutputarray(PFT_GCGP,gcgp_count,npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2);
  setoutputarrayitem(PFT_HARVESTC,pft_harvest,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_RHARVESTC,pft_harvest,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_CONSUMP_WATER_G,cft_consump_water_g,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_CONSUMP_WATER_B,cft_consump_water_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_CLEAF,cft_leaf,carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_NLEAF,cft_leaf,nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_CROOT,cft_root,carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_NROOT,cft_root,nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_VEGC,cft_veg,carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_VEGN,cft_veg,nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(PFT_NLIMIT,cft_nlimit,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(PFT_LAIMAX,cft_laimax,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(GROWING_PERIOD,growing_period,2*(ncft+NGRASS));
  setoutputarray(FPC,fpc,npft-config->nbiomass+1);
  setoutputarray(FPC_BFT,fpc_bft,((config->nbiomass+config->ngrass*2)*2));
  setoutputarray(NV_LAI,nv_lai,npft-config->nbiomass);
  setoutputarray(CFT_PET,cft_pet,2*(ncft+NGRASS));
  setoutputarray(CFT_TRANSP,cft_transp,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_TRANSP_B,cft_transp_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_EVAP,cft_evap,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_EVAP_B,cft_evap_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_INTERC,cft_interc,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_INTERC_B,cft_interc_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_RETURN_FLOW_B,cft_return_flow_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_NIR,cft_nir,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_TEMP,cft_temp,2*(ncft+NGRASS));
  setoutputarray(CFT_PREC,cft_prec,2*(ncft+NGRASS));
  setoutputarray(CFT_SRAD,cft_srad,2*(ncft+NGRASS));
  setoutputarray(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFTFRAC,cftfrac,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_AIRRIG,cft_airrig,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_FPAR,cft_fpar,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(LUC_IMAGE,cft_luc_image,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS));
  setoutputarrayitem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS));
#ifdef DOUBLE_HARVEST
  setoutputarrayitem(PFT_HARVESTC2,pft_harvest2,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_HARVESTN2,pft_harvest2,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_RHARVESTC2,pft_harvest2,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarrayitem(PFT_RHARVESTN2,pft_harvest2,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(GROWING_PERIOD2,growing_period2,2*(ncft+NGRASS));
  setoutputarray(CFT_PET2,cft_pet2,2*(ncft+NGRASS));
  setoutputarray(CFT_TRANSP2,cft_transp2,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_EVAP2,cft_evap2,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_INTERC2,cft_interc2,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_NIR2,cft_nir2,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_TEMP2,cft_temp2,2*(ncft+NGRASS));
  setoutputarray(CFT_PREC2,cft_prec2,2*(ncft+NGRASS));
  setoutputarray(CFT_SRAD2,cft_srad2,2*(ncft+NGRASS));
  setoutputarray(CFTFRAC2,cftfrac2,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(CFT_AIRRIG2,cft_airrig2,2*(ncft+NGRASS+NBIOMASSTYPE));
  setoutputarray(SDATE2,sdate2,2*ncft);
  setoutputarray(HDATE2,hdate2,2*ncft);
  setoutputarray(SYEAR,syear,2*ncft);
  setoutputarray(SYEAR2,syear2,2*ncft);
  setoutputarrayitem(CFT_ABOVEGBMC2,cft_aboveground_biomass2,carbon,2*(ncft+NGRASS));
  setoutputarrayitem(CFT_ABOVEGBMN2,cft_aboveground_biomass2,nitrogen,2*(ncft+NGRASS));
#endif
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
  {
    if(isinit(i))
      ((Real *)(&output->daily))[index]=0;
    index++;
  }
} /* of 'initoutputdata' */
