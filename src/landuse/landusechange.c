/**************************************************************************************/
/**                                                                                \n**/
/**               l  a  n  d  u  s  e  c  h  a  n  g  e  .  c                      \n**/
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
#include "natural.h"
#include "grassland.h"
#include "agriculture.h"
#include "biomass_grass.h"
#include "biomass_tree.h"

#define PASTURE 1  /* cultivation type */
#define BIOMASS_TREE_PLANTATION 2
#define BIOMASS_GRASS_PLANTATION 3

#ifdef IMAGE
#define minnatfrac_luc 0.0002
#else
#define minnatfrac_luc 0.0
#endif

void deforest(Cell *cell,            /**< pointer to cell */
              Real difffrac, /**< stand fraction to deforest (0..1) */
              const Pftpar pftpar[], /**< PFT parameter array */
              Bool intercrop, /**< intercropping possible (TRUE/FALSE) */
              int npft,       /**< number of natural PFTs */
              Bool timberharvest,
              Bool istimber, /**< IMAGE coupling (TRUE/FALSE) */
              Bool irrig,    /**< irrigated stand (TRUE/FALSE) */
              int ncft,       /**< number of crop PFTs */
              int year,       /**< simulation year (AD) */
              Real minnatfrac /**< minimum fraction of natural vegetation */
             )
{
  int s,pos;
  Stand *natstand,*cutstand;
  s=findlandusetype(cell->standlist,NATURAL);
  if(s!=NOT_FOUND)
  {
    natstand=getstand(cell->standlist,s);
    if(natstand->frac>minnatfrac)
    {
      if(difffrac+epsilon>=natstand->frac-minnatfrac)
        difffrac=natstand->frac-minnatfrac;
      pos=addstand(&natural_stand,cell)-1;
      cutstand=getstand(cell->standlist,pos);
      cutstand->frac=difffrac;

      reclaim_land(natstand,cutstand,cell,istimber,npft+ncft);
      if(difffrac+epsilon>=natstand->frac)
      {
        delstand(cell->standlist,s);
        pos=s;
      }
      else
        natstand->frac-=difffrac;
      if(!timberharvest)
      {
        if(setaside(cell,getstand(cell->standlist,pos),pftpar,intercrop,npft,irrig,year))
          delstand(cell->standlist,pos);
      }
    }
  }
  else
    fail(NO_NATURAL_STAND_ERR,TRUE,"No natural stand for deforest, difffrac=%g",difffrac);
} /* of 'deforest' */

static void regrowth(Cell *cell, /* pointer to cell */
                     Real difffrac, /* stand fraction to regrowth (0..1) */
                     const Pftpar *pftpar, /* PFT parameter array */
                     int npft, /* number of natural PFTs */
                     int ntypes, /* number of PFT classes */
                     Bool istimber, /* IMAGE coupling (TRUE/FALSE) */
                     Bool irrig,
                     int ncft, /* number of crop PFTs */
                     int year  /* simulation year (AD) */
                    )
{
  int s,pos,p;
  Real flux_estab;
  Pft *pft;
  Stand *setasidestand,*natstand,*mixstand;
  
  s=findlandusetype(cell->standlist,irrig==TRUE ? SETASIDE_IR : SETASIDE_RF);
  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    if(setasidestand->frac<=epsilon-difffrac) 
    {          /*setaside stand has not enough space for regrowth*/
      mixstand=getstand(cell->standlist,s);
      cutpfts(mixstand);
      difffrac= -mixstand->frac;
      pos=s;
    }
    else
    {
      pos=addstand(irrig==TRUE ? &setaside_ir_stand :&setaside_rf_stand,cell)-1; /*setaside big enough for regrowth*/
      mixstand=getstand(cell->standlist,pos);
      mixstand->frac= -difffrac;
      reclaim_land(setasidestand,mixstand,cell,istimber,npft+ncft);
      setasidestand->frac+=difffrac;
    }

    s=findlandusetype(cell->standlist,NATURAL);
    if(s!=NOT_FOUND)
    {        /*mixing of natural vegetation with regrowth*/
      natstand=getstand(cell->standlist,s);
      mixsoil(natstand,mixstand);
      foreachpft(pft,p,&natstand->pftlist)
        mix_veg(pft,natstand->frac/(natstand->frac-difffrac));
      natstand->frac+=mixstand->frac;
      delstand(cell->standlist,pos);
    }
    else
    {
      mixstand->type->freestand(mixstand);
      mixstand->type=&natural_stand;
      new_natural(mixstand);
      natstand=mixstand; 
    }
    natstand->prescribe_landcover = NO_LANDCOVER;
     
    flux_estab=establishmentpft(natstand,
                                pftpar,npft,ntypes,
                                PREC_MAX,year)*natstand->frac;
    cell->output.flux_estab+=flux_estab;
    cell->output.dcflux-=flux_estab;
  }
}/* of 'regrowth' */

