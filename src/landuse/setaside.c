/**************************************************************************************/
/**                                                                                \n**/
/**               s  e  t  a  s  i  d  e  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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
#include "tree.h"
#include "agriculture.h"

void mixsoil(Stand *stand1,const Stand *stand2)
{
  int l,index,i;
  Real water1,water2;
  //Real absolute_water1[NSOILLAYER],absolute_water2;
  //Real absolute_ice1[NSOILLAYER],absolute_ice2;
#ifdef CHECK_BALANCE
  Real water_before,water_after;
  water_before=soilwater(&stand1->soil)*stand1->frac+soilwater(&stand2->soil)*stand2->frac+stand1->cell->balance.excess_water;
#endif
  forrootsoillayer(l)
  {
    mixpool(stand1->soil.NH4[l],stand2->soil.NH4[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.NO3[l],stand2->soil.NO3[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].fast.carbon,stand2->soil.pool[l].fast.carbon,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].slow.carbon,stand2->soil.pool[l].slow.carbon,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].fast.nitrogen,stand2->soil.pool[l].fast.nitrogen,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].slow.nitrogen,stand2->soil.pool[l].slow.nitrogen,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.k_mean[l].fast,stand2->soil.k_mean[l].fast,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.k_mean[l].slow,stand2->soil.k_mean[l].slow,
            stand1->frac,stand2->frac);
  }
  for(l=0;l<stand2->soil.litter.n;l++)
  {
    index=findlitter(&stand1->soil.litter,stand2->soil.litter.item[l].pft);
    if(index==NOT_FOUND)
      index=addlitter(&stand1->soil.litter,stand2->soil.litter.item[l].pft)-1;
    mixpool(stand1->soil.litter.item[index].ag.leaf.carbon,
            stand2->soil.litter.item[l].ag.leaf.carbon,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].ag.leaf.nitrogen,
            stand2->soil.litter.item[l].ag.leaf.nitrogen,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].agsub.leaf.carbon,
            stand2->soil.litter.item[l].agsub.leaf.carbon,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].agsub.leaf.nitrogen,
            stand2->soil.litter.item[l].agsub.leaf.nitrogen,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].bg.carbon,stand2->soil.litter.item[l].bg.carbon,
          stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].bg.nitrogen,stand2->soil.litter.item[l].bg.nitrogen,
          stand1->frac,stand2->frac);
    for(i=0;i<NFUELCLASS;i++)
    {
      mixpool(stand1->soil.litter.item[index].ag.wood[i].carbon,
              stand2->soil.litter.item[l].ag.wood[i].carbon,stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[index].ag.wood[i].nitrogen,
              stand2->soil.litter.item[l].ag.wood[i].nitrogen,stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[index].agsub.wood[i].carbon,
              stand2->soil.litter.item[l].agsub.wood[i].carbon,stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[index].agsub.wood[i].nitrogen,
              stand2->soil.litter.item[l].agsub.wood[i].nitrogen,stand1->frac,stand2->frac);
    }
  }
  for(l=0;l<stand1->soil.litter.n;l++)
    if(findlitter(&stand2->soil.litter,stand1->soil.litter.item[l].pft)==NOT_FOUND)
    {
      mixpool(stand1->soil.litter.item[l].ag.leaf.carbon,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].ag.leaf.nitrogen,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].agsub.leaf.carbon,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].agsub.leaf.nitrogen,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].bg.carbon,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].bg.nitrogen,0,
              stand1->frac,stand2->frac);
      for(i=0;i<NFUELCLASS;i++)
      {
        mixpool(stand1->soil.litter.item[l].ag.wood[i].carbon,0,
                stand1->frac,stand2->frac);
        mixpool(stand1->soil.litter.item[l].ag.wood[i].nitrogen,0,
                stand1->frac,stand2->frac);
        mixpool(stand1->soil.litter.item[l].agsub.wood[i].carbon,0,
                stand1->frac,stand2->frac);
        mixpool(stand1->soil.litter.item[l].agsub.wood[i].nitrogen,0,
                stand1->frac,stand2->frac);
      }
    }
  mixpool(stand1->soil.litter.agtop_moist,stand2->soil.litter.agtop_moist,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.agtop_temp,stand2->soil.litter.agtop_temp,stand1->frac,stand2->frac);
  updatelitterproperties(stand1,stand1->frac+stand2->frac);

  for(i=0;i<=NFUELCLASS;i++)
    mixpool(stand1->soil.litter.avg_fbd[i],stand2->soil.litter.avg_fbd[i],
            stand1->frac,stand2->frac);

  mixpool(stand1->soil.meanw1,stand2->soil.meanw1,
          stand1->frac,stand2->frac);
  mixpool(stand1->soil.decomp_litter_mean.carbon,stand2->soil.decomp_litter_mean.carbon,
          stand1->frac,stand2->frac);
  mixpool(stand1->soil.decomp_litter_mean.nitrogen,stand2->soil.decomp_litter_mean.nitrogen,
          stand1->frac,stand2->frac);

  /* snowpack is independent of fraction */
  mixpool(stand1->soil.snowpack,stand2->soil.snowpack,stand1->frac,
          stand2->frac);
  foreachsoillayer(l)
  {
    water1 = (stand1->soil.w[l] * stand1->soil.whcs[l] + stand1->soil.ice_depth[l] + stand1->soil.w_fw[l] + stand1->soil.ice_fw[l])*stand1->frac;
    water2 = (stand2->soil.w[l] * stand2->soil.whcs[l] + stand2->soil.ice_depth[l] + stand2->soil.w_fw[l] + stand2->soil.ice_fw[l])*stand2->frac;
    if ((water1 + water2)>0)
      mixpool(stand1->frac_g[l], stand2->frac_g[l], water1, water2);
    else
      stand1->frac_g[l]=0;
    stand1->soil.state[l]=(short)getstate(stand1->soil.temp+l);
    stand1->soil.w[l]=(stand1->soil.w[l]*stand1->soil.whcs[l]*stand1->frac+stand2->soil.w[l]*stand2->soil.whcs[l]*stand2->frac)/(stand1->frac+stand2->frac);
    mixpool(stand1->soil.whcs[l],stand2->soil.whcs[l],stand1->frac,stand2->frac);
    stand1->soil.w[l]/=stand1->soil.whcs[l];
    mixpool(stand1->soil.w_fw[l],stand2->soil.w_fw[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_depth[l],stand2->soil.ice_depth[l],stand1->frac,
            stand2->frac);
    mixpool(stand1->soil.ice_fw[l],stand2->soil.ice_fw[l],stand1->frac,
            stand2->frac);
    mixpool(stand1->soil.freeze_depth[l],stand2->soil.freeze_depth[l],
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_pwp[l],stand2->soil.ice_pwp[l],stand1->frac,
            stand2->frac);
    mixpool(stand1->soil.wpwps[l],stand2->soil.wpwps[l],stand1->frac,
            stand2->frac);
/*
    absolute_water1[l] = stand1->soil.w[l] * stand1->soil.whcs[l] + stand1->soil.w_fw[l] + stand1->soil.wpwps[l] * (1 - stand1->soil.ice_pwp[l]);
    absolute_water2 = stand2->soil.w[l] * stand2->soil.whcs[l] + stand2->soil.w_fw[l] + stand2->soil.wpwps[l] * (1 - stand2->soil.ice_pwp[l]);
    mixpool(absolute_water1[l], absolute_water2, stand1->frac, stand2->frac);

    absolute_ice1[l] = stand1->soil.ice_depth[l] + stand1->soil.ice_fw[l] + stand1->soil.wpwps[l] * stand1->soil.ice_pwp[l];
    absolute_ice2 = stand2->soil.ice_depth[l] + stand2->soil.ice_fw[l] + stand2->soil.wpwps[l] * stand2->soil.ice_pwp[l];
    mixpool(absolute_ice1[l], absolute_ice2, stand1->frac, stand2->frac);
*/
  }

  for(l=0;l<NTILLLAYER;l++)
    mixpool(stand1->soil.df_tillage[l],stand2->soil.df_tillage[l],stand1->frac,stand2->frac);

  pedotransfer(stand1,NULL,NULL,stand1->frac+stand2->frac);
  //pedotransfer(stand1,absolute_water1,absolute_ice1,stand1->frac+stand2->frac);
  /* bedrock needs to be mixed seperately */
  mixpool(stand1->soil.w[BOTTOMLAYER],stand2->soil.w[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.w_fw[BOTTOMLAYER],stand2->soil.w_fw[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.ice_depth[BOTTOMLAYER],stand2->soil.ice_depth[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.ice_fw[BOTTOMLAYER],stand2->soil.ice_fw[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.freeze_depth[BOTTOMLAYER],stand2->soil.freeze_depth[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.ice_pwp[BOTTOMLAYER],stand2->soil.ice_pwp[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.whc[BOTTOMLAYER], stand2->soil.whc[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.whcs[BOTTOMLAYER], stand2->soil.whcs[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.wpwps[BOTTOMLAYER], stand2->soil.wpwps[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.beta_soil[BOTTOMLAYER], stand2->soil.beta_soil[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.bulkdens[BOTTOMLAYER], stand2->soil.bulkdens[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.k_dry[BOTTOMLAYER], stand2->soil.k_dry[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.Ks[BOTTOMLAYER], stand2->soil.Ks[BOTTOMLAYER], stand1->frac, stand2->frac);

  mixpool(stand1->soil.mean_maxthaw,stand2->soil.mean_maxthaw,stand1->frac,
          stand2->frac);
  mixpool(stand1->soil.alag,stand2->soil.alag,stand1->frac,stand2->frac);
  mixpool(stand1->soil.amp,stand2->soil.amp,stand1->frac,stand2->frac);
  mixpool(stand1->soil.rw_buffer,stand2->soil.rw_buffer,stand1->frac,stand2->frac);
#ifdef CHECK_BALANCE
  water_after=soilwater(&stand1->soil)*(stand1->frac+stand2->frac)+stand1->cell->balance.excess_water;
  if(fabs(water_before-water_after)>1e-2)
    fail(INVALID_WATER_BALANCE_ERR,TRUE,"Invalid water balance=%g=%g-%g in mixsoil()",fabs(water_before-water_after),water_before,water_after);
#endif
} /* of 'mixsoil' */

void mixsetaside(Stand *setasidestand,Stand *cropstand,Bool intercrop)
{
  /*assumes that all vegetation carbon pools are zero after harvest*/
  int p;
  Pft *pft;

  mixsoil(setasidestand,cropstand);

  if(intercrop)
  {
    foreachpft(pft,p,&setasidestand->pftlist)
      mix_veg(pft,setasidestand->frac/(setasidestand->frac+cropstand->frac));
  }

  setasidestand->frac+=cropstand->frac;
} /* of 'mixsetaside' */

Bool setaside(Cell *cell,            /**< Pointer to LPJ cell */
              Stand *cropstand,      /**< crop stand */
              const Pftpar pftpar[], /**< PFT parameter array */
              Bool with_tillage,     /**< tillage  (TRUE/FALSE) */
              Bool intercrop,        /**< intercropping possible (TRUE/FALSE) */
              int npft,              /**< number of natural PFTs */
              Bool irrig,            /**< irrigated stand (TRUE/FALSE) */
              int year               /**< simulation year */
             )                       /** \return stand has to be killed (TRUE/FALSE) */
{
  int s,p,n_est;
  Pft *pft;
  Stocks flux_estab,stocks;
  Irrigation *data;
  /* call tillage before */
  if(with_tillage && year >= param.till_startyear)
    tillage(&cropstand->soil,param.residue_frac);

  s=findlandusetype(cell->standlist,irrig? SETASIDE_IR : SETASIDE_RF);
  if(s!=NOT_FOUND)
  {
    mixsetaside(getstand(cell->standlist,s),cropstand,intercrop);
    return TRUE;
  }
  else
  {
    cropstand->type->freestand(cropstand);
    cropstand->type= irrig? &setaside_ir_stand : &setaside_rf_stand;
    cropstand->type->newstand(cropstand);
    data=cropstand->data;
    data->irrigation= irrig? TRUE : FALSE;
#ifdef SAFE
    if(!isempty(&cropstand->pftlist))
      fail(LIST_NOT_EMPTY_ERR,TRUE,"Pftlist is not empty in setaside().");
#endif
    if(intercrop)
    {
      n_est=0;
      for(p=0;p<npft;p++)
      {
        if(establish(cell->gdd[p],pftpar+p,&cell->climbuf) &&
           pftpar[p].type==GRASS && pftpar[p].cultivation_type==NONE)
        {
          addpft(cropstand,pftpar+p,year,0);
          n_est++;
        }
      }
      flux_estab.carbon=flux_estab.nitrogen=0.0;
      foreachpft(pft,p,&cropstand->pftlist)
      {
        stocks=establishment(pft,0,0,n_est);
        flux_estab.carbon+=stocks.carbon;
        flux_estab.nitrogen+=stocks.nitrogen;
      }
      cell->output.flux_estab.carbon+=flux_estab.carbon*cropstand->frac;
      cell->output.flux_estab.nitrogen+=flux_estab.nitrogen*cropstand->frac;
    }
  }
  return FALSE;
} /* of 'setaside' */

/*
- called in landusechange(), update_daily()
- checks if a set-aside stand already exists
  -> if TRUE: call of local function mixsetaside()
  -> if FALSE: set stand variable landusetype to SETASIDE
       if intercropping is possible:
       -> check if pft of type GRASS could be establish (see
          establish.c)
          -> if TRUE: add pft to the pftlist and call specific
             function establishment() (see pft.h)

mixsetaside()

- mixes certain variables of structure soil of the set-aside stand with the
  considered crop stand
  -> calls local function mixpool() (see landuse.h)
- if already intercropping on set-aside stand reduce the leafs & roots dependent
  on the fraction of the crop stand
  -> calls specific function mix_veg() (see pft.h,mix_veg_grass.c,mix_veg_tree.c)
- adds the land fractions of the crop stand to the set-aside stand
- deletes considered crop stand (see delstand() in standlist.c)
*/
