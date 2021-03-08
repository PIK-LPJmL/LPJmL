/***************************************************************************/
/**                                                                       **/
/**         p l a n t _ g a s _ t r a n s p o r t . c                     **/
/**                                                                       **/
/**     Calculates gas transport through plants                           **/
/**                                                                       **/
/**     written by Sibyll Schaphoff                                       **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2017-01-30 11:19:45 +0200 (Mi, 06. Jul 201#$ **/
/**     By         : $Author:: sibylls                         $          **/
/**                                                                       **/
/***************************************************************************/
#include "lpj.h"
#include "soil.h"
#include "grass.h"

#define np -0.5							/*parameter given in Riera et al. 1999*/
#define tiller_weight  0.22             /*[gc] PARAMETER*/
#define tiller_radius 0.003
#define tiller_por 0.7
#define water_min 0.01

static void printch4(const Real CH4[LASTLAYER])
{
  int i;
  for (i = 0; i<LASTLAYER; i++)
    printf(" %g", CH4[i]);
  printf("\n");
}

//#define CHECK_BALANCE

void plant_gas_transport(Stand *stand,  /* pointer to soil data */
                         Real airtemp,  /* air temperature (deg C) */
                         Real pch4,     /* atmospheric CH4 content (ppm) */
                         const Config *config
                        )
{
  Pft *pft;
  Pftgrass *grass;
  Real CH4_air, ScCH4, k_600, kCH4;
  Real O2_air, ScO2, kO2;
  Real soil_moist, V, epsilon_CH4, epsilon_O2, soil_water_moist;                /*in mm*/
  Real tillers, tiller_area, tiller_frac;
  Real CH4, CH4_plant, CH4_plant_all;
  Real O2, O2_plant;
  Real Conc_new, soil_water_vol;
  int l, p;
#ifdef CHECK_BALANCE
  Real start = 0;
  Real ende = 0;

  /* CH4 diffuses out and O2 diffuses into the soil. CH4 is also lossed via transpiration.
  * In practice this value is small.
  * By default upland veg. has small 5% porosity but this can be switched to be equal to inundated porosity.*/
  start = standstocks(stand).carbon + soilmethane(&stand->soil);
#endif
  //printf("plantgas before");
  //printch4(stand->soil.CH4);
  CH4_plant_all=0;
  CH4_air=p_s/R_gas/(airtemp + 273.15)*pch4*1e-6*WCH4;             /*g/m3 methane concentration*/
  O2_air=p_s/R_gas/(airtemp + 273.15)*O2s*WO2;                     /*g/m3 oxygen concentration*/
                                                                         /*Calculate Schmidt number and gas transfer velocity in the*/
                                                                         /*top soil layer.*/
                                                                         /*--------------------------------------------------------*/
  if (airtemp >= 50)
    ScCH4 = 20;
  else
    ScCH4 = 1898 - 110.1 * airtemp + 2.834 * airtemp*airtemp - 0.02791 * airtemp*airtemp*airtemp;
  k_600 = 2.07;                        /*should be k_600=2.07 + 0.215 * pow(wind_speed,1.7);  HERE wind speed is set to zero*/
  kCH4 = k_600*pow((ScCH4 / 600), np);   //piston velocity
  O2_air = p_s / R_gas / (airtemp + 273.15)*O2s*WO2;   /*g/m3 oxygen concentration*/
  if (airtemp >= 40)
    ScO2 = 8;
  else
    ScO2 = 1800.6-120.1*airtemp+3.7818*airtemp*airtemp-0.047608 * airtemp*airtemp*airtemp;
  k_600 = 2.07;                        /*should be k_600=2.07 + 0.215 * pow(wind_speed,1.7);  HERE wind speed is set to zero*/
  kO2 = k_600*pow((ScO2/600),np);

  /*convert cm h-1 to m d-1*/
  kCH4 = kCH4/100*24;
  kO2 = kO2/100*24;

  foreachpft(pft, p, &stand->pftlist)
  {
    if (!istree(pft))
    {
      grass = pft->data;
      for (l = 0; l<LASTLAYER; l++)
      {
        soil_moist=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.ice_depth[l]+stand->soil.ice_fw[l]+stand->soil.wpwps[l]+stand->soil.w_fw[l])/stand->soil.wsats[l];
        V=(stand->soil.wsats[l]-(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.ice_depth[l]+stand->soil.ice_fw[l]+stand->soil.wpwps[l]+stand->soil.w_fw[l]))/soildepth[l];  /*soil air content (m3 air/m3 soil)*/
        epsilon_CH4=max(0.0004,V+soil_moist*stand->soil.wsat[l]*BCH4);
        epsilon_O2=max(0.0004,V+soil_moist*stand->soil.wsat[l]*BO2);
        soil_water_vol=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.wpwps[l]*(1-stand->soil.ice_pwp[l])+stand->soil.w_fw[l]);
        if (soil_water_vol/soildepth[l]>water_min)
        {
          tillers = grass->ind.leaf.carbon*pft->nind*pft->phen / tiller_weight;
          tiller_frac = tillers*pft->par->rootdist[l];
          tiller_area = tiller_radius*tiller_radius*M_PI*tiller_frac*tiller_por;
          Conc_new = 0;
          CH4 = stand->soil.CH4[l] / soildepth[l] / epsilon_CH4 * 1000;
          if (CH4>CH4_air && tiller_area>0 && soil_water_vol>epsilon)
          {
            Conc_new=CH4_air+(CH4-CH4_air)*exp(-kCH4/(soil_water_vol/tiller_area));
            CH4_plant=(CH4-Conc_new)*(soildepth[l]*epsilon_CH4)/1000;
            stand->soil.CH4[l]-= CH4_plant;
            CH4_plant_all+=CH4_plant;
          }
          /*OXYGEN*/
          Conc_new = 0;
          O2=stand->soil.O2[l]/soildepth[l]/epsilon_O2*1000;
          if (O2<O2_air && tiller_area>0 && soil_water_vol>epsilon)
          {
            Conc_new=O2_air+(O2-O2_air)*exp(-kO2/(soil_water_vol/soildepth[l]/tiller_area));
            O2_plant=(O2-Conc_new)*(soildepth[l]*epsilon_O2)/1000;
            stand->soil.O2[l]-= O2_plant;
          }
        }
      }
    }
  }
  if (CH4_plant_all>0)
  {
    getoutput(&stand->cell->output,CH4_EMISSIONS,config)+=CH4_plant_all*stand->frac;
    stand->cell->balance.aCH4_em+=CH4_plant_all*stand->frac;
  }
  else
  {
    getoutput(&stand->cell->output,CH4_SINK,config)-=CH4_plant_all*stand->frac;
    stand->cell->balance.aCH4_sink-=CH4_plant_all*stand->frac;
  }
  getoutput(&stand->cell->output,CH4_PLANT_GAS,config)+=CH4_plant_all*stand->frac;
  //printf("plantgas after");
  //printch4(stand->soil.CH4);
#ifdef CHECK_BALANCE
  ende = standstocks(stand).carbon + soilmethane(&stand->soil);
  if (fabs(start - ende - CH4_plant_all)>epsilon) fprintf(stdout, "C-ERROR: %g start:%g  ende:%g Plant_gas_transp: %g\n", start - ende - CH4_plant_all, start, ende, CH4_plant_all);
#endif
}
