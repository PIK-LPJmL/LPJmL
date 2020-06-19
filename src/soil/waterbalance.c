/**************************************************************************************/
/**                                                                                \n**/
/**         w  a  t  e  r  b  a  l  a  n  c  e  .  c                               \n**/
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
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"

typedef struct
{
  Real duration,S,tp,tp1,Ks;
} Data;

void waterbalance(Stand *stand,           /**< Stand pointer */
                  Real aet_stand[BOTTOMLAYER],
                  Real green_transp[BOTTOMLAYER], /**< green fraction of transpiration */
                  Real *evap,               /**< evaporation (mm) */
                  Real *evap_blue,          /**< evaporation of irrigation water (mm) */
                  Real wet_all,
                  Real eeq,                 /**< equilibrium evapotranspiration (mm) */
                  Real cover,
                  Real *frac_g_evap,        /**< green water share of soil evaporation */
                  Bool rw_manage            /**< do rain water management? (TRUE/FALSE) */
                 )                          /* \return water runoff (mm) */
{
  Real w_evap=0,w_evap_ice=0,whcs_evap=0,soildepth_evap=param.soildepth_evap,evap_ratio,green_evap=0,marginal;
  Real evap_energy,evap_litter,evap_soil;
  Soil *soil;
  int l;
  Real aet=0,updated_soil_water=0,previous_soil_water[NSOILLAYER],evap_out[BOTTOMLAYER];
  Irrigation *data_irrig;
  if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==GRASSLAND)
    data_irrig=stand->data;
  else
    data_irrig=NULL;

  soil=&stand->soil;
  evap_ratio=0.0;
  for(l=0;l<LASTLAYER;l++)
    evap_out[l]=0;
  *frac_g_evap=0;
  evap_litter=evap_soil=0;


  forrootsoillayer(l)
    aet+=aet_stand[l];

  /* Evaporation */
  evap_energy=eeq*PRIESTLEY_TAYLOR*max(1-cover,0.05);
  if(evap_energy>epsilon && (eeq*PRIESTLEY_TAYLOR*(1-wet_all)-aet)>epsilon)
  {
    if(soil->litter.agtop_wcap>epsilon)
    {
      evap_litter=evap_energy*soil->litter.agtop_moist/soil->litter.agtop_wcap*soil->litter.agtop_moist/soil->litter.agtop_wcap*soil->litter.agtop_cover; /* same as for evap_soil */
      evap_litter=min(evap_litter,soil->litter.agtop_moist); /* for very small agtop_meist the result of the above line can be larger than agtop_moist */
      evap_litter=min(evap_litter,eeq*PRIESTLEY_TAYLOR*(1-wet_all)-aet); /* close energy balance */
    }
    if((1-soil->litter.agtop_cover)>epsilon)
    {
      l=0;
whcs_evap=0;
      do
      {
        /*w_evap is water content in soildepth_evap, i.e. that can evaporate */
        w_evap+=(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l]-aet_stand[l])*min(1,soildepth_evap/soildepth[l]);
        w_evap_ice+=(soil->ice_depth[l]+soil->ice_fw[l])*min(1,soildepth_evap/soildepth[l]);

        /* here frag_g_evap is AMOUNT of green soil water after transpiration in upper 30cm */
        *frac_g_evap+=stand->frac_g[l]*(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l]-aet_stand[l])*min(1,soildepth_evap/soildepth[l]);
        l++;
whcs_evap+=soil->whcs[l]*min(1,soildepth_evap/soildepth[l]);
      }while((soildepth_evap-=soildepth[l-1])>0);
      /* here frag_g_evap becomes FRACTION of green water in upper 30cm */
      *frac_g_evap = w_evap>0 ? *frac_g_evap/w_evap : 1;

