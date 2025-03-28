/**************************************************************************************/
/**                                                                                \n**/
/**            c  u  l  t  i  v  a  t  e  .  c                                     \n**/
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
#include "grassland.h"
#include "agriculture.h"

Stocks cultivate(Cell *cell,           /**< cell pointer */
                 Bool irrigation,      /**< irrigated (TRUE/FALSE) */
                 int day,              /**< day (1..365) */
                 Bool wtype,           /**< winter type (TRUE/FALSE) */
                 Stand *setasidestand, /**< pointer to setaside stand */
                 int npft,             /**< number of natural PFTs */
                 int ncft,             /**< number of crop PFTs */
                 int cft,              /**< cft index for set_irrigsystem */
                 int year,             /**< AD */
                 Bool isother,         /**< other stand (TRUE/FALSE) */
                 const Config *config  /**< LPJmL configuration */
                )                      /** \return establihment flux (gC/m2,gN/m2) */
{
  int pos; /*position of new stand in list*/
  int vern_date20;
  Pft *pft;
  Stand *cropstand;
  Irrigation *data;
  Stocks bm_inc;
  Pftcrop *crop;
  Real manure=0;
  Real fertil;
  Real landfrac;
#ifdef CHECK_BALANCE
  int s;
  Stand *stand;
  Stocks start={0,0};
  Real water_before=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area;
  Real water_after=0;
  foreachstand(stand,s,cell->standlist)
  {
    start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
    water_before+=soilwater(&stand->soil)*stand->frac;
  }
#endif

#ifdef IMAGE
  int nagr,s;
  Stand *stand;
#endif
  vern_date20=cell->ml.cropdates[cft].vern_date20;
  landfrac=(isother) ? cell->ml.landfrac[irrigation].grass[0] : cell->ml.landfrac[irrigation].crop[cft];
#ifdef IMAGE
  nagr=2*ncft;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==AGRICULTURE)
      nagr--;

  if(landfrac>setasidestand->frac-nagr*1e-7)
  {
    landfrac=max(setasidestand->frac-nagr*1e-7,1e-8);
  }
#endif
  if(landfrac>=setasidestand->frac-epsilon)
  {
    setasidestand->type->freestand(setasidestand);
    setasidestand->type=(isother) ? &others_stand : &agriculture_stand;
    setasidestand->type->newstand(setasidestand);
    /* delete all PFTs */
    cutpfts(setasidestand,config);
    cropstand=setasidestand;
    cropstand->soil.iswetland=setasidestand->soil.iswetland;
#ifdef DEBUG3
    fprintf(stdout,"cultivate HIER day: %d cft: %d type: %s landfrac: %g  setasidefrac: %g isother: %d defores.nitrogen: %g timber_harvest.n: %g iswetland: %d \n",day,cft,
        setasidestand->type->name,landfrac,setasidestand->frac,isother,cell->balance.deforest_emissions.nitrogen,cell->balance.timber_harvest.nitrogen,cropstand->soil.iswetland);
#endif
  }
  else
  {
    pos=addstand((isother) ? &others_stand : &agriculture_stand,cell);
    cropstand=getstand(cell->standlist,pos-1);
    cropstand->frac=landfrac;
    reclaim_land(setasidestand,cropstand,cell,config->luc_timber,npft+ncft,config);
#ifdef DEBUG3
    fprintf(stdout,"cultivate HIER2 day: %d cft: %d type: %s landfrac: %g setasidefrac: %g  isother: %d iswetland: %d \n",day,cft,
        setasidestand->type->name,landfrac,setasidestand->frac,isother,setasidestand->soil.iswetland);
#endif

    setasidestand->frac-=landfrac;
  }
  if(cell->ml.with_tillage)
  {
    tillage(&cropstand->soil,param.residue_frac);
    updatelitterproperties(cropstand,cropstand->frac);
    if(config->soilpar_option==NO_FIXED_SOILPAR || (config->soilpar_option==FIXED_SOILPAR && year<config->soilpar_fixyear))
      pedotransfer(cropstand,NULL,NULL,cropstand->frac);
  }
  data=cropstand->data;
  data->irrigation= (config->irrig_scenario==ALL_IRRIGATION) || irrigation;
  set_irrigsystem(cropstand,cft,npft,ncft,config);
  if(cft==(config->rice_pft-npft))
  {
    cropstand->slope_mean=0;
    cropstand->Hag_Beta=min(1,(0.09*log(cropstand->slope_mean+0.1)+0.22)/0.43);
    cropstand->soil.iswetland=TRUE;
  }
  else
  {
    cropstand->Hag_Beta=min(1,(0.06*log(cropstand->slope_mean+0.1)+0.22)/0.43);
    cropstand->soil.iswetland=FALSE;
  }
  pft=addpft(cropstand,config->pftpar+npft+cft,year,day,config);
  phen_variety(pft,vern_date20,cell->coord.lat,day,wtype,npft,ncft,config);
