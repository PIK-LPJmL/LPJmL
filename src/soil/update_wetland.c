/*----------------------------------------------------------*/
/*                                                          */
/*                  update_wetland.c                        */
/*  check for and create wetland in climLPJ                 */
/*                                                          */
/*  Thomas Kleinen (thomas.kleinen@zmaw.de)                 */
/*  28/04/2009                                              */
/*                                                          */
/*  Last change: $Date:: 2019-06-24 16:13:15 +0200 (Mo, 2#$ */
/*  By         : $Author:: sibylls                        $ */
/*                                                          */
/*----------------------------------------------------------*/

#include "lpj.h"
#include "natural.h"
#include "wetland.h"

//#define CHECK_BALANCE
#define MINCHANGE 1.e-4
#define MINSIZE 4.e-6
#define ADJUST_CSHIFT

void update_wetland(Cell *cell,          /**< pointer to cell */
                    int ntotpft,         /**< total number of PFTs */
                    int year,            /**< simulation year */
                    const Config *config /**< LPJmL configuration */
                   )
{
  Stand *stand;
  Pft *pft, *wetpft;
  int p, l;
  int s, pos;
  int wetlandstandnum;
  int *position;
  Bool *present;
  Real wetlandarea_old, wetlandarea_new, delta_wetland;
  Stand *natstand, *wetstand;
  Real tmp, slope, slope_max;
  Real wtable_use, lambda;
  Real cti_min, cti_max, p_min, p_max;
  //Real cti_min_max, p_min_max;
  Bool iswetland_change, iswetland;
  Pool ctotal;
  Poolpar kmean_pft, cshift;
  Real socfraction;
  Real frac;
#ifdef CHECK_BALANCE
  Stocks st;
  Stocks start={0,0};
  Stocks end={0,0};
#endif
  position = newvec(int, ntotpft);
  check(position);
  present = newvec(Bool, ntotpft);
  check(present);
  for (p = 0; p<ntotpft; p++)
    present[p] = FALSE;
#ifdef CHECK_BALANCE
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    start.carbon+=(st.carbon+soilmethane(&stand->soil))*stand->frac;
    start.nitrogen+=st.nitrogen*stand->frac;
  }
