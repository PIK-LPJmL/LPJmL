/***************************************************************************/
/**                                                                       **/
/**          o u t p u t _ g b w _ w o o d p l a n t a t i o n . c        **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 21.06.2010                                           **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"
#include "agriculture.h"
#include "woodplantation.h"

void output_gbw_woodplantation(Output *output, /**< output data */
                          const Stand *stand,  /**< stand pointer */
                          Real frac_g_evap,
                          Real evap,           /**< evaporation (mm) */
                          Real evap_blue,      /**< evaporation of irrigation water (mm) */
                          Real return_flow_b,  /**< irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
                          const Real aet_stand[LASTLAYER],
                          const Real green_transp[LASTLAYER],
                          Real intercep_stand,  /**< stand interception (mm) */
                          Real intercep_stand_blue,  /**< stand interception from irrigation(mm) */
                          int ncft,            /**< number of CROPS */
                          Bool pft_output_scaled
                         )
{
  int l;
  Real total_g,total_b;
  Irrigation *data;
  data=stand->data;
  total_g=total_b=0;

  total_g+=intercep_stand-intercep_stand_blue;
  total_b+=intercep_stand_blue;

  total_g+=evap*frac_g_evap;
  total_b+=evap_blue;
  forrootsoillayer(l)
  {
    total_g+=green_transp[l];
    total_b+=aet_stand[l]-green_transp[l];
  }
#ifdef IMAGE
  if(pft_output_scaled)
  {
    output->cft_consump_water_g[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=total_g*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_consump_water_b[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=total_b*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    forrootsoillayer(l)
    {
      output->cft_transp[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=aet_stand[l]*stand->cell->ml.landfrac[data->irrigation].woodplantation;
      output->cft_transp_b[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=(aet_stand[l]-green_transp[l])*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    }

    output->cft_evap[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=evap*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_evap_b[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=evap_blue*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_interc[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=intercep_stand*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_interc_b[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=intercep_stand_blue*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_return_flow_b[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=return_flow_b*stand->cell->ml.landfrac[data->irrigation].woodplantation;
  }
  else
  {
    output->cft_consump_water_g[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=total_g;
    output->cft_consump_water_b[rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)]+=total_b;
    forrootsoillayer(l)
    {
      output->cft_transp[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += aet_stand[l];
      output->cft_transp_b[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += aet_stand[l];
    }

    output->cft_evap[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += evap;
    output->cft_evap_b[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += evap_blue;
    output->cft_interc[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += intercep_stand;
    output->cft_interc_b[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += intercep_stand_blue;
    output->cft_return_flow_b[rwp(ncft) + data->irrigation*(ncft + NGRASS + NBIOMASSTYPE + NWPTYPE)] += return_flow_b;


    if (data->irrigation)
    {
      output->mgcons_irr += total_g*stand->cell->ml.landfrac[1].woodplantation;
      output->mbcons_irr += total_b*stand->cell->ml.landfrac[1].woodplantation;
    }
    else
    {
      output->mgcons_rf += total_g*stand->cell->ml.landfrac[0].woodplantation;
      output->mgcons_rf += total_b*stand->cell->ml.landfrac[0].woodplantation;
    }
  }
#endif
} /* of 'output_gbw_woodplantation' */