static void landexpansion(Cell *cell,            /* cell pointer */
                          Real difffrac,         /* stand fraction to expand */
                          const Pftpar pftpar[], /* PFT parameter array */
                          int npft,              /* number of natural PFTs */
                          int ntypes,            /* number of PFT classes */
                          Stand *grassstand,     /* grassland stand or NULL */
                          Bool irrigation,       /* irrigated stand (TRUE/FALSE) */
                          int cultivate_type,
                          Bool istimber,         /* Image coupling (TRUE/FALSE) */
                          int ncft,              /* number of crop PFTs */
                          int year
                          )
{
  int s,p,pos,q,t;
  Real flux_estab=0;
  int *n_est;
  Pft *pft;
  Irrigation *data;
  Stand *setasidestand,*mixstand;

  s=findlandusetype(cell->standlist,irrigation==TRUE ? SETASIDE_IR : SETASIDE_RF);
  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    if(setasidestand->frac<=epsilon-difffrac) 
    {          /*setaside stand has not enough space for grassland expansion*/
      mixstand=getstand(cell->standlist,s);
      cutpfts(mixstand);
      difffrac= -mixstand->frac;
      pos=s;
    }
    else
    {
      pos=addstand(&natural_stand,cell)-1; /*setaside big enough for grassland expansion*/
      mixstand=getstand(cell->standlist,pos);
      mixstand->frac= -difffrac;
      reclaim_land(setasidestand,mixstand,cell,istimber,npft+ncft);
      setasidestand->frac+=difffrac;
    }

    if(grassstand!=NULL)
    {
      mixsoil(grassstand,mixstand);
      foreachpft(pft,p,&grassstand->pftlist)
        mix_veg(pft,grassstand->frac/(grassstand->frac-difffrac));
      data=grassstand->data;
      data->irrig_stor*=grassstand->frac/(grassstand->frac-difffrac);
      data->irrig_amount*=grassstand->frac/(grassstand->frac-difffrac);
      grassstand->frac+=mixstand->frac;
      delstand(cell->standlist,pos); /* deleting temporary mixstand copy */   
    }
    else
    {
      n_est=newvec(int,ntypes);
      check(n_est);
      for(t=0;t<ntypes;t++)
        n_est[t]=0;
      switch(cultivate_type)
      {
        case PASTURE:
          for(p=0;p<npft;p++)
            if(establish(cell->gdd[p],pftpar+p,&cell->climbuf) &&
              pftpar[p].type==GRASS && pftpar[p].cultivation_type==NONE)
            {
              addpft(mixstand,pftpar+p,year,0);
              n_est[pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type=&grassland_stand;
          new_agriculture(mixstand);
          break;
        case BIOMASS_TREE_PLANTATION:
          for(p=0;p<npft;p++)
            if(establish(cell->gdd[p],pftpar+p,&cell->climbuf) &&
              pftpar[p].type==TREE && pftpar[p].cultivation_type==BIOMASS)
            {
              addpft(mixstand,pftpar+p,year,0);
              n_est[pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type=&biomass_tree_stand;
          new_agriculture(mixstand);
          mixstand->growing_time++;
          mixstand->age++;
          break;
        case BIOMASS_GRASS_PLANTATION:
          for(p=0;p<npft;p++)
            if(establish(cell->gdd[p],pftpar+p,&cell->climbuf) &&
              pftpar[p].type==GRASS && pftpar[p].cultivation_type==BIOMASS)
            {
              addpft(mixstand,pftpar+p,year,0);
              n_est[pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type=&biomass_grass_stand;
          new_agriculture(mixstand);
          break;
        default:
          fail(WRONG_CULTIVATION_TYPE_ERR,TRUE,
               "WRONG CULTIVATION TYPE in landexpansion()");
          break;
      } /* of switch */
      data=mixstand->data;
      data->irrigation=irrigation;
      foreachpft(pft,q,&mixstand->pftlist)
      {
        flux_estab=establishment(pft,0,0,n_est[pft->par->type]);
        if (pft->par->cultivation_type==BIOMASS)
        {
          cell->balance.estab_storage_tree[data->irrigation]-=flux_estab*mixstand->frac;
          flux_estab=0;
        }
        cell->output.flux_estab+=flux_estab*mixstand->frac;
      } /* of foreachpft */
      free(n_est);
    }
  }
} /* of 'landexpansion' */

static void grasslandreduction(Cell *cell,            /* cell pointer */
                               Real difffrac,         /* stand fraction to reduce (0..1) */
                               const Pftpar pftpar[], /* PFT parameter array */
                               Bool intercrop,        /* intercropping possible (TRUE/FALSE) */
                               int npft,              /* number of natural PFTs */
                               int s,                 /* index in stand list */
                               Stand *grassstand,     /* pointer to grassland stand */
                               Bool istimber,         /* Image coupling (TRUE/FALSE) */
                               int ncft,              /* number of crop PFTs */
                               Bool pft_output_scaled,/* pft output scaled with stand frac (TRUE/FALSE)*/
                               int year
                              )
{
  int pos;
  Stand *cutstand;
  Irrigation *data;
  Output *output;

  data=grassstand->data;
  output=&grassstand->cell->output;

  if(grassstand->frac<=difffrac+epsilon)
  {
    /* empty irrig stor and pay back conveyance losses that have been consumed by transport into irrig_stor, only evaporative conv. losses, drainage conv. losses already returned */
    grassstand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*grassstand->cell->coord.area*grassstand->frac;
    grassstand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*grassstand->frac;
    output->mstor_return+=(data->irrig_stor+data->irrig_amount)*grassstand->frac;
    grassstand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->cell->coord.area*grassstand->frac;
    grassstand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->frac;
    output->aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->frac;
    output->aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*grassstand->frac;

    if(pft_output_scaled)
    {
      grassstand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->cell->ml.landfrac[data->irrigation].grass[0];
      grassstand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->cell->ml.landfrac[data->irrigation].grass[1];
      grassstand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*grassstand->cell->ml.landfrac[data->irrigation].grass[0];
      grassstand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*grassstand->cell->ml.landfrac[data->irrigation].grass[1];
    }
    else
    {
      grassstand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
      grassstand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
      grassstand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
      grassstand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
    }

    data->irrig_stor=0;
    data->irrig_amount=0;

    cutpfts(grassstand);
    if(setaside(cell,getstand(cell->standlist,s),pftpar,intercrop,npft,data->irrigation,year))
      delstand(cell->standlist,s);
  }
  else
  {
    pos=addstand(&natural_stand,cell)-1;
    cutstand=getstand(cell->standlist,pos);
    cutstand->frac=difffrac;
    reclaim_land(grassstand,cutstand,cell,istimber,npft+ncft);
    grassstand->frac-=difffrac;
    /* empty irrig stor and pay back conveyance losses that have been consumed by transport into irrig_stor, only evaporative conv. losses, drainage conv. losses already returned */
    grassstand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*grassstand->cell->coord.area*difffrac;
    grassstand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*difffrac;
    output->mstor_return+=(data->irrig_stor+data->irrig_amount)*difffrac;
    grassstand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->cell->coord.area*difffrac;
    grassstand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
    output->aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
    output->aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*difffrac;

    if(pft_output_scaled)
    {
      grassstand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
      grassstand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
      grassstand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*difffrac;
      grassstand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*difffrac;
    }
    else
    {
      grassstand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
      grassstand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
      grassstand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
      grassstand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
    }

    if(setaside(cell,getstand(cell->standlist,pos),pftpar,intercrop,npft,data->irrigation,year))
      delstand(cell->standlist,pos);
  }

} /* of 'grasslandreduction */

void set_irrigsystem(Stand *stand,          /**< stand pointer */
                     int cft,               /**< CFT index (0..ncft-1) */
                     int ncft,              /**< number of crop PFTs */
                     Bool pft_output_scaled /**< pft output scaled with stand frac (TRUE/FALSE)*/
                    )
{
  Irrigation *data;
  Output *output;

  data=stand->data;
  output=&stand->cell->output;

  switch(stand->type->landusetype)
  {
    case AGRICULTURE:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->crop[cft])
      {
        data->irrig_system=stand->cell->ml.irrig_system->crop[cft]; /* irrig_stor was emptied at harvest */
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case GRASSLAND:
      if(data->irrigation) /* if pasture > others but irrig_system not as in input or pastures <= others but irrig_system not as in input */
        if((stand->cell->ml.landfrac[1].grass[1]>stand->cell->ml.landfrac[1].grass[0] && data->irrig_system!=stand->cell->ml.irrig_system->grass[1]) ||
            (stand->cell->ml.landfrac[1].grass[1]<=stand->cell->ml.landfrac[1].grass[0] && data->irrig_system!=stand->cell->ml.irrig_system->grass[0]))
          {
          /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
          stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
          stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
          output->mstor_return+=(data->irrig_stor+data->irrig_amount)*stand->frac;
          stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
          stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          output->aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          output->aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;

          if(pft_output_scaled)
          {
            stand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[data->irrigation].grass[0];
            stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[data->irrigation].grass[1];
            stand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[data->irrigation].grass[0];
            stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[data->irrigation].grass[1];
          }
          else
          {
            stand->cell->output.cft_conv_loss_evap[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
            stand->cell->output.cft_conv_loss_evap[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
            stand->cell->output.cft_conv_loss_drain[rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
            stand->cell->output.cft_conv_loss_drain[rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
          }

          data->irrig_stor=0;
          data->irrig_amount=0;
          /* change irrig_system */
          if(stand->cell->ml.landfrac[1].grass[1]>stand->cell->ml.landfrac[1].grass[0]) /* use irrig system from pasture in case landfrac pasture > landfrac others */
            data->irrig_system=stand->cell->ml.irrig_system->grass[1];
          else
            data->irrig_system=stand->cell->ml.irrig_system->grass[0];
        }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case BIOMASS_TREE:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->biomass_tree)
      {
        /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        output->mstor_return+=(data->irrig_stor+data->irrig_amount)*stand->frac;
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        output->aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        output->aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        if(pft_output_scaled)
        {
          stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[1].biomass_tree;
          stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_tree;
        }
        else
        {
          stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor=0;
        data->irrig_amount=0;
        /* change irrig_system */
        data->irrig_system=stand->cell->ml.irrig_system->biomass_tree;
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case BIOMASS_GRASS:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->biomass_grass)
      {
        /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        output->mstor_return+=(data->irrig_stor+data->irrig_amount)*stand->frac;
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        output->aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        output->aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        if(pft_output_scaled)
        {
          stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[1].biomass_grass;
          stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_grass;
        }
        else
        {
          stand->cell->output.cft_conv_loss_evap[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          stand->cell->output.cft_conv_loss_drain[rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor=0;
        data->irrig_amount=0;
        /* change irrig_system */
        data->irrig_system=stand->cell->ml.irrig_system->biomass_grass;
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    default:
      fail(WRONG_CULTIVATION_TYPE_ERR,TRUE,
           "WRONG CULTIVATION TYPE in set_irrigsystem()");
      break;
  } /* of switch */

  /* ec_canal_type=(stand->soil.par->Ks>20) ? 0 : (stand->soil.par->Ks>=10 && stand->soil.par->Ks<=20) ? 1 : 2; */
  /* link ec to soil type: sand 0.7, loam 0.75, clay 0.85 */
  if(data->irrig_system==SURF)
  {
    data->ec=param.ec_canal[(stand->soil.par->Ks>20) ? 0 : (stand->soil.par->Ks>=10 && stand->soil.par->Ks<=20) ? 1 : 2];
    data->conv_evap=((1-param.ec_canal[0])*0.5)/(1-data->ec); /* evaporative share of conveyance loss changes with soil type (sand: 0.5, loam: 0.6, clay: 0.75) */
  }
  else /* set these parameters also in case of NOIRRIG to avoid zero division */
  {
    data->ec=param.ec_pipe;
    data->conv_evap=0.5; /* for sprinkler and drip: half of conv-losses is assumed to evaporate */
  }

  /* - called in landusechange
   * - function annually updates irrigation systems for grassstand, biomass_grass and biomass_tree stand
   * - crop stands are not changed within current growing season, only with cultivate */

} /*of set_irrigsystem*/


void landusechange(Cell *cell,            /**< pointer to cell */
                   const Pftpar pftpar[], /**< PFT parameter array */
                   int npft,              /**< number of natural PFTs */
                   int ncft,              /**< number of crop PFTs */
                   int ntypes,            /**< number of different PFT classes */
                   Bool intercrop,        /**< intercropping possible (TRUE/FALSE) */
                   Bool istimber,         /**< Image coupling (TRUE/FALSE) */
                   int year,              /**< simulation year (AD) */
                   Bool pft_output_scaled /**< pft output scaled with stand (TRUE/FALSE)*/
                  )
  /* needs to be called before establishment, to ensure that regrowth is possible in the following year*/
{
  Real difffrac,difffrac2,movefrac;
  Stand *stand, *tempstand, *irrigstand;
  Bool irrigation;
  Irrigation *data;
  int cultivation_type;
  Real grassfrac; 
  Real cropfrac;
  Real sum[2]; /* rainfed, irrigated */
  int s,s2,pos;
  Bool i;
#ifdef IMAGE
  Real timberharvest=0;
#endif

  if(cell->ml.dam)
    landusechange_for_reservoir(cell,pftpar,npft,istimber,intercrop,ncft,year);
  /* test if land needs to be reallocated between setaside stands */
  difffrac=crop_sum_frac(cell->ml.landfrac,ncft,cell->ml.reservoirfrac+cell->lakefrac,FALSE)-cell->ml.cropfrac_rf;
  difffrac2=crop_sum_frac(cell->ml.landfrac,ncft,cell->ml.reservoirfrac+cell->lakefrac,TRUE)-cell->ml.cropfrac_ir;

  if(difffrac*difffrac2<-epsilon) /* if one increases while the other decreases */
  {
    s=findlandusetype(cell->standlist,SETASIDE_RF);
    s2=findlandusetype(cell->standlist,SETASIDE_IR);
    if(s!=NOT_FOUND && s2!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      irrigstand=getstand(cell->standlist,s2);
      if(difffrac2<0) /* move irrigated setaside to rainfed setaside */
      {
        movefrac=min(-difffrac2,difffrac);
        if(movefrac+epsilon>=irrigstand->frac)/* move all */
        {
          cutpfts(irrigstand);
          mixsetaside(getstand(cell->standlist,s),irrigstand,intercrop);
          delstand(cell->standlist,s2);
        }
        else
        {
          pos=addstand(&setaside_rf_stand,cell)-1;
          tempstand=getstand(cell->standlist,pos);
          tempstand->frac=movefrac;
          reclaim_land(irrigstand,tempstand,cell,FALSE,npft+ncft);
          if(setaside(cell,getstand(cell->standlist,pos),pftpar,intercrop,npft,FALSE,year))
            delstand(cell->standlist,pos);
          irrigstand->frac-=movefrac;
        }
      }

      else /* move rainfed setaside to irrigated setaside */
      {
        movefrac=min(difffrac2,-difffrac);
        stand=getstand(cell->standlist,s);

        if(movefrac+epsilon>=stand->frac)/* move all */
        {
          cutpfts(stand);
          mixsetaside(getstand(cell->standlist,s2),stand,intercrop);
          delstand(cell->standlist,s);
        }
        else
        {
          pos=addstand(&setaside_ir_stand,cell)-1;
          tempstand=getstand(cell->standlist,pos);
          tempstand->frac=movefrac;
          reclaim_land(stand,tempstand,cell,FALSE,npft+ncft);
          if(setaside(cell,getstand(cell->standlist,pos),pftpar,intercrop,npft,TRUE,year))
             delstand(cell->standlist,pos);
          stand->frac-=movefrac;
        }
      }
    }
  }

  for(i=0;i<2;i++)
  {
    cropfrac= i==0 ? cell->ml.cropfrac_rf : cell->ml.cropfrac_ir;

    difffrac=crop_sum_frac(cell->ml.landfrac,ncft,cell->ml.reservoirfrac+cell->lakefrac,i)-cropfrac; /* hb 8-1-09: added the resfrac, see function AND replaced to BEFORE next three lines */

    grassfrac=cell->ml.landfrac[i].grass[0]+cell->ml.landfrac[i].grass[1]; /* pasture + others */


    if(difffrac>=0.001 && cell->lakefrac+cell->ml.reservoirfrac+cell->ml.cropfrac_rf+cell->ml.cropfrac_ir<0.99999) 
      deforest(cell,difffrac,pftpar,intercrop,npft,FALSE,istimber,i,ncft,year,minnatfrac_luc);  /*deforestation*/
    else if(difffrac<=-0.001) 
      regrowth(cell,difffrac,pftpar,npft,ntypes,istimber,i,ncft,year);        /*regrowth*/

    /* pasture */
    cultivation_type=PASTURE;
    irrigation=i;
    s=findstand(cell->standlist,GRASSLAND,irrigation);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-grassfrac;
      if(difffrac>0.001)
        grasslandreduction(cell,difffrac,pftpar,intercrop,npft,s,stand,istimber,ncft,pft_output_scaled,year);
      else if(difffrac<-0.001)
        landexpansion(cell,difffrac,pftpar,npft,ntypes,stand,irrigation,cultivation_type,istimber,ncft,year);
    }
    else if (grassfrac>0.001)
    {
      difffrac= -grassfrac;
      landexpansion(cell,difffrac,pftpar,npft,ntypes,NULL,irrigation,cultivation_type,istimber,ncft,year);
    }

    /* Biomass plantations */
    cultivation_type=BIOMASS_TREE_PLANTATION;
    irrigation=i;
    s=findstand(cell->standlist,BIOMASS_TREE,irrigation);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-cell->ml.landfrac[i].biomass_tree;
      if(difffrac>0.001)
        grasslandreduction(cell,difffrac,pftpar,intercrop,npft,s,stand,istimber,ncft,pft_output_scaled,year);
      else if(difffrac<-0.001)
        landexpansion(cell,difffrac,pftpar,npft,ntypes,stand,irrigation,
                      cultivation_type,istimber,ncft,year);
    }
    else if (cell->ml.landfrac[i].biomass_tree>0.001)
    {
      difffrac= -cell->ml.landfrac[i].biomass_tree;
      landexpansion(cell,difffrac,pftpar,npft,ntypes,NULL,
        irrigation,cultivation_type,istimber,ncft,year);
    }

    cultivation_type=BIOMASS_GRASS_PLANTATION;
    irrigation=i;
    s=findstand(cell->standlist,BIOMASS_GRASS,irrigation);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-cell->ml.landfrac[i].biomass_grass;
      if(difffrac>0.001)
        grasslandreduction(cell,difffrac,pftpar,intercrop,npft,s,stand,istimber,ncft,pft_output_scaled,year);
      else if(difffrac<-0.001)
        landexpansion(cell,difffrac,pftpar,npft,ntypes,stand,irrigation,
                      cultivation_type,istimber,ncft,year);
    }
    else if (cell->ml.landfrac[i].biomass_grass>0.001)
    {
      difffrac= -cell->ml.landfrac[i].biomass_grass;
      landexpansion(cell,difffrac,pftpar,npft,ntypes,NULL,
        irrigation,cultivation_type,istimber,ncft,year);
    }

    /* End biomass plantations */
  }

  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE) /* do not update for crops, must be done in sowing functions */
      set_irrigsystem(stand,0,ncft,pft_output_scaled); /* no CFT index needed for non-agricultural stands */

#ifdef SAFE
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);
#endif
  sum[0]=sum[1]=0.0;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype!=NATURAL) 
    {
      data=stand->data;
      sum[data->irrigation]+=stand->frac;
    }
  cell->ml.cropfrac_rf=sum[0];
  cell->ml.cropfrac_ir=sum[1];/* could be different from landusefraction input, 
                             due to not harvested winter cereals */
#ifdef IMAGE
  /* if timber harvest not satisfied by agricultural expansion */
  if(istimber && cell->ml.image_data->timber_frac>0.001)
  {
    s=findlandusetype(cell->standlist,NATURAL);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      timberharvest=stand->frac;
      if(timberharvest>0.001)
      {
        /* deforestation without conversion to agricultural land */
        deforest(cell,timberharvest,pftpar,intercrop,npft,TRUE,istimber,FALSE,ncft,year,minnatfrac_luc);
      }
      cell->ml.image_data->timber_frac=0.0;
    }
  }
#endif
} /* of 'landusechange' */


/*
- called in iterateyear.c
- calls the function vec_sum()
  -> vec_sum() sums the fractions of each crop in the considered cell
     -> share of agricultural land in the cell
  -> calls getcelllanduse() (see landuse.h) 
- compares actual share with new share
- if the new crop share is greater than the actual share then calls local function 
  deforest()
  -> deforest() checks if a stand with natural vegetation still exist 
     (see existlandusetype.c in tools)
  -> if no natural stand exists deforest is not possible
     -> error in input file
  -> if natural stand exists: 
     - add new stand to the standlist and save the position of the new stand in
       the standlist (see addstand() in standlist.c)
     - brings new stand to set-aside stand as follows:
       -> calls function reclaim_land() which copies the values of the natural 
          stand to the new stand (except the pftlist) and updates the litter pools
          of the new stand
  -> updates the fraction of the natural stand
     -> deletes the natural stand if fraction is zero
  -> calls the function setaside() which adds the new stand to the set-aside stand 
     if exist, or sets the new stand to the set-aside stand if no set-aside stand 
     still there
      
- if the new crop share is smaller than actual share then calls local function 
  regrowth()
  -> regrowth() checks if set-aside stand exist (see existlandusetype.c in 
     tools)
  -> if no set-aside stand exists regrowth is still not possible
     -> regrowth will be next year
  -> if set-aside stand exists:
     -> distinguish if fraction of set-aside stand is smaller equal or greater 
    than the determined fraction for reforestation
     -> if the fraction is smaller equal the whole set-aside stand gets natural
    -> update of litter pools for the pfts on the set-aside stand
     -> if the fraction is greater a new stand is added to the standlist
    -> call of function reclaim_land()
    -> update of the fraction of the set-aside stand

     -> distinguish if a natural stand exists
    -> if natural stand exist, mix of soil and vegetation (see mixsoil() in 
       setaside.c and specific functions mix_veg_tree/grass.c)
  -> call of function establishmentpft()

- landusechange of managed grassland
  -> distinguish between irrigated and not irrigated managed grassland stand
  -> distinguish if grassland stand exists:
  -> if the new grassland share is smaller than the actual share then calls the
     local function grasslandreduction()
     -> distinguish if fraction of grassland stand is smaller equal or greater 
    than the determined fraction for reforestation
     -> if the fraction is smaller equal the whole grassland stand goes to the 
        set-aside stand
    -> update of litter pools for the pfts on the grassland stand
        -> call of function setaside()
     -> if the fraction is greater a new stand is added to the standlist
    -> call of function reclaim_land()
    -> update of the fraction of the grassland stand
        -> call of function setaside()

  -> if the new grassland share is greater than the actual share then calls the
     local function landexpansion()
     -> if no set-aside stand exists grassland expansion is still not possible
        -> grassland expansion will be next year
     -> if set-aside stand exists:
        -> distinguish if fraction of set-aside stand is smaller equal or greater 
       than the determined fraction for grassland expansion
        -> if the fraction is smaller equal the whole set-aside stand gets 
           grassland
       -> update of litter pools for the pfts on the set-aside stand
        -> if the fraction is greater a new stand is added to the standlist
       -> call of function reclaim_land()
       -> update of the fraction of the set-aside stand

     -> distinguish if a grassland stand exists
     -> if grassland stand exist, mix of soil and vegetation (see mixsoil() in 
    setaside.c and specific function mix_veg_grass.c)
    -> update of the fraction of the grassland stand
     -> if grassland stand does not exist, the temporary mixstand gets grassland 
        stand
        -> call of function establish() and add pft to the pftlist if possible
        -> call of function establishment()
        -> set the variable irrigation

  -> if the grassland stand does not exist and the grassland stand fraction is 
     greater 0 then calls the local function landexpansion()

- sets the actual crop share to the new crop share 
*/