#endif
  wetlandstandnum = NOT_FOUND;
  wetlandarea_old = wetlandarea_new = 0.;
  iswetland_change = iswetland = FALSE;
  s = findlandusetype(cell->standlist, NATURAL);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
  if(s==NOT_FOUND)
    return;
  natstand = getstand(cell->standlist, s);
  s = findlandusetype(cell->standlist, WETLAND);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
  if (s != NOT_FOUND) 
  {
    wetstand = getstand(cell->standlist, s);
    iswetland = TRUE;
  }

  //  modify this to use wtable wanted -> wtable_mean
  wtable_use = cell->hydrotopes.wtable_mean;        /*mean over stands, NEXT try should cell->hydrotopes.wtable_mean*/
  if (wtable_use>0) wtable_use = 0;

  if (!cell->hydrotopes.skip_cell && wtable_use >= -4.5)
  {
    // -----------------------------------------------------------------------------------------------
    //  determine wetland area
    //  min / max cti values
    cti_min = natstand->soil.par->efold*(hydropar.wland_min - wtable_use) + cell->hydrotopes.cti_mean;   //Gleichung 10 Kleinen etal 2013
    cti_max = natstand->soil.par->ctimax;
    //cti_min_max = natstand->soil.par->efold*(hydropar.wland_min - cell->hydrotopes.wtable_max) + cell->hydrotopes.cti_mean; /*maximum should be cell->hydrotopes.wetland_wtable_max*/

    //  min / max p
    p_min = gammp(cell->hydrotopes.cti_phi, ((cti_min - cell->hydrotopes.cti_mu) / cell->hydrotopes.cti_chi));
    if (cti_max >= cell->hydrotopes.cti_mu)
      p_max = gammp(cell->hydrotopes.cti_phi, ((cti_max - cell->hydrotopes.cti_mu) / cell->hydrotopes.cti_chi));
    else
    {
      p_max = 0.;
      cell->hydrotopes.skip_cell = TRUE;
    }

    //p_min_max = gammp(cell->hydrotopes.cti_phi,((cti_min_max - cell->hydrotopes.cti_mu) / cell->hydrotopes.cti_chi));

    wetlandarea_old = cell->hydrotopes.wetland_area;
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

    delta_wetland = wetlandarea_new - wetlandarea_old;

    // -----------------------------------------------------------------------------------------------
    //   Size actually changed? change C pools, stand size, etc.
    if (iswetland_change)
    {
      cell->hydrotopes.changecount++;
      //    check standlist for wetland

      wetlandstandnum = findlandusetype(cell->standlist, WETLAND);

      if (wetlandstandnum != NOT_FOUND)
        wetstand = getstand(cell->standlist, wetlandstandnum);
      // -----------------------------------------------------------------------------------------------
      //    wetland grows
      // printf("delta_wetland=%g\n",delta_wetland);
      if (delta_wetland > 0)
      {
        //      currently no wetland stand
        if (wetlandstandnum == NOT_FOUND)
        {
          //printf("XXX update_wetland.c wetland not exist .\n");
          pos = addstand(&wetland_stand, cell) - 1;
          wetlandstandnum = pos;
          wetstand = getstand(cell->standlist, pos);
          for (l = 0; l<LASTLAYER; l++)
          {
            wetstand->soil.c_shift[l] = newvec(Poolpar, ntotpft);
            check(wetstand->soil.c_shift[l]);
          }
          frac = natstand->frac;
          wetstand->frac = wetlandarea_new;
          wetstand->Hag_Beta = natstand->Hag_Beta;
          wetstand->slope_mean = natstand->slope_mean;
          wetstand->type = &wetland_stand;
          copysoil(&wetstand->soil, &natstand->soil, ntotpft);
          wetstand->soil.iswetland = TRUE;
          for (l = 0; l<NSOILLAYER; l++)
            wetstand->frac_g[l] = natstand->frac_g[l];
          //        make sure there is no C in slow pool
          forrootsoillayer(l)
          {
            wetstand->soil.pool[l].fast.carbon += wetstand->soil.pool[l].slow.carbon;
            wetstand->soil.pool[l].slow.carbon = 0.;

          } /* of forrootsoillayer() */
            //        copy PFT list to wetland stand
          foreachpft(pft, p, &natstand->pftlist)
          {
            wetpft = addpft(wetstand,pft->par,year,365,config);
            mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac);
          }
          natstand->frac = frac - delta_wetland;
        }
        else
        {
          //        currently there is wetland stand
          wetstand = getstand(cell->standlist, wetlandstandnum);
          //printf("XXX update_wetland.c wetland expands delta_wetland=%g.\n",delta_wetland);
          frac = natstand->frac;
          //        modify soil C pools -> acrotelm C density mixture of wetland and non-wetland SOM
          natstand->frac = delta_wetland; // make mixsoil and mix_veg_stock work correctly
          mixsoil(wetstand, natstand);
          pos = 0;
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
              mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac);
            }
            else
            {
              wetpft = addpft(wetstand,pft->par,year,365,config);
              mix_veg_stock(wetpft, pft, wetstand->frac, natstand->frac);
            }
          }
          for (p = 0; p<ntotpft; p++)
            present[p] = FALSE;
          foreachpft(pft, p, &natstand->pftlist)
            present[pft->par->id] = TRUE;
          foreachpft(pft, p, &wetstand->pftlist)
            if(!present[pft->par->id])
              mix_veg(pft,wetstand->frac/(wetstand->frac+natstand->frac));
          wetstand->frac = wetlandarea_new;
          natstand->frac = frac - delta_wetland;

          //        make sure there is no C in slow pool
          forrootsoillayer(l)
          {
            wetstand->soil.pool[l].fast.carbon += wetstand->soil.pool[l].slow.carbon;
            wetstand->soil.pool[l].slow.carbon = 0.;

          } /* of forrootsoillayer */
        }

#ifdef CHECK_BALANCE
        foreachstand(stand, s, cell->standlist)
        {
          st=standstocks(stand);
          end.carbon+=(st.carbon+soilmethane(&stand->soil))*stand->frac;
          end.nitrogen+=st.nitrogen*stand->frac;
        }
        if (fabs(start.carbon - end.carbon)>0.001)
          fprintf(stdout, "C-ERROR in update wetland 1: %g start:%g  ende:%g \n", start.carbon - end.carbon, start.carbon, end.carbon);
        if (fabs(start.nitrogen - end.nitrogen)>0.001)
          fprintf(stdout, "N-ERROR in update wetland 1: %g start:%g  ende:%g \n", start.nitrogen - end.nitrogen, start.nitrogen, end.nitrogen);
