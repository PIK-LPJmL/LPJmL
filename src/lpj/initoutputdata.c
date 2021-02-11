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

#define isinit(index) (isinit2(index,timestep,year,config))

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

static Bool isinit2(int index,int timestep,int year,const Config *config)
{
  if(timestep==ANNUAL && config->outnames[index].timestep>0)
    return ((year-config->outputyear) %  config->outnames[index].timestep==0);
  else
    return config->outnames[index].timestep==timestep;
} /* of 'isinit2' */

void initoutputdata(Output *output,      /**< output data */
                    int timestep,        /**< time step (ANNUAL, MONTHLY, DAILY) */
                    int npft,            /**< number of natural PFTs */
                    int ncft,            /**< number of crop PFTs */
                    int year,            /**< simulation year */
                    const Config *config /**< LPJ configuration */
                   )
{
  int i,index,nirrig,nnat;
  nirrig=2*getnirrig(ncft,config);
  nnat=getnnat(npft,config);
  setoutputvar(DAYLENGTH,daylength);
  setoutputvar(TEMP,temp);
  setoutputvar(SUN,sun);
  setoutputvar(VEGC,veg.carbon);
  setoutputvar(VEGN,veg.nitrogen);
  setoutputvar(SOILC,soil.carbon);
  setoutputvar(MGRASS_SOILC,mgrass_soil.carbon);
  setoutputvar(MGRASS_SOILN,mgrass_soil.nitrogen);
  setoutputvar(MGRASS_LITC,mgrass_litter.carbon);
  setoutputvar(MGRASS_LITN,mgrass_litter.nitrogen);
  setoutputvar(SOILN,soil.nitrogen);
  setoutputvar(MG_VEGC,mg_vegc);
  setoutputvar(MG_SOILC,mg_soilc);
  setoutputvar(MG_LITC,mg_litc);
  setoutputvar(SOILC_SLOW,soil_slow.carbon);
  setoutputvar(SOILN_SLOW,soil_slow.nitrogen);
  setoutputvar(LITC,litter.carbon);
  setoutputvar(LITN,litter.nitrogen);
  setoutputvar(MEANVEGCMANGRASS,mean_vegc_mangrass);
  setoutputvar(BNF_AGR,abnf_agr);
  setoutputvar(NFERT_AGR,anfert_agr);
  setoutputvar(NMANURE_AGR,anmanure_agr);
  setoutputvar(NDEPO_AGR,andepo_agr);
  setoutputvar(NMINERALIZATION_AGR,anmineralization_agr);
  setoutputvar(NIMMOBILIZATION_AGR,animmobilization_agr);
  setoutputvar(NUPTAKE_AGR,anuptake_agr);
  setoutputvar(NLEACHING_AGR,anleaching_agr);
  setoutputvar(N2O_DENIT_AGR,an2o_denit_agr);
  setoutputvar(N2O_NIT_AGR,an2o_nit_agr);
  setoutputvar(NH3_AGR,anh3_agr);
  setoutputvar(N2_AGR,an2_agr);
  setoutputvar(LITFALLN_AGR,alitfalln_agr);
  setoutputvar(HARVESTN_AGR,aharvestn_agr);
  setoutputvar(SEEDN_AGR,aseedn_agr);
  setoutputvar(DELTA_NORG_SOIL_AGR,adelta_norg_soil_agr);
  setoutputvar(DELTA_NMIN_SOIL_AGR,adelta_nmin_soil_agr);
  setoutputvar(DELTA_NVEG_SOIL_AGR,adelta_nveg_soil_agr);
  setoutputvar(CELLFRAC_AGR,cellfrac_agr);
  setoutputarrayitem(SOILC_LAYER,soil_layer,carbon,LASTLAYER);
  setoutputarrayitem(SOILN_LAYER,soil_layer,nitrogen,LASTLAYER);
  setoutputarray(SOILC_AGR_LAYER,soilc_agr_layer,LASTLAYER);
  setoutputarray(SOILNO3_LAYER,soilno3_layer,LASTLAYER);
  setoutputarray(SOILNH4_LAYER,soilnh4_layer,LASTLAYER);
  setoutputarray(PFT_LAIMAX,pft_laimax,nnat+nirrig);
  setoutputarrayitem(PFT_NLEAF,pft_leaf,nitrogen,nnat+nirrig);
  setoutputarrayitem(PFT_CLEAF,pft_leaf,carbon,nnat+nirrig);
  setoutputarrayitem(PFT_NROOT,pft_root,nitrogen,nnat+nirrig);
  setoutputarrayitem(PFT_CROOT,pft_root,carbon,nnat+nirrig);
  setoutputarrayitem(PFT_NSAPW,pft_sapw,nitrogen,nnat+nirrig);
  setoutputarrayitem(PFT_CSAPW,pft_sapw,carbon,nnat+nirrig);
  setoutputarrayitem(PFT_NHAWO,pft_hawo,nitrogen,nnat+nirrig);
  setoutputarrayitem(PFT_CHAWO,pft_hawo,carbon,nnat+nirrig);
  setoutputarrayitem(PFT_VEGN,pft_veg,nitrogen,nnat+nirrig);
  setoutputarrayitem(PFT_VEGC,pft_veg,carbon,nnat+nirrig);
  setoutputarray(PFT_NLIMIT,pft_nlimit,nnat+nirrig);
  setoutputvar(MAXTHAW_DEPTH,maxthaw_depth);
  setoutputvar(SOILNO3,soilno3);
  setoutputvar(SOILNH4,soilnh4);
  setoutputvar(AGB,agb);
  setoutputvar(AGB_TREE,agb_tree);
  setoutputvar(NPP,npp);
  setoutputvar(NPP_AGR,npp_agr);
  setoutputvar(GPP,gpp);
  setoutputvar(RH,rh);
  setoutputvar(RH_AGR,rh_agr);
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
  if(config->nwptype)
    setoutputarray(WFT_VEGC,wft_vegc,config->nwft);
#ifdef IMAGE
  setoutputvar(WD_GW,mwd_gw);
  setoutputvar(WD_AQ,mwd_aq);
#endif
  setoutputvar(PRODUCT_POOL_FAST,product_pool.fast);
  setoutputvar(PRODUCT_POOL_SLOW,product_pool.slow);
  setoutputvar(PROD_TURNOVER,prod_turnover);
  setoutputvar(WATERUSE_HIL,awateruse_hil);
  setoutputvar(WATERUSECONS,waterusecons);
  setoutputvar(WATERUSEDEM,waterusedem);
  setoutputvar(NEGC_FLUXES,neg_fluxes.carbon);
  setoutputvar(NEGN_FLUXES,neg_fluxes.nitrogen);
  setoutputvar(MEAN_VEGC_MANGRASS,mean_vegc_mangrass);  
  setoutputvar(VEGC_AVG,vegc_avg);
  setoutputvar(RH_LITTER,rh_litter);
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
  setoutputvar(FLUX_AUTOFERT,flux_nfert);
  setoutputarray(SDATE,sdate,2*ncft);
  setoutputarray(HDATE,hdate,2*ncft);
  setoutputarray(CFT_SWC,cft_mswc,2*ncft);
  setoutputarray(CFT_SWC,nday_month,2*ncft);
  setoutputarray(PFT_NPP,pft_npp,nnat+nirrig);
  setoutputarray(PFT_NUPTAKE,pft_nuptake,nnat+nirrig);
  setoutputarray(PFT_NDEMAND,pft_ndemand,nnat+nirrig);
  setoutputarray(HUSUM,husum,2*ncft);
  setoutputarray(CFT_RUNOFF,cft_runoff,2*ncft);
  setoutputarray(CFT_N2O_DENIT,cft_n2o_denit,2*ncft);
  setoutputarray(CFT_N2O_NIT,cft_n2o_nit,2*ncft);
  setoutputarray(CFT_N2_EMIS,cft_n2_emis,2*ncft);
  setoutputarray(CFT_LEACHING,cft_leaching,2*ncft);
  setoutputarray(CFT_C_EMIS,cft_c_emis,2*ncft);
  setoutputarray(PFT_GCGP,pft_gcgp,nnat+nirrig);
  setoutputarray(PFT_GCGP,gcgp_count,nnat+nirrig);
  setoutputarrayitem(PFT_HARVESTC,pft_harvest,harvest.carbon,nirrig);
  setoutputarrayitem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,nirrig);
  setoutputarrayitem(PFT_RHARVESTC,pft_harvest,residual.carbon,nirrig);
  setoutputarrayitem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,nirrig);
  setoutputarray(CFT_CONSUMP_WATER_G,cft_consump_water_g,nirrig);
  setoutputarray(CFT_CONSUMP_WATER_B,cft_consump_water_b,nirrig);
  setoutputarray(GROWING_PERIOD,growing_period,2*(ncft+NGRASS));
  setoutputarray(FPC,fpc,nnat+1);
  setoutputarray(PFT_MORT,pft_mort,nnat);
  setoutputarray(FPC_BFT,fpc_bft,((config->nbiomass+config->ngrass*2)*2));
  setoutputarray(NV_LAI,nv_lai,nnat);
  setoutputarray(CFT_PET,cft_pet,2*(ncft+NGRASS));
  setoutputarray(CFT_TRANSP,cft_transp,nirrig);
  setoutputarray(CFT_TRANSP_B,cft_transp_b,nirrig);
  setoutputarray(CFT_EVAP,cft_evap,nirrig);
  setoutputarray(CFT_EVAP_B,cft_evap_b,nirrig);
  setoutputarray(CFT_INTERC,cft_interc,nirrig);
  setoutputarray(CFT_INTERC_B,cft_interc_b,nirrig);
  setoutputarray(CFT_RETURN_FLOW_B,cft_return_flow_b,nirrig);
  setoutputarray(CFT_IRRIG_EVENTS,cft_irrig_events,nirrig);
  setoutputarray(CFT_NIR,cft_nir,nirrig);
  setoutputarray(CFT_TEMP,cft_temp,2*(ncft+NGRASS));
  setoutputarray(CFT_PREC,cft_prec,2*(ncft+NGRASS));
  setoutputarray(CFT_SRAD,cft_srad,2*(ncft+NGRASS));
  setoutputarray(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,nirrig);
  setoutputarray(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,nirrig);
  setoutputarray(CFTFRAC,cftfrac,nirrig);
  setoutputarray(CFT_AIRRIG,cft_airrig,nirrig);
  setoutputarray(CFT_FPAR,cft_fpar,nirrig);
  setoutputarray(LUC_IMAGE,cft_luc_image,nirrig);
  setoutputarrayitem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS));
  setoutputarrayitem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS));
  setoutputarray(CFT_NFERT,cft_nfert,nirrig);
  if(config->double_harvest)
  {
    setoutputarrayitem(PFT_HARVESTC2,dh->pft_harvest2,harvest.carbon,nirrig);
    setoutputarrayitem(PFT_HARVESTN2,dh->pft_harvest2,harvest.nitrogen,nirrig);
    setoutputarrayitem(PFT_RHARVESTC2,dh->pft_harvest2,residual.carbon,nirrig);
    setoutputarrayitem(PFT_RHARVESTN2,dh->pft_harvest2,residual.nitrogen,nirrig);
    setoutputarray(GROWING_PERIOD2,dh->growing_period2,2*(ncft+NGRASS));
    setoutputarray(CFT_PET2,dh->cft_pet2,2*(ncft+NGRASS));
    setoutputarray(CFT_TRANSP2,dh->cft_transp2,nirrig);
    setoutputarray(CFT_EVAP2,dh->cft_evap2,nirrig);
    setoutputarray(CFT_INTERC2,dh->cft_interc2,nirrig);
    setoutputarray(CFT_NIR2,dh->cft_nir2,nirrig);
    setoutputarray(CFT_TEMP2,dh->cft_temp2,2*(ncft+NGRASS));
    setoutputarray(CFT_PREC2,dh->cft_prec2,2*(ncft+NGRASS));
    setoutputarray(CFT_SRAD2,dh->cft_srad2,2*(ncft+NGRASS));
    setoutputarray(CFTFRAC2,dh->cftfrac2,nirrig);
    setoutputarray(CFT_AIRRIG2,dh->cft_airrig2,nirrig);
    setoutputarray(SDATE2,dh->sdate2,2*ncft);
    setoutputarray(HDATE2,dh->hdate2,2*ncft);
    setoutputarray(SYEAR,dh->syear,2*ncft);
    setoutputarray(SYEAR2,dh->syear2,2*ncft);
    setoutputarrayitem(CFT_ABOVEGBMC2,dh->cft_aboveground_biomass2,carbon,2*(ncft+NGRASS));
    setoutputarrayitem(CFT_ABOVEGBMN2,dh->cft_aboveground_biomass2,nitrogen,2*(ncft+NGRASS));
    setoutputarray(CFT_RUNOFF2,dh->cft_runoff2,2*ncft);
    setoutputarray(CFT_N2O_DENIT2,dh->cft_n2o_denit2,2*ncft);
    setoutputarray(CFT_N2O_NIT2,dh->cft_n2o_nit2,2*ncft);
    setoutputarray(CFT_N2_EMIS2,dh->cft_n2_emis2,2*ncft);
    setoutputarray(CFT_LEACHING2,dh->cft_leaching2,2*ncft);
    setoutputarray(CFT_C_EMIS2,dh->cft_c_emis2,2*ncft);
    setoutputarray(CFT_NFERT2,dh->cft_nfert2,nirrig);
    setoutputarray(PFT_NUPTAKE2,dh->pft_nuptake2,nnat+nirrig);
  }
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
  {
    if(isinit(i))
      ((Real *)(&output->daily))[index]=0;
    index++;
  }
} /* of 'initoutputdata' */
