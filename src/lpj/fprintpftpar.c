/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  p  f  t  p  a  r  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints PFT parameter                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintpftpar(FILE *file,          /**< pointer to text file */
                  const Pftpar *pftpar /**< pointer to PFT parameter */
                 )
{
  int i;
  char *phenology[]={"evergreen","raingreen","summergreen","any","cropgreen"};
  char *cultivation_type[]={"none","biomass","annual crop"};
  char *path[]={"no pathway","C3","C4"};
  fputs("------------------------------------------------------------------------------\n",file);
  fprintf(file,"Id:\t\t%d\n"
               "Name:\t\t%s\n"
               "Cult. type:\t%s\n",pftpar->id,pftpar->name,cultivation_type[pftpar->cultivation_type]);
  fprintf(file,"rootdist:\t");
  for(i=0;i<LASTLAYER;i++)
    fprintf(file,"%g ",pftpar->rootdist[i]);
  fprintf(file,"\nCN:\t\t");
  for(i=0;i<NHSG;i++)
    fprintf(file,"%g ",pftpar->cn[i]);
  fprintf(file,"\n"
               "beta root:\t%g\n"
               "minwscal:\t%g\n"
               "gmin:\t\t%g (mm/s)\n"
               "respcoeff:\t%g\n"
               "nmax:\t\t%g (mg/g)\n"
               "resist:\t\t%g\n"
               "longevity:\t%g (yr)\n"
               "SLA:\t\t%g (m2/gC)\n"
               "lmro ratio:\t%g\n"
               "ramp:\t\t%g\n"
               "LAI sapl:\t%g\n"
               "gdd5min:\t%g\n"
               "twmax:\t\t%g (deg C)\n"
               "twmax_daily:\t%g (deg C)\n"
               "gddbase:\t%g (deg C)\n"
               "min temprange:\t%g\n"
               "emax:\t\t%g (mm/d)\n"
               "intc:\t\t%g\n"
               "alphaa:\t\t%g\n"
               "albedo_leaf:\t%g\n"
               "albedo_stem:\t%g\n"
               "albedo_litter:\t%g\n"
               "snowcanopyfrac:\t%g\n"
               "lightextcoeff:\t%g\n"
               "tmin_sl:\t%g\n"
               "tmin_base:\t%g\n"
               "tmin_tau:\t%g\n"
               "tmax_sl:\t%g\n"
               "tmax_base:\t%g\n"
               "tmax_tau:\t%g\n"
               "light_sl:\t%g\n"
               "light_base:\t%g\n"
               "light_tau:\t%g\n"
               "wscal_sl:\t%g\n"
               "wscal_base:\t%g\n"
               "wscal_tau:\t%g\n"
               "mort_max:\t%g (1/yr)\n"
               "phenology:\t%s\n"
               "path:\t\t%s\n"
               "temp CO2:\t%g %g (deg C)\n"
               "temp photos:\t%g %g (deg C)\n"
               "temp:\t\t%g %g (deg C)\n"
               "min aprec:\t%g (mm)\n"
               "flam:\t\t%g\n"
               "k_litter10:\t%g %g (1/yr)\n"
               "k_litter10_q10_wood:\t%g\n"
               "soc_k:\t\t%g\n"
               "alpha_fuelp:\t%g\n"
               "vpd_par:\t\t%g\n"
               "fuel bulk dens.:\t%g (kg/m3)\n"
               "emis. factor:\t%g %g %g %g %g %g\n"
               "wind damp.:\t%g\n"
               "roughness length:\t%g\n",
          pftpar->beta_root,
          pftpar->minwscal,pftpar->gmin,pftpar->respcoeff,pftpar->nmax,
          pftpar->resist,
          pftpar->longevity,pftpar->sla,pftpar->lmro_ratio,1.0/pftpar->ramp,
          pftpar->lai_sapl,pftpar->gdd5min,
          pftpar->twmax,pftpar->twmax_daily,pftpar->gddbase,pftpar->min_temprange,
          pftpar->emax,pftpar->intc,
          pftpar->alphaa, pftpar->albedo_leaf, pftpar->albedo_stem, pftpar->albedo_litter, pftpar->snowcanopyfrac, pftpar->lightextcoeff, 
          pftpar->tmin.sl, pftpar->tmin.base, pftpar->tmin.tau, pftpar->tmax.sl, pftpar->tmax.base, pftpar->tmax.tau, pftpar->light.sl, pftpar->light.base, pftpar->light.tau, pftpar->wscal.sl, pftpar->wscal.base, pftpar->wscal.tau,
          pftpar->mort_max,phenology[pftpar->phenology],path[pftpar->path],
          pftpar->temp_co2.low,pftpar->temp_co2.high,pftpar->temp_photos.low,
          pftpar->temp_photos.high,pftpar->temp.low,pftpar->temp.high,
          pftpar->aprec_min,
          pftpar->flam,pftpar->k_litter10.leaf*NDAYYEAR,
          pftpar->k_litter10.wood*NDAYYEAR,pftpar->k_litter10.q10_wood,
          pftpar->soc_k,pftpar->alpha_fuelp,pftpar->vpd_par,
          pftpar->fuelbulkdensity,pftpar->emissionfactor.co2,
          pftpar->emissionfactor.co,pftpar->emissionfactor.ch4,
          pftpar->emissionfactor.voc,pftpar->emissionfactor.tpm,
          pftpar->emissionfactor.nox,pftpar->windspeed,pftpar->roughness);
  pftpar->fprintpar(file,pftpar); /* call type-specific print function */
} /* of 'fprintpftpar' */
