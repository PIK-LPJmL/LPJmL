/**************************************************************************************/
/**                                                                                \n**/
/**         i  n  f  i  l  _  p  e  r  c  _  i  r  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates water balance                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"
#include "crop.h"

#define NPERCO 0.4  /*controls the amount of nitrate removed from the surface layer in runoff relative to the amount removed via percolation.  0.5 in Neitsch:SWAT MANUAL*/



Real infil_perc_irr(Stand *stand,        /**< Stand pointer */
                    Real infil,          /**< infiltration water (mm) */
                    Real infil_vol_enth, /**< volumetric enthalpy contained in infil (J/m^3) */
                    Real *return_flow_b, /**< blue water return flow (mm) */
                    int npft,            /**< number of natural PFTs */
                    int ncft,            /**< number of crop PFTs */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return water runoff (mm) */
{
  Real runoff;
  Real perc,slug;
  Real TT; /*traveltime in [mm/h]*/
  Real HC; /*hydraulic conductivity in [mm/h]*/
  Real influx;
  Real frac_g_influx,soil_infil;
  Real outflux,grunoff,inactive_water[NSOILLAYER];
  Real runoff_surface,freewater,soildepth_irrig,deficit;
  Soil *soil;
  Real srunoff;
  Real lrunoff; /* intermediate variable for leaching of lateral runoff */
  Real NO3surf=0; /* amount of nitrate transported with surface runoff gN/m2 */
  Real NO3perc_ly=0; /* nitrate leached to next lower layer with percolation gN/m2 */
  Real NO3lat=0; /* amount of nitrate transported with lateral runoff gN/m2 */
  Real w_mobile=0; /* Water mixing with nutrient in layer mmH2O */
  Real concNO3_mobile; /* concentration of nitrate in solution gN/mm */
  Real vno3; /* temporary for calculating concNO3_mobile */
  Real ww; /* temporary for calculating concNO3_mobile */
  Real vol_water_enth=0; /* volumetric enthalpy of inflowing or outflowing water J/m^3 */

  int l,p;
  int infil_loop_count=1;
  Real updated_soil_water=0,previous_soil_water[NSOILLAYER];
  Irrigation *data_irrig;
  Pft *pft;
  Pftcrop *crop;
  String line;
  data_irrig=stand->data;

  soil=&stand->soil;
  influx=grunoff=perc=frac_g_influx=freewater=0.0;
  runoff_surface=runoff=outflux=0;
  soil_infil=param.soil_infil;
   /*infiltration*/
  if(config->rw_manage && (stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND ||
                           stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE ||
                           stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS))
    soil_infil=param.soil_infil_rw; /* parameter to increase soil infiltration rate */

  for(l=0;l<NSOILLAYER;l++)
  {
    freewater+=soil->w_fw[l];
    if (soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>1)
      freewater+=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
  }

  while(infil > epsilon || freewater > epsilon)
  {
    NO3perc_ly=0;
    freewater=0.0;
    slug=min(4,infil);
    infil=infil-slug;
    if(data_irrig->irrig_system==SPRINK || data_irrig->irrig_system==DRIP)
      influx=slug;        /*no surface runoff for DRIP and Sprinkler*/
    else
    {
      if(1-(soil->w[0]*soil->whcs[0]+soil->w_fw[0]+soil->ice_depth[0]+soil->ice_fw[0])/(soil->wsats[0]-soil->wpwps[0])>=0)
      influx=slug*pow(1-(soil->w[0]*soil->whcs[0]+soil->w_fw[0]+soil->ice_depth[0]+soil->ice_fw[0])/(soil->wsats[0]-soil->wpwps[0]),(1/soil_infil));
      else
        influx=0;
    }
    runoff_surface+=slug - influx;
    srunoff=slug-influx; /*surface runoff used for leaching */
    *return_flow_b+=slug - influx;
    frac_g_influx=0; /* first layer has only blue influx, but lower layers with percolation have mixed frac_g_influx */

    if(data_irrig->irrig_system==DRIP)
    {
      /* in case of Drip: directly fill up field cap of first two soil layers, no surface runoff, lateral runoff or percolation */
      /* -> this allows simulating perfect irrigation: drip + irrg_threshold = 1 (keep in mind: plant can still be somewhat stressed, if roots go deeper than 2. layer) */
      vol_water_enth=infil_vol_enth; /* set enthalpy of water flowing into top layer to the infil enthalpy */
      for(l=0;l<LASTLAYER && influx>epsilon;l++)
      {
        previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        soil->w[l]+=influx/soil->whcs[l];
        reconcile_layer_energy_with_water_shift(soil,l,influx,vol_water_enth,config); /* account for enthalpy of water inflow  */
        influx=max((soil->w[l]-1)*soil->whcs[l]+soil->ice_depth[l],0); /* if water plus ice is above capacity, excess water flows to next layer */
        soil->w[l]=min(soil->w[l],1-soil->ice_depth[l]/soil->whcs[l]); /* limit water content to capacity */
        vol_water_enth=(soil->temp[l]>=0?c_water:c_ice)*soil->temp[l]+(soil->temp[l]>=0?c_water2ice:0); /* set enthalpy of water flowing out of this layer and into the below layer */
        reconcile_layer_energy_with_water_shift(soil,l,-influx,vol_water_enth,config); /* account for enthalpy of water outflow  */

        /*update frac_g: new green fraction equals old green amount + new green amount divided by total water */
        updated_soil_water=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        if(updated_soil_water>previous_soil_water[l] && updated_soil_water>0)
          stand->frac_g[l]=(previous_soil_water[l]*stand->frac_g[l])/updated_soil_water;

      } /* soil layer loop */
      outflux+=influx;
      *return_flow_b+=influx;
      influx=0.0;
    }
    else
    {
      /* Sprinkler and Surface water infiltration */
      vol_water_enth=infil_vol_enth; /* set toplayer vol enth to the infil enthalpy */
      for(l=0;l<NSOILLAYER;l++)
      {
        previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        soil->w[l]+=(soil->w_fw[l]+influx)/soil->whcs[l];
        soil->w_fw[l]=0.0;
        reconcile_layer_energy_with_water_shift(soil,l,influx,vol_water_enth,config); /* account for enthalpy of water inflow  */
        lrunoff=0;
        inactive_water[l]=soil->ice_depth[l]+soil->wpwps[l]+soil->ice_fw[l];

        /*update frac_g to influx */
        updated_soil_water=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        if(previous_soil_water[l]-updated_soil_water>epsilon)
        {
          fprintf(stderr,"Cell (%s) infil_perc_irr.c error updated smaller then previous --- updated=  %3.12f --- previous=  %3.12f --- diff=  %3.12f\n",sprintcoord(line,&stand->cell->coord),updated_soil_water,previous_soil_water[l],updated_soil_water-previous_soil_water[l]);
          fflush(stderr);
        }
        if(updated_soil_water>previous_soil_water[l] && updated_soil_water>0)
          stand->frac_g[l]=(previous_soil_water[l]*stand->frac_g[l] + (updated_soil_water - previous_soil_water[l])*frac_g_influx)/updated_soil_water; /* new green fraction equals old green amount + new green amount divided by total water */

        /* lateral runoff of water above saturation */
        if ((soil->w[l]*soil->whcs[l])>(soildepth[l]-soil->freeze_depth[l])*(soil->wsat[l]-soil->wpwp[l]))
        {
          grunoff=(soil->w[l]*soil->whcs[l])-((soildepth[l]-soil->freeze_depth[l])*(soil->wsat[l]-soil->wpwp[l]));
          soil->w[l]-=grunoff/soil->whcs[l];
          reconcile_layer_energy_with_water_shift(soil,l,-min(grunoff,influx),vol_water_enth,config); /* subtract enth of runoff until influx, assuming vol_enth of above layer  */
          vol_water_enth=soil->freeze_depth[l]/soildepth[l]*(c_ice*soil->temp[l]) + (1-soil->freeze_depth[l]/soildepth[l])*(c_water*soil->temp[l]+c_water2ice);
          reconcile_layer_energy_with_water_shift(soil,l,-max(grunoff-influx,0),vol_water_enth, config); /* subtract enth of runoff above influx, assuming vol_enth of current layer */
          runoff+=grunoff;
          lrunoff+=grunoff;
          *return_flow_b+=grunoff*(1-stand->frac_g[l]);
        }
        if((inactive_water[l]+soil->w[l]*soil->whcs[l])>soil->wsats[l])
        {
          grunoff=(inactive_water[l]+soil->w[l]*soil->whcs[l])-soil->wsats[l];
          soil->w[l]-=grunoff/soil->whcs[l];
          reconcile_layer_energy_with_water_shift(soil,l,-min(grunoff,influx),vol_water_enth, config); /* subtract enth of runoff until influx, assuming vol_enth of above layer  */
          vol_water_enth=soil->freeze_depth[l]/soildepth[l]*(c_ice*soil->temp[l]) + (1-soil->freeze_depth[l]/soildepth[l])*(c_water*soil->temp[l]+c_water2ice);
          reconcile_layer_energy_with_water_shift(soil,l,-max(grunoff-influx,0),vol_water_enth, config); /* subtract enth of runoff above influx, assuming vol_enth of current layer */
          runoff+=grunoff;
          lrunoff+=grunoff;
          *return_flow_b+=grunoff*(1-stand->frac_g[l]);
        }
        influx=0.0;
        vol_water_enth=soil->freeze_depth[l]/soildepth[l]*(c_ice*soil->temp[l]) + (1-soil->freeze_depth[l]/soildepth[l])*(c_water*soil->temp[l]+c_water2ice);

        if (soildepth[l]>soil->freeze_depth[l])
        {
          /*percolation*/
          if((soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-param.percthres)>epsilon)
          {
            HC=soil->Ks[l]*pow(((soil->w[l]*soil->whcs[l]+inactive_water[l])/soil->wsats[l]),soil->beta_soil[l]);
            TT=((soil->w[l]-param.percthres)*soil->whcs[l]+soil->ice_depth[l])/HC;
            perc=((soil->w[l]-param.percthres)*soil->whcs[l]+soil->ice_depth[l])*(1-exp(-24/TT));
            /*correction of percolation for water content of the following layer*/
            if (l<BOTTOMLAYER)
            {
              if(1-(soil->w[l+1]*soil->whcs[l+1]+soil->w_fw[l+1]+soil->ice_depth[l+1]+soil->ice_fw[l+1])/(soil->wsats[l+1]-soil->wpwps[l+1])>=0)
              perc=perc*sqrt(1-(soil->w[l+1]*soil->whcs[l+1]+soil->w_fw[l+1]+soil->ice_depth[l+1]+soil->ice_fw[l+1])/(soil->wsats[l+1]-soil->wpwps[l+1]));
              else perc=0;
            }
#ifdef SAFE
            if (perc< 0)
              printf("perc<0 ; TT %3.3f HC %3.3f perc  %3.3f w[%d]  %3.7f\n",TT,HC,perc/soil->whcs[l],l,soil->w[l]);
            if (perc/soil->whcs[l]>(soil->w[l]+epsilon))
              printf("perc>w ; Cell (%s); TT %3.3f HC %3.3f perc  %3.7f w[%d]  %3.7f\n",
                     sprintcoord(line,&stand->cell->coord),TT,HC,perc/soil->whcs[l],l,soil->w[l]);
#endif
            soil->w[l]-=perc/soil->whcs[l];

            if (fabs(soil->w[l])< epsilon)
            {
              perc+=(soil->w[l])*soil->whcs[l];
              soil->w[l]=0;
            }
            reconcile_layer_energy_with_water_shift(soil,l,-perc,vol_water_enth,config); /* subtract enthalpy of percolating water */
            getoutputindex(&stand->cell->output,PERC,l,config)+=perc*stand->frac;
            if(l==BOTTOMLAYER)
            {
#ifdef IMAGE
              stand->cell->discharge.dmass_gw+=perc*stand->frac*stand->cell->coord.area;
#else
              outflux+=perc;
#endif
              *return_flow_b+=perc*(1-stand->frac_g[l]);
            }
            else
            {
              influx=perc;
              frac_g_influx=stand->frac_g[l];
            }
            if(config->with_nitrogen && l<BOTTOMLAYER)
            {
              /* determination of nitrate concentration in mobile water */
              w_mobile=vno3=concNO3_mobile=0;
              /* w_mobile as in Neitsch et al. 2005: Eq. 4:2.1.3 */
              w_mobile = perc + srunoff + lrunoff;
              if (w_mobile > epsilon)
              {
                ww = -w_mobile / ((1 - soil->par->anion_excl) * soil->wsats[l]);  /* Eq 4:2.1.2 */
                //ww = -w_mobile / (soil->wsats[l]-soil->wpwps[l]);
                vno3 = soil->NO3[l] * (1 - exp(ww));
                concNO3_mobile = max(vno3/w_mobile, 0);
              }
              /* nitrate movement with percolation */
              /* nitrate percolating from overlying layer */

              soil->NO3[l] += NO3perc_ly;
              NO3perc_ly=0;
              /* calculate nitrate in surface runoff */
              /* assume that there is no N in surface runoff as it does not infiltrate */
              if(l==-999)
              {
                NO3surf = NPERCO * concNO3_mobile * srunoff; /* Eq. 4:2.1.5 */
                NO3surf = min(NO3surf, soil->NO3[l]);
                soil->NO3[l] -= NO3surf;
              }
              else
                NO3surf=0;

              srunoff=0.0; /* not used for lower soil layers */
              if (l==0){
                NO3lat = NPERCO * concNO3_mobile * lrunoff; /* Eq. 4:2.1.6 */
              }
              else
                NO3lat = concNO3_mobile * lrunoff; /* Eq. 4:2.1.7 */
              NO3lat = min(NO3lat, soil->NO3[l]);
              soil->NO3[l] -= NO3lat;
              /* nitrate percolating from this layer */
              NO3perc_ly = concNO3_mobile * perc;  /*Eq 4:2.1.8*/
              NO3perc_ly = min(NO3perc_ly,soil->NO3[l]);
              soil->NO3[l] -= NO3perc_ly;

              getoutput(&stand->cell->output,LEACHING,config)+=(NO3surf + NO3lat)*stand->frac;
              stand->cell->balance.n_outflux+=(NO3surf + NO3lat)*stand->frac;
              if(isagriculture(stand->type->landusetype))
                getoutput(&stand->cell->output,NLEACHING_AGR,config)+=(NO3surf + NO3lat)*stand->frac;
              if(stand->type->landusetype==GRASSLAND)
                getoutput(&stand->cell->output,NO3_LEACHING_MGRASS,config)+=(NO3surf+NO3lat)*stand->frac;
              if(stand->type->landusetype==AGRICULTURE)
              {
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(config->separate_harvests)
                  {
                    crop=pft->data;
                    crop->sh->leachingsum+=NO3surf + NO3lat;
                  }
                  else
                    getoutputindex(&stand->cell->output,CFT_LEACHING,pft->par->id-npft+data_irrig->irrigation*ncft,config)+=NO3surf + NO3lat;
                }
              }
            } /* end of if(config->with_nitrogen) */
          } /*end percolation*/
        } /* if soil depth > freeze_depth */
      } /* soil layer loop */
      getoutput(&stand->cell->output,LEACHING,config)+=NO3perc_ly*stand->frac;
      stand->cell->balance.n_outflux+=NO3perc_ly*stand->frac;
      if(isagriculture(stand->type->landusetype))
        getoutput(&stand->cell->output,NLEACHING_AGR,config)+=NO3perc_ly*stand->frac;
      if(stand->type->landusetype==GRASSLAND)
        getoutput(&stand->cell->output,NO3_LEACHING_MGRASS,config)+=NO3perc_ly*stand->frac;
      if(stand->type->landusetype==AGRICULTURE)
      {
        foreachpft(pft,p,&stand->pftlist)
        {
          if(config->separate_harvests)
          {
            crop=pft->data;
            crop->sh->leachingsum+=NO3perc_ly;
          }
          else
            getoutputindex(&stand->cell->output,CFT_LEACHING,pft->par->id-npft+data_irrig->irrigation*ncft,config)+=NO3perc_ly;
        }
      }

    } /* if not drip */
    /* recompute the soil temperature every two iterations, to allow temperature changes to affect following percolation energy transfer */
    if (infil_loop_count%2==0 && config->percolation_heattransfer)
    {
      apply_perc_enthalpy(soil);
      Soil_thermal_prop th;
      calc_soil_thermal_props(UNKNOWN,&th,soil,soil->wi_abs_enth_adj,soil->sol_abs_enth_adj,config->johansen,FALSE);
      compute_mean_layer_temps_from_enth(soil->temp,soil->enth,&th);
    }
    infil_loop_count+=1;
  } /* while infil > 0 */

  for(l=0;l<NSOILLAYER;l++)
  {
    /*reallocate water above field capacity to freewater */
    if (soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>1)
    {
      freewater=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
      soil->w_fw[l]+=freewater;
      soil->w[l]-=freewater/soil->whcs[l];
    }
    if (fabs(soil->w_fw[l])<epsilon)
    {
      runoff+=soil->w_fw[l];
      soil->w_fw[l]=0;
    }
    if (fabs(soil->w[l])<epsilon)
    {
      runoff+=soil->w[l]*soil->whcs[l];
      soil->w[l]=0;
    }
    if(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l]<epsilon)
      stand->frac_g[l]=1.0;
#ifdef SAFE
    if(stand->frac_g[l]<(-0.01) || stand->frac_g[l]>(1.01))
    {
      fprintf(stderr,"Cell (%s) infil perc frac_g error frac_g=  %3.12f standtype= %s\n",sprintcoord(line,&stand->cell->coord),stand->frac_g[l],stand->type->name);
      fflush(stderr);
    }
    if (soil->w[l]<0)
    {
      fprintf(stderr,"Cell (%s) icedepth[%d]= %3.8f fw_ice= %.6f w_fw=%.6f w=%.6f soilwater=%.6f wsats=%.6f whcs=%f\n",
              sprintcoord(line,&stand->cell->coord),l,soil->ice_depth[l],soil->ice_fw[l],soil->w_fw[l],soil->w[l]*soil->whcs[l],
              allwater(soil,l)+allice(soil,l),soil->wsats[l],soil->whcs[l]);
      fflush(stderr);
      fail(NEGATIVE_SOIL_MOISTURE_ERR,TRUE,
           "Cell (%s) Soil-moisture %d negative: %g, lutype %s soil_type %s in infil_perc_irr\n",
           sprintcoord(line,&stand->cell->coord),l,soil->w[l],stand->type->name,soil->par->name);
    }
#endif
  } /* soil layer loop */

  /* evaluate soil water deficit in upper 50cm (irrigation depth) after irrigation event */
  soildepth_irrig=SOILDEPTH_IRRIG;
  l=0;
  deficit=0.0;
  do
  {
    if (stand->soil.freeze_depth[l]< soildepth[l])
      deficit+=max(0,(1-stand->soil.w[l]-stand->soil.ice_depth[l]/stand->soil.whcs[l])*stand->soil.whcs[l]*min(1,soildepth_irrig/soildepth[l])*(1-stand->soil.freeze_depth[l]/soildepth[l]));
    l++;
  }while((soildepth_irrig-=soildepth[l-1])>0);

  getoutput(&stand->cell->output,UNMET_DEMAND,config)+=deficit; /* daily irrigation deficit in mm*/

#ifdef SAFE
  if(config->with_nitrogen)
    forrootsoillayer(l)
      if (soil->NO3[l]<-epsilon)
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"infil_prec_irr: Cell (%s) NO3=%g<0 in layer %d",sprintcoord(line,&stand->cell->coord),soil->NO3[l],l);
#endif

  /* write outputs */
  getoutput(&stand->cell->output,SEEPAGE,config)+=outflux*stand->frac; /* bottom layer percolation*/
  getoutput(&stand->cell->output,RUNOFF_LAT,config)+=runoff*stand->frac; /* lateral runoff */
  getoutput(&stand->cell->output,RUNOFF_SURF,config)+=runoff_surface*stand->frac; /* surface runoff */

  return runoff+outflux+runoff_surface;

} /* of 'infil_perc_irr' */
