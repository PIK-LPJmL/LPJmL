/**************************************************************************************/
/**                                                                                \n**/
/**                i  n  f  i  l  _  p  e  r  c  .  c                              \n**/
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

#define NPERCO 0.5  /* controls the amount of nitrate removed from the surface layer in runoff relative to the amount removed via percolation.  0.5 in Neitsch:SWAT MANUAL*/
#define OMEGA  6    /* adjustable parameter for impedance factor*/
#define maxWTP -500 /* max height of standing water [mm]*/

static int findwtlayer(const Soil *soil)
{
  int l,jwt;
  if(soil->wtable<0)
    return 0;
  jwt=0;
  foreachsoillayer(l)
  if (soil->wtable >= layerbound[l])
    jwt = l;
  else
    break;
  return jwt;
} /* of 'findwtlayer' */

Real infil_perc(Stand *stand,        /**< Stand pointer */
    Real infil,          /**< rainfall + melting water - interception_stand (mm) + rw_irrig */
    Real *return_flow_b, /**< blue water return flow (mm) */
    int npft,            /**< number of natural PFTs */
    int ncft,            /**< number of crop PFTs */
    const Config *config /**< LPJ configuration */
)                     /** \return water runoff (mm) */
{
  Real runoff,qcharge_layer,qcharge_tot,qcharge_tot1,fill,runoff_out;
  Real perc,slug,tolitter;
  Real TT; /*traveltime in [mm/h]*/
  Real HC; /*hydraulic conductivity in [mm/h]*/
  Real influx,alpha;
  Real inactive_water[NSOILLAYER],frac_g_influx;
  Real outflux,grunoff,lat_runoff_last;  // outflux from last layer
  Real runoff_surface,freewater,soil_infil;
  Real srunoff;
  Real lrunoff[NSOILLAYER],nrsub_top[BOTTOMLAYER],ndrain_perched_out[BOTTOMLAYER];                /* intermediate variable for leaching of lateral runoff */
  Real pperc[BOTTOMLAYER];                                                                        /* intermediate variable for leaching of via percolation */
  Real NO3surf=0; /* amount of nitrate transported with surface runoff gN/m2 */
  Real NO3perc_ly=0; /* nitrate leached to next lower layer with percolation gN/m2 */
  Real NO3lat=0; /* amount of nitrate transported with lateral runoff gN/m2 */
  Real w_mobile=0; /* Water mixing with nutrient in layer mmH2O */
  Real concNO3_mobile; /* concentration of nitrate in solution gN/mm */
  Real vno3; /* temporary for calculating concNO3_mobile */
  Real ww; /* temporary for calculating concNO3_mobile */
  int jwt,icet;
  Real infil_layer[NTILLLAYER];
  Real sz,f;
  Soil *soil;
  int l,p;
  Real updated_soil_water=0,previous_soil_water[NSOILLAYER];
  Pft *pft;
  String line;
  Irrigation *data_irrig;
  Pftcrop *crop;
  Real S;                  /*aquifer specific yield (-)*/
  Real Theta_ice, k_perch_max, frost_depth,soildepth_irrig,deficit;
  Real icefrac[NSOILLAYER];
  Real s_node = 0;                                                //soil moisture
  Real s1, vol_eq, tempi, temp0, voleq1, fff,ka;
  Real icesum, depthsum, rsub_top_max, rsub_top, wtable_tmp;
  Real tmp_vol, zq, smp, smpmin, smp1, wh_zwt, wh, layerbound2;
  Real q_perch_max, k_drai_perch, k_perch, wtsub, drain_perched, drain_perched_out, drain_perched_layer;
  Real rsub_top_tot, rsub_top_layer, active_wa, tmp_water;
  //Real sat_lev = 0.9;
  Real prec=infil;

#ifdef CHECK_BALANCE
  Real start, end;
  Stocks n_before,n_after;
  start = end = 0;
  Real water_after, water_before,balancew;
  water_before=soilwater(&stand->soil);
#endif

  if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==SETASIDE_WETLAND || stand->type->landusetype==BIOMASS_GRASS ||
      stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS  ||  stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS)
    data_irrig=stand->data;
  else
    data_irrig=NULL;

  soil=&stand->soil;
  soil_infil=param.soil_infil; /* default to draw square root for infiltration factor*/
  if(config->rw_manage && (stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==OTHERS  ||
      stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS))
    soil_infil=param.soil_infil_rw; /* parameter to increase soil infiltration rate */
  for(l=0;l<NTILLLAYER;l++)
    infil_layer[l]=0.0;

  /* absorption of rainwater by litter */
  tolitter=min(soil->litter.agtop_wcap-soil->litter.agtop_moist,infil*soil->litter.agtop_cover);
  if(tolitter>epsilon)
  {
    soil->litter.agtop_moist+=tolitter;
    infil-=tolitter;
  }

  influx=grunoff=perc=frac_g_influx=freewater= qcharge_layer=qcharge_tot=lat_runoff_last=wtsub=tmp_water=rsub_top_max=0.0;
  rsub_top_layer=k_drai_perch=k_perch=icesum=k_perch_max=slug=fill=srunoff=0.0;
  runoff_surface=runoff=outflux=rsub_top=rsub_top_tot=drain_perched_out=qcharge_tot1=drain_perched_layer=drain_perched=0.0;
  q_perch_max=active_wa=runoff_out=depthsum=wtable_tmp=alpha=s1=vol_eq=NO3perc_ly=0;
  // The layer index of the first unsaturated layer, i.e., the layer right above the water table
  forrootsoillayer(l)
  lrunoff[l]=pperc[l]=ndrain_perched_out[l]=nrsub_top[l]=0.0;
  jwt=findwtlayer(soil);
  if (soil->wtable<maxWTP)
    soil->wtable=maxWTP;
  icet=NSOILLAYER-1;
  foreachsoillayer(l)
  if (soil->freeze_depth[l]/(soildepth[l])>0.05)
  {
    icet = l;
    break;
  }
  frost_depth=layerbound[icet]-soil->freeze_depth[icet];
  foreachsoillayer(l)
  icefrac[l]=(soil->ice_depth[l]+soil->ice_fw[l]+soil->wpwps[l]*soil->ice_pwp[l])/soil->wsats[l];
  Theta_ice=pow(10,-OMEGA*icefrac[jwt]);

