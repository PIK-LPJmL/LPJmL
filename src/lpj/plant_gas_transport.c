/**************************************************************************************/
/**                                                                                \n**/
/**         p l a n t _ g a s _ t r a n s p o r t . c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Calculates gas transport through plants                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"

#define np -0.5              /* parameter given in Riera et al. 1999*/
#define tiller_weight  0.22  /* [gc] PARAMETER*/
#define tiller_radius 0.003
#define tiller_por 0.7
#define water_min 0.01
//#define DEBUG

#ifdef DEBUG

static void printch4(const Real CH4[LASTLAYER])
{
  int i;
  for (i = 0; i<LASTLAYER; i++)
    printf(" %g", CH4[i] / soildepth[i] * 1000);
  printf("\n");
}

static void printO2(const Real O2[LASTLAYER])
{
  int i;
  for (i = 0; i<LASTLAYER; i++)
    printf(" %g", O2[i] / soildepth[i] * 1000);
  printf("\n");
}

#endif

void plant_gas_transport(Stand *stand,        /**< pointer to soil data */
                         Real airtemp,        /**< air temperature (deg C) */
                         Real pch4,           /**< atmospheric CH4 content (ppm) */
                         const Config *config /**< LPJmL configutation */
                        )
{
  Pft *pft;
  Pftgrass *grass;
  Real CH4_air, ScCH4, k_600, kCH4;
  Real O2_air, ScO2, kO2;
  Real soil_moist, V, epsilon_CH4, epsilon_O2;                /*in mm*/
  Real tillers, tiller_area, tiller_frac;
  Real CH4, CH4_plant, CH4_plant_all,CH4_rice;
  Real O2, O2_plant;
  Real Conc_new, soil_water_vol;
  int l, p;
#ifdef CHECK_BALANCE
  Real start = 0;
  Real end = 0;

  /* CH4 diffuses out and O2 diffuses into the soil. CH4 is also lossed via transpiration.
   * In practice this value is small.
   * By default upland veg. has small 5% porosity but this can be switched to be equal to inundated porosity.
   */
  start = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4;
#endif
  CH4_plant_all=0;
  CH4_air=p_s/R_gas/degCtoK(airtemp)*pch4*1e-6*WCH4; /*g/m3 methane concentration*/
  O2_air=p_s/R_gas/degCtoK(airtemp)*O2s*WO2;         /*g/m3 oxygen concentration*/
#ifdef DEBUG
  printf("plantgas before");
  printch4(stand->soil.CH4);
  printO2(stand->soil.O2);
  printf("CH4_air:%g O2_air:%g\n",CH4_air,O2_air);
#endif

  /* Calculate Schmidt number and gas transfer velocity in the*/
  /* top soil layer.*/
  /*--------------------------------------------------------*/
  if (airtemp >= 50)
    ScCH4 = 20;
  else
    ScCH4 = 1898 - 110.1 * airtemp + 2.834 * airtemp*airtemp - 0.02791 * airtemp*airtemp*airtemp;
  k_600 = 2.07;                        /*should be k_600=2.07 + 0.215 * pow(wind_speed,1.7);  HERE wind speed is set to zero*/
  kCH4 = k_600*pow((ScCH4 / 600), np);   //piston velocity
  O2_air = p_s / R_gas / degCtoK(airtemp)*O2s*WO2;   /*g/m3 oxygen concentration*/
  if (airtemp >= 40)
    ScO2 = 8;
  else
    ScO2 = 1800.6-120.1*airtemp+3.7818*airtemp*airtemp-0.047608 * airtemp*airtemp*airtemp;
  k_600 = 2.07;                        /*should be k_600=2.07 + 0.215 * pow(wind_speed,1.7);  HERE wind speed is set to zero*/
  kO2 = k_600*pow((ScO2/600),np);

  /*convert cm h-1 to m d-1*/
  kCH4 = kCH4/100*24;
  kO2 = kO2/100*24;
  CH4_rice=CH4_plant=0;
  foreachpft(pft, p, &stand->pftlist)
  {
    if (!istree(pft))
    {
      grass = pft->data;
      for (l = 0; l<LASTLAYER; l++)
      {
        soil_moist=getsoilmoist(&stand->soil,l);
        V=getV(&stand->soil,l);  /*soil air content (m3 air/m3 soil)*/
        epsilon_CH4=max(0.0001,V+soil_moist*stand->soil.wsat[l]*BCH4);
        epsilon_O2=max(0.0001,V+soil_moist*stand->soil.wsat[l]*BO2);
        soil_water_vol=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.wpwps[l]*(1-stand->soil.ice_pwp[l])+stand->soil.w_fw[l])/soildepth[l];  //in  m-3 *1000/1000/soildepth
        if (soil_water_vol>water_min)
        {
          tillers = grass->ind.leaf.carbon*pft->nind*pft->phen / tiller_weight;
          tiller_frac = tillers*pft->par->rootdist[l];
          tiller_area = max(0.001,tiller_radius*tiller_radius*M_PI*tiller_frac*tiller_por);
          Conc_new = 0;
          CH4 = stand->soil.CH4[l] / soildepth[l] / epsilon_CH4 * 1000;
          if (tiller_area>0 && soil_water_vol>epsilon)
          {
            Conc_new=CH4_air+(CH4-CH4_air)*exp(-kCH4/(soil_water_vol/tiller_area));
            //printf("Conc_new: %g exp_t:%g tiller_area: %g \n",Conc_new,exp(-kCH4/(soil_water_vol/tiller_area)),tiller_area);
            CH4_plant=(CH4-Conc_new)*(soildepth[l]*epsilon_CH4)/1000;
            stand->soil.CH4[l]-= CH4_plant;
            if(stand->soil.CH4[l]<0)
            {
              CH4_plant+=stand->soil.CH4[l];
              stand->soil.CH4[l]=0;
            }
            CH4_plant_all+=CH4_plant;
          }
          if(pft->par->id==config->rice_pft && CH4_plant>0)
            CH4_rice+=CH4_plant;
          /*OXYGEN*/
          Conc_new = 0;
          O2=stand->soil.O2[l]/soildepth[l]/epsilon_O2*1000;
          if (tiller_area>0 && soil_water_vol>epsilon)
          {
            Conc_new=O2_air+(O2-O2_air)*exp(-kO2/(soil_water_vol/soildepth[l]/tiller_area));
            O2_plant=(O2-Conc_new)*(soildepth[l]*epsilon_O2)/1000;
            stand->soil.O2[l]-= O2_plant;
            if(stand->soil.O2[l]<0) stand->soil.O2[l]=0;
          }
        }
      }
    }
  }
  if (CH4_plant_all>0)
  {
    getoutput(&stand->cell->output,CH4_EMISSIONS,config)+=CH4_plant_all*stand->frac;
    if(stand->type->landusetype==WETLAND)
      getoutput(&stand->cell->output,CH4_EMISSIONS_WET,config)+=CH4_plant_all*WC/WCH4;
    stand->cell->balance.aCH4_em+=CH4_plant_all*stand->frac;
    if(CH4_rice>0) stand->cell->balance.aCH4_rice+=CH4_rice*stand->frac;
    getoutput(&stand->cell->output,CH4_PLANT_GAS,config)+=CH4_plant_all*stand->frac;
    if(CH4_rice>0) getoutput(&stand->cell->output,CH4_RICE_EM,config)+=CH4_rice*stand->frac/stand->cell->balance.ricefrac;
    if((stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==AGRICULTURE  || stand->type->landusetype==SETASIDE_WETLAND) && (CH4_rice==0  || stand->soil.iswetland))
    {
      stand->cell->balance.aCH4_setaside+=CH4_plant_all*stand->frac;
      getoutput(&stand->cell->output,CH4_SETASIDE,config)+=CH4_plant_all*stand->frac;
    }
  }
  else
  {
    getoutput(&stand->cell->output,CH4_SINK,config)+=CH4_plant_all*stand->frac;
    stand->cell->balance.aCH4_sink+=CH4_plant_all*stand->frac;
  }
#ifdef DEBUG
  printf("plantgas after");
  printch4(stand->soil.CH4);
  printO2(stand->soil.O2);
#endif
#ifdef CHECK_BALANCE
  end = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4;
  if (fabs(start - end - CH4_plant_all*WC/WCH4)>epsilon) fprintf(stderr, "C_ERROR: plant_gas %g start:%g  end:%g Plant_gas_transp: %g\n", start - end - CH4_plant_all*WC/WCH4, start, end, CH4_plant_all*WC/WCH4);
#endif

} /* of 'plant_gas_transport' */
