/**************************************************************************************/
/**                                                                                \n**/
/**              i  n  i  t  s  o  i  l  .  c                                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes soil variables                                        \n**/
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

STATIC Real calc_soil_dry_therm_cond(Real);

Bool initsoil(Stand *stand,           /**< Pointer to stand data */
              const Soilpar *soilpar, /**< soil parameter array */
              int ntotpft,            /**< number of PFT including crops*/
              const Config *config    /**< LPJmL configuration */
             )                        /** \return TRUE on error */
{
  Soil *soil;
  Real epsilon_gas,V,soilmoist;
  int l,p;
  soil=&stand->soil;
  soil->par=soilpar;
  soil->fastfrac=param.fastfrac;
  forrootsoillayer(l)
  {
    soil->pool[l].fast.carbon=soil->pool[l].slow.carbon=soil->k_mean[l].fast=soil->k_mean[l].slow=0.0;
    soil->decay_rate[l].fast=soil->decay_rate[l].slow=0.0;
    if(soilpar->type==ROCK || soilpar->type==ICE)
      soil->pool[l].slow.nitrogen=soil->pool[l].fast.nitrogen=soil->NH4[l]=soil->NO3[l]=0.0;
    else
    {
      soil->pool[l].slow.nitrogen=param.init_soiln.slow*(pow(10,0.35*logmidlayer[l])
      -(l>0 ? pow(10,0.35*logmidlayer[l-1]): 0));                                              //assuming a mean soc_k value for all regions
      soil->pool[l].fast.nitrogen=param.init_soiln.fast*(pow(10,0.35*logmidlayer[l])
      -(l>0 ? pow(10,0.35*logmidlayer[l-1]): 0));
      soil->pool[l].slow.carbon=soil->pool[l].slow.nitrogen*soil->par->cn_ratio;
      soil->pool[l].fast.carbon=soil->pool[l].fast.nitrogen*soil->par->cn_ratio;
      soil->NH4[l]=soil->NO3[l]=soil->pool[l].slow.nitrogen/100;
    }
    soil->c_shift[l]=newvec(Poolpar,ntotpft);
    checkptr(soil->c_shift[l]);
    soil->socfraction[l]=newvec(Real,ntotpft);
    check(soil->socfraction[l]);
  }
  soil->YEDOMA=soil->alag=soil->amp=soil->meanw1=soil->decomp_litter_mean.carbon=soil->decomp_litter_mean.nitrogen=0.0;
  soil->snowpack=soil->icefrac = 0.0;
  soil->decomp_litter_pft=newvec(Stocks,ntotpft);
  checkptr(soil->decomp_litter_pft);
  for (p=0;p<ntotpft;p++)
  {
    soil->decomp_litter_pft[p].carbon=0.0;
    soil->decomp_litter_pft[p].nitrogen=0.0;
  }
#ifdef MICRO_HEATING
  soil->litter.decomC=0.0;
#endif
  soil->w_evap=0.0;
  soil->count=0;
  soil->wa = 6000;
  soil->wtable = layerbound[BOTTOMLAYER-1]+soildepth[0];
  soil->iswetland = FALSE;
  for (l=0;l<NSOILLAYER;l++)
  {
    soil->w[l]=0.8;
    soil->w_fw[l]=0.0;
    soil->ice_fw[l]=0.0;
    soil->ice_depth[l]=0;
    soil->freeze_depth[l]=0;
    soil->ice_pwp[l]=0;
    soil->state[l]=NOSTATE;
    soil->perc_energy[l]=0;
    soil->whcs[l]=0;
    soil->wi_abs_enth_adj[l]=0.0;
    soil->sol_abs_enth_adj[l]=0.0;
#ifdef MICRO_HEATING
    soil->micro_heating[l]=0;
    soil->decomC[l]=0;
#endif
  }
  for (p=0;p<ntotpft;p++)
    {
      soil->c_shift[0][p].fast=0.55;
      soil->c_shift[0][p].slow=0.55;
      soil->socfraction[0][p]=pow(10,config->pftpar[p].soc_k*logmidlayer[0]);
    }
  for (l=1;l<LASTLAYER;l++)
    for (p=0;p<ntotpft;p++)
    {
        soil->c_shift[l][p].fast=0.45/(LASTLAYER-1);
        soil->c_shift[l][p].slow=0.45/(LASTLAYER-1);
        soil->socfraction[l][p]=pow(10,config->pftpar[p].soc_k*logmidlayer[l])-pow(10,config->pftpar[p].soc_k*logmidlayer[l-1]);
    }
  soil->maxthaw_depth=2;
  soil->mean_maxthaw=layerbound[BOTTOMLAYER];
  for(l=0;l<NSOILLAYER+1;++l)
    soil->temp[l] = soil->amean_temp[l] =0.0;
  for(l=0;l<NHEATGRIDP;++l)
    soil->enth[l]=0.0;
  for (l=0;l<=NFUELCLASS;l++)
    soil->litter.avg_fbd[l]=0.0;
  soil->snowheight=soil->snowfraction=soil->rw_buffer=0;
  soil->snowdens = snowdens_first;
  for(l=0;l<NTILLLAYER;l++)
    soil->df_tillage[l]=1.0;
  if(config->soilpar_option==PRESCRIBED_SOILPAR)
  {
    for(l=0;l<LASTLAYER;l++)
    {
      soil->wfc[l]=soilpar->wfc;
      soil->wpwp[l]=soilpar->wpwp;
      soil->whc[l]=soilpar->wfc-soilpar->wpwp;
      soil->whcs[l]=soil->whc[l]*soildepth[l];
      soil->wpwps[l]=soilpar->wpwp*soildepth[l];
      soil->wsat[l]=soilpar->wsat;
      soil->wsats[l]=soilpar->wsat*soildepth[l];
      soil->bulkdens[l]=(1-soilpar->wsat)*MINERALDENS;
      soil->beta_soil[l]=-2.655/log10(soilpar->wfc/soilpar->wsat);
      soil->Ks[l] = soilpar->Ks;
      soil->b[l]=soil->par->b;
      soil->psi_sat[l]=soil->par->psi_sat;
    }
    /*assume last layer is bedrock in 6-layer version */
    soil->wfc[BOTTOMLAYER]=soilpar->wfc;
    soil->wpwp[BOTTOMLAYER]=soilpar->wpwp;
    soil->whc[BOTTOMLAYER]=0.049;/*0.05 fc - 0.001 wpwps = 0.049 for field capacity water */
    soil->whcs[BOTTOMLAYER]=soil->whc[BOTTOMLAYER]*soildepth[BOTTOMLAYER];
    soil->wpwps[BOTTOMLAYER]=0.001*soildepth[BOTTOMLAYER];
    soil->wsat[BOTTOMLAYER]=soilpar->wsat;
    soil->wsats[BOTTOMLAYER]=0.08*soildepth[BOTTOMLAYER];
    soil->bulkdens[BOTTOMLAYER]=(1-soil->wsats[BOTTOMLAYER]/soildepth[BOTTOMLAYER])*MINERALDENS;
    soil->beta_soil[BOTTOMLAYER]=-2.655/log10(soilpar->wfc/soilpar->wsat);
    soil->Ks[BOTTOMLAYER] = soilpar->Ks;
    soil->b[BOTTOMLAYER]=2.91 + 0.159*1*100;
    soil->psi_sat[BOTTOMLAYER]=20;
  }
  else
  {
    if(soil->par->type==ROCK)
    {
      foreachsoillayer(l)
      {
        soil->wsat[l] = 0.006;
        soil->wpwp[l] = 0.0001;
        soil->wfc[l] = 0.005;
        soil->whc[l] = soil->wfc[l] - soil->wpwp[l];
        soil->whcs[l] = soil->whc[l] * soildepth[l];
        soil->wpwps[l] = soil->wpwp[l] * soildepth[l];
        soil->wsats[l] = soil->wsat[l] * soildepth[l];
        soil->bulkdens[l] = (1 - soil->wsats[l] / soildepth[l])*MINERALDENS;
        soil->Ks[l] = 0.1;
        soil->beta_soil[l] = -2.655 / log10(soil->wfc[l] / soil->wsat[l]);
        soil->b[l]=2.91 + 0.159*soil->par->clay*100;
        soil->psi_sat[l]=10*pow(10,1.88-0.0131*soil->par->sand*100);
       }
    }
    else
    {
      foreachsoillayer(l)
      {
        soil->wsat[l] = 0.468;
        soil->wpwp[l] = 0.284;
        soil->wfc[l] = 0.398;
        soil->whc[l] = soil->wfc[l]-soil->wpwp[l];
        soil->whcs[l] = soil->wsat[l] * soildepth[l];
        soil->wpwps[l] = soil->wpwp[l] * soildepth[l];
        soil->wsats[l] = soil->wsat[l] * soildepth[l];
        soil->bulkdens[l] = (1 - soil->wsat[l])*MINERALDENS;
        soil->k_dry[l] = (0.135*soil->bulkdens[l] + 64.7) / (MINERALDENS - 0.947*soil->bulkdens[l]);
        soil->Ks[l] = 3.5;
        soil->b[l]=soil->par->b;
        soil->psi_sat[l]=soil->par->psi_sat;
      }
      pedotransfer(stand,NULL,NULL,stand->frac);
    }
  }
  /*assume last layer is bedrock in 6-layer version */
  soil->wsat[BOTTOMLAYER] = 0.006;
  soil->wpwp[BOTTOMLAYER] = 0.001;
  soil->wfc[BOTTOMLAYER] = 0.003;
  soil->whc[BOTTOMLAYER] = soil->wfc[BOTTOMLAYER] - soil->wpwp[BOTTOMLAYER]; /*0.006 wsat - 0.002 whc - 0.001 wpwp = 0.003 for free water */
  soil->whcs[BOTTOMLAYER] = soil->whc[BOTTOMLAYER] * soildepth[BOTTOMLAYER];
  soil->wpwps[BOTTOMLAYER] = soil->wpwp[BOTTOMLAYER] * soildepth[BOTTOMLAYER];
  soil->wsats[BOTTOMLAYER] = soil->wsat[BOTTOMLAYER] * soildepth[BOTTOMLAYER];
  soil->bulkdens[BOTTOMLAYER] = (1 - soil->wsats[BOTTOMLAYER] / soildepth[BOTTOMLAYER])*MINERALDENS;
  soil->k_dry[BOTTOMLAYER] = 0.039*pow(soil->wsats[BOTTOMLAYER] / soildepth[BOTTOMLAYER], -2.2);
  soil->Ks[BOTTOMLAYER] = 0.1;
  soil->beta_soil[BOTTOMLAYER] = -2.655 / log10(soil->wfc[BOTTOMLAYER] / soil->wsat[BOTTOMLAYER]);
  soil->b[BOTTOMLAYER]=2.91 + 0.159*1*100;
  soil->psi_sat[BOTTOMLAYER]=20;

  for (l=0;l<LASTLAYER;l++)
  {
    V=getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    soilmoist=getsoilmoist(soil,l);
    epsilon_gas=getepsilon_O2(V,soilmoist,soil->wsat[l],BO2);
    soil->O2[l]=p_s/R_gas/(10+273.15)*O2s*WO2*soildepth[l]*epsilon_gas/1000; /*266 g/m3 converted to g/m2 per layer*/
    epsilon_gas=getepsilon_CH4(V,soilmoist,soil->wsat[l],BCH4);
    soil->CH4[l]=p_s/R_gas/(10+273.15)*param.pch4*1e-9*WCH4*soildepth[l]*epsilon_gas/1000;    /* corresponding to atmospheric CH4 concentration to g/m2 per layer*/
  }
  foreachsoillayer(l)
  {
    soil->k_dry[l]=calc_soil_dry_therm_cond(soil->bulkdens[l]);
  }
  return FALSE;
} /* of 'initsoil' */

STATIC Real calc_soil_dry_therm_cond(Real bulk_density)
{
  /* Source: Johanssen O (1975) Thermal conductivity of soils. University of
   * Trondheim */
  Real k_dry=0.0;
  k_dry=(0.135*bulk_density+64.7) / (MINERALDENS-0.947*bulk_density);
  return k_dry;
} /* of 'calc_soil_dry_therm_cond' */