#ifdef CHECK_BALANCE
  start=soil->wa;
  n_before=soilstocks(soil);
  n_before.nitrogen=n_before.nitrogen*stand->frac+stand->cell->balance.n_outflux;
#endif


  for(l=0;l<NSOILLAYER;l++)
  {
    previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
    freewater+=soil->w_fw[l];
    if (soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>1)
      freewater+=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
  }

  soil_infil *= (1 + soil->litter.agtop_cover*param.soil_infil_litter);         /*soil_infil is scaled between 2 and 6, based on Jaegermeyr et al. 2016*/
  while(infil > epsilon || freewater > epsilon)
  {
    NO3perc_ly=0;
    freewater=0.0;
    slug=min(4,infil);
    infil=infil-slug;
    if(data_irrig!=NULL  && (data_irrig->irrig_system==SPRINK || data_irrig->irrig_system==DRIP))
      influx=slug;        /*no surface runoff for DRIP and Sprinkler*/
    else
    {
      if(1-(soil->w[0]*soil->whcs[0]+soil->w_fw[0]+soil->ice_depth[0]+soil->ice_fw[0])/(soil->wsats[0]-soil->wpwps[0])>=0)
        alpha=pow(1-max(0,(soil->w[0]*soil->whcs[0]+soil->w_fw[0]+soil->ice_depth[0]+soil->ice_fw[0])/(soil->wsats[0]-soil->wpwps[0])),(1/soil_infil));
      else
        alpha=0;
      influx=slug*(1-stand->Hag_Beta+alpha*stand->Hag_Beta);
      runoff_surface+=slug-influx;
      srunoff=slug-influx; /*surface runoff used for leaching */
    }
    frac_g_influx=1; /* first layer has only green influx, but lower layers with percolation have mixed frac_g_influx */
    if(data_irrig!=NULL && data_irrig->irrig_system==DRIP)
    {
      /* in case of Drip: directly fill up field cap of first two soil layers, no surface runoff, lateral runoff or percolation */
      /* -> this allows simulating perfect irrigation: drip + irrg_threshold = 1 (keep in mind: plant can still be somewhat stressed, if roots go deeper than 2. layer) */
      for(l=0;l<LASTLAYER && influx>epsilon;l++)
      {
        previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        soil->w[l]+=influx/soil->whcs[l];
        influx=max((soil->w[l]-1)*soil->whcs[l]+soil->ice_depth[l],0);
        soil->w[l]-=influx/soil->whcs[l];
        soil->w_fw[l]+=influx;
        influx=0;
        if(soil->w_fw[l]>(soil->wsats[l]-soil->whcs[l]))
        {
          influx=soil->w_fw[l]-(soil->wsats[l]-soil->whcs[l]);
          soil->w_fw[l]-=influx;
        }
        /*update frac_g: new green fraction equals old green amount + new green amount divided by total water */
        updated_soil_water=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        if(updated_soil_water>previous_soil_water[l] && updated_soil_water>0)
          stand->frac_g[l]=(previous_soil_water[l]*stand->frac_g[l])/updated_soil_water;
      }
      outflux+=influx;
      *return_flow_b+=influx;
      influx=0.0;
    }
    else
    {
      for(l=0;l<NSOILLAYER;l++)
      {
        if(l<NTILLLAYER)
          infil_layer[l]+=influx;
        previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        soil->w[l]+=(soil->w_fw[l]+influx)/soil->whcs[l];
        soil->w_fw[l]=0.0;
        influx=0.0;
        lrunoff[l]=0.0;
        inactive_water[l]=soil->ice_depth[l]+soil->wpwps[l]+soil->ice_fw[l];

        /*update frac_g to influx */
        updated_soil_water=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
        /*
        if(previous_soil_water[l]-updated_soil_water>epsilon)
        {
          fprintf(stderr,"Cell (%s) infil_perc.c error updated smaller then previous --- updated=  %3.12f --- previous=  %3.12f --- diff=  %3.12f\n",sprintcoord(line,&stand->cell->coord),updated_soil_water,previous_soil_water[l],updated_soil_water-previous_soil_water[l]);
          fflush(stderr);
        }
         */
        if(updated_soil_water>previous_soil_water[l] && updated_soil_water>0)
        {
          stand->frac_g[l]=(previous_soil_water[l]*stand->frac_g[l] + (updated_soil_water - previous_soil_water[l])*frac_g_influx)/updated_soil_water; /* new green fraction equals old green amount + new green amount divided by total water */
        }

        /* lateral runoff of water above saturation */
        if ((soil->w[l]*soil->whcs[l])>(soildepth[l]-soil->freeze_depth[l])*(soil->wsat[l]-soil->wpwp[l]))
        {
          grunoff=(soil->w[l]*soil->whcs[l])-((soildepth[l]-soil->freeze_depth[l])*(soil->wsat[l]-soil->wpwp[l]));
          soil->w[l]-=grunoff/soil->whcs[l];
          *return_flow_b+=grunoff*(1-stand->frac_g[l]);
          if (l<(BOTTOMLAYER-1) || l<jwt || l<icet)
          {
            runoff+=grunoff;
            lrunoff[l]+=grunoff;
          }
          else
          {
            lat_runoff_last+=grunoff;
            lrunoff[l]+=grunoff;
          }
        }
        /*needed here??? -> Only if (soildepth[l]-soil->freeze_depth[l])*(soil->wsat[l]-soil->wpwp[l])!=(soil->ice_depth[l]+soil->ice_fw[l])*/
        if((inactive_water[l]+soil->w[l]*soil->whcs[l])>soil->wsats[l])
        {
          grunoff=(inactive_water[l]+soil->w[l]*soil->whcs[l])-soil->wsats[l];
          soil->w[l]-=grunoff/soil->whcs[l];
          if (l<(BOTTOMLAYER-1) || l<jwt || l<icet)
          {
            runoff+=grunoff;
            lrunoff[l]+=grunoff;
          }
          else
          {
            lat_runoff_last+=grunoff;
            lrunoff[l]+=grunoff;
          }
          *return_flow_b+=grunoff*(1-stand->frac_g[l]);
        }

        if (soildepth[l]>soil->freeze_depth[l])
        {
          /*percolation*/
          if((soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)>epsilon/soil->whcs[l])
          {
            HC=soil->Ks[l]*pow(((soil->w[l]*soil->whcs[l]+inactive_water[l])/soil->wsats[l]),soil->beta_soil[l]);
            TT=((soil->w[l]-1)*soil->whcs[l]+soil->ice_depth[l])/HC;
            perc=((soil->w[l]-1)*soil->whcs[l]+soil->ice_depth[l])*(1-exp(-24/TT));
            /*correction of percolation for water content of the following layer*/
            if (l<BOTTOMLAYER)
            {
              if(1-(soil->w[l+1]*soil->whcs[l+1]+soil->w_fw[l+1]+soil->ice_depth[l+1]+soil->ice_fw[l+1])/(soil->wsats[l+1]-soil->wpwps[l+1])<0)
                perc=0;
              else
                perc*=sqrt(1-(soil->w[l+1]*soil->whcs[l+1]+soil->w_fw[l+1]+soil->ice_depth[l+1]+soil->ice_fw[l+1])/(soil->wsats[l+1]-soil->wpwps[l+1]));
            }
#ifdef SAFE
            if (perc<0)
              fprintf(stderr,"1perc>w ; Cell (%s); perc: %g TT %3.3f HC %3.3f perc  %3.3f w[%d]  %3.7f\n",sprintcoord(line,&stand->cell->coord),perc,TT,HC,perc/soil->whcs[l],l,soil->w[l]);
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
            if(l<BOTTOMLAYER)
              pperc[l]+=perc;
          } /*end percolation*/
        } /* if soil depth > freeze_depth */
      } /* soil layer loop */
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
    if (soil->w_fw[l]<0)
    {
      runoff_out+=soil->w_fw[l];
      soil->w_fw[l]=0;
    }
    if (soil->w[l]<0)
    {
      runoff_out+=soil->w[l]*soil->whcs[l];
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
      if(soil->w[l]<-epsilon)
      {
        fprintf(stderr,"Cell (%s) infil= %3.10f icedepth[%d]= %3.8f fw_ice= %.6f w_fw=%.6f w=%.6f soilwater=%.6f wsats=%.6f whcs=%f prec=%.6f \n",
            sprintcoord(line,&stand->cell->coord),infil,l,soil->ice_depth[l],soil->ice_fw[l],soil->w_fw[l],soil->w[l]*soil->whcs[l],
            allwater(soil,l)+allice(soil,l),soil->wsats[l],soil->whcs[l],prec);
        fflush(stderr);
        fail(NEGATIVE_SOIL_MOISTURE_ERR,TRUE,TRUE,
            "Cell (%s) Soil-moisture %d negative: %g, lutype %s soil_type %s in infil_perc",
            sprintcoord(line,&stand->cell->coord),l,soil->w[l],stand->type->name,soil->par->name);
      }
      soil->w[l]=0;
    }
#endif
  } /* soil layer loop */

#ifdef SAFE
  if(config->with_nitrogen)
    forrootsoillayer(l)
    if (soil->NO3[l]<-epsilon)
      fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"infil_perc: Cell(%s) NO3=%g<0 in layer %d",sprintcoord(line,&stand->cell->coord),soil->NO3[l],l);
