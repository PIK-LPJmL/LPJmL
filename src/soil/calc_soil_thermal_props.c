/*
The function calculates thermal properties of the soil for the different vertical layers.
It returs volumetric heat capacity, and thermal conductivity for the soil in frozen and 
unfrozen state based on soil texture and water content.
For the conductivity it uses the approach described by Johansen (1977)
*/
 #define pow10f(x) exp(2.302585092994046*x)  

#define K_SOLID 8       /* Thermal conductivity of solid components in saturated state */
#define K_ICE   2.2     /* Thermal conductivity of ice */
#define K_WATER 0.57    /* Thermal conductivity of liquid water*/

#define K_SOLID_log 0.90308998699      /* Thermal conductivity of solid components in saturated state */
#define K_ICE_log   0.34242268082     /* Thermal conductivity of ice */
#define K_WATER_log -0.24412514432    /* Thermal conductivity of liquid water*/

#include "lpj.h"

double fastPow(double a, double b) {
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}

void calc_soil_thermal_props(Soil_thermal_prop *th,  /*< Soil thermal property structure that is set or modified */
                     const Soil *soil,               /*< Soil structure from which water content etc is obtained  */
                     const Real *waterc_abs,         /*< Absolute total water content of soillayers (including ice) */
                     const Real *solc_abs,           /*< Absolute total content of solids of soillayers*/
                     Bool johansen,                  /*< Flag to activate johansen method */
                     Bool with_conductivity          /*< Flag to activate conductivity update  */
                     ) 
{
  int  layer, j;
  Real c_froz, c_unfroz;             /* frozen and unfrozen heat capacities */
  Real lam_froz, lam_unfroz;         /* frozen and unfrozen conductivities */
  Real latent_heat;                  /* latent heat of fusion depending on water content */
  Real lam_sat_froz, lam_sat_unfroz; /* frozen and unfrozen conductivities for saturated soil */
  Real waterc_abs_layer;             /* total absolute water content of soil */
  Real solidc_abs_layer;             /* total absolute solid content of soil */
  Real sat;                          /* saturation of soil */
  Real ke_unfroz, ke_froz;           /* kersten number for unfrozen and frozen soil */
  Real por;                          /* porosity of the soil */

  /* thermal resistance from last (resp first) layer to layer border*/
  Real resistance_froz_prev=0, resistance_froz_cur=0,resistance_unfroz_prev=0,resistance_unfroz_cur=0;              
  Real prev_length_to_border =0, cur_length_to_border;
  Real soillayer_depth_m;
  Real tmp;                          /* temporary variable */

  for (layer = 0; layer < NSOILLAYER; ++layer) {

    if (!johansen)
    {
     // fail(-1, TRUE, "Currently only the Johansen method to\ 
     //                 calculate soil thermal conductivities is implemented.");
     printf("Only Johansen implemented");

    }

    /* get absolute water and solid content of soil */
    if(waterc_abs == NULL){
      waterc_abs_layer = allwater(soil,layer)+allice(soil,layer);
    }
    else 
      waterc_abs_layer = waterc_abs[layer];

    if(solc_abs == NULL)
      solidc_abs_layer = soildepth[layer] - soil->wsats[layer];
    else 
      solidc_abs_layer = solc_abs[layer];    
     
    if(with_conductivity)
    {
      /* get frozen and unfrozen conductivity with johansens approach */
      por            = soil -> wsat[layer];
      tmp =  K_SOLID_log* (1 - por);
      lam_sat_froz   = pow(10, tmp + K_ICE_log * por); /* geometric mean  */
      lam_sat_unfroz = pow(10, tmp + K_WATER_log * por);
      if(soil->wsats[layer]<epsilon)
        sat=0;
      else
        sat        =  waterc_abs_layer / soil->wsats[layer];
      ke_unfroz  = (sat < 0.1 ? 0 : log10(sat) + 1); /* fine soil parametrisation of Johansen */
      ke_froz    =  sat;
      lam_froz   = (lam_sat_froz   - soil->k_dry[layer]) * ke_froz   + soil->k_dry[layer]; 
      lam_unfroz = (lam_sat_unfroz - soil->k_dry[layer]) * ke_unfroz + soil->k_dry[layer];
    }
    /* get frozen and unfrozen volumetric heat capacity */
    c_froz   = (c_mineral * solidc_abs_layer + c_ice   * waterc_abs_layer) / soildepth[layer];
    c_unfroz = (c_mineral * solidc_abs_layer + c_water * waterc_abs_layer) / soildepth[layer];

    /* get volumetric latent heat   */
    latent_heat = waterc_abs_layer / soildepth[layer] * c_water2ice;
    cur_length_to_border = (soildepth[layer]/1000)/(GPLHEAT*2);
    resistance_froz_cur =  cur_length_to_border/lam_froz;
    resistance_unfroz_cur = cur_length_to_border/lam_unfroz;
    for (j = 0; j < GPLHEAT; ++j) { /* iterate through gridpoints of the layer */

      if(with_conductivity)
      {
        /* set properties of j-th layer element */
        /* maximum element refernced = GPLHEAT*(NSOILLAYER-1)+GPLHEAT-1 = NHEATGRIDP-1 */


        if(j==0){
          th->lam_frozen[GPLHEAT * layer + j]   = 
          (prev_length_to_border+cur_length_to_border)/(resistance_froz_cur+resistance_froz_prev);
          th->lam_unfrozen[GPLHEAT * layer + j] =  
          (prev_length_to_border+cur_length_to_border)/(resistance_unfroz_cur+resistance_unfroz_prev);
        }else{
          th->lam_frozen[GPLHEAT * layer + j]   = lam_froz;
          th->lam_unfrozen[GPLHEAT * layer + j] = lam_unfroz;
        }
        
      }
      /* set properties of j-th layer gridpoint */
      th->c_frozen [GPLHEAT * layer + j]    = c_froz;
      th->c_unfrozen[GPLHEAT * layer + j]   = c_unfroz;
      th->latent_heat[GPLHEAT * layer + j]  = latent_heat;
    }
    resistance_froz_prev=resistance_froz_cur;
    resistance_unfroz_prev=resistance_unfroz_cur;
    prev_length_to_border=cur_length_to_border;
  }
}
