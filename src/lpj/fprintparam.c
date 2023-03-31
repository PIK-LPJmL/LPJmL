/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  p  a  r  a  m  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints LPJ parameter                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintparam(FILE *file,    /**< pointer to text file */
                 int npft,      /**< number of natural PFTs */
                 int ncft,      /**< number of crop PFTs */
                 const Config *config /**< LPJ configuration */
                )
{
  int p;
  fprintf(file,"Parameter settings\n"
               "k_litter10:\t%g (1/yr)\n"
               "k_soil10:\t%g %g (1/yr)\n"
               "max snow pack:\t%g (mm)\n"
               "evap soildepth:\t%g (mm)\n"
               "soil infil:\t%g\n"
               "soil infil litt:\t%g\n"
               "pre. CO2:\t%g (ppm)\n"
               "k:\t\t%g (1/yr)\n"
               "theta:\t\t%g\n"
               "alphac3:\t%g\n"
               "alphac4:\t%g\n"
               "r_growth:\t%g\n"
               "GM:\t\t%g\n"
               "ALPHAM:\t\t%g\n"
               "ko25:\t\t%g (Pa)\n"
               "kc25:\t\t%g (Pa)\n"
               "atmfrac:\t%g\n"
               "fastfrac:\t%g\n"
               "bioturbate:\t%g (1/yr)\n"
               "K_MORT:\t\t%g\n"
               "fpc_tree_max:\t%g\n"
               "temp_response:\t%g\n"
               "percthres:\t%g\n",
          param.k_litter10*NDAYYEAR,param.k_soil10.fast*NDAYYEAR,
          param.k_soil10.slow*NDAYYEAR,param.maxsnowpack,param.soildepth_evap,
          param.soil_infil,param.soil_infil_litter,param.co2_p,
          param.k,param.theta,param.alphac3,param.alphac4,
          param.r_growth,param.GM,param.ALPHAM,
          param.ko25,param.kc25,param.atmfrac,param.fastfrac,1-pow(1-param.bioturbate,NDAYYEAR),param.k_mort,
          param.fpc_tree_max,param.temp_response,param.percthres);
  if(config->equilsoil)
  {
    fprintf(file,"veg_equil_year:\t%d\n",param.veg_equil_year);
    fprintf(file,"veg_equil_unlim:\t%s\n",bool2str(param.veg_equil_unlim));
    fprintf(file,"nequilsoil:\t%d\n",param.nequilsoil);
    fprintf(file,"equisoil_interval:\t%d\n",param.equisoil_interval);
    fprintf(file,"equisoil_years:\t%d\n",param.equisoil_years);
    fprintf(file,"equisoil_fadeout:\t%d\n",param.equisoil_fadeout);
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    fprintf(file,"firedura:\t%g\n"
            "fire_intens:\t%g\n"
            "hum_ign:\t%g\n",
            param.firedura,param.fire_intens,param.hum_ign);
  if(config->with_nitrogen)
    fprintf(file,"k_max:\t\t%g (1/d)\n"
            "k_2:\t\t%g\n"
            "p:\t\t%g\n"
            "N0:\t\t%g (mgN/gC)\n"
            "k_temp:\t\t%g\n"
            "min_c_bnf:\t%g (gC/m2)\n"
            "q_ash:\t\t%g\n"
            "sapwood_rec:\t%g\n"
            "T_m:\t\t%g (deg C)\n"
            "T_0:\t\t%g (deg C)\n"
            "T_r:\t\t%g (deg C)\n"
            "init_soiln:\t%g %g (gN/m2)\n"
            "fertilizer rate:\t%g (gN/m2/yr)\n"
            "par_sink_limit:\t%g\n",
            param.k_max,param.k_2,param.p,param.n0,param.k_temp,
            param.min_c_bnf,param.q_ash,param.sapwood_recovery,param.T_m,
            param.T_0,param.T_r,
            param.init_soiln.fast,param.init_soiln.slow,
            param.fertilizer_rate,param.par_sink_limit);
  if(config->withlanduse!=NO_LANDUSE)
  {
    if(config->with_nitrogen)
    {
      fprintf(file,"nfert_split:\t%g\n",param.nfert_split);
      fprintf(file,"nfert_split_frac:\t%g\n",param.nfert_split_frac);
      fprintf(file,"nfert_no3_frac:\t%g\n",param.nfert_no3_frac);
      fprintf(file,"nmanure_nh4_frac:\t%g\n",param.nmanure_nh4_frac);
      fprintf(file,"manure_cn:\t%g\n", param.manure_cn);
      fprintf(file,"manure_rate:\t%g\n", param.manure_rate);
    }
    fprintf(file,"residues_frac:\t%g\n",param.residue_frac);
    if(config->residue_treatment)
    {
      fprintf(file,"residue rate:\t%g (gC/m2/yr)\n",param.residue_rate);
      fprintf(file,"residue pool:\t%g (gC/m2)\n",param.residue_pool);
      fprintf(file,"residue fbg:\t%g\n",param.residue_fbg);
      if(config->with_nitrogen)
        fprintf(file,"residue C:N:\t%g\n",param.residue_cn);
    }
    fprintf(file,"mixing_efficiency:\t%g\n",param.mixing_efficiency);
    fprintf(file,"livestock density on grassland:\t%g\n",
            param.lsuha);
    fprintf(file,"annual prec lim:\t%g (mm)\n",param.aprec_lim);
    fprintf(file,"irrigation soilfrac:\t%g\n",param.irrigation_soilfrac);
    fprintf(file,"canal conveyance eff. sand:\t%g\n",param.ec_canal[0]);
    fprintf(file,"canal conveyance eff. loam:\t%g\n",param.ec_canal[1]);
    fprintf(file,"canal conveyance eff. clay:\t%g\n",param.ec_canal[2]);
    fprintf(file,"saturation level surface:\t%g\n",param.sat_level[1]);
    fprintf(file,"saturation level sprink:\t%g\n",param.sat_level[2]);
    fprintf(file,"saturation level drip:\t%g\n",param.sat_level[3]);
    fprintf(file,"drip evap. reduction:\t%g\n",param.drip_evap);
    fprintf(file,"residues in soil:\t%g\n",param.residues_in_soil);
    fprintf(file,"residues in soil before till_startyear:\t%g\n",param.residues_in_soil_notillage);
    if(config->with_nitrogen)
    {
      fprintf(file,"nfert split:\t%g\n",param.nfert_split);
      fprintf(file,"nfrac grazing:\t%g\n",param.nfrac_grazing);
      fprintf(file,"nfrac grassharvest:\t%g\n",param.nfrac_grassharvest);
    }
    fprintf(file,"timber fraction wp:\t%g\n",param.ftimber_wp);
    if(config->istimber)
    {
      fprintf(file,"fraction burnt:\t%g\n",param.fburnt);
      fprintf(file,"timber fraction:\t%g\n",param.ftimber);
      fprintf(file,"harvest fast frac:\t%g\n",param.harvest_fast_frac);
      fprintf(file,"product turnover:\t%g %g (1/yr)\n",
              param.product_turnover.fast,param.product_turnover.slow);
    }
    if(config->rw_manage)
    {
      fprintf(file,"esoil reduction:\t%g\n",param.esoil_reduction);
      fprintf(file,"rw buffer max:\t\t%g\n",param.rw_buffer_max);
      fprintf(file,"frac to stored:\t\t%g\n",param.frac_ro_stored);
      fprintf(file,"rw irrig thres:\t\t%g\n",param.rw_irrig_thres);
      fprintf(file,"soil infil rw:\t\t%g\n",param.soil_infil_rw);
      fprintf(file,"yield gap bridge:\t%g\n",param.yield_gap_bridge);
    }
    if(config->with_nitrogen)
      fprintf(file,"allocation threshold:\t%g\n",param.allocation_threshold);
    fprintf(file,"hfrac2:\t\t%g (gC/m2)\n",param.hfrac2);
    fprintf(file,"hfrac biomass:\t%g\n",param.hfrac_biomass);
    fprintf(file,"rootreduction:\t%g\n",param.rootreduction);
    fprintf(file,"Phen limit:\t%g\n",param.phen_limit);
  }
  fputs("Soil parameter\n",file);
  fprintsoilpar(file,config);
  fputs("PFT parameter\n",file);
  if(config->withlanduse!=NO_LANDUSE)
  {
    for(p=0;p<npft+ncft;p++)
      fprintpftpar(file,config->pftpar+p,config);
    fputs("------------------------------------------------------------------------------\n"
          "Country parameter\n",file);
    fprintcountrypar(file,config->countrypar,npft,ncft,config);
    fputs("Region parameter\n",file);
    fprintregionpar(file,config->regionpar,config->nregions);
  }
  else
    for(p=0;p<getnnat(npft,config);p++)
      fprintpftpar(file,config->pftpar+p,config);
  fputs("------------------------------------------------------------------------------\n",file);
  fprintoutputvar(file,config->outnames,NOUT,npft,ncft,config);
} /* of 'fprintparam' */