#endif

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

  jwt=findwtlayer(soil);

  //THIS IS THE IMPLEMENTATION OF THE WATER TABLE DEPTH FOLLOWING CLM4.5
  // use analytical expression for aquifer specific yield

  Theta_ice=pow(10,-OMEGA*icefrac[jwt+1]);
  s_node=(allwater(soil,jwt)/soildepth[jwt] + allice(soil,jwt)/soildepth[jwt])/soil->wsat[jwt];
  s_node=max(s_node,0.02);
  s_node=min(1, s_node);

  //  this is the expression for unsaturated hydraulic conductivity
  //  q_recharge=-ka*(-soil->par->psi_sat/(wtable-layerbound[jwt-1]))*24
  //  if (wtable==layerbound[jwt]) q_recharge=-ka*(-soil->par->psi_sat/(wtable+1))*24
  //  equilibrium soil matric potential = soil->par->psi_sat*((soil->par->wsat*((soil->par->psi_sat+wtable-soildepth[jwt])/soil->par->psi_sat)^(-1/soil->par->b))/soil->par->wsat)^-soil->par->b
  //  equilibrium volumetric water content = (soil->par->wsat*((soil->par->psi_sat+wtable-soildepth[jwt])/soil->par->psi_sat)^(-1/soil->par->b))
  //  calculate the equilibrium water content based on the water table depth

  //  If water table is below soil column calculate zq for the last layer
  if(soil->wtable > layerbound[BOTTOMLAYER])
  {
    if (soil->wtable==layerbound[jwt] || (soil->wtable-layerbound[jwt])<epsilon)
      vol_eq=soil->wsat[jwt];
    else
    {
      tempi=1;
      temp0=pow(((soil->par->psi_sat+soil->wtable-layerbound[jwt])/soil->par->psi_sat),(1-1/soil->par->b));
      vol_eq=-soil->par->psi_sat*soil->wsat[jwt]/(1-1/soil->par->b)/(soil->wtable-layerbound[jwt])*(tempi-temp0);
      vol_eq=max(vol_eq,0.0);
      vol_eq=min(soil->wsat[jwt],vol_eq);
    }
  }
  else
  {
    if (soil->wtable==layerbound[jwt] || (soil->par->psi_sat+soil->wtable-layerbound[jwt])<epsilon || soil->wtable<0)
    {
      vol_eq = soil->wsat[jwt];
    }
    else if (jwt==0)
    {
      temp0 =pow(((soil->par->psi_sat+soil->wtable-layerbound[jwt])/soil->par->psi_sat),(1-1/soil->par->b));
      tempi = 1;
      voleq1 = -soil->par->psi_sat*soil->wsat[jwt]/(1-1/soil->par->b)/(soil->wtable-layerbound[jwt])*(tempi-temp0);
      vol_eq = (voleq1*(soil->wtable) + soil->wsat[jwt]*(layerbound[jwt]-soil->wtable))/soildepth[jwt];
    }
    else if (soil->wtable<layerbound[jwt] && soil->wtable>layerbound[jwt-1])
    {
      temp0=pow(((soil->par->psi_sat+soil->wtable-layerbound[jwt-1])/soil->par->psi_sat),(1-1/soil->par->b));
      tempi= 1.0;
      voleq1= -soil->par->psi_sat*soil->wsat[jwt]/(1-1/soil->par->b)/(soil->wtable-layerbound[jwt-1])*(tempi-temp0);
      vol_eq = (voleq1*(soil->wtable-layerbound[jwt-1]) + soil->wsat[jwt]*(layerbound[jwt]-soil->wtable))/soildepth[jwt];
      vol_eq = min(soil->wsat[jwt],vol_eq);
      vol_eq = max(vol_eq,0);
    }
    else
    {
      tempi =pow(((soil->par->psi_sat+soil->wtable-layerbound[jwt])/soil->par->psi_sat),(1-1/soil->par->b));
      temp0 = pow(((soil->par->psi_sat+soil->wtable-layerbound[jwt-1])/soil->par->psi_sat),(1-1/soil->par->b));
      vol_eq = -soil->par->psi_sat*soil->wsat[jwt]/(1-1/soil->par->b)/(layerbound[jwt]-layerbound[jwt-1])*(tempi-temp0);
      vol_eq = max(vol_eq,0.0);
      vol_eq = min(soil->wsat[jwt],vol_eq);
    }
  }
  smpmin =-1e8;                                             // restriction of soil potential (mm)
  tmp_vol=max(vol_eq/soil->wsat[jwt],0.01);                 // equilibrium volumetric water
  zq=-soil->par->psi_sat*pow(tmp_vol,-soil->par->b);        // eq. 7.126 equilibrium soil matric potential
  zq= max(smpmin, zq);                                      // equilibrium matric potential for layer [mm]
  smp = -soil->par->psi_sat*pow(s_node,-soil->par->b);      // mm (soil matric potential)
  smp1=max(smpmin,smp);
  wh=smp1-zq;
  wh_zwt=0;                                                 // water head at the water table depth (mm) always zero
  if(jwt<(NSOILLAYER-1))
  {
    s_node=(allwater(soil,jwt+1)/soildepth[jwt+1] + allice(soil,jwt+1)/soildepth[jwt+1])/soil->wsat[jwt+1];      // soil wetness
    s_node=max(s_node,0.02);
    s_node=min(1, s_node);
    ka=Theta_ice*soil->Ks[jwt+1]*24*pow(s_node,2*soil->par->b+3);
  }
  else
  {
    ka=Theta_ice*soil->Ks[jwt]*24*pow(s_node,2*soil->par->b+3);
  }
  if(soil->wtable<=layerbound[BOTTOMLAYER])  //SiS-TEST
  {
    if((soil->wtable-layerbound[jwt])<(soil->par->psi_sat*0.1) || jwt==0)
      qcharge_tot+= -ka*(wh_zwt-wh)/(soil->wtable+1);
    else                                                                     //assuming flux is at water table interface, saturation deeper than water table
      qcharge_tot+= -ka*(wh_zwt-wh)/((soil->wtable-layerbound[jwt])*2);
  }
  qcharge_tot= max(-10.0,qcharge_tot);
  qcharge_tot= min(10.0,qcharge_tot);

  qcharge_tot1=outflux+lat_runoff_last+runoff;                              //runoff_out includes lateral runoff from layers


  //*******************************qcharge_tot1 raises water table
  if (soil->wtable>0)
    S=soil->wsat[jwt]*(1.-pow((1.+(soil->wtable/soil->par->psi_sat)),(-1./soil->par->b)));
  else
    S=soil->wsat[jwt]*(1.-pow((1.+(1/soil->par->psi_sat)),(-1./soil->par->b)));
  S=max(S,0.02);
  qcharge_tot+=qcharge_tot1;
  if(soil->wtable>layerbound[BOTTOMLAYER])
  {
    soil->wa+=qcharge_tot1;                      //water in the unconfined aquifer (mm)
    soil->wtable-=(qcharge_tot)/S;
    qcharge_tot1=0;
    soil->w_fw[BOTTOMLAYER]+=max(0,(soil->wa-5000));
    soil->wa=min(soil->wa,5000);
    if(soil->w_fw[BOTTOMLAYER]>soil->wsats[BOTTOMLAYER]-soil->wpwps[BOTTOMLAYER]-soil->ice_fw[BOTTOMLAYER]-soil->whcs[BOTTOMLAYER])
    {
      runoff_out+=soil->w_fw[BOTTOMLAYER]-(soil->wsats[BOTTOMLAYER]-soil->wpwps[BOTTOMLAYER]-soil->ice_fw[BOTTOMLAYER]-soil->whcs[BOTTOMLAYER]);
      soil->w_fw[BOTTOMLAYER]=(soil->wsats[BOTTOMLAYER]-soil->wpwps[BOTTOMLAYER]-soil->ice_fw[BOTTOMLAYER]-soil->whcs[BOTTOMLAYER]);
    }
  }
  else if(qcharge_tot>0)
  {
    for(l=jwt;l>=0;l--)
    {
      layerbound2=(l==0 && layerbound[l]>soil->wtable) ?  -50 : layerbound[l];
      if(soil->wtable!=layerbound2 && l<icet)
      {
        if (soil->wtable>0)
          S=soil->wsat[l]*(1.-pow((1.+(soil->wtable/soil->par->psi_sat)),(-1./soil->par->b)));
        else
          S=soil->wsat[l]*(1.-pow((1.+(1/soil->par->psi_sat)),(-1./soil->par->b)));
        S=max(S,0.02);
        qcharge_layer=min(qcharge_tot,(S*(soil->wtable - layerbound2)));  // try to raise water table to account for qcharge
        qcharge_layer=max(qcharge_layer,0);
        fill=max(0,qcharge_tot1);
        soil->w[l]+=fill/soil->whcs[l];
        if((soil->w[l]+soil->ice_depth[l]/soil->whcs[l])>1)
        {
          soil->w_fw[l]+=((soil->w[l]+soil->ice_depth[l]/soil->whcs[l])-1)*soil->whcs[l];
          soil->w[l]=1-soil->ice_depth[l]/soil->whcs[l];
        }
        if(soil->w_fw[l]>(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]-soil->ice_fw[l]))
        {
          fill-=soil->w_fw[l]-(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]-soil->ice_fw[l]);
          soil->w_fw[l]=soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]-soil->ice_fw[l];
        }
        qcharge_tot1-=fill;
        qcharge_tot-=qcharge_layer;
        soil->wtable-=qcharge_layer/S;           // includes qcharge_to includes q_recharge-q_drain
        if (qcharge_tot <= 0)
          break;
      }
    }
  }
  else
  {                                      // deepening water table (negative qcharge)
    for(l=jwt;l<BOTTOMLAYER;l++)
    {
      layerbound2=(layerbound[l]>soil->wtable) ?  layerbound[l] : layerbound[l+1]; // TODO TEST IF REALLY NECESSARY
      if(soil->wtable!=layerbound2 && l<icet)
      {
        if (soil->wtable>0)
          S=soil->wsat[l]*(1.-pow((1.+(soil->wtable/soil->par->psi_sat)),(-1./soil->par->b)));
        else
          S=soil->wsat[l]*(1.-pow((1.+(1/soil->par->psi_sat)),(-1./soil->par->b)));
        S=max(S,0.02);
        qcharge_layer=max(qcharge_tot,-(S*(layerbound2-soil->wtable)));  //try to lower water table to account for qcharge
        qcharge_layer=min(qcharge_layer,0);
        fill=max(0,qcharge_tot1);
        soil->w[l]+=fill/soil->whcs[l];
        if((soil->w[l]+soil->ice_depth[l]/soil->whcs[l])>1)
        {
          soil->w_fw[l]+=((soil->w[l]+soil->ice_depth[l]/soil->whcs[l])-1)*soil->whcs[l];
          soil->w[l]=1-soil->ice_depth[l]/soil->whcs[l];
        }
        if(soil->w_fw[l]>(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]-soil->ice_fw[l]))
        {
          fill-=soil->w_fw[l]-(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]-soil->ice_fw[l]);
          soil->w_fw[l]=soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]-soil->ice_fw[l];
        }
        qcharge_tot1-=fill;
        qcharge_tot-=qcharge_layer;
        if(qcharge_tot>=0)
        {
          soil->wtable-=qcharge_layer/S;                        // ALLOW NEGATIVE WATER TABLE DEPTH - mimic inundation
          break;
        }
        else
          soil->wtable=layerbound[l];                          // PLEASE CHECK AGAIN
      }
    }
  }
  if(qcharge_tot1>0)
    runoff_out+=qcharge_tot1;

  // define frost table as first frozen layer with unfrozen layer above it
  icet=NSOILLAYER-1;
  foreachsoillayer(l)
  if(soil->freeze_depth[l]/(soildepth[l])>0.05)
  {
    icet=l;
    break;
  }
  jwt=findwtlayer(soil);

  //==  LATERAL FLOW ==================================================
  // water table above frost table

  q_perch_max = 0.864 * sin(stand->slope_mean*3.6*M_PI/180);                          //1e-5mm/s specify maximum drainage rate mm/d
  frost_depth=layerbound[icet]-soil->freeze_depth[icet];

  if(frost_depth<layerbound[BOTTOMLAYER-1])
  {
    if(soil->wtable<frost_depth)
    {                                                //ONLY IF WATER_TABLE ABOVE FROST_DEPTH
      k_perch=0;
      wtsub=0;
      for(l=jwt;l<=icet;l++)
      {
        Theta_ice=pow(10,(-OMEGA*(((icefrac[l]*soildepth[l])+(icefrac[l+1]*soildepth[l+1]))/(soildepth[l]+soildepth[l+1]))));
        k_perch+=(Theta_ice*soil->Ks[l]*24*soildepth[l]);                         // ks in mm/h converted to day
        wtsub+=soildepth[l];                                                      // summation of soildepth for layers below water table (mm**2/s)
      }
      if (wtsub > 0)
        k_perch=k_perch/wtsub;                                       // ks is adjusted as in pedotransfer.c
      drain_perched = q_perch_max*k_perch*(frost_depth-soil->wtable);
      drain_perched_out=drain_perched;
      drain_perched*=-1;
      for(l=jwt;l<=icet;l++)
      {
        drain_perched_layer=0.0;
        if(soil->w_fw[l]>epsilon)
        {
          drain_perched_layer=max(-soil->w_fw[l],drain_perched);
          soil->w_fw[l]+=drain_perched_layer;
          drain_perched-=drain_perched_layer;
          if(drain_perched>=0)
          {
            soil->wtable-=drain_perched_layer/((soil->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-soil->wpwps[l]*soil->ice_pwp[l])/soildepth[l]);
            break;
          }
          else
            soil->wtable=layerbound[l];
        }
        ndrain_perched_out[l]-=drain_perched_layer;
      }
      drain_perched_out+=drain_perched;
      drain_perched=0;
    }
  }
  else
  {
    //==  Topographic runoff  ==================================================

    fff=2.5;       //m-1 decay factor originally 2.5 in CLM4.5
    icesum=0;
    depthsum=0;
    active_wa=0;
    rsub_top_tot=0;

    jwt=0;
    foreachsoillayer(l)
    if(soil->wtable>layerbound[l])
      jwt=l;
    else
      break;
    if(soil->wtable<0)
      jwt=0;

    for(l=jwt;l<BOTTOMLAYER;l++)
    {
      icesum+=icefrac[l]*soildepth[l];
      depthsum+=soildepth[l];
      active_wa+=soil->w_fw[l];
    }
    if(jwt>=(BOTTOMLAYER-1))
      depthsum=layerbound[BOTTOMLAYER-1];
    Theta_ice=pow(10,(-OMEGA*(icesum/depthsum)));                                               // OMEGA of 6 gives a high impact on low ice fractions (0.1 -> 0.2511886)
    rsub_top_max=20*sin(stand->slope_mean*3.6*M_PI/180);                                        // 20 mm day-1 suggested by Karpouzas etal, 2006, Christen etal, 2006 ->50 mm day-1
    rsub_top=Theta_ice*rsub_top_max*exp(-fff*soil->wtable/1000);
    rsub_top=min(rsub_top,active_wa);
    //! use analytical expression for aquifer specific yield
    if(soil->wtable>=layerbound[BOTTOMLAYER-1])
    {
      if (soil->wtable>0)
        S=soil->wsat[l]*(1.-pow((1.+(soil->wtable/soil->par->psi_sat)),(-1./soil->par->b)));
      else
        S=soil->wsat[l]*(1.-pow((1.+(1/soil->par->psi_sat)),(-1./soil->par->b)));
      S=max(S,0.02);
      soil->wa-=rsub_top;
      soil->wtable+=rsub_top/S;
      tmp_water=max(0,(soil->wa-5000));
      soil->w_fw[BOTTOMLAYER-1]+=tmp_water;
      soil->wa-=tmp_water;
      nrsub_top[BOTTOMLAYER-1]+=tmp_water;
      if(soil->w_fw[BOTTOMLAYER-1]>soil->wsats[BOTTOMLAYER-1]-soil->wpwps[BOTTOMLAYER-1]-soil->ice_fw[BOTTOMLAYER-1]-soil->whcs[BOTTOMLAYER-1])
      {
        runoff_out+=soil->w_fw[BOTTOMLAYER-1]-(soil->wsats[BOTTOMLAYER-1]-soil->wpwps[BOTTOMLAYER-1]-soil->ice_fw[BOTTOMLAYER-1]-soil->whcs[BOTTOMLAYER-1]);
        soil->w_fw[BOTTOMLAYER-1]=(soil->wsats[BOTTOMLAYER-1]-soil->wpwps[BOTTOMLAYER-1]-soil->ice_fw[BOTTOMLAYER-1]-soil->whcs[BOTTOMLAYER-1]);
      }
    }
    else
    {
      //Now remove water via rsub_top
      rsub_top_tot-=rsub_top;

      if(rsub_top_tot > 0)
      {
        if(rsub_top_tot > 1)
          fprintf(stderr,"RSUB_TOP IS POSITIVE = %g in Drainage!\n",rsub_top_tot);
        rsub_top_tot=rsub_top=0;
      }
      else
      {                                         //deepening water table
        for(l=(jwt+1);l<BOTTOMLAYER-1;l++)
        {
          layerbound2=(layerbound[l]>soil->wtable) ?  layerbound[l] : layerbound[l+1];
          if (soil->wtable>0)
            S=soil->wsat[l]*(1.-pow((1.+(soil->wtable/soil->par->psi_sat)),(-1./soil->par->b)));
          else
            S=soil->wsat[l]*(1.-pow((1.+(1/soil->par->psi_sat)),(-1./soil->par->b)));
          S=max(S,0.02);
          rsub_top_layer=max(rsub_top_tot,-(S*(layerbound2-soil->wtable)));
          rsub_top_layer=max(min(0,rsub_top_layer),-soil->w_fw[l]);
          soil->w_fw[l]+=rsub_top_layer;
          rsub_top_tot-=rsub_top_layer;
          if (rsub_top_tot>=-epsilon)
          {
            soil->wtable-=rsub_top_layer/S;
            break;
          }
          else
            soil->wtable=layerbound[l];
          nrsub_top[l]-=rsub_top_layer;
        }
        soil->wtable-=rsub_top_tot/S;
        if(rsub_top_tot<0)
        {
          if(soil->wa<(-rsub_top_tot))
            runoff_out+=rsub_top_tot;
          else
            soil->wa+=rsub_top_tot;
        }
      }
    }
  }                   //else if not frozen

  for(l=0;l<NSOILLAYER;l++)
  {
    if (soil->w_fw[l]>soil->wsats[l]-soil->wpwps[l]-soil->ice_fw[l]-soil->whcs[l])
    {
      tmp_water=soil->w_fw[l]-(soil->wsats[l]-soil->wpwps[l]-soil->whcs[l]-soil->ice_fw[l]);
      soil->w_fw[l]-=tmp_water;

      if(l<(BOTTOMLAYER-1))
        soil->w_fw[l+1]+=tmp_water;
      else
        runoff_out+=tmp_water;
    }
    if (soil->w[l]<0)
    {
      runoff_out+=soil->w[l];
      soil->w[l]=0;
    }
    if (soil->w_fw[l]<0)
    {
      runoff_out+=soil->w_fw[l];
      soil->w_fw[l]=0;
    }
  } /* of for(l=0;l<NSOILLAYER;l++) */

  if(runoff_out<0)
  {
    rsub_top+=runoff_out;
    runoff_out=0;
  }

  if(config->with_nitrogen)
  {
    srunoff=runoff_surface;
    forrootsoillayer(l)
    if (soildepth[l]>soil->freeze_depth[l])
    {
      /* determination of nitrate concentration in mobile water */
      w_mobile=vno3=concNO3_mobile=0;
      /* w_mobile as in Neitsch et al. 2005: Eq. 4:2.1.3 */
      w_mobile = pperc[l]+srunoff+lrunoff[l]+nrsub_top[l]+ndrain_perched_out[l];
      if (w_mobile>epsilon)
      {
        ww=-w_mobile/((1-soil->par->anion_excl)*soil->wsats[l]);  /* Eq 4:2.1.2 */
        vno3=soil->NO3[l]*(1-exp(ww));
        concNO3_mobile=max(vno3/w_mobile, 0);
      }
      /* nitrate movement with percolation */
      /* nitrate percolating from overlying layer */

      soil->NO3[l]+=NO3perc_ly;
      NO3perc_ly=NO3surf=NO3lat=0;
      /* calculate nitrate in surface runoff */
      if(l==0 && srunoff>epsilon)
      {
        NO3surf=NPERCO*concNO3_mobile*srunoff; /* Eq. 4:2.1.5 */
        NO3surf=min(NO3surf,soil->NO3[l]);
        soil->NO3[l]-=NO3surf;
      }
      else
        NO3surf=0;

      srunoff=0.0; /* not used for lower soil layers */
      if (l==0)
        NO3lat=NPERCO*concNO3_mobile*(lrunoff[l]+nrsub_top[l]+ndrain_perched_out[l]); /* Eq. 4:2.1.6 */
      else
        NO3lat=concNO3_mobile*(lrunoff[l]+nrsub_top[l]+ndrain_perched_out[l]); /* Eq. 4:2.1.7 */
      if(NO3lat>epsilon)
      {
        NO3lat=min(NO3lat,soil->NO3[l]);
      }
      else
        NO3lat=0;

      NO3lat=min(NO3lat,soil->NO3[l]);
      soil->NO3[l]-=NO3lat;

      /* nitrate percolating from this layer */
      NO3perc_ly=concNO3_mobile*(pperc[l]);  /*Eq 4:2.1.8*/
      if(NO3perc_ly>epsilon)
      {
        NO3perc_ly=min(NO3perc_ly,soil->NO3[l]);
        soil->NO3[l]-=NO3perc_ly;
      }
      else
        NO3perc_ly=0;

      getoutput(&stand->cell->output,LEACHING,config)+=(NO3surf+NO3lat)*stand->frac;
      stand->cell->balance.n_outflux+=(NO3surf + NO3lat)*stand->frac;
      if(isagriculture(stand->type->landusetype))
        getoutput(&stand->cell->output,NLEACHING_AGR,config)+=(NO3surf+NO3lat)*stand->frac;
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
    } /* if soil depth > freeze_depth */
    getoutput(&stand->cell->output,LEACHING,config)+=NO3perc_ly*stand->frac;
    stand->cell->balance.n_outflux+=NO3perc_ly*stand->frac;
    if(isagriculture(stand->type->landusetype))
      getoutput(&stand->cell->output,NLEACHING_AGR,config)+=NO3perc_ly*stand->frac;
#ifdef SAFE
    forrootsoillayer(l)
    if (soil->NO3[l]<-epsilon)
      fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"infil_perc: Cell(%s) NO3=%g<0 in layer %d",sprintcoord(line,&stand->cell->coord),soil->NO3[l],l);
