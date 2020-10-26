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
               "pre. CO2:\t%g (ppm)\n"
               "k:\t\t%g (1/yr)\n"
               "theta:\t\t%g\n"
               "k_beer:\t\t%g\n"
               "alphac3:\t%g\n"
               "alphac4:\t%g\n"
               "bc3:\t\t%g\n"
               "bc4:\t\t%g\n"
               "r_growth:\t%g\n"
               "GM:\t\t%g\n"
               "ALPHAM:\t\t%g\n"
               "ko25:\t\t%g (Pa)\n"
               "kc25:\t\t%g (Pa)\n"
               "atmfrac:\t%g\n"
               "fastfrac:\t%g\n"
               "K_MORT:\t\t%g\n"
               "fpc_tree_max:\t%g\n"
               "temp_response_a:\t%g\n"
               "temp_response_b:\t%g\n",
          param.k_litter10*NDAYYEAR,param.k_soil10.fast*NDAYYEAR,
          param.k_soil10.slow*NDAYYEAR,param.maxsnowpack,param.soildepth_evap,
          param.soil_infil,param.co2_p,
          param.k,param.theta,param.k_beer,param.alphac3,param.alphac4,
          param.bc3,param.bc4,param.r_growth,param.GM,param.ALPHAM,
          param.ko25,param.kc25,param.atmfrac,param.fastfrac,param.k_mort,
          param.fpc_tree_max,param.temp_response_a,param.temp_response_b);
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
            "par_sink_limit:\t%g\n",
            param.k_max,param.k_2,param.p,param.n0,param.k_temp,
            param.min_c_bnf,param.q_ash,param.sapwood_recovery,param.T_m,
            param.T_0,param.T_r,param.par_sink_limit);
  if(config->withlanduse!=NO_LANDUSE)
  {
    fprintf(file,"livestock density on grassland:\t%g\n",
            param.lsuha);
    fprintf(file,"annual prec lim:\t%g (mm)\n",param.aprec_lim);
    fprintf(file,"irrig. threshold C3 dry:\t%g\n",param.irrig_threshold_c3_dry);
    fprintf(file,"irrig. threshold C3 humid:\t%g\n",
            param.irrig_threshold_c3_humid);
    fprintf(file,"irrig. threshold C4:\t%g\n",param.irrig_threshold_c4);
    fprintf(file,"irrig. threshold Rice:\t%g\n",param.irrig_threshold_rice);
    fprintf(file,"irrigation soilfrac:\t%g\n",param.irrigation_soilfrac);
    fprintf(file,"canal conveyance eff. sand:\t%g\n",param.ec_canal[0]);
    fprintf(file,"canal conveyance eff. loam:\t%g\n",param.ec_canal[1]);
    fprintf(file,"canal conveyance eff. clay:\t%g\n",param.ec_canal[2]);
    fprintf(file,"saturation level surface:\t%g\n",param.sat_level[1]);
    fprintf(file,"saturation level sprink:\t%g\n",param.sat_level[2]);
    fprintf(file,"saturation level drip:\t%g\n",param.sat_level[3]);
    fprintf(file,"drip evap. reduction:\t%g\n",param.drip_evap);
    fprintf(file,"residues in soil:\t%g\n",param.residues_in_soil);
    if(config->with_nitrogen)
      fprintf(file,"nfert split:\t%g\n",param.nfert_split);
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
    fprintf(file,"allocation threshold:\t%g\n",param.allocation_threshold);
    fprintf(file,"hfrac2:\t\t%g (gC/m2)\n",param.hfrac2);
    fprintf(file,"rootreduction:\t%g\n",param.rootreduction);
  }
  fputs("Soil parameter\n",file);
  fprintsoilpar(file,config->soilpar,config->nsoil,config->with_nitrogen);
  fputs("PFT parameter\n",file);
  if(config->withlanduse!=NO_LANDUSE)
  {
    for(p=0;p<npft+ncft;p++)
      fprintpftpar(file,config->pftpar+p,config);
    fputs("------------------------------------------------------------------------------\n"
          "Country parameter\n",file);
    fprintcountrypar(file,config->countrypar,config->ncountries,ncft);
    fputs("Region parameter\n",file);
    fprintregionpar(file,config->regionpar,config->nregions);
  }
  else
    for(p=0;p<npft-config->nbiomass;p++)
      fprintpftpar(file,config->pftpar+p,config);
  fputs("------------------------------------------------------------------------------\n",file);
  fprintoutputvar(file,config->outnames,NOUT);
} /* of 'fprintparam' */
