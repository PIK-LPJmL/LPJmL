/**************************************************************************************/
/**                                                                                \n**/
/**                        a r e a _ b u r n t . c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function calculates burnt area based on fire duration and rate of spread       \n**/
/**                                                                                \n**/
/** These are empiral derived equations from experimental sites in Canada          \n**/
/** (reference: CFFBG: Development and structure of the Canadian Forest Fire       \n**/
/** Behaviour Predictions Systems, Forestry Canada Fire Danger Group, 1992)        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define  max_fireduration_from_queue(queue) getmaxitemqueue(queue,4) /* 4 is the index of the last element in datatype Fire */

/* Function calculates length to breadth ratio of assumed elliptical fire shape */

static Real getlength_breadth_ratio(Real windsp_cover,     /**< mid-flame wind speed (m/min) */
                                    Real fpc_sum,          /**< sum of all FPCs */
                                    const Real fpc_total[] /**< plant-type specfic FPC sums */
                                   )                       /** \return length to breadth ratio */
{
  Real length_breadth_ratio;
  Real lb_grass,lb_tree,base,lb_crop;
  if(fpc_sum==0)
    return 0;
  if (windsp_cover < 16.67)
    return 1;
  windsp_cover*=0.06; /* Conversion of units from m/min to km/h of windspeed */
  base=1.0-(exp(-0.03*windsp_cover));
  /* total tree fpc and total grass fpc for dampening effect on wind speed */
  lb_tree=fpc_total[TREE]*(1.0+(8.729*(pow(base,2.155))));
  lb_grass=fpc_total[GRASS]*(1.1+pow(windsp_cover,0.464));
  lb_crop=fpc_total[CROP]*(1.1+pow(windsp_cover,0.464));
  length_breadth_ratio=(lb_tree+lb_grass+lb_crop)/fpc_sum;
  if (length_breadth_ratio > 8)
    length_breadth_ratio = 8;
  return length_breadth_ratio;
} /* of 'getlength_breadth_ratio' */

