/**************************************************************************************/
/**                                                                                \n**/
/**                    s  o  i  l  t  e  m  p  .  c                                \n**/
/**                                                                                \n**/
/**     More advanced soil temperature and moisture sceme                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/*#define USE_LINEAR_CONTACT_T  */ /*linear interpolation between temperatures seems to give a reasonable approximation of contact temperatures between layers*/

Real soiltemp_lag(const Soil *soil,      /**< Soil data */
                  const Climbuf *climbuf /**< Climate buffer */
                 )                       /** \return soil temperature (deg C) */
{
  Real a,b,temp_lag;
  if(soil->w[0]<epsilon)
    return climbuf->temp[NDAYS-1];
  linreg(&a,&b,climbuf->temp,NDAYS);
  temp_lag=a+b*(NDAYS-1-soil->alag*LAG_CONV);
  return climbuf->atemp_mean+soil->amp*(temp_lag-climbuf->atemp_mean);
} /* of 'soiltemp_lag' */

/* heat conduction equation: dT/dt = th_diff*d2T/dz2
 * is solved with a finite-difference solution
 * algorithm and stability criterion are taken from:
 * Y.Bayazitoglu / M.N.Oezisik: Elements of Heat Transfer (1988)
 */
 
void soiltemp(Soil *soil,   /**< pointer to soil data */
              Real airtemp  /**< air temperature (deg C) */             
             )
{
  Real th_diff[NSOILLAYER],      /* thermal diffusivity [m2/s]*/
       heatcap,                  /* heat capacity [J/m2/K] or [J/m3/K]*/
       lambda[NSOILLAYER],       /* thermal conductivity [W/K/m]*/
       t_upper,                  /* temperature of upper soil/air/snow layer*/
       t_lower,                  /* temperature of lower soil layer*/
       admit_upper,admit_lower,  /* thermal admittance*/
       admit[NSOILLAYER],
       t_contact_u, t_contact_l, /* contact temperatures between two layers [deg C]*/
       dT,                       /* change in temperature [K] */
       heat,heat2,               /* energy for water/ice conversions [J/m2]*/
       dt,                       /* change in time [s]*/
       z0;                       /* depth of hypothetic Zero-degree Isotherme (assuming homogeneous soil properties)*/
  Real layer;
  int l;
  unsigned long int heat_steps[NSOILLAYER],t; 

  layer=heat=0;
  /*temperature change of upper soillayer by precipitation, of lower layers by percolation water*/
  for(l=0; l<NSOILLAYER;l++)
  {
#ifdef MICRO_HEATING
    if (fabs(soil->perc_energy[l])>epsilon || fabs(soil->micro_heating[l])>epsilon)
#else
    if (fabs(soil->perc_energy[l])>epsilon)
#endif
    {
      heat=soil->perc_energy[l]; /*[J/m2]*/
#ifdef MICRO_HEATING
      heat+=soil->micro_heating[l];
      soil->micro_heating[l]=0;
#endif
      soil->perc_energy[l]=0;
      heatcap=soilheatcap(soil,l); /*[J/m2/K]*/
      if(heat>epsilon)
      {
        if (soil->state[l]==BELOW_T_ZERO || 
            soil->state[l]==THAWING ||
            soil->state[l]==AT_T_ZERO)
        {
          dT=min(heat/heatcap,T_zero-soil->temp[l]);
          heat2=heat-dT*heatcap;
          heat-=heat2;
          if(heat2>epsilon && allice(soil,l)>epsilon)
            soilice2moisture(soil,&heat2,l);
          heat+=heat2;   
        }            
      }
      else
      {
        if (soil->state[l]==ABOVE_T_ZERO ||
            soil->state[l]==FREEZING ||
            soil->state[l]==AT_T_ZERO)
        {
          dT=max(heat/heatcap,T_zero-soil->temp[l]);
          heat2=heat-dT*heatcap;
          if(heat2<-epsilon && allwater(soil,l)>epsilon)
            moisture2soilice(soil,&heat2,l);
          heat+=heat2; 
        }          
      }
      soil->temp[l]+=heat/heatcap;
    } /*endif (precipitation or percolation energy present)*/
  }/*endfor each soil layer*/
  
  /*thermal properties for each soillayer*/
  for(l=0;l<NSOILLAYER;++l)
  {
    /* calculate thermal diffusivities */
    heatcap=soilheatcap(soil,l)/soildepth[l]*1000.; /*[J/m3/K]*/
    lambda[l]=soilconduct(soil,l);
    th_diff[l]=lambda[l]/heatcap; /* assuming no change in thermal diffusivity in this timestep/layer */
#ifndef USE_LINEAR_CONTACT_T
    admit[l]=sqrt(lambda[l]*heatcap);
#endif
    /* stability criterion for finite-difference solution; */
    dt = 0.5*(soildepth[l]*soildepth[l]*1e-6)/th_diff[l];
    heat_steps[l]=(unsigned long)(timestep2sec(1.0,NSTEP_DAILY)/dt)+1;
    /* convert any latent energy present in this soil layer */
    if((soil->state[l]==BELOW_T_ZERO && allwater(soil,l)>epsilon)
        || (soil->state[l]==ABOVE_T_ZERO && (allice(soil,l)>epsilon)))
    {
      heat=0;
      convert_water(soil,l,&heat);
      soil->state[l]=(short)getstate(soil->temp+l);
    }  
  } /* of 'for(l=0;...)' */
 
  t_upper=airtemp;
  admit_upper=admit[0];
  /* calculate soil temperatures */
  for(l=0;l<NSOILLAYER;++l)
  {
    t_lower=(l==BOTTOMLAYER) ? t_upper : soil->temp[l+1];
    admit_lower=(l==BOTTOMLAYER) ? admit[l] : admit[l+1];
    for (t=0; t<heat_steps[l];++t)
    {
      /* temperature change during this timestep*/
      dT=th_diff[l]*timestep2sec(1.0,heat_steps[l])/(soildepth[l]*soildepth[l])*1000000
          *(t_upper+t_lower-2*soil->temp[l]);
      if((dT>-epsilon&&dT<epsilon) || t==maxheatsteps)
        break;
      if(soil->temp[l]*t_upper>0 && t_upper*t_lower>0 && (soil->temp[l]+dT)*t_upper>0)
      {
        soil->temp[l]+=dT;
        soil->state[l]=(short)getstate(soil->temp+l);
      }
      /* consider freezing / thawing processes */
      else
      {
        /* contact temperatures between soil layers
         * are used to determine, which layer dominates interface processes
         * and thus, whether there should be freezing/thawing at one boundary
         */
#ifndef USE_LINEAR_CONTACT_T
        /* see Campbell/Norman:Environmental Biophysics, 2nd Ed. 
         * for a definition of thermal admittance
         */
        t_contact_u=(admit_upper*t_upper+admit[l]*soil->temp[l])/(admit_upper+admit[l]);
        t_contact_l=(admit_lower*t_lower+admit[l]*soil->temp[l])/(admit_lower+admit[l]);
#else
        /* TODO: test performance / accuracy of linear approximation
         * seems to be not very much faster and curves do not look as nice
         */
        depth=(l==TOPLAYER) ? 0 : soildepth[l-1];
        t_contact_u=(t_upper-soil->temp[l])/(0.5*(soildepth[l]+depth))*0.5*soildepth[l]+soil->temp[l];
        if(fabs(t_contact_u)<epsilon)
          t_contact_u=0.0;
        depth=(l==BOTTOMLAYER) ? 0: soildepth[l+1];
        t_contact_l=(t_lower-soil->temp[l])/(0.5*(soildepth[l]+depth))*0.5*soildepth[l]+soil->temp[l];
        if(fabs(t_contact_l)<epsilon)
          t_contact_l=0.0;
#endif
        /* depth of 0-degree isotherme, if thermal properties were homogeneous 
         * across l and no freezing/thawing occured
         * is used to determine proportion of energy for water conversion
         */
        /* TODO:they may be both < 0 (but error is probably small)*/
        if(t_contact_u*soil->temp[l]<0)
          z0=0.5*soildepth[l]-(-soil->temp[l]*0.5*soildepth[l]/(t_upper-soil->temp[l]));   
        else if (t_contact_l*soil->temp[l]<0)
          z0=0.5*soildepth[l]+(-soil->temp[l]*0.5*soildepth[l]/(t_lower-soil->temp[l]));
        else
          z0=soildepth[l];
        if(z0<soildepth[l])
        {
          /* phase transition in this layer */
          /* use part of available energy for water conversion*/                
          heat=z0/soildepth[l]*lambda[l]*dT/soildepth[l]*1000*timestep2sec(1.0,heat_steps[l]);       
          if (dT>0 && allice(soil,l)>epsilon)
          {
            if(getstate(soil->temp+l)==ABOVE_T_ZERO)
              convert_water(soil,l,&heat);
            else
              soilice2moisture(soil, &heat, l);
            soil->state[l]=THAWING;                                                 
          }
          else if (heat < 0 && allwater(soil,l)>epsilon)
          {
            if(getstate(soil->temp+l)==BELOW_T_ZERO)
              convert_water(soil,l,&heat);
            else
              moisture2soilice(soil, &heat, l);
            soil->state[l]=FREEZING;
          }                   
          dT=(1-z0/soildepth[l])*dT+heat/lambda[l]*(soildepth[l]*1e-3)/timestep2sec(1.0,heat_steps[l]);
        }
        else
        { /*whole layer is heated*/
          /* energy corresponding to dT*/
          heat=lambda[l]*dT/soildepth[l]*1000*timestep2sec(1.0,heat_steps[l]);
          convert_water(soil,l,&heat);
          /*energy left after water conversion changes temperature*/
          dT=heat*(soildepth[l]*1e-3)/lambda[l]/timestep2sec(1.0,heat_steps[l]);
          soil->state[l]=(short)getstate(soil->temp+l);
        }
        soil->temp[l]+=dT;
        if(soil->state[l]!=FREEZING && soil->state[l]!=THAWING)
          soil->state[l]=(short)getstate(soil->temp+l);
      } /* endif (freezing/thawing occurs)*/
    }/*foreach heat step*/
    t_upper=soil->temp[l];
    admit_upper=admit[l];   
  }/*foreach soil layer*/
  for (l=0;l<=BOTTOMLAYER;l++)
  {
    layer+=soildepth[l];
    if(soil->freeze_depth[l]>epsilon)
      break;
  }
  if (soil->maxthaw_depth<layer-soil->freeze_depth[l])
    soil->maxthaw_depth=layer-soil->freeze_depth[l]; 
} /* of 'soiltemp' */             
