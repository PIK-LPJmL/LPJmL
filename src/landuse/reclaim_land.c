/**************************************************************************************/
/**                                                                                \n**/
/**          r  e  c  l  a  i  m  _  l a  n  d  .  c                               \n**/
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

static void remove_vegetation_copy(Soil *soil, /* soil pointer */
                                   const Stand *stand, /* stand pointer */
                                   Cell *cell, /* cell pointer */
                                   Real standfrac, /* stand fraction (0..1) */
                                   Bool luc_timber,
                                   const Config *config
                                  )
{
  int p;
  Pft *pft;
  Real nind;
  Real ftimber; /* fraction harvested for timber */
  Stocks harvest;
  Stocks stocks;
  Stocks trad_biofuel;
#if defined IMAGE && defined COUPLED
  Bool tharvest=FALSE;
  if(luc_timber)
    ftimber=min(1,cell->ml.image_data->timber_frac/standfrac);
#else
  Poolpar frac;
  ftimber=1;
  /* fast and slow separation based on wood demand for pulpwood and particles (fast) and sawlog, veneer and others (slow) */
  /* remainder is burnt, if param.fburn is set to 1.0 */
  frac.fast=param.harvest_fast_frac; /* 76% of cut trees is harvested and 26% of harvested wood into fast pools, so 34% of harvested wood goes to fast pools */
  frac.slow=1-param.harvest_fast_frac; /* 50% of all cut trees go to slow, that is 66% of all harvested (76%) */
#endif

  foreachpft(pft,p,&stand->pftlist)
  {
    nind = pft->nind;

    /* if plot is deforested, wood is returned to litter, harvested or burnt
    * allows for mixed use, first harvesting a fraction of the stand,
    * then burning a fraction, then returning the rest to the litter pools
    */
    if(luc_timber)
    {
      if(pft->par->type==TREE)
      {
#if defined IMAGE && defined COUPLED
#ifdef DEBUG_IMAGE
        if/*(ftimber>0 ||
          (cell->coord.lon-.1<-43.25 && cell->coord.lon+.1>-43.25 && cell->coord.lat-.1<-11.75 && cell->coord.lat+.1>-11.75)||
          (cell->coord.lon-.1<94.25 && cell->coord.lon+.1>94.25 && cell->coord.lat-.1<22.25 && cell->coord.lat+.1>22.25))*/
          (cell->coord.lon>102.2 && cell->coord.lon < 102.3 && cell->coord.lat >28.7 && cell->coord.lat< 28.8)
        {
          printf("A %g/%g timber_harvest: %s ftimber %g standfrac %g littersum %g vegcsum %g nind %g %g\n",cell->coord.lon,cell->coord.lat,pft->par->name,ftimber,
                 standfrac,litterstocks(&soil->litter).carbon,vegc_sum(pft),pft->nind,nind);
          fflush(stdout);
        }
#endif
        /* harvesting timber */
        getoutput(&cell->output,FTIMBER,config)=ftimber;
        harvest=timber_harvest(pft,soil,&cell->ml.product,
                               cell->ml.image_data->timber_f,ftimber,standfrac,&nind,&trad_biofuel,config,cell->ml.image_data->timber_frac,cell->ml.image_data->takeaway);
        cell->balance.timber_harvest.carbon+=harvest.carbon;
        cell->balance.timber_harvest.nitrogen+=harvest.nitrogen;
        getoutput(&cell->output,TIMBER_HARVESTC,config)+=harvest.carbon;
        getoutput(&cell->output,NBP,config)-=harvest.carbon;
        getoutput(&cell->output,TIMBER_HARVESTN,config)+=harvest.nitrogen;
        getoutput(&cell->output,TRAD_BIOFUEL,config)+=trad_biofuel.carbon;
        getoutput(&cell->output,NBP,config)-=trad_biofuel.carbon;
        cell->balance.trad_biofuel.carbon+=trad_biofuel.carbon;
        cell->balance.trad_biofuel.nitrogen+=trad_biofuel.nitrogen;
#ifdef DEBUG_IMAGE
        /*if(ftimber>0 ||
          (cell->coord.lon-.1<-43.25 && cell->coord.lon+.1>-43.25 && cell->coord.lat-.1<-11.75 && cell->coord.lat+.1>-11.75)||
          (cell->coord.lon-.1<94.25 && cell->coord.lon+.1>94.25 && cell->coord.lat-.1<22.25 && cell->coord.lat+.1>22.25))*/
        if(cell->coord.lon>102.2 && cell->coord.lon < 102.3 && cell->coord.lat >28.7 && cell->coord.lat< 28.8)
        {
          printf("B %g/%g timber_burn: %s fburn %g standfrac %g littersum %g vegcsum %g nind %g %g\n",cell->coord.lon,cell->coord.lat,pft->par->name,cell->ml.image_data->fburnt,
                 standfrac,litterstocks(&soil->litter).carbon,vegc_sum(pft),pft->nind,nind);
          fflush(stdout);
        }
#endif
#else
        harvest=timber_harvest(pft,soil,frac,param.ftimber,standfrac,&nind,&trad_biofuel,config);
#endif
        getoutput(&cell->output,TRAD_BIOFUEL,config)+=trad_biofuel.carbon;
        getoutput(&cell->output,NBP,config)-=trad_biofuel.carbon;
        cell->balance.trad_biofuel.carbon+=trad_biofuel.carbon;
        cell->balance.trad_biofuel.nitrogen+=trad_biofuel.nitrogen;
        cell->balance.timber_harvest.carbon+=harvest.carbon;
        cell->balance.timber_harvest.nitrogen+=harvest.nitrogen;
        getoutput(&cell->output,TIMBER_HARVESTC,config)+=harvest.carbon;
        getoutput(&cell->output,NBP,config)-=harvest.carbon;
        getoutput(&cell->output,TIMBER_HARVESTN,config)+=harvest.nitrogen;
#if defined IMAGE && defined COUPLED
        /* burning wood */
        getoutput(&cell->output,FBURN,config)=cell->ml.image_data->fburnt;
#ifdef DEBUG_IMAGE_CELL
        printf("fburnt %g %g\n",cell->output.fburn,cell->ml.image_data->fburnt);
        fflush(stdout);
#endif
        stocks=timber_burn(pft,cell->ml.image_data->fburnt,&soil->litter,nind,config);
#else
        stocks=timber_burn(pft,param.fburnt,&soil->litter,nind,config);
#endif
        getoutput(&cell->output,DEFOREST_EMIS,config)+=stocks.carbon*standfrac;
        getoutput(&cell->output,NBP,config)-=stocks.carbon*standfrac;
        cell->balance.deforest_emissions.carbon+=stocks.carbon*standfrac;
        getoutput(&cell->output,DEFOREST_EMIS_N,config)+=stocks.nitrogen*(1-param.q_ash)*standfrac;
        cell->balance.deforest_emissions.nitrogen+=stocks.nitrogen*(1-param.q_ash)*standfrac;
        soil->NO3[0]+=stocks.nitrogen*param.q_ash;
      } /* if tree */
    } /* is timber */
#if defined DEBUG_IMAGE && defined COUPLED
    /*if(ftimber>0 ||
      (cell->coord.lon-.1<-43.25 && cell->coord.lon+.1>-43.25 && cell->coord.lat-.1<-11.75 && cell->coord.lat+.1>-11.75)||
      (cell->coord.lon-.1<94.25 && cell->coord.lon+.1>94.25 && cell->coord.lat-.1<22.25 && cell->coord.lat+.1>22.25))*/
    if(cell->coord.lon>102.2 && cell->coord.lon < 102.3 && cell->coord.lat >28.7 && cell->coord.lat< 28.8)
    {
      printf("C %g/%g timber_burn: %s fburn %g standfrac %g littersum %g vegcsum %g nind %g %g\n",cell->coord.lon,cell->coord.lat,pft->par->name,cell->ml.image_data->fburnt,
             standfrac,litterstocks(&soil->litter).carbon,vegc_sum(pft),pft->nind,nind);
      fflush(stdout);
    }
#endif
    /* rest goes to litter */
    litter_update(&soil->litter,pft,nind,config);
#ifdef DEBUG_IMAGE_CELL
    if(ftimber>0 ||
      (cell->coord.lon-.1<-43.25 && cell->coord.lon+.1>-43.25 && cell->coord.lat-.1<-11.75 && cell->coord.lat+.1>-11.75)||
      (cell->coord.lon-.1<94.25 && cell->coord.lon+.1>94.25 && cell->coord.lat-.1<22.25 && cell->coord.lat+.1>22.25))
    {
      printf("D %g/%g timber_burn: %s fburn %g standfrac %g littersum %g vegcsum %g nind %g %g\n",cell->coord.lon,cell->coord.lat,pft->par->name,cell->ml.image_data->fburnt,
             standfrac,litterstocks(&soil->litter).carbon,vegc_sum(pft),pft->nind,nind);
      fflush(stdout);
    }
#endif

  } /* of foreachpft */
#if defined IMAGE && defined COUPLED
  if(tharvest)
    cell->ml.image_data->timber_frac-=ftimber*standfrac;
#endif
  /* There should be no vegetation on stand2, only the soil carbon was copied to stand2
   * therefore delpft is not necessary here */

}/* of 'remove_vegetation_copy' */