#endif

      }
      // -----------------------------------------------------------------------------------------------

      else if (delta_wetland < 0.)
      {
        if (wetlandstandnum == NOT_FOUND)
        {
          //printf("XXX update_wetland.c wants to shrink non-existent wetland.\n");
        }
        else
        {
          wetstand = getstand(cell->standlist, wetlandstandnum);
          //printf("XXX update_wetland.c wants to shrink .\n");

          if (-delta_wetland > wetstand->frac)
            delta_wetland = -wetstand->frac;

          //        mix wetland soil carbon into non-wetland
          frac = wetstand->frac;
          wetstand->frac = -delta_wetland;  // trick to make mixsoil do it
          mixsoil(natstand, wetstand);
          pos = 0;
          foreachpft(pft, p, &natstand->pftlist)
          {
            present[pft->par->id] = TRUE;
            position[pft->par->id] = pos;
            pos++;
          }

          foreachpft(wetpft, p, &wetstand->pftlist)
          {
            if (present[wetpft->par->id])
            {
              pft = getpft(&natstand->pftlist, position[wetpft->par->id]);   //STODO
              mix_veg_stock(pft, wetpft, natstand->frac, wetstand->frac);
            }
            else
            {
              if (wetpft->par->peatland)
                litter_update(&natstand->soil.litter, wetpft, -delta_wetland/(natstand->frac+wetstand->frac),config);
              else
              {
                pft = addpft(natstand,wetpft->par,year,365,config);
                mix_veg_stock(pft, wetpft, natstand->frac, wetstand->frac);
              }
            }
          }
          for (p = 0; p<ntotpft; p++)
            present[p] = FALSE;
          foreachpft(pft, p, &wetstand->pftlist)
            present[pft->par->id] = TRUE;
          foreachpft(pft, p, &natstand->pftlist)
            if(!present[pft->par->id])
              mix_veg(pft,natstand->frac/(natstand->frac+wetstand->frac));

          //        shrink wetland stand
          wetstand->frac = wetlandarea_new;
          natstand->frac -= delta_wetland;

          //        no wetland left?
          if (wetlandarea_new <= 0.)
          {
            //          remove stand
            printf("DELETE WETLAND STAND\n");
            delstand(cell->standlist, wetlandstandnum);
            natstand->frac = 1.;
            wetlandstandnum = NOT_FOUND;
          }
        }
      }  // shrink wetland end
      s = findlandusetype(cell->standlist, NATURAL);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
      natstand = getstand(cell->standlist, s);
      s = findlandusetype(cell->standlist, WETLAND);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
      if (s != NOT_FOUND)
      {
        wetstand = getstand(cell->standlist, s);
        iswetland = TRUE;
      }
      slope = cell->slope_min;
      if (iswetland && wetstand->frac>0.001 && cell->slope>epsilon && (fabs(cell->slope_min - cell->slope_max)>epsilon))
      {
        lambda = 1 / cell->slope;
        slope_max = log(1 - wetstand->frac - epsilon) / (-1 / lambda);
        slope_max /= -(exp(-lambda*cell->slope_max) - exp(-lambda*cell->slope_min));
        if (slope_max<cell->slope_min)
        {
          slope_max = cell->slope_min;
          slope = cell->slope_min;
        }
        else
        {
          slope = (exp(lambda*-slope_max)*(-1 / lambda - slope_max)) - (exp(lambda*-cell->slope_min)*(-1 / lambda - cell->slope_min));  //calculation of the integral of the PDF to get  mean slope
          slope /= (-(exp(-lambda*slope_max) - (exp(-lambda*cell->slope_min))));                                                 //normalising the mean slope for a specific range
        }
        wetstand->Hag_Beta = max(0, (0.05*log(slope + 0.1) + 0.12) / 0.43);
        wetstand->slope_mean = slope;

        slope = exp(lambda*-cell->slope_max)*(-1 / lambda - cell->slope_max) - exp(lambda*-slope_max)*(-1 / lambda - slope_max);  //calculation of the integral of the PDF to get  mean slope
        slope /= -(exp(-lambda*cell->slope_max) - exp(-lambda*slope_max));                                                 //normalising the mean slope for a specific range
        natstand->Hag_Beta = min(1, (0.06*log(slope + 0.1) + 0.22) / 0.43);
        natstand->slope_mean = slope;
      }
      //else
       // natstand->Hag_Beta = min(1, (0.06*log(cell->slope + 0.1) + 0.22) / 0.43);

    }
  } /* of' if cell->hydrotopes.skip_cell*/

    /* totc2=methane2=0;

    foreachstand(stand,s,cell->standlist){
    totc2+=standstocks(stand).carbon*stand->frac;
    //totc2+=soilcarbon(&stand->soil);
    foreachpft(pft,p,&stand->pftlist){
    const Pfttree *tree;
    tree=pft->data;
    if(!istree(pft)) totc2+=vegc_sum(pft)*stand->frac;
    // if(!istree(pft)) printf("turn_litt.leaf %g %g root  %g\n",tree->turn.leaf,tree->turn_litt.leaf,tree->turn_litt.root);
    }
    methane2+=standmethane(stand)*stand->frac;
    }
    printf("balanceC in update wetland: %g methane: %g toc: %g\n", totc_all-totc2-methane2, methane-methane2,totc-totc2);
    */
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
        //fprintf(stdout," cpool.slow=%.5f  cpool.fast=%.5f\n",stand->soil.cpool[l].slow, stand->soil.cpool[l].fast);
      }
      for (p = 0; p<stand->soil.litter.n; p++)
      {
        cshift.slow=cshift.fast=0;
        kmean_pft.fast = kmean_pft.slow = 0.;
        forrootsoillayer(l)
        {
          if (stand->type->landusetype == WETLAND)
            socfraction = pow(10, stand->soil.litter.item[p].pft->soc_k*0.9*logmidlayer[l])
            - (l>0 ? pow(10, stand->soil.litter.item[p].pft->soc_k*0.9*logmidlayer[l - 1]) : 0);
          else
            socfraction = pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l])
            - (l>0 ? pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l - 1]) : 0);

          kmean_pft.fast += socfraction*stand->soil.k_mean[l].fast / (stand->soil.count / NDAYYEAR);
          kmean_pft.slow += socfraction*stand->soil.k_mean[l].slow / (stand->soil.count / NDAYYEAR);

        }

        forrootsoillayer(l)
        {
          if (stand->type->landusetype == WETLAND)
            socfraction = pow(10, stand->soil.litter.item[p].pft->soc_k*0.9*logmidlayer[l])
            - (l>0 ? pow(10, stand->soil.litter.item[p].pft->soc_k*0.9*logmidlayer[l - 1]) : 0);
          else
            socfraction = pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l])
            - (l>0 ? pow(10, stand->soil.litter.item[p].pft->soc_k*logmidlayer[l - 1]) : 0);


          if (stand->soil.decomp_litter_mean.carbon>100)
          {
            if (ctotal.fast.carbon>5000)   stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].fast = (socfraction*stand->soil.k_mean[l].fast / (stand->soil.count / NDAYYEAR)) / kmean_pft.fast;
            //stand->soil.c_shift_fast[l][stand->soil.litter.ag[p].pft->id]*=socfraction*ctotal.fast/stand->soil.cpool[l].fast;
            if (ctotal.slow.carbon>1000)   stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].slow = (socfraction*stand->soil.k_mean[l].slow / (stand->soil.count / NDAYYEAR)) / kmean_pft.slow;
            // stand->soil.c_shift_slow[l][stand->soil.litter.ag[p].pft->id]*=socfraction*ctotal.slow/stand->soil.cpool[l].slow;
          }
          cshift.fast += stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].fast;
          cshift.slow += stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].slow;
        }
        forrootsoillayer(l)
        {
          stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].slow /= cshift.slow;
          stand->soil.c_shift[l][stand->soil.litter.item[p].pft->id].fast /= cshift.fast;
          //fprintf(stdout,"c_shift_slow = %.5f cshift.slow = %.5f %s k.mean_slow= %.5f decom_mean= %.5f \n",stand->soil.c_shift_slow[l][stand->soil.litter.ag[p].pft->id],cshift.slow,stand->soil.litter.ag[p].pft->name,stand->soil.k_mean[l].slow/(stand->soil.count/NDAYYEAR),stand->soil.decomp_litter_mean/(stand->soil.count/NDAYYEAR));
        }
      } /* of for(p=0;p<stand->soil.litter.n;p++) */
      forrootsoillayer(l)
        stand->soil.k_mean[l].slow = stand->soil.k_mean[l].fast = 0.0;
      stand->soil.count = 0;
      stand->soil.decomp_litter_mean.carbon = 0.0;
    }
  }
#endif

#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen=0;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+soilmethane(&stand->soil))*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
  }
  if (fabs(start.carbon - end.carbon)>0.001)
    fprintf(stdout, "C-ERROR in update wetland 2: %g start:%g  ende:%g \n", start.carbon - end.carbon, start.carbon, end.carbon);
  if (fabs(start.nitrogen - end.nitrogen)>0.001)
    fprintf(stdout, "N-ERROR in update wetland 2: %g start:%g  ende:%g \n", start.nitrogen - end.nitrogen, start.nitrogen, end.nitrogen);
#endif
  free(present);
  free(position);
  //  printf("update_wetland.c end.\n");
} /* of 'update_wetland' */
