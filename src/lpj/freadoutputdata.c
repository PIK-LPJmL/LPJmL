/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  a  d  o  u  t  p  u  t  d  a  t  a  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads output data from restart file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define isread(index) (config->outnames[index].timestep>1)

#define readoutputvarreal(index,name) if(isread(index))\
   freadreal(&output->name,1,swap,file)

#define readoutputarrayreal(index,name,n) if(isread(index))\
   freadreal(output->name,n,swap,file)

#define readoutputarrayint(index,name,n) if(isread(index))\
   freadint(output->name,n,swap,file)

#define readoutputvarint(index,name,n) if(isread(index))\
   freadint(&output->name,n,swap,file)

#define readoutputarrayitem(index,name,item,n) if(isread(index))\
  {\
    for(i=0;i<n;i++)\
     freadreal(&output->name[i].item,1,swap,file);\
  }

Bool freadoutputdata(FILE *file,          /**< pointer to restart file */
                     Output *output,      /**< output data */
                     int npft,            /**< number of natural PFTs */
                     int ncft,            /**< number of crop PFTs */
                     Bool swap,           /**< byte order has to be changed? */
                     const Config *config /**< LPJ configuration */
                    )
{
  int i,index;
  readoutputvarreal(DAYLENGTH,daylength);
  readoutputvarreal(TEMP,temp);
  readoutputvarreal(SUN,sun);
  readoutputvarreal(VEGC,veg.carbon);
  readoutputvarreal(VEGN,veg.nitrogen);
  readoutputvarreal(SOILC,soil.carbon);
  readoutputvarreal(SOILN,soil.nitrogen);
  readoutputvarreal(MG_VEGC,mg_vegc);
  readoutputvarreal(MG_SOILC,mg_soilc);
  readoutputvarreal(MG_LITC,mg_litc);
  readoutputvarreal(SOILC_SLOW,soil_slow.carbon);
  readoutputvarreal(SOILN_SLOW,soil_slow.nitrogen);
  readoutputvarreal(LITC,litter.carbon);
  readoutputvarreal(LITN,litter.nitrogen);
  readoutputvarreal(MEANVEGCMANGRASS,mean_vegc_mangrass);
  readoutputvarreal(BNF_AGR,abnf_agr);
  readoutputvarreal(NFERT_AGR,anfert_agr);
  readoutputvarreal(NMANURE_AGR,anmanure_agr);
  readoutputvarreal(NDEPO_AGR,andepo_agr);
  readoutputvarreal(NMINERALIZATION_AGR,anmineralization_agr);
  readoutputvarreal(NIMMOBILIZATION_AGR,animmobilization_agr);
  readoutputvarreal(NUPTAKE_AGR,anuptake_agr);
  readoutputvarreal(NLEACHING_AGR,anleaching_agr);
  readoutputvarreal(N2O_DENIT_AGR,an2o_denit_agr);
  readoutputvarreal(N2O_NIT_AGR,an2o_nit_agr);
  readoutputvarreal(NH3_AGR,anh3_agr);
  readoutputvarreal(N2_AGR,an2_agr);
  readoutputvarreal(LITFALLN_AGR,alitfalln_agr);
  readoutputvarreal(HARVESTN_AGR,aharvestn_agr);
  readoutputvarreal(SEEDN_AGR,aseedn_agr);
  readoutputvarreal(DELTA_NORG_SOIL_AGR,adelta_norg_soil_agr);
  readoutputvarreal(DELTA_NMIN_SOIL_AGR,adelta_nmin_soil_agr);
  readoutputvarreal(DELTA_NVEG_SOIL_AGR,adelta_nveg_soil_agr);
  readoutputvarreal(CELLFRAC_AGR,cellfrac_agr);
  readoutputarrayitem(SOILC_LAYER,soil_layer,carbon,LASTLAYER);
  readoutputarrayitem(SOILN_LAYER,soil_layer,nitrogen,LASTLAYER);
  readoutputarrayreal(SOILNO3_LAYER,soilno3_layer,LASTLAYER);
  readoutputarrayreal(SOILNH4_LAYER,soilnh4_layer,LASTLAYER);
  readoutputarrayreal(PFT_LAIMAX,pft_laimax,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_NLEAF,pft_leaf,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_CLEAF,pft_leaf,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_NROOT,pft_root,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_CROOT,pft_root,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_NSAPW,pft_sapw,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_CSAPW,pft_sapw,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_NHAWO,pft_hawo,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_CHAWO,pft_hawo,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_VEGC,pft_veg,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_VEGN,pft_veg,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(PFT_NLIMIT,pft_nlimit,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputvarreal(MAXTHAW_DEPTH,maxthaw_depth);
  readoutputvarreal(SOILNO3,soilno3);
  readoutputvarreal(SOILNH4,soilnh4);
  readoutputvarreal(AGB,agb);
  readoutputvarreal(AGB_TREE,agb_tree);
  readoutputvarreal(NPP,npp);
  readoutputvarreal(NPP_AGR,npp_agr);
  readoutputvarreal(GPP,gpp);
  readoutputvarreal(RH,rh);
  readoutputvarreal(RH_AGR,rh_agr);
  readoutputvarreal(EVAP,evap);
  readoutputvarreal(INTERC,interc);
  readoutputvarreal(TRANSP,transp);
  readoutputvarreal(PET,pet);
  readoutputvarreal(RUNOFF,runoff);
  readoutputvarreal(LITFALLC,alittfall.carbon);
  readoutputvarreal(LITFALLN,alittfall.nitrogen);
  readoutputvarreal(FIREC,fire.carbon);
  readoutputvarreal(FIREN,fire.nitrogen);
  readoutputvarreal(BURNTAREA,burntarea);
  readoutputvarreal(FLUX_FIREWOOD,flux_firewood.carbon);
  readoutputvarreal(FLUX_FIREWOOD,flux_firewood.nitrogen);
  readoutputvarreal(FIREF,firef);
  readoutputvarreal(FLUX_ESTABC,flux_estab.carbon);
  readoutputvarreal(FLUX_ESTABN,flux_estab.nitrogen);
  readoutputvarreal(HARVESTC,flux_harvest.carbon);
  readoutputvarreal(HARVESTN,flux_harvest.nitrogen);
  readoutputvarreal(RHARVEST_BURNTC,flux_rharvest_burnt.carbon);
  readoutputvarreal(RHARVEST_BURNTN,flux_rharvest_burnt.nitrogen);
  readoutputvarreal(RHARVEST_BURNT_IN_FIELDC,flux_rharvest_burnt_in_field.carbon);
  readoutputvarreal(RHARVEST_BURNT_IN_FIELDN,flux_rharvest_burnt_in_field.nitrogen);
  readoutputvarreal(INPUT_LAKE,input_lake);
  readoutputvarreal(DEFOREST_EMIS,deforest_emissions.carbon);
  readoutputvarreal(DEFOREST_EMIS,deforest_emissions.nitrogen);
  readoutputvarreal(TRAD_BIOFUEL,trad_biofuel);
  readoutputvarreal(FBURN,fburn);
  readoutputvarreal(FTIMBER,ftimber);
  readoutputvarreal(TIMBER_HARVESTC,timber_harvest.carbon);
  readoutputvarreal(TIMBER_HARVESTC,timber_harvest.nitrogen);
#if defined IMAGE || defined INCLUDEWP
  readoutputvarreal(WFT_VEGC,wft_vegc,config->nwft);
#endif
#ifdef IMAGE
  readoutputvarreal(WD_GW,mwd_gw);
  readoutputvarreal(WD_AQ,mwd_aq);
#endif
  readoutputvarreal(PRODUCT_POOL_FAST,product_pool.fast);
  readoutputvarreal(PRODUCT_POOL_SLOW,product_pool.slow);
  readoutputvarreal(PROD_TURNOVER,prod_turnover);
  readoutputvarreal(WATERUSE_HIL,awateruse_hil);
  readoutputvarreal(WATERUSECONS,waterusecons);
  readoutputvarreal(WATERUSEDEM,waterusedem);
  readoutputvarreal(NEGC_FLUXES,neg_fluxes.carbon);
  readoutputvarreal(NEGN_FLUXES,neg_fluxes.nitrogen);
  readoutputvarreal(MEAN_VEGC_MANGRASS,mean_vegc_mangrass);  
  readoutputvarreal(VEGC_AVG,vegc_avg);
  readoutputvarreal(RH_LITTER,rh_litter);
  readoutputvarreal(DISCHARGE,mdischarge);
  readoutputvarreal(ADISCHARGE,adischarge);
  readoutputvarreal(WATERAMOUNT,mwateramount);
  readoutputvarreal(TRANSP_B,mtransp_b);
  readoutputvarreal(EVAP_B,mevap_b);
  readoutputvarreal(INTERC_B,minterc_b);
  readoutputarrayreal(SWC,mswc,NSOILLAYER);
  readoutputvarreal(SWC1,mswc2[0]);
  readoutputvarreal(SWC2,mswc2[1]);
  readoutputvarreal(SWC3,mswc2[2]);
  readoutputvarreal(SWC4,mswc2[3]);
  readoutputvarreal(SWC5,mswc2[4]);
  readoutputvarreal(ROOTMOIST,mrootmoist);
  readoutputvarreal(IRRIG,irrig);
  readoutputvarreal(RETURN_FLOW_B,mreturn_flow_b);
  readoutputvarreal(EVAP_LAKE,mevap_lake);
  readoutputvarreal(EVAP_RES,mevap_res);
  readoutputvarreal(PREC_RES,mprec_res);
  readoutputvarreal(NFIRE,mnfire);
  readoutputvarreal(FIREDI,mfiredi);
  readoutputvarreal(FIREEMISSION_CO2,mfireemission.co2);
  readoutputvarreal(FIREEMISSION_CO,mfireemission.co);
  readoutputvarreal(FIREEMISSION_CH4,mfireemission.ch4);
  readoutputvarreal(FIREEMISSION_VOC,mfireemission.voc);
  readoutputvarreal(FIREEMISSION_TPM,mfireemission.tpm);
  readoutputvarreal(FIREEMISSION_NOX,mfireemission.nox);
  readoutputvarreal(TEMP_IMAGE,mtemp_image);
  readoutputvarreal(PREC_IMAGE,mprec_image);
  readoutputvarreal(SUN_IMAGE,msun_image);
  readoutputvarreal(WET_IMAGE,mwet_image);
  readoutputarrayreal(SOILTEMP,msoiltemp,NSOILLAYER);
  readoutputvarreal(SOILTEMP1,msoiltemp2[0]);
  readoutputvarreal(SOILTEMP2,msoiltemp2[1]);
  readoutputvarreal(SOILTEMP3,msoiltemp2[2]);
  readoutputvarreal(SOILTEMP4,msoiltemp2[3]);
  readoutputvarreal(SOILTEMP5,msoiltemp2[4]);
  readoutputvarreal(SOILTEMP6,msoiltemp2[5]);
  readoutputvarreal(NUPTAKE,mn_uptake);
  readoutputvarreal(LEACHING,mn_leaching);
  readoutputvarreal(N2O_DENIT,mn2o_denit);
  readoutputvarreal(N2O_NIT,mn2o_nit);
  readoutputvarreal(N2_EMIS,mn2_emissions);
  readoutputvarreal(BNF,mbnf);
  readoutputvarreal(N_MINERALIZATION,mn_mineralization);
  readoutputvarreal(N_VOLATILIZATION,mn_volatilization);
  readoutputvarreal(N_IMMO,mn_immo);
  readoutputvarreal(RES_STORAGE,mres_storage);
  readoutputvarreal(RES_DEMAND,mres_demand);
  readoutputvarreal(TARGET_RELEASE,mtarget_release);
  readoutputvarreal(RES_CAP,mres_cap);
  readoutputvarreal(WD_UNSUST,mwd_unsustainable);
  readoutputvarreal(UNMET_DEMAND,munmet_demand);
  readoutputvarreal(WD_LOCAL,mwd_local);
  readoutputvarreal(WD_NEIGHB,mwd_neighb);
  readoutputvarreal(WD_RES,mwd_res);
  readoutputvarreal(CONV_LOSS_EVAP,mconv_loss_evap);
  readoutputvarreal(CONV_LOSS_DRAIN,mconv_loss_drain);
  readoutputvarreal(STOR_RETURN,mstor_return);
  readoutputvarreal(PREC,prec);
  readoutputvarreal(RAIN,mrain);
  readoutputvarreal(SNOWF,msnowf);
  readoutputvarreal(MELT,mmelt);
  readoutputvarreal(SWE,mswe);
  readoutputvarreal(SNOWRUNOFF,msnowrunoff);
  readoutputvarreal(RUNOFF_SURF,mrunoff_surf);
  readoutputvarreal(RUNOFF_LAT,mrunoff_lat);
  readoutputvarreal(SEEPAGE,mseepage);
  readoutputvarreal(FAPAR,fapar);
  readoutputvarreal(ALBEDO,malbedo);
  readoutputvarreal(PHEN_TMIN,mphen_tmin);
  readoutputvarreal(PHEN_TMAX,mphen_tmax);
  readoutputvarreal(PHEN_LIGHT,mphen_light);
  readoutputvarreal(PHEN_WATER,mphen_water);
  readoutputvarreal(WSCAL,mwscal);
  readoutputvarreal(GCONS_RF,mgcons_rf);
  readoutputvarreal(GCONS_IRR,mgcons_irr);
  readoutputvarreal(BCONS_IRR,mbcons_irr);
  readoutputvarreal(IRRIG_RW,mirrig_rw);
  readoutputvarreal(LAKEVOL,mlakevol);
  readoutputvarreal(LAKETEMP,mlaketemp);
  readoutputarrayint(SDATE,sdate,2*ncft);
  readoutputarrayint(HDATE,hdate,2*ncft);
  readoutputarrayreal(CFT_SWC,cft_mswc,2*ncft);
  readoutputarrayint(CFT_SWC,nday_month,2*ncft);
  readoutputarrayreal(PFT_NPP,pft_npp,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  readoutputarrayreal(PFT_NUPTAKE,pft_nuptake,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  readoutputarrayreal(PFT_NDEMAND,pft_ndemand,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  readoutputarrayreal(HUSUM,husum,2*ncft);
  readoutputarrayreal(CFT_RUNOFF,cft_runoff,2*ncft);
  readoutputarrayreal(CFT_N2O_DENIT,cft_n2o_denit,2*ncft);
  readoutputarrayreal(CFT_N2O_NIT,cft_n2o_nit,2*ncft);
  readoutputarrayreal(CFT_N2_EMIS,cft_n2_emis,2*ncft);
  readoutputarrayreal(CFT_LEACHING,cft_leaching,2*ncft);
  readoutputarrayreal(CFT_C_EMIS,cft_c_emis,2*ncft);
  readoutputarrayreal(PFT_GCGP,pft_gcgp,npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  readoutputarrayreal(PFT_GCGP,gcgp_count,npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  readoutputarrayitem(PFT_HARVESTC,pft_harvest,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_RHARVESTC,pft_harvest,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_CONSUMP_WATER_G,cft_consump_water_g,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_CONSUMP_WATER_B,cft_consump_water_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(GROWING_PERIOD,growing_period,2*(ncft+NGRASS));
  readoutputarrayreal(FPC,fpc,npft-config->nbiomass-config->nwft+1);
  readoutputarrayreal(FPC_BFT,fpc_bft,((config->nbiomass+config->ngrass*2)*2));
  readoutputarrayreal(NV_LAI,nv_lai,npft-config->nbiomass-config->nwft);
  readoutputarrayreal(CFT_PET,cft_pet,2*(ncft+NGRASS));
  readoutputarrayreal(CFT_TRANSP,cft_transp,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_TRANSP_B,cft_transp_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_EVAP,cft_evap,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_EVAP_B,cft_evap_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_INTERC,cft_interc,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_INTERC_B,cft_interc_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_RETURN_FLOW_B,cft_return_flow_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayint(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_NIR,cft_nir,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_TEMP,cft_temp,2*(ncft+NGRASS));
  readoutputarrayreal(CFT_PREC,cft_prec,2*(ncft+NGRASS));
  readoutputarrayreal(CFT_SRAD,cft_srad,2*(ncft+NGRASS));
  readoutputarrayreal(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFTFRAC,cftfrac,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_AIRRIG,cft_airrig,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(CFT_FPAR,cft_fpar,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayreal(LUC_IMAGE,cft_luc_image,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  readoutputarrayitem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS));
  readoutputarrayitem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS));
  if(config->double_harvest)
  {
    readoutputarrayitem(PFT_HARVESTC2,dh->pft_harvest2,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayitem(PFT_HARVESTN2,dh->pft_harvest2,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayitem(PFT_RHARVESTC2,dh->pft_harvest2,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayitem(PFT_RHARVESTN2,dh->pft_harvest2,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayreal(GROWING_PERIOD2,dh->growing_period2,2*(ncft+NGRASS));
    readoutputarrayreal(CFT_PET2,dh->cft_pet2,2*(ncft+NGRASS));
    readoutputarrayreal(CFT_TRANSP2,dh->cft_transp2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayreal(CFT_EVAP2,dh->cft_evap2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayreal(CFT_INTERC2,dh->cft_interc2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayreal(CFT_NIR2,dh->cft_nir2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayreal(CFT_TEMP2,dh->cft_temp2,2*(ncft+NGRASS));
    readoutputarrayreal(CFT_PREC2,dh->cft_prec2,2*(ncft+NGRASS));
    readoutputarrayreal(CFT_SRAD2,dh->cft_srad2,2*(ncft+NGRASS));
    readoutputarrayreal(CFTFRAC2,dh->cftfrac2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayreal(CFT_AIRRIG2,dh->cft_airrig2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
    readoutputarrayint(SDATE2,dh->sdate2,2*ncft);
    readoutputarrayint(HDATE2,dh->hdate2,2*ncft);
    readoutputarrayint(SYEAR,dh->syear,2*ncft);
    readoutputarrayint(SYEAR2,dh->syear2,2*ncft);
    readoutputarrayitem(CFT_ABOVEGBMC2,dh->cft_aboveground_biomass2,carbon,2*(ncft+NGRASS));
    readoutputarrayitem(CFT_ABOVEGBMN2,dh->cft_aboveground_biomass2,nitrogen,2*(ncft+NGRASS));
    readoutputarrayreal(CFT_RUNOFF2,dh->cft_runoff2,2*ncft);
    readoutputarrayreal(CFT_N2O_DENIT2,dh->cft_n2o_denit2,2*ncft);
    readoutputarrayreal(CFT_N2O_NIT2,dh->cft_n2o_nit2,2*ncft);
    readoutputarrayreal(CFT_N2_EMIS2,dh->cft_n2_emis2,2*ncft);
    readoutputarrayreal(CFT_LEACHING2,dh->cft_leaching2,2*ncft);
    readoutputarrayreal(CFT_C_EMIS2,dh->cft_c_emis2,2*ncft);
    readoutputarrayreal(PFT_NUPTAKE2,dh->pft_nuptake2,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  }
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
  {
    if(isread(i))
     freadreal(((Real *)(&output->daily))+index,1,swap,file);
    index++;
  }
  return FALSE;
} /* of 'freadoutputdata' */
