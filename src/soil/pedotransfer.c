/**************************************************************************************/
/**                                                                                \n**/
/**               p e d o t r a n s f e r .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#define maxSOM_dens 130000 //g*m-3
#define psi_som 10.3 /**> saturated suction (mm) for organic matter (Letts, 2000)*/
#define b_som   2.7   /**> ! Clapp Hornberger paramater for oragnic soil (Letts, 2000)*/
#define DENOMINATOR 3.81671282562382 // log(1500) - log(33)

#define DENOMINATOR 3.81671282562382 // log(1500) - log(33)

void pedotransfer(Stand *stand,  /**< pointer to stand */
                  Real *abswmm,
                  Real *absimm,
                  Real standfrac /**< stand fraction (0..1) */
                 )
{
  int l;
  const Soilpar *soilpar;
  Soil *soil;
  Real om_layer;
  Real wpwpt; /* 1500kPa moisture, first solution, v% */
  Real w_fc; /* field capacity without density effect */
  Real wfct; /* 33kPa moisture, first solution, v% */
  Real w_sat; /* saturation without density effect */
  Real ws33t; /* SAT-33 kPa moisture, first solution, v% */
  Real ws33; /* SAT-33 kPa moisture, normal density, v% */
  Real wmm, imm; /* actual water content in mm */
  Real lambda;
  Real excess = 0;
  Real dispose=0,dispose2=0;
  Real f_sc=0.0;
#ifdef CHECK_BALANCE
  Real w_before,w_after;
#endif
#ifdef USE_TIMING
  double tstart;
  timing_start(tstart);
#endif
  soil=&stand->soil;
  soilpar = soil->par;
#ifdef SAFE
  String line;
#endif
#ifdef CHECK_BALANCE
  if(abswmm==NULL)
    w_before=soilwater(soil);
  else
  {
    w_before=soil->snowpack+soil->rw_buffer+soil->litter.agtop_moist;
    w_before+=soil->w[BOTTOMLAYER]*soil->whcs[BOTTOMLAYER]+soil->ice_depth[BOTTOMLAYER]+soil->w_fw[BOTTOMLAYER]+soil->ice_fw[BOTTOMLAYER]+soil->wpwps[BOTTOMLAYER];
    forrootsoillayer(l)
      w_before+=abswmm[l]+absimm[l];
  }
#endif
  if (soilpar->type != ROCK)
  {
    forrootsoillayer(l)
    {
      if (abswmm == NULL)
        wmm = soil->w[l] * soil->whcs[l] + soil->w_fw[l] + soil->wpwps[l] * (1 - soil->ice_pwp[l]); /* compute absolute water in mm */
      else
        wmm = abswmm[l];
      if (absimm == NULL)
        imm = soil->ice_depth[l] + soil->ice_fw[l] + soil->wpwps[l] * soil->ice_pwp[l]; /* compute absolute ice content in mm */
      else
        imm = absimm[l];
      om_layer = 2 * ((soil->pool[l].fast.carbon + soil->pool[l].slow.carbon) / ( (1 - soil->wsat[l])*MINERALDENS * soildepth[l]))*100;  /* calculation of soil organic matter in % */
      if (om_layer > 8)
        om_layer = 8;
      if (om_layer < 0)
        om_layer = 0;
      
      /* pedotransfer function following Saxton&Rawls 2006: */
      wpwpt = -0.024*soilpar->sand + 0.487*soilpar->clay + 0.006*om_layer + 0.005*(soilpar->sand*om_layer) - 0.013*(soilpar->clay*om_layer) + 0.068*(soilpar->sand*soilpar->clay) + 0.031;
      soil->wpwp[l] = wpwpt + (0.14 * wpwpt - 0.02);
      soil->wpwps[l] = soil->wpwp[l] * soildepth[l];
      ws33t = 0.278*soilpar->sand + 0.034*soilpar->clay + 0.022*om_layer - 0.018*(soilpar->sand*om_layer) - 0.027*(soilpar->clay*om_layer) - 0.584*(soilpar->sand*soilpar->clay) + 0.078;
      ws33 = ws33t + (0.636*ws33t - 0.107);

      wfct = -0.251*soilpar->sand + 0.195*soilpar->clay + 0.011*om_layer + 0.006*(soilpar->sand*om_layer) - 0.027*(soilpar->clay*om_layer) + 0.452*(soilpar->sand*soilpar->clay) + 0.299;
      w_fc = (wfct + (((1.283*wfct)*(1.283*wfct)) - 0.374*wfct - 0.015));

      w_sat = w_fc + ws33 - 0.097*soilpar->sand + 0.043;
      f_sc=min(1,((soil->pool[l].fast.carbon + soil->pool[l].slow.carbon)/(soildepth[l]/1000))/maxSOM_dens);
      if(f_sc<0) f_sc=0;

      //psi_sat_min=10*pow(1,1.88-0.0131*soilpar->sand*100);  // lawrence and slater caluclate it this way, I take precsribed parameter for the moment
      soil->psi_sat[l]=(1-f_sc)*soil->par->psi_sat + psi_som*f_sc;
      soil->b[l]=(1-f_sc)*soil->par->b + b_som*f_sc;

      if(l<NTILLLAYER)
      {
        soil->wsat[l] = 1 - (1-w_sat)*soil->df_tillage[l];
        soil->wfc[l] = w_fc - 0.2 * (w_sat - soil->wsat[l]);
      }
      else
      {
        soil->wsat[l]=w_sat;
        soil->wfc[l]=w_fc;
      }

      soil->wsats[l] = soil->wsat[l] * soildepth[l];

      if (soil->wsat[l] - soil->wfc[l] < 0.05)
        soil->wfc[l] = soil->wsat[l] - 0.05;

#ifdef SAFE
      if (soil->wsat[l] > 1)
        printf("Cell (%s) wsat[%d] %g, wpwp[%d] %g, wfc[%d] %g, om_soil %g, ice_pwp:%g in pedotransfer\n", sprintcoord(line,&stand->cell->coord),l, soil->wsat[l], l, soil->wpwp[l], l, soil->wfc[l], om_layer, soil->ice_pwp[l]);
      if (soil->wsats[l]<1e-10)
        printf("Cell (%s) wsat[%d] %3.3f,  wfc[%d] %3.3f, ws33 %3.3f, sand %3.3f, in pedotransfer\n",sprintcoord(line,&stand->cell->coord), l, soil->wsat[l], l, soil->wfc[l], ws33, soilpar->sand);
#endif

      soil->beta_soil[l] = -2.655 / log10(soil->wfc[l] / soil->wsat[l]);
      soil->whc[l] = soil->wfc[l] - soil->wpwp[l];
      soil->whcs[l] = soil->whc[l] * soildepth[l];

      /* Calculation of Ks */
      lambda =  (log(soil->wfc[l]/soil->wpwp[l]))/DENOMINATOR; //(log(1500) - log(33));
      soil->Ks[l] = 1930*pow((soil->wsat[l]-soil->wfc[l]),(3-lambda));

      soil->ice_pwp[l] = min(imm / soil->wpwps[l], 1);
      imm -= soil->ice_pwp[l] * soil->wpwps[l];
      /* re-distribute absolute water */
      if (imm > epsilon * 1e-3)
      {
        soil->ice_depth[l] = min(imm, soil->whcs[l]);
        imm -= soil->ice_depth[l];
        imm=max(0,imm);
        soil->ice_fw[l] = min(imm, soil->wsats[l] - soil->wfc[l] * soildepth[l]);
        imm -= soil->ice_fw[l];
      }
      else
        soil->ice_depth[l] = soil->ice_fw[l] = 0;

      if (soil->ice_pwp[l] < 1)
        wmm -= soil->wpwps[l] * (1 - soil->ice_pwp[l]);
      if (wmm > epsilon * 1e-3)
      {
        soil->w[l] = min(wmm / soil->whcs[l], 1);
        wmm -= soil->whcs[l] * soil->w[l];
        soil->w_fw[l] = min(wmm, soil->wsats[l] - soil->wfc[l] * soildepth[l]);
        wmm -= soil->w_fw[l];
      }
      else
        soil->w[l] = soil->w_fw[l] = 0;

      /* assure numerical stability */
      /* if more water than soil can hold (above pwp) */
      if((soil->w[l]*soil->whcs[l]+soil->w_fw[l]+soil->ice_depth[l]+soil->ice_fw[l])>(soil->wsats[l]-soil->wpwps[l]))
      {
        dispose=(soil->w[l]*soil->whcs[l]+soil->w_fw[l]+soil->ice_depth[l]+soil->ice_fw[l])-(soil->wsats[l]-soil->wpwps[l]);
        /* dispose liquid free water first if there is any */
        if(soil->w_fw[l]>epsilon)
        {
          dispose2=min(soil->w_fw[l],dispose);
          soil->w_fw[l]-=dispose2; //(soil->w[l]*soil->whcs[l]+soil->w_fw[l]+soil->ice_depth[l]+soil->ice_fw[l])-(soil->wsats[l]-soil->wpwps[l]);
          soil->w_fw[l]=max(0,soil->w_fw[l]);
          excess+=dispose2;
          dispose-=dispose2;
        }
        /* if there still is water to dispose, take from free ice */
        if(dispose>epsilon && soil->ice_fw[l]>epsilon)
        {
          dispose2=min(soil->ice_fw[l],dispose);
          soil->ice_fw[l]-=dispose2; //(soil->w[l]*soil->whcs[l]+soil->w_fw[l]+soil->ice_depth[l]+soil->ice_fw[l])-(soil->wsats[l]-soil->wpwps[l]);
          soil->ice_fw[l]=max(0,soil->ice_fw[l]);
          excess+=dispose2;
          dispose-=dispose2;
        }
        if(dispose>epsilon && soil->w[l]*soil->whcs[l]>epsilon)
        {
          dispose2=min(soil->w[l]*soil->whcs[l],dispose);
          soil->w[l]-=dispose2/soil->whcs[l];
          soil->w[l]=max(0,soil->w[l]);
          excess+=dispose2;
          dispose-=dispose2;
        }
        if(dispose>epsilon && soil->ice_depth[l]>epsilon)
        {
          dispose2=min(soil->ice_depth[l],dispose);
          soil->ice_depth[l]-=dispose2;
          soil->ice_depth[l]=max(0,soil->ice_depth[l]);
          excess+=dispose2;
          dispose-=dispose2;
        }
      }
      soil->bulkdens[l] = (1 - soil->wsat[l])*MINERALDENS;
      soil->k_dry[l] = (0.135*soil->bulkdens[l] + 64.7) / (MINERALDENS - 0.947*soil->bulkdens[l]);
      excess+=wmm+imm;
      /* check if plant available water and ice do not exceed 1.0 */
      dispose=soil->w[l] + soil->ice_depth[l]/soil->whcs[l] - 1.0;
      if(dispose > 0)
      {
        if(soil->w[l]*soil->whcs[l]>epsilon)
        {
          dispose2=min(soil->w[l],dispose);
          soil->w[l]-=dispose2;
          soil->w[l]=max(0,soil->w[l]);
          excess+=dispose2*soil->whcs[l];
          dispose-=dispose2;
        }
        if(dispose*soil->whcs[l]>epsilon && soil->ice_depth[l] > epsilon)
        {
          dispose2=min(soil->ice_depth[l]/soil->whcs[l],dispose);
          soil->ice_depth[l]-=dispose2*soil->whcs[l];
          soil->ice_depth[l]=max(0,soil->ice_depth[l]);
          excess+=dispose2*soil->whcs[l];
          dispose-=dispose2;
        }
      }
    } /* end of forrootsoillayer */

    stand->cell->balance.excess_water+=excess*standfrac;
    //stand->cell->discharge.drunoff+=excess*standfrac;
#ifdef DEBUG_WB
    foreachsoillayer(l)
    if (soil->w[l]< -epsilon || soil->w_fw[l]< -epsilon )
    {   fprintf(stderr,"\n\npedotransfer Cell (%s) soilwater=%.6f soilice=%.6f wsats=%.6f agtop_moist=%.6f\n",
          sprintcoord(line,&stand->cell->coord),allwater(soil,l),allice(soil,l),soil->wsats[l],soil->litter.agtop_moist);
      fflush(stderr);
      fprintf(stderr,"Soil-moisture layer %d negative: w:%g, fw:%g,lutype %s soil_type %s \n\n",
          l,soil->w[l],soil->w_fw[l],stand->type->name,soil->par->name);
    }
#endif

#ifdef CHECK_BALANCE
    w_after=soilwater(&stand->soil)+excess;
    if(fabs(w_before-w_after)>epsilon)
      fprintf(stderr,"ERROR pedotransfer: %.2f/%.2f water balance=%.10f=%.10f-%.10f (excess is %.10f) in pedotransfer() wmm %.10f imm %.10f.\n",
              stand->cell->coord.lon,stand->cell->coord.lat,fabs(w_before-w_after),w_before,w_after+excess,excess,wmm,imm);
#endif
  } /* end of if not ROCK */
#ifdef USE_TIMING
  timing_stop(PEDOTRANSFER_FCN,tstart);
#endif
} /* of 'pedotransfer' */

/* Reference: Saxton and Rawls (2006): Soil Water Characteristic Estimates by Texture and Organic Matter for Hydrologic Solutions, Soil Sci. Soc. Am. J. 70:1569-1578 */
