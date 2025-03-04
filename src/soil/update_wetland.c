/**************************************************************************************/
/**                                                                                \n**/
/**             u p d a t e _ w e t l a n d .  c                                   \n**/
/**                                                                                \n**/
/**        Scheme to update wetland area using a gamma distribution                \n**/
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
#include "wetland.h"

#define MINCHANGE 1.e-4
#define MINSIZE 4.e-6
#define ADJUST_CSHIFT

void update_wetland(Cell *cell,          /**< pointer to cell */
                    int ntotpft,         /**< total number of PFTs */
                    int ncft,            /**< number of crop PFTs */
                    int year,            /**< simulation year */
                    const Config *config /**< LPJmL configuration */
                   )
{
  Stand *stand;
  Pft *pft, *wetpft, *pft_save;
  int p,pn, l;
  int s,s2, pos;
  int wetlandstandnum,natstandnum;
  int *position;
  Bool *present;
  Real wetlandarea_old, wetlandarea_new, delta_wetland;
  Stand *natstand, *wetstand, *wetstand2;
  Real tmp, slope, slope_max;
  Real wtable_use, lambda;
  Real cti_min, cti_max, p_min, p_max;
  //Real cti_min_max, p_min_max;
  Bool iswetland_change, iswetland;
  Pool ctotal;
  Poolpar kmean_pft, cshift;
  Real socfraction,crop_wetland;
  Real frac;
#ifdef CHECK_BALANCE
  Stand *checkstand;
  Stocks st;
  Stocks start={0,0};
  Stocks end={0,0};
  Real water_before=0;
  Real water_after=0;
#endif
  position = newvec(int, ntotpft);
  check(position);
  present = newvec(Bool, ntotpft);
  check(present);
  for (p = 0; p<ntotpft; p++)
    present[p] = FALSE;
  s2=NOT_FOUND;
#ifdef CHECK_BALANCE
  water_before=cell->balance.excess_water;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    start.carbon+=(st.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    start.nitrogen+=st.nitrogen*stand->frac;
    water_before+=soilwater(&stand->soil)*stand->frac;
  }
#endif
  wetlandstandnum=natstandnum=NOT_FOUND;
  wetlandarea_old=wetlandarea_new=crop_wetland=0.;
  slope=cell->slope_min;
  iswetland_change = iswetland = FALSE;
  s = findlandusetype(cell->standlist, NATURAL);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
  if(s!=NOT_FOUND)
  {
    natstand = getstand(cell->standlist,s);
    natstandnum=s;
  }
  s = findlandusetype(cell->standlist, WETLAND);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
  if (s != NOT_FOUND)
  {
    wetstand = getstand(cell->standlist, s);
    iswetland = TRUE;
    wetlandarea_old=wetstand->frac;
    wetlandstandnum=s;
  }

  //  modify this to use wtable wanted -> wtable_mean
  wtable_use = cell->hydrotopes.wtable_mean;
  if (wtable_use>0) wtable_use = 0;
#ifdef CHECK_BALANCE2
  if(year==1846)
  {
    foreachstand(stand,s,cell->standlist)
      fprintf(stderr,"%s: year= %d standfrac: %g standtype: %d iswetland: %d \n",
               __FUNCTION__,year,stand->frac, stand->type->landusetype,stand->soil.iswetland);
  }
#endif

  if (!cell->hydrotopes.skip_cell && wtable_use >= -4.5 && natstandnum!=NOT_FOUND)
  {
    natstand = getstand(cell->standlist,natstandnum);
    // -----------------------------------------------------------------------------------------------
    //  determine wetland area
    //  min / max cti values
    cti_min = natstand->soil.par->efold*(config->hydropar.wland_min - wtable_use) + cell->hydrotopes.cti_mean;   //Gleichung 10 Kleinen etal 2013
    cti_max = natstand->soil.par->ctimax;
    //cti_min_max = natstand->soil.par->efold*(hydropar.wland_min - cell->hydrotopes.wtable_max) + cell->hydrotopes.cti_mean; /*maximum should be cell->hydrotopes.wetland_wtable_max*/

    //  min / max p
    p_min = gammp(cell->hydrotopes.cti_phi, ((cti_min - cell->hydrotopes.cti_mu) / cell->hydrotopes.cti_chi));
    if (cti_max >= cell->hydrotopes.cti_mu)
      p_max = gammp(cell->hydrotopes.cti_phi, ((cti_max - cell->hydrotopes.cti_mu) / cell->hydrotopes.cti_chi));
    else
    {
      p_max = 0.;
    }

    //p_min_max = gammp(cell->hydrotopes.cti_phi,((cti_min_max - cell->hydrotopes.cti_mu) / cell->hydrotopes.cti_chi));

    //wetlandarea_old = cell->hydrotopes.wetland_area;
    wetlandarea_new = (p_max - p_min)*(1-cell->lakefrac);

    if (wetlandarea_new > 0.)
    {
      //   running mean of wetland area
      cell->hydrotopes.wetland_area_runmean = runmean_add(cell->hydrotopes.wetland_area_runmean, wetlandarea_new, (float)HYD_AV_TIME);

      //   determine CTI index at wetland area mean
      tmp = itersolve(p_min, p_max, cti_min, cti_max, (p_min + p_max) / 2., cell->hydrotopes.cti_phi,
                      cell->hydrotopes.cti_chi, cell->hydrotopes.cti_mu, 0);
      //   running mean of cti in wetland area
      cell->hydrotopes.wetland_cti_runmean = runmean_add(cell->hydrotopes.wetland_cti_runmean, tmp, (Real)HYD_AV_TIME);
    }

    if (cell->hydrotopes.wetland_area > 0.)
    {
      //    is change for wetland reached?
      if (fabs(cell->hydrotopes.wetland_area_runmean - cell->hydrotopes.wetland_area) >= MINCHANGE)
      {
        //     update all values
        cell->hydrotopes.wetland_area = cell->hydrotopes.wetland_area_runmean;
        cell->hydrotopes.wetland_cti = cell->hydrotopes.wetland_cti_runmean;
        iswetland_change = TRUE;
        wetlandarea_new = cell->hydrotopes.wetland_area;

        //      remove wetland if too small
        if (cell->hydrotopes.wetland_area_runmean < MINSIZE)
        {
          cell->hydrotopes.wetland_area = 0.;
          cell->hydrotopes.wetland_cti = 0.;
          wetlandarea_new = cell->hydrotopes.wetland_area;
        }
      }
    }
    else
    {
      //    is minimum size for wetland reached?
      if (cell->hydrotopes.wetland_area_runmean >= MINSIZE)
      {
        //      update all values
        cell->hydrotopes.wetland_area = cell->hydrotopes.wetland_area_runmean;
        cell->hydrotopes.wetland_cti = cell->hydrotopes.wetland_cti_runmean;
        iswetland_change = TRUE;
        wetlandarea_new = cell->hydrotopes.wetland_area;
      }
    }
    //    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
    //    if(s!=NOT_FOUND)
    //    {
    //      setasidestand=getstand(cell->standlist,s);
    //      crop_wetland=setasidestand->frac;
    //    }
    foreachstand(stand,s,cell->standlist)
    {
      if(stand->soil.iswetland && stand->type->landusetype!=WETLAND)
        crop_wetland+=stand->frac;
    }

    delta_wetland = wetlandarea_new - wetlandarea_old-crop_wetland;
    //iswetland_change = FALSE;
    // -----------------------------------------------------------------------------------------------
    //   Size actually changed? change C pools, stand size, etc.
    if (iswetland_change && (wetlandarea_new>crop_wetland))
    {
      cell->hydrotopes.changecount++;
      //    check standlist for wetland

      wetlandstandnum = findlandusetype(cell->standlist, WETLAND);

      if (wetlandstandnum != NOT_FOUND)
        wetstand = getstand(cell->standlist, wetlandstandnum);
      // -----------------------------------------------------------------------------------------------
      //    wetland grows
      if (delta_wetland > 0)
      {
        //      currently no wetland stand
        if (wetlandstandnum == NOT_FOUND)
        {
#ifdef CHECK_BALANCE
        if(year==1846)
        {
          fprintf(stderr,"XXX update_wetland.c wetland not exist .\n");
          foreachstand(checkstand,s,cell->standlist)
            fprintf(stderr,"type %s frac:%g s: %d iswetland: %d delta_wetland: %g wetlandnew: %g wetlandarea_new: %g crop_wetland: %g\n",
                    checkstand->type->name,checkstand->frac,s,checkstand->soil.iswetland,delta_wetland,wetlandarea_new-crop_wetland,wetlandarea_new,crop_wetland);
        }
#endif
          pos = addstand(&wetland_stand, cell) - 1;
          wetlandstandnum = pos;
          wetstand = getstand(cell->standlist, pos);
          wetstand->soil.decomp_litter_pft=newvec(Stocks,ntotpft);
          check(wetstand->soil.decomp_litter_pft);
          for (l = 0; l<LASTLAYER; l++)
          {
            wetstand->soil.c_shift[l] = newvec(Poolpar, ntotpft);
            check(wetstand->soil.c_shift[l]);
          }
          frac = natstand->frac;
          if(frac<(wetlandarea_new-crop_wetland))
            wetstand->frac = frac;
          else
            wetstand->frac = wetlandarea_new-crop_wetland;
          delta_wetland=wetstand->frac-wetlandarea_old;
          wetstand->Hag_Beta = natstand->Hag_Beta;
          wetstand->slope_mean = natstand->slope_mean;
          wetstand->type = &wetland_stand;
          copysoil(&wetstand->soil, &natstand->soil, ntotpft);
          wetstand->soil.iswetland = TRUE;
          for (l = 0; l<NSOILLAYER; l++)
            wetstand->frac_g[l] = 1;
//          //        make sure there is no C in slow pool
//          forrootsoillayer(l)
//          {
//            wetstand->soil.pool[l].fast.carbon += wetstand->soil.pool[l].slow.carbon;
//            wetstand->soil.pool[l].fast.nitrogen+=wetstand->soil.pool[l].slow.nitrogen;
//            wetstand->soil.pool[l].slow.carbon=wetstand->soil.pool[l].slow.nitrogen=0.;
//
//          } /* of forrootsoillayer() */
          //        copy PFT list to wetland stand
          foreachpft(pft, p, &natstand->pftlist)
          {
            wetpft = addpft(wetstand,pft->par,year,365,config);
            //mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac);
            // mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac);
            copypft(wetpft,pft);
          }
          natstand->frac = frac - delta_wetland;

          if(natstand->frac<0)
          {
            //       remove stand
            //printf("DELETE NATURAL STAND\n");
            delstand(cell->standlist,natstandnum);
            natstandnum = NOT_FOUND;
          }
#ifdef CHECK_BALANCE
          if(year==1846)
            foreachstand(checkstand,s,cell->standlist)
              fprintf(stderr,"type %s frac:%g s: %d iswetland: %d delta_wetland: %g wetlandarea_old: %g delta_wetland: %g\n",
                      checkstand->type->name,checkstand->frac,s,checkstand->soil.iswetland,delta_wetland,wetlandarea_old,delta_wetland);
#endif
        }
        else
        {
          //        currently there is wetland stand
          wetstand = getstand(cell->standlist, wetlandstandnum);
          frac = natstand->frac;
          //        modify soil C pools -> acrotelm C density mixture of wetland and non-wetland SOM
          natstand->frac = min(frac,delta_wetland); // make mixsoil and mix_veg_stock work correctly
          delta_wetland=natstand->frac;
          pos = 0;
          mixsoil(wetstand, natstand,year,ntotpft,config);
          foreachpft(pft, p, &wetstand->pftlist)
          {
            present[pft->par->id] = TRUE;
            position[pft->par->id] = pos;
            pos++;
          }
          foreachpft(pft, p, &natstand->pftlist)
          {

            if (present[pft->par->id])
            {
              wetpft = getpft(&wetstand->pftlist, position[pft->par->id]);   //TODO
              if(mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac,config))
              {
                delpft(&wetstand->pftlist,position[pft->par->id]);
                delpft(&natstand->pftlist,p);
                p--;
                pos=0;
                foreachpft(pft_save, pn, &wetstand->pftlist)
                {
                  present[pft_save->par->id] = TRUE;
                  position[pft_save->par->id] = pos;
                  pos++;
                }
              }
            }
            else
            {
              wetpft = addpft(wetstand,pft->par,year,365,config);
              if(mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac,config))
              {
                delpft(&wetstand->pftlist,wetstand->pftlist.n-1);
                delpft(&natstand->pftlist,p);
                p--;
                pos=0;
                foreachpft(pft_save, pn, &wetstand->pftlist)
                {
                  present[pft_save->par->id] = TRUE;
                  position[pft_save->par->id] = pos;
                  pos++;
                }
              }
            }
          }
          for (p = 0; p<ntotpft; p++)
            present[p] = FALSE;
          foreachpft(pft, p, &natstand->pftlist)
          present[pft->par->id] = TRUE;
          foreachpft(pft, p, &wetstand->pftlist)
          if(!present[pft->par->id])
            mix_veg(pft,wetstand->frac/(wetstand->frac+natstand->frac));
          natstand->frac=frac-delta_wetland;
          wetstand->frac+=delta_wetland;
          //    make sure there is no C in slow pool
//          forrootsoillayer(l)
//          {
//            wetstand->soil.pool[l].fast.carbon+=wetstand->soil.pool[l].slow.carbon;
//            wetstand->soil.pool[l].fast.nitrogen+=wetstand->soil.pool[l].slow.nitrogen;
//            wetstand->soil.pool[l].slow.carbon=wetstand->soil.pool[l].slow.nitrogen=0.;
//
//          } /* of forrootsoillayer */
#ifdef CHECK_BALANCE
         if(year==1846)
          foreachstand(checkstand,s,cell->standlist)
            fprintf(stderr,"1 wetland exists and grows type %s frac:%g s: %d iswetland: %d delta_wetland: %g wetlandarea_old: %g delta_wetland: %g\n",
                    checkstand->type->name,checkstand->frac,s,checkstand->soil.iswetland,delta_wetland,wetlandarea_old,delta_wetland);
#endif
        }


        if(natstand->frac <= 0.)
        {
          //       remove stand
          //printf("DELETE NATURAL STAND\n");
          delstand(cell->standlist,natstandnum);
          natstandnum = NOT_FOUND;
        }
        if(wetstand->frac <= 0.)
        {
          //       remove stand
          //printf("DELETE WETLAND STAND\n");
          delstand(cell->standlist,wetlandstandnum);
          wetlandstandnum = NOT_FOUND;
        }

#ifdef CHECK_BALANCE
        water_after=cell->balance.excess_water;
        foreachstand(stand, s, cell->standlist)
        {
          st=standstocks(stand);
          end.carbon+=(st.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
          end.nitrogen+=st.nitrogen*stand->frac;
          water_after+=soilwater(&stand->soil)*stand->frac;
        }
        if (fabs(start.carbon - end.carbon)>0.001)
          fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s: %g start:%g  end:%g",
               __FUNCTION__,start.carbon - end.carbon, start.carbon, end.carbon);
        if (fabs(start.nitrogen - end.nitrogen)>0.001)
          fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s: %g start:%g  end:%gn",
               __FUNCTION__,start.nitrogen - end.nitrogen, start.nitrogen, end.nitrogen);
        if (fabs(water_before - water_after)>0.001)
          fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "1 Invalid water balance in %s: %g start:%g  end:%g excess_water:%g\n",
               __FUNCTION__, water_before - water_after, water_before, water_after,cell->balance.excess_water);