#ifdef DEBUG3
  if(cft==config->rice_pft-npft)
    fprintf(stdout,"cultivate A D D P F T year: %d day: %d bminc: %g cft: %d irrig: %d standfrac: %g landfrac: %g setasidfrac: %g iswetland: %d \n",year,day,pft->bm_inc.carbon,cft,irrigation,cropstand->frac,landfrac,setasidestand->frac,setasidestand->soil.iswetland);
#endif
  bm_inc.carbon=pft->bm_inc.carbon*cropstand->frac;
  bm_inc.nitrogen=pft->bm_inc.nitrogen*cropstand->frac;
  if (cell->ml.manure_nr != NULL)
  {
    manure = (isother) ? cell->ml.manure_nr[irrigation].grass[0] : cell->ml.manure_nr[irrigation].crop[cft];
    cropstand->soil.NH4[0] += manure*param.nmanure_nh4_frac*param.nfert_split_frac;
    cropstand->soil.litter.item->agsub.leaf.carbon += manure*param.manure_cn*param.nfert_split_frac;
    cropstand->soil.litter.item->agsub.leaf.nitrogen += manure*(1-param.nmanure_nh4_frac)*param.nfert_split_frac;
    cell->balance.influx.carbon += manure*param.manure_cn*cropstand->frac*param.nfert_split_frac;
    cell->balance.influx.nitrogen += manure*cropstand->frac*param.nfert_split_frac;
    getoutput(&cell->output,NMANURE_AGR,config)+=manure*cropstand->frac*param.nfert_split_frac;
    getoutput(&cell->output,NAPPLIED_MG,config)+=manure*cropstand->frac*param.nfert_split_frac;
    /* store remainder of manure for second application */
    crop = pft->data;
    crop->nmanure=manure*(1-param.nfert_split_frac);
  }
  if (cell->ml.fertilizer_nr != NULL)
  {
    fertil = (isother) ? cell->ml.fertilizer_nr[irrigation].grass[0] : cell->ml.fertilizer_nr[irrigation].crop[cft];
    cropstand->soil.NO3[0] += fertil*param.nfert_no3_frac*param.nfert_split_frac;
    cropstand->soil.NH4[0] += fertil*(1 - param.nfert_no3_frac)*param.nfert_split_frac;
    cell->balance.influx.nitrogen += fertil*param.nfert_split_frac*cropstand->frac;
    getoutput(&cell->output,NFERT_AGR,config)+=fertil*param.nfert_split_frac*cropstand->frac;
    getoutput(&cell->output,NAPPLIED_MG,config)+=fertil*param.nfert_split_frac*cropstand->frac;
    /* store remainder of fertilizer for second application */
    crop = pft->data;
    crop->nfertilizer = fertil*(1-param.nfert_split_frac);
  }
#ifdef CHECK_BALANCE
  Real end=0;
  water_after=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area;
 foreachstand(stand,s,cell->standlist)
  {
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    water_after+=soilwater(&stand->soil)*stand->frac;
  }
  if (fabs(end-start.carbon-bm_inc.carbon-manure*param.manure_cn*cropstand->frac*param.nfert_split_frac)>0.001)
  {
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s: day: %d   %.4f start: %.4f  end: %.3f  bm_inc.carbon: %.4f manure: %.4f\n",
         "cropstand->frac: %g cropstand.carbon: %g setasidestand->frac: %g setasidestand.carbon: %g",
         __FUNCTION__,day,end-start.carbon-bm_inc.carbon-manure*param.manure_cn*cropstand->frac*param.nfert_split_frac,
         start.carbon, end,bm_inc.carbon,manure*param.manure_cn*cropstand->frac*param.nfert_split_frac,
         cropstand->frac,(standstocks(cropstand).carbon + soilmethane(&cropstand->soil)),setasidestand->frac,(standstocks(setasidestand).carbon + soilmethane(&setasidestand->soil)*WC/WCH4));
  }
  end=0;
  foreachstand(stand,s,cell->standlist)
  {
    end+=standstocks(stand).nitrogen*stand->frac;
  }
  if (fabs(end-start.nitrogen-bm_inc.nitrogen-(manure+fertil)*cropstand->frac*param.nfert_split_frac)>0.01)
  {
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s: day: %d  cft: %d %.4f start: %.4f  end: %.3f  bm_inc.nitrogen: %.4f manure: %.4f fertil: %f\n",
         "cropstand->frac: %g cropstand.nitrogen: %g setasidestand->frac: %g setasidestand.nitrogen: %g defores.nitrogen: %g timber_harvest.n: %g\n ",
         __FUNCTION__,day,cft,end-start.nitrogen-bm_inc.nitrogen-(manure+fertil)*cropstand->frac*param.nfert_split_frac,
         start, end,bm_inc.nitrogen,manure*cropstand->frac*param.nfert_split_frac,fertil*cropstand->frac*param.nfert_split_frac,
        cropstand->frac,standstocks(cropstand).nitrogen,setasidestand->frac,standstocks(setasidestand).nitrogen,cell->balance.deforest_emissions.nitrogen,cell->balance.timber_harvest.nitrogen);
  }
  if(fabs(water_before-water_after)>0.001)
  {
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s: day %d water_after: %g water_before: %g",
          __FUNCTION__,day,water_after,water_before);
  }


