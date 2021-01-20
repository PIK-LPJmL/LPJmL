/**************************************************************************************/
/**                                                                                \n**/
/**                f  w  r  i  t  e  o  u  t  p  u  t  d  a  t  a  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes output data to restart file                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define iswrite(index) (config->outnames[index].timestep>1)

#define writeoutputvar(index,name,type) if(iswrite(index))\
   fwrite(&output->name,sizeof(type),1,file)

#define writeoutputarray(index,name,n,type) if(iswrite(index))\
   fwrite(output->name,sizeof(type),n,file)

#define writeoutputarrayitem(index,name,item,n,type) if(iswrite(index))\
  {\
    for(i=0;i<n;i++)\
     fwrite(&output->name[i].item,sizeof(type),1,file);\
  }

void fwriteoutputdata(FILE *file,           /**< pointer to restart file */
                      const Output *output, /**< output data */
                      int npft,             /**< number of natural PFTs */
                      int ncft,             /**< number of crop PFTs */
                      const Config *config  /**< LPJ configuration */
                     )
{
  int i,index;
  writeoutputvar(DAYLENGTH,daylength,Real);
  writeoutputvar(TEMP,temp,Real);
  writeoutputvar(SUN,sun,Real);
  writeoutputvar(VEGC,veg.carbon,Real);
  writeoutputvar(VEGN,veg.nitrogen,Real);
  writeoutputvar(SOILC,soil.carbon,Real);
  writeoutputvar(SOILN,soil.nitrogen,Real);
  writeoutputvar(MG_VEGC,mg_vegc,Real);
  writeoutputvar(MG_SOILC,mg_soilc,Real);
  writeoutputvar(MG_LITC,mg_litc,Real);
  writeoutputvar(SOILC_SLOW,soil_slow.carbon,Real);
  writeoutputvar(SOILN_SLOW,soil_slow.nitrogen,Real);
  writeoutputvar(LITC,litter.carbon,Real);
  writeoutputvar(LITN,litter.nitrogen,Real);
  writeoutputvar(MEANVEGCMANGRASS,mean_vegc_mangrass,Real);
  writeoutputvar(BNF_AGR,abnf_agr,Real);
  writeoutputvar(NFERT_AGR,anfert_agr,Real);
  writeoutputvar(NMANURE_AGR,anmanure_agr,Real);
  writeoutputvar(NDEPO_AGR,andepo_agr,Real);
  writeoutputvar(NMINERALIZATION_AGR,anmineralization_agr,Real);
  writeoutputvar(NIMMOBILIZATION_AGR,animmobilization_agr,Real);
  writeoutputvar(NUPTAKE_AGR,anuptake_agr,Real);
  writeoutputvar(NLEACHING_AGR,anleaching_agr,Real);
  writeoutputvar(N2O_DENIT_AGR,an2o_denit_agr,Real);
  writeoutputvar(N2O_NIT_AGR,an2o_nit_agr,Real);
  writeoutputvar(NH3_AGR,anh3_agr,Real);
  writeoutputvar(N2_AGR,an2_agr,Real);
  writeoutputvar(LITFALLN_AGR,alitfalln_agr,Real);
  writeoutputvar(HARVESTN_AGR,aharvestn_agr,Real);
  writeoutputvar(SEEDN_AGR,aseedn_agr,Real);
  writeoutputvar(DELTA_NORG_SOIL_AGR,adelta_norg_soil_agr,Real);
  writeoutputvar(DELTA_NMIN_SOIL_AGR,adelta_nmin_soil_agr,Real);
  writeoutputvar(DELTA_NVEG_SOIL_AGR,adelta_nveg_soil_agr,Real);
  writeoutputvar(CELLFRAC_AGR,cellfrac_agr,Real);
  writeoutputarrayitem(SOILC_LAYER,soil_layer,carbon,LASTLAYER,Real);
  writeoutputarrayitem(SOILN_LAYER,soil_layer,nitrogen,LASTLAYER,Real);
  writeoutputarray(SOILNO3_LAYER,soilno3_layer,LASTLAYER,Real);
  writeoutputarray(SOILNH4_LAYER,soilnh4_layer,LASTLAYER,Real);
  writeoutputarray(PFT_LAIMAX,pft_laimax,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_NLEAF,pft_leaf,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_CLEAF,pft_leaf,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_NROOT,pft_root,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_CROOT,pft_root,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_NSAPW,pft_sapw,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_CSAPW,pft_sapw,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_NHAWO,pft_hawo,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_CHAWO,pft_hawo,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_VEGC,pft_veg,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_VEGN,pft_veg,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(PFT_NLIMIT,pft_nlimit,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputvar(MAXTHAW_DEPTH,maxthaw_depth,Real);
  writeoutputvar(SOILNO3,soilno3,Real);
  writeoutputvar(SOILNH4,soilnh4,Real);
  writeoutputvar(AGB,agb,Real);
  writeoutputvar(AGB_TREE,agb_tree,Real);
  writeoutputvar(NPP,npp,Real);
  writeoutputvar(NPP_AGR,npp_agr,Real);
  writeoutputvar(GPP,gpp,Real);
  writeoutputvar(RH,rh,Real);
  writeoutputvar(RH_AGR,rh_agr,Real);
  writeoutputvar(EVAP,evap,Real);
  writeoutputvar(INTERC,interc,Real);
  writeoutputvar(TRANSP,transp,Real);
  writeoutputvar(PET,pet,Real);
  writeoutputvar(RUNOFF,runoff,Real);
  writeoutputvar(LITFALLC,alittfall.carbon,Real);
  writeoutputvar(LITFALLN,alittfall.nitrogen,Real);
  writeoutputvar(FIREC,fire.carbon,Real);
  writeoutputvar(FIREN,fire.nitrogen,Real);
  writeoutputvar(BURNTAREA,burntarea,Real);
  writeoutputvar(FLUX_FIREWOOD,flux_firewood.carbon,Real);
  writeoutputvar(FLUX_FIREWOOD,flux_firewood.nitrogen,Real);
  writeoutputvar(FIREF,firef,Real);
  writeoutputvar(FLUX_ESTABC,flux_estab.carbon,Real);
  writeoutputvar(FLUX_ESTABN,flux_estab.nitrogen,Real);
  writeoutputvar(HARVESTC,flux_harvest.carbon,Real);
  writeoutputvar(HARVESTN,flux_harvest.nitrogen,Real);
  writeoutputvar(RHARVEST_BURNTC,flux_rharvest_burnt.carbon,Real);
  writeoutputvar(RHARVEST_BURNTN,flux_rharvest_burnt.nitrogen,Real);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDC,flux_rharvest_burnt_in_field.carbon,Real);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDN,flux_rharvest_burnt_in_field.nitrogen,Real);
  writeoutputvar(INPUT_LAKE,input_lake,Real);
  writeoutputvar(DEFOREST_EMIS,deforest_emissions.carbon,Real);
  writeoutputvar(DEFOREST_EMIS,deforest_emissions.nitrogen,Real);
  writeoutputvar(TRAD_BIOFUEL,trad_biofuel,Real);
  writeoutputvar(FBURN,fburn,Real);
  writeoutputvar(FTIMBER,ftimber,Real);
  writeoutputvar(TIMBER_HARVESTC,timber_harvest.carbon,Real);
  writeoutputvar(TIMBER_HARVESTC,timber_harvest.nitrogen,Real);
#if defined IMAGE || defined INCLUDEWP
  writeoutputarray(WFT_VEGC,wft_vegc,config->nwft,Real);
#endif
#ifdef IMAGE
  writeoutputvar(WD_GW,mwd_gw,Real);
  writeoutputvar(WD_AQ,mwd_aq,Real);
#endif
  writeoutputvar(PRODUCT_POOL_FAST,product_pool.fast,Real);
  writeoutputvar(PRODUCT_POOL_SLOW,product_pool.slow,Real);
  writeoutputvar(PROD_TURNOVER,prod_turnover,Real);
  writeoutputvar(WATERUSE_HIL,awateruse_hil,Real);
  writeoutputvar(WATERUSECONS,waterusecons,Real);
  writeoutputvar(WATERUSEDEM,waterusedem,Real);
  writeoutputvar(NEGC_FLUXES,neg_fluxes.carbon,Real);
  writeoutputvar(NEGN_FLUXES,neg_fluxes.nitrogen,Real);
  writeoutputvar(MEAN_VEGC_MANGRASS,mean_vegc_mangrass,Real);  
  writeoutputvar(VEGC_AVG,vegc_avg,Real);
  writeoutputvar(RH_LITTER,rh_litter,Real);
  writeoutputvar(DISCHARGE,mdischarge,Real);
  writeoutputvar(ADISCHARGE,adischarge,Real);
  writeoutputvar(WATERAMOUNT,mwateramount,Real);
  writeoutputvar(TRANSP_B,mtransp_b,Real);
  writeoutputvar(EVAP_B,mevap_b,Real);
  writeoutputvar(INTERC_B,minterc_b,Real);
  writeoutputarray(SWC,mswc,NSOILLAYER,Real);
  writeoutputvar(SWC1,mswc2[0],Real);
  writeoutputvar(SWC2,mswc2[1],Real);
  writeoutputvar(SWC3,mswc2[2],Real);
  writeoutputvar(SWC4,mswc2[3],Real);
  writeoutputvar(SWC5,mswc2[4],Real);
  writeoutputvar(ROOTMOIST,mrootmoist,Real);
  writeoutputvar(IRRIG,irrig,Real);
  writeoutputvar(RETURN_FLOW_B,mreturn_flow_b,Real);
  writeoutputvar(EVAP_LAKE,mevap_lake,Real);
  writeoutputvar(EVAP_RES,mevap_res,Real);
  writeoutputvar(PREC_RES,mprec_res,Real);
  writeoutputvar(NFIRE,mnfire,Real);
  writeoutputvar(FIREDI,mfiredi,Real);
  writeoutputvar(FIREEMISSION_CO2,mfireemission.co2,Real);
  writeoutputvar(FIREEMISSION_CO,mfireemission.co,Real);
  writeoutputvar(FIREEMISSION_CH4,mfireemission.ch4,Real);
  writeoutputvar(FIREEMISSION_VOC,mfireemission.voc,Real);
  writeoutputvar(FIREEMISSION_TPM,mfireemission.tpm,Real);
  writeoutputvar(FIREEMISSION_NOX,mfireemission.nox,Real);
  writeoutputvar(TEMP_IMAGE,mtemp_image,Real);
  writeoutputvar(PREC_IMAGE,mprec_image,Real);
  writeoutputvar(SUN_IMAGE,msun_image,Real);
  writeoutputvar(WET_IMAGE,mwet_image,Real);
  writeoutputarray(SOILTEMP,msoiltemp,NSOILLAYER,Real);
  writeoutputvar(SOILTEMP1,msoiltemp2[0],Real);
  writeoutputvar(SOILTEMP2,msoiltemp2[1],Real);
  writeoutputvar(SOILTEMP3,msoiltemp2[2],Real);
  writeoutputvar(SOILTEMP4,msoiltemp2[3],Real);
  writeoutputvar(SOILTEMP5,msoiltemp2[4],Real);
  writeoutputvar(SOILTEMP6,msoiltemp2[5],Real);
  writeoutputvar(NUPTAKE,mn_uptake,Real);
  writeoutputvar(LEACHING,mn_leaching,Real);
  writeoutputvar(N2O_DENIT,mn2o_denit,Real);
  writeoutputvar(N2O_NIT,mn2o_nit,Real);
  writeoutputvar(N2_EMIS,mn2_emissions,Real);
  writeoutputvar(BNF,mbnf,Real);
  writeoutputvar(N_MINERALIZATION,mn_mineralization,Real);
  writeoutputvar(N_VOLATILIZATION,mn_volatilization,Real);
  writeoutputvar(N_IMMO,mn_immo,Real);
  writeoutputvar(RES_STORAGE,mres_storage,Real);
  writeoutputvar(RES_DEMAND,mres_demand,Real);
  writeoutputvar(TARGET_RELEASE,mtarget_release,Real);
  writeoutputvar(RES_CAP,mres_cap,Real);
  writeoutputvar(WD_UNSUST,mwd_unsustainable,Real);
  writeoutputvar(UNMET_DEMAND,munmet_demand,Real);
  writeoutputvar(WD_LOCAL,mwd_local,Real);
  writeoutputvar(WD_NEIGHB,mwd_neighb,Real);
  writeoutputvar(WD_RES,mwd_res,Real);
  writeoutputvar(CONV_LOSS_EVAP,mconv_loss_evap,Real);
  writeoutputvar(CONV_LOSS_DRAIN,mconv_loss_drain,Real);
  writeoutputvar(STOR_RETURN,mstor_return,Real);
  writeoutputvar(PREC,prec,Real);
  writeoutputvar(RAIN,mrain,Real);
  writeoutputvar(SNOWF,msnowf,Real);
  writeoutputvar(MELT,mmelt,Real);
  writeoutputvar(SWE,mswe,Real);
  writeoutputvar(SNOWRUNOFF,msnowrunoff,Real);
  writeoutputvar(RUNOFF_SURF,mrunoff_surf,Real);
  writeoutputvar(RUNOFF_LAT,mrunoff_lat,Real);
  writeoutputvar(SEEPAGE,mseepage,Real);
  writeoutputvar(FAPAR,fapar,Real);
  writeoutputvar(ALBEDO,malbedo,Real);
  writeoutputvar(PHEN_TMIN,mphen_tmin,Real);
  writeoutputvar(PHEN_TMAX,mphen_tmax,Real);
  writeoutputvar(PHEN_LIGHT,mphen_light,Real);
  writeoutputvar(PHEN_WATER,mphen_water,Real);
  writeoutputvar(WSCAL,mwscal,Real);
  writeoutputvar(GCONS_RF,mgcons_rf,Real);
  writeoutputvar(GCONS_IRR,mgcons_irr,Real);
  writeoutputvar(BCONS_IRR,mbcons_irr,Real);
  writeoutputvar(IRRIG_RW,mirrig_rw,Real);
  writeoutputvar(LAKEVOL,mlakevol,Real);
  writeoutputvar(LAKETEMP,mlaketemp,Real);
  writeoutputarray(SDATE,sdate,2*ncft,Real);
  writeoutputarray(HDATE,hdate,2*ncft,Real);
  writeoutputarray(CFT_SWC,cft_mswc,2*ncft,Real);
  writeoutputarray(CFT_SWC,nday_month,2*ncft,int);
  writeoutputarray(PFT_NPP,pft_npp,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2,Real);
  writeoutputarray(PFT_NUPTAKE,pft_nuptake,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2,Real);
  writeoutputarray(PFT_NDEMAND,pft_ndemand,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2,Real);
  writeoutputarray(HUSUM,husum,2*ncft,Real);
  writeoutputarray(CFT_RUNOFF,cft_runoff,2*ncft,Real);
  writeoutputarray(CFT_N2O_DENIT,cft_n2o_denit,2*ncft,Real);
  writeoutputarray(CFT_N2O_NIT,cft_n2o_nit,2*ncft,Real);
  writeoutputarray(CFT_N2_EMIS,cft_n2_emis,2*ncft,Real);
  writeoutputarray(CFT_LEACHING,cft_leaching,2*ncft,Real);
  writeoutputarray(CFT_C_EMIS,cft_c_emis,2*ncft,Real);
  writeoutputarray(PFT_GCGP,pft_gcgp,npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2,Real);
  writeoutputarray(PFT_GCGP,gcgp_count,npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2,Real);
  writeoutputarrayitem(PFT_HARVESTC,pft_harvest,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_RHARVESTC,pft_harvest,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_CONSUMP_WATER_G,cft_consump_water_g,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_CONSUMP_WATER_B,cft_consump_water_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(GROWING_PERIOD,growing_period,2*(ncft+NGRASS),Real);
  writeoutputarray(FPC,fpc,npft-config->nbiomass-config->nwft+1,Real);
  writeoutputarray(FPC_BFT,fpc_bft,((config->nbiomass+config->ngrass*2)*2),Real);
  writeoutputarray(NV_LAI,nv_lai,npft-config->nbiomass-config->nwft,Real);
  writeoutputarray(CFT_PET,cft_pet,2*(ncft+NGRASS),Real);
  writeoutputarray(CFT_TRANSP,cft_transp,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_TRANSP_B,cft_transp_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_EVAP,cft_evap,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_EVAP_B,cft_evap_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_INTERC,cft_interc,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_INTERC_B,cft_interc_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_RETURN_FLOW_B,cft_return_flow_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),int);
  writeoutputarray(CFT_NIR,cft_nir,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_TEMP,cft_temp,2*(ncft+NGRASS),Real);
  writeoutputarray(CFT_PREC,cft_prec,2*(ncft+NGRASS),Real);
  writeoutputarray(CFT_SRAD,cft_srad,2*(ncft+NGRASS),Real);
  writeoutputarray(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFTFRAC,cftfrac,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_AIRRIG,cft_airrig,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(CFT_FPAR,cft_fpar,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarray(LUC_IMAGE,cft_luc_image,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
  writeoutputarrayitem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS),Real);
  writeoutputarrayitem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS),Real);
  if(config->double_harvest)
  {
    writeoutputarrayitem(PFT_HARVESTC2,dh->pft_harvest2,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarrayitem(PFT_HARVESTN2,dh->pft_harvest2,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarrayitem(PFT_RHARVESTC2,dh->pft_harvest2,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarrayitem(PFT_RHARVESTN2,dh->pft_harvest2,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(GROWING_PERIOD2,dh->growing_period2,2*(ncft+NGRASS),Real);
    writeoutputarray(CFT_PET2,dh->cft_pet2,2*(ncft+NGRASS),Real);
    writeoutputarray(CFT_TRANSP2,dh->cft_transp2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(CFT_EVAP2,dh->cft_evap2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(CFT_INTERC2,dh->cft_interc2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(CFT_NIR2,dh->cft_nir2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(CFT_TEMP2,dh->cft_temp2,2*(ncft+NGRASS),Real);
    writeoutputarray(CFT_PREC2,dh->cft_prec2,2*(ncft+NGRASS),Real);
    writeoutputarray(CFT_SRAD2,dh->cft_srad2,2*(ncft+NGRASS),Real);
    writeoutputarray(CFTFRAC2,dh->cftfrac2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(CFT_AIRRIG2,dh->cft_airrig2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),Real);
    writeoutputarray(SDATE2,dh->sdate2,2*ncft,int);
    writeoutputarray(HDATE2,dh->hdate2,2*ncft,int);
    writeoutputarray(SYEAR,dh->syear,2*ncft,int);
    writeoutputarray(SYEAR2,dh->syear2,2*ncft,int);
    writeoutputarrayitem(CFT_ABOVEGBMC2,dh->cft_aboveground_biomass2,carbon,2*(ncft+NGRASS),Real);
    writeoutputarrayitem(CFT_ABOVEGBMN2,dh->cft_aboveground_biomass2,nitrogen,2*(ncft+NGRASS),Real);
    writeoutputarray(CFT_RUNOFF2,dh->cft_runoff2,2*ncft,Real);
    writeoutputarray(CFT_N2O_DENIT2,dh->cft_n2o_denit2,2*ncft,Real);
    writeoutputarray(CFT_N2O_NIT2,dh->cft_n2o_nit2,2*ncft,Real);
    writeoutputarray(CFT_N2_EMIS2,dh->cft_n2_emis2,2*ncft,Real);
    writeoutputarray(CFT_LEACHING2,dh->cft_leaching2,2*ncft,Real);
    writeoutputarray(CFT_C_EMIS2,dh->cft_c_emis2,2*ncft,Real);
    writeoutputarray(PFT_NUPTAKE2,dh->pft_nuptake2,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2,Real);
  }
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
  {
    if(iswrite(i))
      fwrite(((Real *)(&output->daily))+index,sizeof(Real),1,file);
    index++;
  }
} /* of 'fwriteoutputdata' */
