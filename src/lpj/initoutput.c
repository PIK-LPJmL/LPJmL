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

Bool initoutput(Output *output,      /**< Output data */
                int npft,            /**< number of natural PFTs */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJmL configuration */
               )                     /**\ return TRUE on error */
{
  int nirrig;
  int nnat; 
  nirrig=2*getnirrig(ncft,config);
  nnat=getnnat(npft,config);
  output->sdate=newvec(int,2*ncft);/* allocate memory for output */
  checkptr(output->sdate);
  output->hdate=newvec(int,2*ncft);
  checkptr(output->hdate);
  output->pft_npp=newvec(Real,nnat+nirrig);
  checkptr(output->pft_npp);
  output->husum=newvec(Real,2*ncft);
  checkptr(output->husum);
  output->pft_nuptake=newvec(Real,nnat+nirrig);
  checkptr(output->pft_nuptake);
  output->pft_ndemand=newvec(Real,nnat+nirrig);
  output->mpft_lai=newvec(Real,nnat+nirrig);
  checkptr(output->mpft_lai);
  output->pft_gcgp=newvec(Real,nnat+nirrig);
  checkptr(output->pft_gcgp);
  output->gcgp_count=newvec(Real,nnat+nirrig);
  checkptr(output->gcgp_count);
  output->pft_harvest=newvec(Harvest,nirrig);
  checkptr(output->pft_harvest);
  output->fpc = newvec(Real,nnat+1);
  checkptr(output->fpc);
  output->wpc = newvec(Real,nnat+1);
  check(output->wpc);
  output->pft_mort = newvec(Real,nnat);
  checkptr(output->pft_mort);
  output->cftfrac=newvec(Real,nirrig);
  checkptr(output->cftfrac);
  output->cft_consump_water_g=newvec(Real,nirrig);
  checkptr(output->cft_consump_water_g);
  output->cft_consump_water_b=newvec(Real,nirrig);
  checkptr(output->cft_consump_water_b);
  output->growing_period=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->growing_period);
  output->cft_pet=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_pet);
  output->cft_irrig_events=newvec(int,nirrig);
  checkptr(output->cft_irrig_events);
  output->cft_transp=newvec(Real,nirrig);
  checkptr(output->cft_transp);
  output->cft_transp_b=newvec(Real,nirrig);
  checkptr(output->cft_transp_b);
  output->cft_evap=newvec(Real,nirrig);
  checkptr(output->cft_evap);
  output->cft_evap_b=newvec(Real,nirrig);
  checkptr(output->cft_evap_b);
  output->cft_interc=newvec(Real,nirrig);
  checkptr(output->cft_interc);
  output->cft_interc_b=newvec(Real,nirrig);
  checkptr(output->cft_interc_b);
  output->cft_return_flow_b=newvec(Real,nirrig);
  checkptr(output->cft_return_flow_b);
  output->cft_nir=newvec(Real,nirrig);
  checkptr(output->cft_nir);
  output->cft_temp=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_temp);
  output->cft_prec=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_prec);
  output->cft_srad=newvec(Real,2*(ncft+NGRASS));
  checkptr(output->cft_srad);
  output->cft_aboveground_biomass=newvec(Stocks,2*(ncft+NGRASS));
  checkptr(output->cft_aboveground_biomass);
  output->cft_airrig=newvec(Real,nirrig);
  checkptr(output->cft_airrig);
  output->cft_fpar=newvec(Real,nirrig);
  checkptr(output->cft_fpar);
  output->cft_luc_image=newvec(Real,nirrig);
  checkptr(output->cft_luc_image);
  output->pft_laimax=newvec(Real,nnat+nirrig);
  checkptr(output->pft_laimax);
  output->pft_root=newvec(Stocks,nnat+nirrig);
  checkptr(output->pft_root);
  output->pft_leaf=newvec(Stocks,nnat+nirrig);
  checkptr(output->pft_leaf);
  output->pft_sapw=newvec(Stocks,nnat+nirrig);
  checkptr(output->pft_sapw);
  output->pft_hawo=newvec(Stocks,nnat+nirrig);
  checkptr(output->pft_hawo);
  output->pft_veg=newvec(Stocks,nnat+nirrig);
  checkptr(output->pft_veg);
  output->pft_nlimit=newvec(Real,nnat+nirrig);
  checkptr(output->pft_nlimit);
  if(config->nwptype)
  {
    output->wft_vegc = newvec(Real, config->nwft);
    checkptr(output->wft_vegc);
  }
  else
    output->wft_vegc=NULL;
  output->cft_conv_loss_evap=newvec(Real,nirrig);
  checkptr(output->cft_conv_loss_evap);
  output->cft_conv_loss_drain=newvec(Real,nirrig);
  checkptr(output->cft_conv_loss_drain);
  output->nv_lai=newvec(Real, nnat);
  checkptr(output->nv_lai);
  output->fpc_bft=newvec(Real, (config->nbiomass+2*config->ngrass)*2);
  checkptr(output->fpc_bft);
  output->cft_mswc=newvec(Real,2*ncft);
  checkptr(output->cft_mswc);
  output->nday_month=newvec(int,2*ncft);
  checkptr(output->nday_month);
  output->cft_runoff=newvec(Real,2*ncft);
  checkptr(output->cft_runoff);
  output->cft_n2o_denit=newvec(Real,2*ncft);
  checkptr(output->cft_n2o_denit);
  output->cft_n2o_nit=newvec(Real,2*ncft);
  checkptr(output->cft_n2o_nit);
  output->cft_n2_emis=newvec(Real,2*ncft);
  checkptr(output->cft_n2_emis);
  output->cft_c_emis=newvec(Real,2*ncft);
  checkptr(output->cft_c_emis);
  output->cft_leaching=newvec(Real,2*ncft);
  checkptr(output->cft_leaching);
  output->cft_nfert=newvec(Real,2*nirrig);
  check(output->cft_nfert);
  if(config->double_harvest)
  {
    output->dh=new(Output_doubleharvest);
    checkptr(output->dh);
    output->dh->sdate2=newvec(int,2*ncft); /* allocate memory for output */
    checkptr(output->dh->sdate2);
    output->dh->hdate2=newvec(int,2*ncft);
    checkptr(output->dh->hdate2);
    output->dh->husum2=newvec(Real,2*ncft);
    checkptr(output->dh->husum2);
    output->dh->syear=newvec(int,2*ncft); /* allocate memory for output */
    checkptr(output->dh->syear);
    output->dh->syear2=newvec(int,2*ncft); /* allocate memory for output */
    checkptr(output->dh->syear2);
    output->dh->pft_harvest2=newvec(Harvest,nirrig);
    checkptr(output->dh->pft_harvest2);
    output->dh->cftfrac2=newvec(Real,nirrig);
    checkptr(output->dh->cftfrac2);
    output->dh->cft_pet2=newvec(Real,2*(ncft+NGRASS));
    checkptr(output->dh->cft_pet2);
    output->dh->cft_transp2=newvec(Real,nirrig);
    checkptr(output->dh->cft_transp2);
    output->dh->cft_evap2=newvec(Real,nirrig);
    checkptr(output->dh->cft_evap2);
    output->dh->cft_interc2=newvec(Real,nirrig);
    checkptr(output->dh->cft_interc2);
    output->dh->cft_nir2=newvec(Real,nirrig);
    checkptr(output->dh->cft_nir2);
    output->dh->cft_airrig2=newvec(Real,nirrig);
    checkptr(output->dh->cft_airrig2);
    output->dh->growing_period2=newvec(Real,2*(ncft+NGRASS));
    checkptr(output->dh->growing_period2);
    output->dh->cft_temp2=newvec(Real,2*(ncft+NGRASS));
    checkptr(output->dh->cft_temp2);
    output->dh->cft_prec2=newvec(Real,2*(ncft+NGRASS));
    checkptr(output->dh->cft_prec2);
    output->dh->cft_srad2=newvec(Real,2*(ncft+NGRASS));
    checkptr(output->dh->cft_srad2);
    output->dh->cft_aboveground_biomass2=newvec(Stocks,2*(ncft+NGRASS));
    checkptr(output->dh->cft_aboveground_biomass2);
    output->dh->cft_runoff2=newvec(Real,2*ncft);
    checkptr(output->dh->cft_runoff2);
    output->dh->cft_n2o_denit2=newvec(Real,2*ncft);
    checkptr(output->dh->cft_n2o_denit2);
    output->dh->cft_n2o_nit2=newvec(Real,2*ncft);
    checkptr(output->dh->cft_n2o_nit2);
    output->dh->cft_n2_emis2=newvec(Real,2*ncft);
    checkptr(output->dh->cft_n2_emis2);
    output->dh->cft_c_emis2=newvec(Real,2*ncft);
    checkptr(output->dh->cft_c_emis2);
    output->dh->cft_leaching2=newvec(Real,2*ncft);
    checkptr(output->dh->cft_leaching2);
    output->dh->pft_nuptake2=newvec(Real,nnat+nirrig);
    checkptr(output->dh->pft_nuptake2);
    output->dh->cft_nfert2=newvec(Real,2*nirrig);
    check(output->dh->cft_nfert2);
  }
  else
    output->dh=NULL;
  output->daily.cft=config->crop_index;
  output->daily.irrigation=config->crop_irrigation;
  return FALSE;
} /* of 'initoutput' */
