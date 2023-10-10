/**************************************************************************************/
/**                                                                                \n**/
/**                 b  a  s  e  .  c                                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates burnt area with a GLM approach                         \n**/
/**     (BASE = Burnt Area Simulator for Europe)                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

//BASE 1.0:

//#define x_0 -0.1042      /** intercept **/
//#define a 0.02910      /** PopDens **/
//#define b -13.94       /** HDI **/
//#define c 0.0009521   /** mean annual GPP **/
//#define d 0.09634      /** FWI **/
//#define e 0.00007252   /** interaction of FWI and mean annual GPP**/
//#define f -0.4155      /** Dominanat PFT Conifer **/
//#define g 0.621       /** Dominant PFT Dry Pine **/
//#define h 1.611        /** Dominant PFT Non-native Broadleaved **/

#define x_0 -1.522 // intercept
#define a 0.5991 // FAPAR12
#define b 0.03439 // Popdens
#define c -16.76 // HDI
#define d 0.07516 //Slope
#define e 0.8561 //TPI
#define f 0.1543 // tree cover grid cell
#define g -0.003474 // quadratic tree cover grid cell
#define h -0.8132 // GPP_index
#define i 0.1501 // FWI	
#define j -0.1742 //Dominanat PFT Conifer
#define k 0.6303 //Dominant PFT Dry Pine
#define l 1.366//Dominant PFT Non-native Broadleaved
#define m -0.0672// GPP_index * FWI


Real base(Input *input,
          Real Popdens,
          Real fwi,
          Real gpp_avg,
          Real fpc_tree,
          Real GPP_index,
          Real FAPAR12,
          int cell) //returns fraction burnt
{
  
  Real HDI, Conifer, DryPine, NonNativeBL, TPI, Slope, y;
  HDI=getrealdata(input->hdi,cell);
  Conifer=getrealdata(input->conifer,cell);
  DryPine=getrealdata(input->drypine,cell);
  NonNativeBL=getrealdata(input->nonnativebl,cell);
  TPI=getrealdata(input->tpi,cell);
  Slope=getrealdata(input->slope,cell);

  //BASE 1.0:
  //y = x_0 + a*sqrt(Popdens) + b*HDI + c*gpp_avg + d*fwi + f*Conifer + g*DryPine + h*NonNativeBL;
  
  y = x_0 + a*FAPAR12 + b*sqrt(Popdens) +
      c*HDI + d*Slope + e*TPI + f*fpc_tree + g*(fpc_tree*fpc_tree) +
      h*GPP_index + i*fwi + j*Conifer + k*DryPine + l*NonNativeBL +
      m*GPP_index*fwi;

  //printf("GPP_index: %g, FAPAR12: %g, fpc_tree: %g\n",GPP_index, FAPAR12, fpc_tree);
  //if((exp(y)/(1+exp(y)))>0.5) 
  //  printf("burnt fraction: %g, popdens: %g, HDI:%f, gpp: %g, fwi: %g, Conifer: %g, DryPine: %g, NonNativeBL: %g\n", exp(y)/(1+exp(y)),Popdens, HDI, gpp_avg, fwi, Conifer, DryPine, NonNativeBL);

  return exp(y)/(1+exp(y)); //reverse log-link function
}
