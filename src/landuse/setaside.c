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
  forrootsoillayer(l)
  {
    mixpool(stand1->soil.cpool[l].fast,stand2->soil.cpool[l].fast,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.cpool[l].slow,stand2->soil.cpool[l].slow,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.k_mean[l].fast,stand2->soil.k_mean[l].fast,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.k_mean[l].slow,stand2->soil.k_mean[l].slow,
            stand1->frac,stand2->frac);
  }
  for(l=0;l<stand2->soil.litter.n;l++)
  {
    index=findlitter(&stand1->soil.litter,stand2->soil.litter.ag[l].pft);
    if(index==NOT_FOUND)
      index=addlitter(&stand1->soil.litter,stand2->soil.litter.ag[l].pft)-1;
    mixpool(stand1->soil.litter.ag[index].trait.leaf,
            stand2->soil.litter.ag[l].trait.leaf,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.bg[index],stand2->soil.litter.bg[l],
          stand1->frac,stand2->frac);
    for(i=0;i<NFUELCLASS;i++)
      mixpool(stand1->soil.litter.ag[index].trait.wood[i],
              stand2->soil.litter.ag[l].trait.wood[i],stand1->frac,stand2->frac);
  }
  for(l=0;l<stand1->soil.litter.n;l++)
    if(findlitter(&stand2->soil.litter,stand1->soil.litter.ag[l].pft)==NOT_FOUND)
    {
      mixpool(stand1->soil.litter.ag[l].trait.leaf,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.bg[l],0,
              stand1->frac,stand2->frac);
      for(i=0;i<NFUELCLASS;i++)
        mixpool(stand1->soil.litter.ag[l].trait.wood[i],0,
                stand1->frac,stand2->frac);
    }
  for(i=0;i<=NFUELCLASS;i++)
    mixpool(stand1->soil.litter.avg_fbd[i],stand2->soil.litter.avg_fbd[i],
            stand1->frac,stand2->frac);

  mixpool(stand1->soil.meanw1,stand2->soil.meanw1,
          stand1->frac,stand2->frac);
  mixpool(stand1->soil.decomp_litter_mean,stand2->soil.decomp_litter_mean,
          stand1->frac,stand2->frac);

  /* snowpack is independent of fraction */
  mixpool(stand1->soil.snowpack,stand2->soil.snowpack,stand1->frac,
          stand2->frac);
  foreachsoillayer(l)
  {
    mixpool(stand1->soil.w[l],stand2->soil.w[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.w_fw[l],stand2->soil.w_fw[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_depth[l],stand2->soil.ice_depth[l],stand1->frac,
            stand2->frac);
    mixpool(stand1->soil.ice_fw[l],stand2->soil.ice_fw[l],stand1->frac,
            stand2->frac);
    mixpool(stand1->soil.freeze_depth[l],stand2->soil.freeze_depth[l],
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_pwp[l],stand2->soil.ice_pwp[l],stand1->frac,
            stand2->frac);
    stand1->soil.state[l]=(short)getstate(stand1->soil.temp+l);

    water1=(stand1->soil.w[l]*stand1->soil.par->whcs[l]+stand1->soil.ice_depth[l]+stand1->soil.w_fw[l]+stand1->soil.ice_fw[l])*stand1->frac;
    water2=(stand2->soil.w[l]*stand2->soil.par->whcs[l]+stand2->soil.ice_depth[l]+stand2->soil.w_fw[l]+stand2->soil.ice_fw[l])*stand2->frac;
    if((water1+water2)>0)
      mixpool(stand1->frac_g[l],stand2->frac_g[l],water1,water2);
  }
  mixpool(stand1->soil.w_evap,stand2->soil.w_evap,stand1->frac,stand2->frac);
  if ((stand1->soil.w_evap*stand1->soil.par->whc[0]*param.soildepth_evap)>(stand1->soil.w[0]*stand1->soil.par->whcs[0]))
    stand1->soil.w_evap=min(1.0,stand1->soil.w[0]*soildepth[0]/param.soildepth_evap);
  if (stand1->soil.w_evap<0)
    stand1->soil.w_evap=0;

  mixpool(stand1->soil.mean_maxthaw,stand2->soil.mean_maxthaw,stand1->frac,
          stand2->frac);
  mixpool(stand1->soil.alag,stand2->soil.alag,stand1->frac,stand2->frac);
  mixpool(stand1->soil.amp,stand2->soil.amp,stand1->frac,stand2->frac);
  mixpool(stand1->soil.rw_buffer,stand2->soil.rw_buffer,stand1->frac,stand2->frac);
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
              Bool intercrop,        /**< intercropping possible (TRUE/FALSE) */
              int npft,              /**< number of natural PFTs */
              Bool irrig,            /**< irrigated stand (TRUE/FALSE) */
              int year               /**< simulation year */
             )                       /** \return stand has to be killed (TRUE/FALSE) */
{
  int s,p,n_est;
  Pft *pft;
  Real flux_estab;
  Irrigation *data;

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
      flux_estab=0.0;
      foreachpft(pft,p,&cropstand->pftlist)
        flux_estab+=establishment(pft,0,0,n_est);
      cell->output.flux_estab+=flux_estab*cropstand->frac;
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