//      whcs_evap=param.soildepth_evap*soil->whc[0];
      if(w_evap/whcs_evap<1)
        evap_soil=evap_energy*w_evap/whcs_evap*w_evap/whcs_evap*(1-soil->litter.agtop_cover);
      else
        evap_soil=evap_energy*(1-soil->litter.agtop_cover);    /* if above field cap then it's potential evap*/

      if (evap_soil>(w_evap-w_evap_ice))
        evap_soil=w_evap-w_evap_ice;
    }

  //*evap=min(*evap,eeq*PRIESTLEY_TAYLOR*(1-wet_all)-aet); /*close the energy balance*/

    //if(stand->type->landusetype!=NATURAL)
    if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==GRASSLAND)
      if(data_irrig->irrigation && data_irrig->irrig_system==DRIP)
        evap_soil*=(1-(param.drip_evap*(1-*frac_g_evap))); /*reduced blue soil evaporation in case of DRIP irrigation */

    if(rw_manage)
      evap_soil*=(1-param.esoil_reduction); /* reduced soil evaporation - JH: should this also apply to evap_litter? */

    evap_ratio=(w_evap-w_evap_ice>0) ? evap_soil/(w_evap-w_evap_ice) : 0;

    soil->litter.agtop_moist-=evap_litter;
    *evap=evap_litter+evap_soil;
    /* calculate frac_g of total evap assuming frag_g_evap_litter = 1 */
    *frac_g_evap=(evap_litter+evap_soil)>0 ? (evap_litter+*frac_g_evap*evap_soil)/(evap_litter+evap_soil) : 1;
  }


  soildepth_evap=param.soildepth_evap;
  for (l=0;l<NSOILLAYER;l++)
  {
    previous_soil_water[l]=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
    marginal=0;

    if(l<BOTTOMLAYER)
      /* release transpiration iiicedepth[%d] %3.12f, whcs[%d] %3.12f in line 136 waterbalance\n", l, soil->w[l], l, soil->w_fw[l], soil->ice_depth[l], soil->whcs[l])er */
      soil->w_fw[l]-=aet_stand[l];
    if(soildepth_evap>0)
    {
      /* release evaporation water */
      evap_out[l]=(soil->w[l]*soil->whcs[l]+soil->w_fw[l])*evap_ratio*min(1,soildepth_evap/soildepth[l]);
      soil->w_fw[l]-=evap_out[l];
      soildepth_evap-=soildepth[l];
    }
    if(soil->w_fw[l]<0)
    {
//      printf("w[%d] %3.12f, fw[%d] %3.12f in line 120 waterbalance\n", l, soil->w[l], l, soil->w_fw[l]);
      soil->w[l]+=soil->w_fw[l]/soil->whcs[l];
      soil->w_fw[l]=0;
//      printf("w[%d] %3.12f, fw[%d] %3.12f in line 123 waterbalance\n", l, soil->w[l], l, soil->w_fw[l]);
    }
    if (soil->w[l]< -1e-12)
    {
      fprintf(stderr,"Pixel: %.2f %.2f negative soil water after evap and transp w= %3.5f evap=  %3.5f transp=  %3.2f\n",
          stand->cell->coord.lat,stand->cell->coord.lon,soil->w[l],evap_out[l],aet_stand[l]);
      fflush(stderr);
    }
    /* reallocate water above field capacity to freewater; needed here since thawing permafrost can increase soil->w */
    if (soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>1)
    {
//      printf("w[%d] %3.12f, fw[%d] %3.12f in line 133 waterbalance\n", l, soil->w[l], l, soil->w_fw[l]);
      soil->w_fw[l]+=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
#ifdef DEBUG
fprintf(stderr,"w[%d] %3.12f, fw[%d] %3.12f, icedepth[%d] %3.12f, whcs[%d] %3.12f in line 135 waterbalance\n", l, soil->w[l], l, soil->w_fw[l], l,soil->ice_depth[l],l, soil->whcs[l]);
#endif
      marginal+=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1)*soil->whcs[l];
      soil->w[l]-=soil->w[l]+soil->ice_depth[l]/soil->whcs[l]-1;
#ifdef DEBUG
 fprintf(stderr,"w[%d] %3.12f, fw[%d] %3.12f, icedepth[%d] %3.12f, whcs[%d] %3.12f in line 136 waterbalance\n", l, soil->w[l], l, soil->w_fw[l],l, soil->ice_depth[l],l, soil->whcs[l]);
#endif
    }
    if (fabs(soil->w_fw[l])<1e-12)
    {
      marginal+=soil->w_fw[l];
      soil->w_fw[l]=0;
    }
    if (soil->w[l]<1e-12)
    {
      if (soil->w[l]<-epsilon)
      {
        fail(NEGATIVE_SOIL_MOISTURE_ERR, TRUE,
             "LINE 146 Pixel: %f %f Soil-moisture %d negative: %g, lutype %s soil_type %s in waterbalance()",
             stand->cell->coord.lat, stand->cell->coord.lon, l, soil->w[l], stand->type->name, soil->par->name);
      }
      marginal += soil->w[l] * soil->whcs[l];
      soil->w[l] = 0;
    }
    if(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l]<1e-12)
      stand->frac_g[l]=1.0;

    /* adapt shares of green water */
    updated_soil_water=soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
    if(l<BOTTOMLAYER)
    {
      if((updated_soil_water+marginal)!=previous_soil_water[l])
      {
#ifdef SAFE
        if(fabs(updated_soil_water+marginal-previous_soil_water[l])-fabs(aet_stand[l]+evap_out[l])>epsilon)
        {
          fprintf(stderr,"Pixel: %.2f %.2f, soil water balance error change=  %3.5f evap=  %3.5f transp=  %3.5f balance=  %3.5f\n",stand->cell->coord.lat,stand->cell->coord.lon,previous_soil_water[l]-updated_soil_water,evap_out[l],aet_stand[l],previous_soil_water[l]-updated_soil_water-aet_stand[l]-evap_out[l]);
          fflush(stderr);
        }   
#endif
        if(updated_soil_water>epsilon)
        {
          green_transp[l]=aet_stand[l]*stand->frac_g[l];
          green_evap=evap_out[l]*stand->frac_g[l];
          *evap_blue+=evap_out[l]*(1-stand->frac_g[l]); /*blue evap*/
          stand->frac_g[l]=(previous_soil_water[l]*stand->frac_g[l]-green_transp[l]-green_evap-marginal)/updated_soil_water; /* new green fraction equals old green amount - green losses divided by total water */
        }
        else
        {
          green_transp[l]=aet_stand[l]*stand->frac_g[l];
          *evap_blue+=evap_out[l]*(1-stand->frac_g[l]); /*blue evap*/
          stand->frac_g[l]=1.0;
        }
      }
      if(stand->frac_g[l]< -0.01 || stand->frac_g[l]>1.01)
      {
        fprintf(stderr,"ERROR214: cell %.2f %.2f frac_g error in waterbalance(), frac_g=  %3.6f layer= %d w= %3.9f w_fw= %3.9f evap= %3.9f transp= %3.9f marginal= %3.9f standtype= %s\n",stand->cell->coord.lat,stand->cell->coord.lon,stand->frac_g[l],l,soil->w[l]*soil->whcs[l],soil->w_fw[l],evap_out[l],aet_stand[l],marginal,stand->type->name);
        fflush(stderr);
      }
    }