#endif

    /*cropstand->soil.NH4[0] += manure*fmanure_NH4;
    cropstand->soil.litter.item->agsub.leaf.carbon += manure*param.manure_cn;
    cropstand->soil.litter.item->agsub.leaf.nitrogen += manure*(1 - fmanure_NH4);
    cell->output.flux_estab.carbon += manure*param.manure_cn*cropstand->frac;
    cell->balance.flux_estab.carbon += manure*param.manure_cn*cropstand->frac;
    cell->balance.n_influx += manure*cropstand->frac;

    if (manure*fmanure_NH4 < param.nfert_split)
    {
      if (fertil <= (param.nfert_split - manure*fmanure_NH4))
      {
        cropstand->soil.NO3[0] += fertil*split_fert;
        cropstand->soil.NH4[0] += fertil*(1 - split_fert);
        cell->balance.n_influx += fertil*cropstand->frac;
      }
      else
      {
        cropstand->soil.NO3[0] += (param.nfert_split - manure*fmanure_NH4)*split_fert;
        cropstand->soil.NH4[0] += (param.nfert_split - manure*fmanure_NH4)*(1 - split_fert);
        cell->balance.n_influx += (param.nfert_split - manure*fmanure_NH4)*cropstand->frac;
        crop = pft->data;
        crop->nfertilizer = fertil - (param.nfert_split - manure*fmanure_NH4);
      }
    }
    else
    {
      crop = pft->data;
      crop->nfertilizer = fertil;
    }*/
  return bm_inc;
} /* of 'cultivate' */

/*
- called in sowing()
- comparison of the land fraction (landfrac) of the considered cft with the 
  fraction of the set-aside stand (setasidestand->frac)
  -> is the land fraction of the cft greater or equal as the fraction of the
     set-aside stand:
  -> sets the landusetype of the set-aside stand to AGRICULTURE 
     (defined in stand.h)
  -> kills all pfts of the set-aside stand and updates the litter pools
  -> adds considered cft to the pftlist of the stand (see addpft() in 
     pftlist.h)
     (-> addpft() calls function newpft() (see newpft.c);
      -> newpft() calls specific functions (here new_crop.c, see below)) 
  -> creates a variable crop of type Pftcrop with the informations of the 
     crop-specific variables of the new cft (see getpft() in pftlist.h) 
     with the aim to change informations
  -> calls function phen_variety() (see below)
  -> sets wtype to TRUE or FALSE (this information comes from function sowing())
  -> sets irrigation to TRUE or FALSE (this information comes from function
     sowing())

  -> is the land fraction of the cft smaller as the fraction of the set-aside 
     stand
     -> adds a new stand to the standlist (see addstand() in standlist.c)
     -> addstand() returns the length of the standlist which is also the
        position of the new stand in the standlist
     -> creates a variable cropstand of type Stand with the informations of 
        the new stand (see getstand() in stand.h) with the aim to change 
        informations
     -> calls function reclaim_land()
     -> adds considered cft to the pftlist of the stand (see addpft() in 
        pftlist.h)
     -> creates a variable crop of type Pftcrop with the informations of the
        crop-specific variables of the new cft (see getpft() in pftlist.h) 
        with the aim to change informations
     -> calls function phen_variety() (see below)
     -> sets wtype to TRUE or FALSE (this information comes from function 
        sowing())
     -> sets the landusetype of the new cropstand to AGRICULTURE 
     -> sets irrigation to TRUE or FALSE (this information comes from function
        sowing())
     -> sets the frac of the new cropstand to the landfrac
     -> subtracts the frac of the set-aside stand with the landfrac
*/
