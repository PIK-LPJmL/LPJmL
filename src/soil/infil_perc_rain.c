/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  f  i  l  _  p  e  r  c  _  r  a  i  n  .  c                        \n**/
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

Real infil_perc_rain(Stand *stand,        /**< Stand pointer */
                     Real infil,          /**< rainfall + melting water - interception_stand (mm) + rw_irrig */
                     Real *return_flow_b, /**< blue water return flow (mm) */
                     int npft,            /**< number of natural PFTs */
                     int ncft,            /**< number of crop PFTs */
                     const Config *config /**< LPJ configuration */
                    )                     /** \return water runoff (mm) */
{
  Real runoff;
  Real perc,slug,tolitter;
  Real TT; /*traveltime in [mm/h]*/
  Real HC; /*hydraulic conductivity in [mm/h]*/
  Real influx;
  Real frac_g_influx;
  Real outflux,grunoff,inactive_water[NSOILLAYER];
  Real runoff_surface,freewater,soil_infil;
  Real srunoff;
  Real lrunoff; /* intermediate variable for leaching of lateral runoff */
  Real NO3surf=0; /* amount of nitrate transported with surface runoff gN/m2 */
  Real NO3perc_ly=0; /* nitrate leached to next lower layer with percolation gN/m2 */
  Real NO3lat=0; /* amount of nitrate transported with lateral runoff gN/m2 */
  Real w_mobile=0; /* Water mixing with nutrient in layer mmH2O */
  Real concNO3_mobile; /* concentration of nitrate in solution gN/mm */
  Real vno3; /* temporary for calculating concNO3_mobile */
  Real ww; /* temporary for calculating concNO3_mobile */
  Real sz,f;
  Real infil_layer[NTILLLAYER];
  Soil *soil;
  int l,p;
  Real updated_soil_water=0,previous_soil_water[NSOILLAYER];
  Pft *pft;
  String line;
  Irrigation *data_irrig;
  Pftcrop *crop;

  if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS||  stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS)
    data_irrig=stand->data;
  else
    data_irrig=NULL;

  soil=&stand->soil;
  soil_infil=param.soil_infil; /* default to draw square root for infiltration factor*/
  for(l=0;l<NTILLLAYER;l++)
    infil_layer[l]=0.0;

  /* absorption of rainwater by litter */
  tolitter=min(soil->litter.agtop_wcap-soil->litter.agtop_moist,infil*soil->litter.agtop_cover);
  if(tolitter>epsilon)
  {
     soil->litter.agtop_moist+=tolitter;
     infil-=tolitter;
  }

  influx=grunoff=perc=frac_g_influx=freewater=0.0;
  runoff_surface=runoff=outflux=0;
  if(config->rw_manage && (stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS ||
                           stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE ||
                           stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS))
    soil_infil=param.soil_infil_rw; /* parameter to increase soil infiltration rate */


  for(l=0;l<NSOILLAYER;l++)
  {
    previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
    freewater+=soil->w_fw[l];
    if (soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>1)
      freewater+=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
  }

  soil_infil *= (1 + soil->litter.agtop_cover*param.soil_infil_litter); /*soil_infil is scaled between 2 and 6, based on Jaegermeyr et al. 2016*/
  while(infil > epsilon || freewater > epsilon)
  {
    NO3perc_ly=0;
    freewater=0.0;
    slug=min(4,infil);
    infil=infil-slug;
    if(1-(soil->w[0]*soil->whcs[0]+soil->w_fw[0]+soil->ice_depth[0]+soil->ice_fw[0])/(soil->wsats[0]-soil->wpwps[0])>=0)
      influx=slug*pow(1-(soil->w[0]*soil->whcs[0]+soil->w_fw[0]+soil->ice_depth[0]+soil->ice_fw[0])/(soil->wsats[0]-soil->wpwps[0]),(1/soil_infil));
    else
      influx=0;
    runoff_surface+=slug - influx;
    srunoff=slug-influx; /*surface runoff used for leaching */
    frac_g_influx=1; /* first layer has only green influx, but lower layers with percolation have mixed frac_g_influx */

    for(l=0;l<NSOILLAYER;l++)
    {
      if(l<NTILLLAYER)
        infil_layer[l]+=influx;
      previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
      soil->w[l]+=(soil->w_fw[l]+influx)/soil->whcs[l];
      soil->w_fw[l]=0.0;
      influx=0.0;
      lrunoff=0.0;
      inactive_water[l]=soil->ice_depth[l]+soil->wpwps[l]+soil->ice_fw[l];

      /*update frac_g to influx */
      updated_soil_water=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
      if(previous_soil_water[l]-updated_soil_water>epsilon)
      {
        fprintf(stderr,"Cell (%s) infil_perc_rain.c error updated smaller then previous --- updated=  %3.12f --- previous=  %3.12f --- diff=  %3.12f\n",sprintcoord(line,&stand->cell->coord),updated_soil_water,previous_soil_water[l],updated_soil_water-previous_soil_water[l]);
        fflush(stderr);
      }
      if(updated_soil_water>previous_soil_water[l] && updated_soil_water>0)
      {
          stand->frac_g[l]=(previous_soil_water[l]*stand->frac_g[l] + (updated_soil_water - previous_soil_water[l])*frac_g_influx)/updated_soil_water; /* new green fraction equals old green amount + new green amount divided by total water */
      }

      /* lateral runoff of water above saturation */
      if ((soil->w[l]*soil->whcs[l])>(soildepth[l]-soil->freeze_depth[l])*(soil->wsat-soil->wpwp))
      {
        grunoff=(soil->w[l]*soil->whcs[l])-((soildepth[l]-soil->freeze_depth[l])*(soil->wsat-soil->wpwp));
        soil->w[l]-=grunoff/soil->whcs[l];
        runoff+=grunoff;
        lrunoff+=grunoff;
        *return_flow_b+=grunoff*(1-stand->frac_g[l]);
      }
      /*needed here??? -> Only if (soildepth[l]-soil->freeze_depth[l])*(soil->wsat[l]-soil->wpwp[l])!=(soil->ice_depth[l]+soil->ice_fw[l])*/
      if((inactive_water[l]+soil->w[l]*soil->whcs[l])>soil->wsats[l])
      {
        grunoff=(inactive_water[l]+soil->w[l]*soil->whcs[l])-soil->wsats[l];
        soil->w[l]-=grunoff/soil->whcs[l];
        runoff+=grunoff;
        lrunoff+=grunoff;
        *return_flow_b+=grunoff*(1-stand->frac_g[l]);
      }

      if (soildepth[l]>soil->freeze_depth[l])
      {
        /*percolation*/
        if((soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-param.percthres)>epsilon/soil->whcs[l])
        {
          HC=soil->Ks[l]*pow(((soil->w[l]*soil->whcs[l]+inactive_water[l])/soil->wsats[l]),soil->beta_soil[l]);
          TT=((soil->w[l]-param.percthres)*soil->whcs[l]+soil->ice_depth[l])/HC;
          perc=((soil->w[l]-param.percthres)*soil->whcs[l]+soil->ice_depth[l])*(1-exp(-24/TT));
          //printf("HC=%g,TT=%g,perc=%h\n",HC,TT,perc);
          /*correction of percolation for water content of the following layer*/
          if (l<BOTTOMLAYER)
          {
            if(1-(soil->w[l+1]*soil->whcs[l+1]+soil->w_fw[l+1]+soil->ice_depth[l+1]+soil->ice_fw[l+1])/(soil->wsats[l+1]-soil->wpwps[l+1])<0)
              perc=0;
            else
              perc*=sqrt(1-(soil->w[l+1]*soil->whcs[l+1]+soil->w_fw[l+1]+soil->ice_depth[l+1]+soil->ice_fw[l+1])/(soil->wsats[l+1]-soil->wpwps[l+1]));
          }
#ifdef SAFE
          if (perc< 0)
            printf("perc<0 ; TT %3.3f HC %3.3f perc  %3.3f w[%d]  %3.7f\n",TT,HC,perc/soil->whcs[l],l,soil->w[l]);
          if (perc/soil->whcs[l]>(soil->w[l]+epsilon))
            printf("perc>w ; Cell (%s); TT %3.3f HC %3.3f perc  %3.7f w[%d]  %3.7f\n",
                   sprintcoord(line,&stand->cell->coord),TT,HC,perc/soil->whcs[l],l,soil->w[l]);
#endif
          soil->w[l]-=perc/soil->whcs[l];

          if (fabs(soil->w[l])< epsilon/soil->whcs[l])
          {
            perc+=(soil->w[l])*soil->whcs[l];
            soil->w[l]=0;
          }
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
            soil->perc_energy[l+1]=((soil->temp[l]-soil->temp[l+1])*perc*1e-3)*c_water;
          }
          if(config->with_nitrogen && l<BOTTOMLAYER)
          {
            /* nitrate movement with percolation */
            /* nitrate percolating from overlying layer */

            soil->NO3[l] += NO3perc_ly;   // from layer above
            NO3perc_ly=0;
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
            if (l==0)
              NO3lat = NPERCO * concNO3_mobile * lrunoff; /* Eq. 4:2.1.6 */
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
            if(stand->type->landusetype==AGRICULTURE)
            {
              foreachpft(pft,p,&stand->pftlist)
              {
                if(config->double_harvest)
                {
                  crop=pft->data;
                  crop->dh->leachingsum+=NO3perc_ly;
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
    if(config->withdailyoutput && (stand->type->landusetype==NATURAL && ALLNATURAL==config->crop_index))
      getoutput(&stand->cell->output,D_LEACHING,config)+=NO3perc_ly;
    if(config->withdailyoutput && (stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND))
    {
      foreachpft(pft,p,&stand->pftlist)
      {
        if(pft->par->id==config->crop_index && config->crop_irrigation==data_irrig->irrigation)
        {
          getoutput(&stand->cell->output,D_LEACHING,config)=NO3perc_ly;
        }
      }
    }
    if(stand->type->landusetype==AGRICULTURE)
    {
      foreachpft(pft,p,&stand->pftlist)
      {
        if(config->double_harvest)
        {
          crop=pft->data;
          crop->dh->leachingsum+=NO3perc_ly;
        }
        else
          getoutputindex(&stand->cell->output,CFT_LEACHING,pft->par->id-npft+data_irrig->irrigation*ncft,config)+=NO3perc_ly;
      }
    }
  } /* while infil > 0 */

  for(l=0;l<NSOILLAYER;l++)
  {
    /*reallocate water above field capacity to freewater */
    if (soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>1)
    {
      soil->w_fw[l]=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
      soil->w[l]-=soil->w_fw[l]/soil->whcs[l];
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
    fprintf(stderr,"Cell (%s) infil= %3.10f icedepth[%d]= %3.8f fw_ice= %.6f w_fw=%.6f w=%.6f soilwater=%.6f wsats=%.6f whcs=%f\n",
            sprintcoord(line,&stand->cell->coord),infil,l,soil->ice_depth[l],soil->ice_fw[l],soil->w_fw[l],soil->w[l]*soil->whcs[l],
            allwater(soil,l)+allice(soil,l),soil->wsats[l],soil->whcs[l]);
    fflush(stderr);
    fail(NEGATIVE_SOIL_MOISTURE_ERR,TRUE,
         "Cell (%s) Soil-moisture %d negative: %g, lutype %s soil_type %s in infil_perc_rain\n",
         sprintcoord(line,&stand->cell->coord),l,soil->w[l],stand->type->name,soil->par->name);
   }
#endif
  } /* soil layer loop */

#ifdef SAFE
  if(config->with_nitrogen)
    forrootsoillayer(l)
      if (soil->NO3[l]<-epsilon)
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"infil_prec_rain: Cell(%s) NO3=%g<0 in layer %d",sprintcoord(line,&stand->cell->coord),soil->NO3[l],l);
#endif

  /* Rainwater Harvesting: store part of surface runoff for supplementary irrigation */
  if(config->rw_manage && ((stand->type->landusetype==AGRICULTURE && !data_irrig->irrigation) || stand->type->landusetype==SETASIDE_RF))
  {
    soil->rw_buffer+=param.frac_ro_stored*runoff_surface;
    runoff_surface-=param.frac_ro_stored*runoff_surface;
    if(soil->rw_buffer > param.rw_buffer_max)
    {
      runoff_surface+=soil->rw_buffer - param.rw_buffer_max;
      soil->rw_buffer=param.rw_buffer_max;
      //printf("Warning! rw_buffer has reached maximum capacity\n");
    }
  }
  for(l=0;l<NTILLLAYER;l++)
  {
    sz = 0.2*infil_layer[l]*(1.0 + 2.0*stand->soil.par->sand*100 / (stand->soil.par->sand*100 + exp(8.597 - 0.075*stand->soil.par->sand*100))) / pow(soildepth[NTILLLAYER-1]/1000,0.6);
    f = sz / (sz + exp(3.92 - 0.0226*sz));
    stand->soil.df_tillage[l]+=f*(1 - stand->soil.df_tillage[l]);
  }

  /*writing output*/
  getoutput(&stand->cell->output,SEEPAGE,config)+=outflux*stand->frac;
  getoutput(&stand->cell->output,RUNOFF_LAT,config)+=runoff*stand->frac;
  getoutput(&stand->cell->output,RUNOFF_SURF,config)+=runoff_surface*stand->frac;

  return runoff+outflux+runoff_surface;

} /* of 'infil_perc_rain' */