Real area_burnt(Real *fire_durat,       /**< fire duration (min) */
                Real *ndayfire,         /**< number of days with fire */
                Real *firedurationdays, /**< mean number of days a fire burns */
                Real *burnt_area_max,   /**< size of fires in the largest active fire class (ha)*/
                const Real fireduration[2],   /**< fire duration interval (min) */
                Real fire_danger_index, /**< fire danger index (0..1) */
                Real num_fires,         /**< number of fires */
                Real windsp_cover,      /**< mid-flame wind speed (m/min) */
                Real ros_forward,       /**< rate of spread (m/min) */
                int ntypes,             /**< number of PFT types */
                Stand *stand,           /**< pointer to stand */
                Bool ismaxfire          /**< maximum fire size enabled (TRUE/FALSE) */
               )                        /** \return area burnt (hectare) */
{
  Real dbf=0,length_breadth_ratio;
  Real ros_backward;
  Real *fpc_total; /* total grid FPC for PFTs */
  Real d_area_burnt=0;
  Real burnt_area_sum=0;
  Fire fire={0,0,0,0};
  Real wind_cover_cum;
  Real fpcsum;
  int i;
  if(ros_forward<=0)
    ros_forward=0;
  /* calculating backward (against the wind) rate of spread */
  ros_backward = ros_forward * exp(-0.012 * windsp_cover); /*Can FBP System*/
  if (ros_backward <= 0.05)
    ros_backward = 0.0;

  fpc_total=newvec(Real,ntypes);
  check(fpc_total);
  fpcsum=fpc_sum(fpc_total,ntypes,&stand->pftlist);
  length_breadth_ratio=getlength_breadth_ratio(windsp_cover,fpcsum,fpc_total);

  if(length_breadth_ratio <= 0)
  {
    burnt_area_sum = 0;
    *fire_durat=0;
  }
  else
  {
    /*  fire duration as a function of daily fire danger index */
    *fire_durat=(fireduration[1]+1)/(1.0+((fireduration[1]/fireduration[0]-1)*exp(param.firedura*fire_danger_index)));
    dbf = (ros_backward+ros_forward) * *fire_durat;  /* length of the major axis of the fire ellipse dbf in m*/
    d_area_burnt = (num_fires * M_PI_4/length_breadth_ratio * dbf*dbf)*1e-4;
    burnt_area_sum = d_area_burnt;
#ifdef DEBUG
   if (d_area_burnt>0)
     printf("normal burnt area= %g, fire_dura=%g,num_fires=%g,max_dura=%g,fdi=%g\n",
            burnt_area_sum,*fire_durat,num_fires,max_fireduration,fire_danger_index);
#endif
  }
  *burnt_area_max=0;
  if(stand->type->max_ndayfire==0)
    *ndayfire=(*fire_durat==0) ? 0 : 1;
  else
  {
    *ndayfire=0;
    if (fire_danger_index < 0.15) // condition for fire extinction
      for(i=0;i<stand->type->max_ndayfire;i++)
        setqueue(stand->fires,(Real *)&fire,i); /* set all values to zero */
    else
    {
      wind_cover_cum=0;
      /* loop over increasing age of fires in days */
      for(i=0;i<stand->type->max_ndayfire;i++)
      {
        getqueue(stand->fires,(Real *)&fire,i); /* get fire properties up to today for fire age i+1 days */
        wind_cover_cum+=fire.wind_cover;
        fire.dbf += (ros_backward+ros_forward) * *fire_durat;  /* fire duration in min , dbf in m*/
        /* calculating length breadth ratio based on average wind speed over the fire's spread days */
        length_breadth_ratio=getlength_breadth_ratio((windsp_cover+wind_cover_cum)/(i+2),fpcsum,fpc_total);
        if(length_breadth_ratio<=0) /* avoid divide by zero */
          fire.burnt_area=0;
        else
        {
          /* applying maximum fire size condition if terrain based fire spread limits are enabled */
          if(ismaxfire && M_PI_4/length_breadth_ratio * fire.dbf*fire.dbf*1e-4 > stand->cell->max_firesize)
          {
            /* getting daily burnt area by adding increase in size from all duration classes */
            burnt_area_sum += max(0,fire.num_fires*stand->cell->max_firesize - fire.burnt_area);
            fire.burnt_area=fire.num_fires*stand->cell->max_firesize;
          }
          else
          {
            /* getting daily burnt area by adding increase in size from all duration classes */
            burnt_area_sum += max(0,(fire.num_fires * M_PI_4/length_breadth_ratio * fire.dbf*fire.dbf)*1e-4 - fire.burnt_area);
            fire.burnt_area = (fire.num_fires * M_PI_4/length_breadth_ratio * fire.dbf*fire.dbf)*1e-4;
          }
          /* getting size of the biggest fire */
          *burnt_area_max=max(*burnt_area_max,(fire.num_fires>0) ? fire.burnt_area/fire.num_fires : 0.0);
        }
#ifdef DEBUG
        if (fire.burnt_area>0)
          printf("burnt area: %g, on burn day: %d burnt area sum= %g\n", (fire.num_fires * M_PI_4/length_breadth_ratio * fire.dbf*fire.dbf)*1e-4 - fire.burnt_area, i, burnt_area_sum);
#endif
        /* fire class emptied if maximum fire size condition is met */
        if((ismaxfire && M_PI_4/length_breadth_ratio * fire.dbf*fire.dbf*1e-4 > stand->cell->max_firesize))
          fire.burnt_area=fire.dbf=fire.wind_cover=fire.num_fires=0;

        if(fire.burnt_area>0)
        {
          *ndayfire = 1; /* identify current day as a fire spread day */
        }
        setqueue(stand->fires,(Real *)&fire,i); /* update fire properties of current fire age class */
      } /* of for(i=0,...) */
    }
    /* adding fires that started today to the queue */
    fire.burnt_area=d_area_burnt;
    fire.num_fires=num_fires;
    fire.dbf=dbf;
    fire.wind_cover=windsp_cover;
    if(stand->type->landusetype==NATURAL)
    {
#ifdef DEBUG
      printf("d_area_burnt: %g, num_fires: %d, dbf: %g, wind_cover:%g\n",fire.burnt_area, fire.num_fires,fire.dbf,fire.wind_cover);
#endif
      *firedurationdays=max_fireduration_from_queue(stand->fires);
    }
    else
      *firedurationdays=0;
    putqueue(stand->fires,(Real *)&fire);
  }

  free(fpc_total);
  return burnt_area_sum;
} /* of 'area_burnt' */