#endif
        check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac,ncft);

      }
      // -----------------------------------------------------------------------------------------------

      else if (delta_wetland < 0.)
      {
#ifdef CHECK_BALANCE
         if(year==1846)
          foreachstand(checkstand,s,cell->standlist)
            fprintf(stderr,"2 wetland exists and shrinks type %s frac:%g s: %d iswetland: %d delta_wetland: %g wetlandarea_old: %g delta_wetland: %g\n",
                    checkstand->type->name,checkstand->frac,s,checkstand->soil.iswetland,delta_wetland,wetlandarea_old,delta_wetland);
#endif
        if (wetlandstandnum == NOT_FOUND)
        {
          //printf("XXX update_wetland.c wants to shrink non-existent wetland.\n");
        }
        else
        {
          wetstand = getstand(cell->standlist, wetlandstandnum);
          //fprintf(stderr,"XXX update_wetland.c wants to shrink in %d.   wetlandarea_new: %.4f wetlandarea_old: %.4f\n",year,wetlandarea_new,wetlandarea_old);
          if(-delta_wetland>wetstand->frac)
            delta_wetland = -wetstand->frac;
          //fprintf(stderr,"wetfrac: %g natfrac: %g crop_wetland: %g delta_wetland: %g\n",wetstand->frac,natstand->frac,crop_wetland,delta_wetland);

          //        mix wetland soil carbon into non-wetland
          frac = wetstand->frac;
          wetstand->frac = -delta_wetland;  // trick to make mixsoil do it
          pos = 0;
          foreachpft(pft, p, &natstand->pftlist)
          {
            present[pft->par->id] = TRUE;
            position[pft->par->id] = pos;
            pos++;
          }
          mixsoil(natstand, wetstand,year,ntotpft,config);

          foreachpft(wetpft, p, &wetstand->pftlist)
          {
            if (present[wetpft->par->id])
            {
              pft = getpft(&natstand->pftlist, position[wetpft->par->id]);   //STODO
              if(mix_veg_stock(pft, wetpft, natstand->frac, wetstand->frac,config))
              {
                delpft(&natstand->pftlist,position[wetpft->par->id]);
                delpft(&wetstand->pftlist,p);
                p--;
                pos=0;
                foreachpft(pft_save, pn, &natstand->pftlist)
                {
                  present[pft_save->par->id] = TRUE;
                  position[pft_save->par->id] = pos;
                  pos++;
                }
              }
           }
            else
            {
              if (wetpft->par->peatland)
              {
                pft = addpft(natstand,wetpft->par,year,365,config);
                if(mix_veg_stock(pft, wetpft, natstand->frac, wetstand->frac,config))
                {
                  delpft(&wetstand->pftlist,p);
                  delpft(&natstand->pftlist,natstand->pftlist.n-1);
                  p--;
                  pos=0;
                  foreachpft(pft_save, pn, &natstand->pftlist)
                  {
                    present[pft_save->par->id] = TRUE;
                    position[pft_save->par->id] = pos;
                    pos++;
                  }
                }
                else
                {
                  litter_update(&natstand->soil.litter, pft,pft->nind,config);
                  delpft(&natstand->pftlist,natstand->pftlist.n-1);
                }
              }
              else
              {
                pft = addpft(natstand,wetpft->par,year,365,config);
                if(mix_veg_stock(pft, wetpft, natstand->frac, wetstand->frac,config))
                {
                  delpft(&natstand->pftlist,natstand->pftlist.n-1);
                  delpft(&wetstand->pftlist,p);
                  p--;
                  pos=0;
                  foreachpft(pft_save, pn, &natstand->pftlist)
                  {
                    present[pft_save->par->id] = TRUE;
                    position[pft_save->par->id] = pos;
                    pos++;
                  }
                }

              }
            }
          }
          //mixsoil(natstand, wetstand,year,config);

          for (p = 0; p<ntotpft; p++)
            present[p] = FALSE;
          foreachpft(pft, p, &wetstand->pftlist)
          present[pft->par->id] = TRUE;
          foreachpft(pft, p, &natstand->pftlist)
          if(!present[pft->par->id])
            mix_veg(pft,natstand->frac/(natstand->frac+wetstand->frac));

          //        shrink wetland stand
          wetstand->frac=frac+delta_wetland;
          natstand->frac-= delta_wetland;

          //        no wetland left?
          if (wetstand->frac <= 0.)
          {
            //          remove stand
            printf("DELETE WETLAND STAND\n");
            delstand(cell->standlist, wetlandstandnum);
            wetlandstandnum = NOT_FOUND;
          }
        }
      }  // shrink wetland end
      s = findlandusetype(cell->standlist, WETLAND);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
      if(s == NOT_FOUND)
        s = findlandusetype(cell->standlist, SETASIDE_WETLAND);
      else
        s2 = findlandusetype(cell->standlist, SETASIDE_WETLAND);
      if (s != NOT_FOUND)
      {
        wetstand = getstand(cell->standlist, s);
        frac=wetstand->frac;
        if(s2 != NOT_FOUND)
        {
          wetstand2 = getstand(cell->standlist, s);
          frac+=wetstand2->frac;
        }
        iswetland = TRUE;

        if(cell->slope>0)
          lambda=1/cell->slope;
        else
          lambda=1000;
        slope=cell->slope_min;

        if (iswetland && (fabs(cell->slope_min - cell->slope_max)>epsilon))
        {
          slope_max=log(1 - frac - epsilon)/(-lambda);
          if(slope_max>cell->slope_max) slope_max=cell->slope_max;
          if (slope_max<cell->slope_min)
          {
            slope_max = cell->slope_min;
            slope = cell->slope_min;
          }
          else
          {
            slope = (exp(lambda*-slope_max)*(-1 / lambda - slope_max)) - (exp(lambda*-cell->slope_min)*(-1 / lambda - cell->slope_min));  //calculation of the integral of the PDF to get  mean slope
            //slope /= (-(exp(-lambda*slope_max) - (exp(-lambda*cell->slope_min))));                                                 //normalising the mean slope for a specific range
          }
          if(cell->slope<slope)
            slope=cell->slope;
          wetstand->slope_mean = slope;
          wetstand->Hag_Beta = min(1, (0.03*log(slope + 0.1) + 0.22) / 0.43);

          s=findlandusetype(cell->standlist,NATURAL);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
          if(s == NOT_FOUND)
            s = findlandusetype(cell->standlist, AGRICULTURE);
          if(s == NOT_FOUND)
            s = findlandusetype(cell->standlist, GRASSLAND);
          if(s!=NOT_FOUND)
          {
            natstand = getstand(cell->standlist,s);
            slope=exp(lambda*-cell->slope_max)*(-1/lambda-cell->slope_max)- exp(lambda*-slope_max)*(-1/lambda-slope_max);  //calculation of the integral of the PDF to get  mean slope
            slope/=-(exp(-lambda*cell->slope_max)-exp(-lambda*slope_max));                                                 //normalising the mean slope for a specific range
            natstand->Hag_Beta=min(1,(0.06*log(slope+0.1)+0.22)/0.43);
            natstand->slope_mean=slope;
          }
        }
        else
        {
          s=findlandusetype(cell->standlist,NATURAL);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
          if(s == NOT_FOUND)
            s = findlandusetype(cell->standlist, AGRICULTURE);
          if(s == NOT_FOUND)
            s = findlandusetype(cell->standlist, GRASSLAND);
          if(s!=NOT_FOUND)
          {
            natstand = getstand(cell->standlist,s);
            natstand->slope_mean=cell->slope;
            natstand->Hag_Beta=min(1,(0.06*log(cell->slope+0.1)+0.22)/0.43);
          }
        }
        foreachstand(stand, s, cell->standlist)
        {
          if(stand->type->landusetype!=WETLAND && stand->type->landusetype!=SETASIDE_WETLAND)
          {
            if(natstand!=NULL)
            {
              stand->slope_mean=natstand->slope_mean;
              stand->Hag_Beta=min(1,(0.06*log(stand->slope_mean+0.1)+0.22)/0.43);
            }
          }
          else
          {
            if(wetstand!=NULL)
            {
              stand->slope_mean=wetstand->slope_mean;
              stand->Hag_Beta=min(1,(0.03*log(stand->slope_mean+0.1)+0.22)/0.43);
            }
          }
        }

      }
    }
  } /* of' if cell->hydrotopes.skip_cell*/

 #ifdef ADJUST_CSHIFT
  foreachstand(stand, s, cell->standlist)
  {
    if ((stand->soil.count / NDAYYEAR) >= 100 && stand->soil.c_shift[0][0].fast>soildepth[0] / layerbound[BOTTOMLAYER - 1])
    {
      ctotal.fast.carbon = ctotal.slow.carbon = 0.0;
      ctotal.fast.nitrogen = ctotal.slow.nitrogen = 0.0;
      forrootsoillayer(l)
      {
        ctotal.fast.carbon += stand->soil.pool[l].fast.carbon;
        ctotal.slow.carbon += stand->soil.pool[l].slow.carbon;
        ctotal.fast.nitrogen += stand->soil.pool[l].fast.nitrogen;
        ctotal.slow.nitrogen += stand->soil.pool[l].slow.nitrogen;
      }
      for (p = 0; p<stand->soil.litter.n; p++)
      {
        cshift.slow=cshift.fast=0;
        kmean_pft.fast = kmean_pft.slow = 0.;
        forrootsoillayer(l)
        {
          socfraction = pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l])
            - (l>0 ? pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l - 1]) : 0);

          kmean_pft.fast += socfraction*stand->soil.k_mean[l].fast / (stand->soil.count / NDAYYEAR);
          kmean_pft.slow += socfraction*stand->soil.k_mean[l].slow / (stand->soil.count / NDAYYEAR);
        }

        forrootsoillayer(l)
        {
          socfraction = pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l])
            - (l>0 ? pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l - 1]) : 0);

          if (stand->soil.decomp_litter_mean.carbon>100)
          {
            if (ctotal.fast.carbon>5000)   stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].fast = (socfraction*stand->soil.k_mean[l].fast / (stand->soil.count / NDAYYEAR)) / kmean_pft.fast;
            if (ctotal.slow.carbon>1000)   stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].slow = (socfraction*stand->soil.k_mean[l].slow / (stand->soil.count / NDAYYEAR)) / kmean_pft.slow;
          }
          cshift.fast += stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].fast;
          cshift.slow += stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].slow;
        }
        forrootsoillayer(l)
        {
          stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].slow /= cshift.slow;
          stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].fast /= cshift.fast;
        }
      } /* of for(p=0;p<stand->soil.litter.n;p++) */
      forrootsoillayer(l)
      stand->soil.k_mean[l].slow = stand->soil.k_mean[l].fast = 0.0;
      stand->soil.count = 0;
      stand->soil.decomp_litter_mean.carbon = 0.0;
    }
  }
#endif

  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac,ncft);


#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen=0;
  water_after=cell->balance.excess_water;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    water_after+=soilwater(&stand->soil)*stand->frac;
  }
  if (fabs(start.carbon - end.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s: %g start:%g  end:%g",
         __FUNCTION__,start.carbon - end.carbon, start.carbon, end.carbon);
  if (fabs(start.nitrogen - end.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s: %g start:%g  end:%g",
         __FUNCTION__,start.nitrogen - end.nitrogen, start.nitrogen, end.nitrogen);
  if (fabs(water_before - water_after)>epsilon)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s: %g start:%g  end:%g lakefrac:%g wetlandfrac:%g ",
         __FUNCTION__,water_before - water_after, water_before, water_after,cell->lakefrac, cell->wetlandfrac);
#endif
  free(present);
  free(position);
} /* of 'update_wetland' */
