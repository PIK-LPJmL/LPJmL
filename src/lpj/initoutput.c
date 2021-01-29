/**************************************************************************************/
/**                                                                                \n**/
/**                    i  n  i  t  o  u  t  p  u  t  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates output data                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}

Bool initoutput(Output *output, /**< Output data */
                int cft,        /**< CFT for daily output */
                Bool irrigation,/**< irrigation for daily output */
                int npft,       /**< number of natural PFTs */
                int nbiomass,   /**< number of biomass PFTs */
                int ncft        /**< number of crop PFTs */
               )                /**\ return TRUE on error */
{
  output->sdate=newvec(int,2*ncft);/* allocate memory for output */
  checkptr(output->sdate);
  output->hdate=newvec(int,2*ncft);
  checkptr(output->hdate);
  output->husum=newvec(Real,2*ncft);
  check(output->husum);
  output->pft_npp=newvec(Real,(npft-nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->pft_npp);
  output->pft_nuptake=newvec(Real,(npft-nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->pft_nuptake);
  output->pft_ndemand=newvec(Real,(npft-nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE));
  output->pft_gcgp=newvec(Real,(npft-nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->pft_gcgp);
  output->gcgp_count=newvec(Real,(npft-nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->gcgp_count);
  output->pft_harvest=newvec(Harvest,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->pft_harvest);
  output->fpc = newvec(Real, (npft-nbiomass)+1);
  checkptr(output->fpc);
  output->cftfrac=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cftfrac);
  output->cft_consump_water_g=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_consump_water_g);
  output->cft_consump_water_b=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_consump_water_b);
  output->growing_period=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->growing_period);
  output->cft_pet=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_pet);
  output->cft_irrig_events=newvec(int,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_irrig_events);
  output->cft_transp=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_transp);
  output->cft_transp_b=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_transp_b);
  output->cft_evap=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_evap);
  output->cft_evap_b=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_evap_b);
  output->cft_interc=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_interc);
  output->cft_interc_b=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_interc_b);
  output->cft_return_flow_b=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_return_flow_b);
  output->cft_nir=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_nir);
  output->cft_temp=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_temp);
  output->cft_prec=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_prec);
  output->cft_srad=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_srad);
  output->cft_aboveground_biomass=newvec(Stocks,2*(ncft+NGRASS));
  checkptr(output->cft_aboveground_biomass);
  output->cft_airrig=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_airrig);
  output->cft_fpar=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_fpar);
  output->cft_luc_image=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_luc_image);
  output->cft_conv_loss_evap=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_conv_loss_evap);
  output->cft_conv_loss_drain=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_conv_loss_drain);
  output->cft_leaf=newvec(Stocks,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_leaf);
  output->cft_root=newvec(Stocks,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_root);
  output->cft_veg=newvec(Stocks,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_veg);
  output->cft_nlimit=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_nlimit);
  output->cft_laimax = newvec(Real, 2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_laimax);
  output->cft_mswc=newvec(Real,2*ncft);
  checkptr(output->cft_mswc);
  output->nday_month=newvec(int,2*ncft);
  checkptr(output->nday_month);
  output->cft_runoff=newvec(Real,2*ncft);
  check(output->cft_runoff);
  output->cft_n2o_denit=newvec(Real,2*ncft);
  check(output->cft_n2o_denit);
  output->cft_n2o_nit=newvec(Real,2*ncft);
  check(output->cft_n2o_nit);
  output->cft_n2_emis=newvec(Real,2*ncft);
  check(output->cft_n2_emis);
  output->cft_c_emis=newvec(Real,2*ncft);
  check(output->cft_c_emis);
  output->cft_leaching=newvec(Real,2*ncft);
  check(output->cft_leaching);
  output->cft_nfert=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  check(output->cft_nfert);
#ifdef DOUBLE_HARVEST
  output->sdate2=newvec(int,2*ncft); /* allocate memory for output */
  checkptr(output->sdate2);
  output->hdate2=newvec(int,2*ncft);
  checkptr(output->hdate2);
  output->husum2=newvec(Real,2*ncft);
  check(output->husum2);
  output->syear=newvec(int,2*ncft); /* allocate memory for output */
  checkptr(output->syear);
  output->syear2=newvec(int,2*ncft); /* allocate memory for output */
  checkptr(output->syear2);
  output->pft_harvest2=newvec(Harvest,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->pft_harvest2);
  output->cftfrac2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cftfrac2);
  output->cft_pet2=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_pet2);
  output->cft_transp2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_transp2);
  output->cft_evap2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_evap2);
  output->cft_interc2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_interc2);
  output->cft_nir2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_nir2);
  output->cft_airrig2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->cft_airrig2);
  output->growing_period2=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->growing_period2);
  output->cft_temp2=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_temp2);
  output->cft_prec2=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_prec2);
  output->cft_srad2=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_srad2);
  output->cft_aboveground_biomass2=newvec(Stocks,2*(ncft+NGRASS));
  checkptr(output->cft_aboveground_biomass2);
  output->cft_runoff2=newvec(Real,2*ncft);
  check(output->cft_runoff2);
  output->cft_n2o_denit2=newvec(Real,2*ncft);
  check(output->cft_n2o_denit2);
  output->cft_n2o_nit2=newvec(Real,2*ncft);
  check(output->cft_n2o_nit2);
  output->cft_n2_emis2=newvec(Real,2*ncft);
  check(output->cft_n2_emis2);
  output->cft_c_emis2=newvec(Real,2*ncft);
  check(output->cft_c_emis2);
  output->cft_leaching2=newvec(Real,2*ncft);
  check(output->cft_leaching2);
  output->pft_nuptake2=newvec(Real,(npft-nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE));
  checkptr(output->pft_nuptake2);
  output->cft_nfert2=newvec(Real,2*(ncft+NGRASS+NBIOMASSTYPE));
  check(output->cft_nfert2);
#endif
  initoutput_annual(output, npft, nbiomass,ncft);
  output->daily.cft=cft;
  output->daily.irrigation=irrigation;
  return FALSE;
} /* of 'initoutput' */