void reclaim_land(const Stand *stand1,Stand *stand2,Cell *cell,Bool luc_timber,int ntotpft,
                  const Config *config)
{
  int l,p;
  Soil *soil;
  soil=&stand2->soil;
  stand2->fire_sum=stand1->fire_sum;

  for (l=0;l<LASTLAYER;l++)
  {
    soil->c_shift[l]=newvec(Poolpar,ntotpft);
    check(soil->c_shift[l]);
  }
  for (p=0;p<ntotpft;p++)
  {
    soil->c_shift[0][p].fast=1;
    soil->c_shift[0][p].slow=1;
  }
  for (l=1;l<LASTLAYER;l++)
    for (p=0;p<ntotpft;p++)
    {
      soil->c_shift[l][p].fast=0;
      soil->c_shift[l][p].slow=0;
    }
  soil->decomp_litter_pft=newvec(Stocks,ntotpft);
  check(soil->decomp_litter_pft);
  copysoil(&stand2->soil,&stand1->soil,ntotpft);
  for(l=0;l<NSOILLAYER;l++)
    stand2->frac_g[l]=stand1->frac_g[l];
  remove_vegetation_copy(&stand2->soil,stand1,cell,stand2->frac,
                         luc_timber,config);
}/* of 'reclaim_land' */

/*
- called in cultivate(), deforest(), regrowth(), landexpansion(),
  grasslandreduction()
- copies the values of the variables of one stand to another stand except of the
  pftlist
- calls local function copysoil()
  -> copysoil() copies the values of all variables in struct Soil (see soil.h)
- calls local function remove_vegetation_crop()
  -> remove_vegetation_crop() removes the pfts from the new stand by updating the
     litter pools of the new stand
*/