#ifdef SAFE
    if(l==BOTTOMLAYER)
    {
      if(stand->frac_g[l]< -0.01 || stand->frac_g[l]>1.01)
      {
        fprintf(stderr,"ERROR214: cell  %.2f %.2f frac_g error in waterbalance() at bottomlayer, frac_g=  %3.6f layer= %d w= %3.9f w_fw= %3.9f standtype= %s\n",stand->cell->coord.lat,stand->cell->coord.lon,stand->frac_g[l],l,soil->w[l]*soil->whcs[l],soil->w_fw[l],stand->type->name);
        fflush(stderr);
      }
      if(fabs(updated_soil_water+marginal-previous_soil_water[l])>epsilon)
      {
        fprintf(stderr,"bottomlayer error updated=  %3.12f previous=  %3.5f standtype= %s\n",updated_soil_water,previous_soil_water[l],stand->type->name);
        fflush(stderr);
      }
    }
    if (soil->w[l]< -1e-12)
    {
      fprintf(stderr,"Pixel: %.2f %.2f aet= %3.5f evap=  %3.5f cover=  %3.2f soilwater=%.6f wsats[%d]=%.6f\n",
              stand->cell->coord.lat,stand->cell->coord.lon,aet_stand[l],*evap,cover,allwater(soil,l)+allice(soil,l),l,soil->wsats[l]);
      fflush(stderr);
     fail(NEGATIVE_SOIL_MOISTURE_ERR,TRUE,
          "line 210 Pixel: %f %f Soil-moisture %d negative: %g, lutype %s soil_type %s in waterbalance()",
          stand->cell->coord.lat,stand->cell->coord.lon,l,soil->w[l],stand->type->name,soil->par->name);
    }
#endif
    if(stand->frac_g[l]>1)
      stand->frac_g[l]=1;
    if(stand->frac_g[l]<0)
      stand->frac_g[l]=0;
  stand->cell->discharge.drunoff+=marginal*stand->frac;
  } /* soil layer loop */
} /* of 'waterbalance' */
