#include "lpj.h"
#include <errno.h>

void freezefrac2soil(Soil *soil, 
                     const Real *freezfrac
                    )
{
  int layer;

  Real tar_ice;   /* target absolute ice content of layer (or ice content remaining)*/
  Real tar_ice_o; /* Only for TESTING: Save original tar_ice  value*/
  Real t_wi;      /* absolute total waterice content of layer */
  Real n_wi;      /* normal soil waterice, absolute  */
  Real f_wi;      /* free soil waterice, absolute  */
  Real wp_wi;     /* below wiltingpoint soil waterice, absolute  */
  
  Real frac;      /* generic fraction */

  for(layer=0;layer<NSOILLAYER;++layer){

    soil->freeze_depth[layer] = freezfrac[layer] * soildepth[layer];

    /* get the absolute quantities */
    t_wi  = allwater(soil,layer) + allice(soil,layer);
    if(t_wi<epsilon)
      continue; /* Without any water there is nothing to do */
    n_wi  = soil->w[layer] * soil->whcs[layer] + soil->ice_depth[layer];
    wp_wi = soil->wpwps[layer];
    f_wi  = soil->w_fw[layer] + soil->ice_fw[layer];

    /* do a sanity check  */
    if(fabs(t_wi-(n_wi+wp_wi+f_wi))>0.0000001){
        printf("ERROR: Waterice contents not summing up to total\n" );
        exit(1);
        //printf("t_i %f, n_wi+wp_wi+f_wi %f \n", t_wi, n_wi+wp_wi+f_wi);
    }

    /* set the target */
    tar_ice   = t_wi * freezfrac[layer];
    tar_ice_o = tar_ice;
    
    /* Freeze n_wi and wp_wi first*/
    if((wp_wi+n_wi)<tar_ice )
    { /* wp_wi and n_wi will be completly ice */
      tar_ice -= wp_wi+n_wi;
      soil->ice_depth[layer] = n_wi;
      soil->w[layer] = 0.0;
      soil->ice_pwp[layer] = 1.0;
    }
    else
    { /* wp_wi and n_wi will be partly ice */
      frac = tar_ice/(wp_wi+n_wi);
      tar_ice = 0.0; 
      soil->ice_depth[layer] = frac     * n_wi;
      soil->w[layer]         = (1-frac) * n_wi / soil->whcs[layer];
      soil->ice_pwp[layer]=frac;
      soil->ice_fw[layer] = 0.0;
      soil->w_fw[layer]   = f_wi;
    }

    /* Only freeze f_wi if still needed */
    if(tar_ice>epsilon)
    {
      if(f_wi>0){
        frac = tar_ice/f_wi;
      }
      else{
        printf("ERROR, target ice not yet met, but there is no more water to freeze.\n");
        exit(1);
      }
      soil->ice_fw[layer] = frac     * f_wi;
      soil->w_fw[layer]   = (1-frac) * f_wi;
    }

    /* Check if the target was met */
    // if(fabs(tar_ice_o-
    //   (soil->ice_fw[layer]+soil->ice_depth[layer]+soil->ice_pwp[layer]*soil->wpwps[layer]))
    //   >0.000001 ){
    //   printf("ERROR, Target ice content was not met.\n" );
    //   exit(1);
    // }
    // /* Check if no water is lost */
    // if(fabs(soil->ice_fw[layer]+soil->w_fw[layer]-f_wi)>0.00001 ||
    //    fabs(soil->w[layer]*soil->whcs[layer]+soil->ice_depth[layer]-n_wi)>0.00001 ){
    //   printf("ERROR, Water ist lost during conversion process.\n" );
    //   exit(1);
    // }
  }
}