#endif
  } /* end of if(config->with_nitrogen) */

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

#ifdef CHECK_BALANCE

  n_after=soilstocks(soil);
  n_after.nitrogen=n_after.nitrogen*stand->frac+stand->cell->balance.n_outflux;
  if(fabs(n_after.nitrogen-n_before.nitrogen)>0.0001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s, Cell (lat:%g lon:%g), N balance:%g",
        __FUNCTION__,stand->cell->coord.lat,stand->cell->coord.lon,n_after.nitrogen-n_before.nitrogen);

  water_after=soilwater(&stand->soil);
  balancew=water_after-water_before-prec+runoff_surface+runoff_out+drain_perched_out+rsub_top;
  if(fabs(balancew)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid water balance in %s: balanceW: %g water_before: %g water_after: %g standtype: %s standfrac: %g runoff_surface: %g "
        "drain_perched_out: %g runoff_out: %g rsub_top: %g rw_buff: %g wa: %g infil: %g",
        __FUNCTION__,balancew,water_before,water_after,stand->type->name,stand->frac,runoff_surface,drain_perched_out,runoff_out,rsub_top,soil->rw_buffer,soil->wa,prec);
#endif

  if(stand->type->landusetype!=WETLAND && stand->cell->hydrotopes.skip_cell==FALSE && stand->frac<1-epsilon)
  {
    stand->cell->lateral_water+=(runoff_out+drain_perched_out+rsub_top)*stand->frac;
    if(stand->cell->lateral_water<0)
    {
      forrootsoillayer(l)
                   {
        if(soil->w_fw[l]>stand->cell->lateral_water)
        {
          soil->w_fw[l]+=stand->cell->lateral_water;
          stand->cell->lateral_water=0;
          break;
        }
        if(soil->w[l]*soil->whcs[l]>stand->cell->lateral_water)
        {
          soil->w[l]+=stand->cell->lateral_water/soil->whcs[l];
          stand->cell->lateral_water=0;
          break;
        }
                   }
    }
    return runoff_surface;
  }
  else
    return runoff_surface+runoff_out+drain_perched_out+rsub_top;
} /* of 'infil_perc' */
