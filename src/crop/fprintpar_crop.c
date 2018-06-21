/**************************************************************************************/
/**                                                                                \n**/
/**           f  p  r  i  n  t  p  a  r  _  c  r  o  p  .  c                       \n**/
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
#include "crop.h"

void fprintpar_crop(FILE *file, /**< pointer to text file */
                    const Pftpar *par /**< pointer to PFT parameter */
                   )
{
  char *calcmethod[]={"no calc","prec calc","temp wtyp calc","temp styp calc",
                      "temp prec calc","multi crop"}; 
  Pftcroppar *croppar;
  croppar=par->data;
  fprintf(file,"sdate calcmethod:\t %s\n"
               "init date:\t%d %d\n"
               "hlimit:\t\t%d\n"
               "fallow days:\t%d (d)\n"
               "temp fall:\t%g (deg C)\n"
               "temp spring:\t%g (deg C)\n"
               "temp vern:\t%g (deg C)\n"
               "trg:\t\t%g %g\n"
               "pvd:\t\t%g\n"
               "psens:\t\t%g\n"
               "pb:\t\t%g (h)\n"
               "ps:\t\t%g (h)\n"
               "phus:\t\t%g %g\n"
               "phuw:\t\t%g %g\n"
               "phu par:\t%g\n"
               "base temp:\t%g %g (deg C)\n"
               "fphuc:\t\t%g\n"
               "flaimaxc:\t%g\n"
               "fphuk:\t\t%g\n"
               "flaimaxk:\t%g\n"
               "fphusen:\t%g\n"
               "flaimaxharvest:\t%g\n"
               "min, max LAI:\t%g %g\n"
               "opt,min hi:\t%g %g\n"
               "shapesenescencenorm:\t%g\n"
               "C:N ratio:\t%g %g %g\n",
          calcmethod[croppar->calcmethod_sdate],
          croppar->initdate.sdatenh,croppar->initdate.sdatesh,
          croppar->hlimit,croppar->fallow_days,
          croppar->temp_fall,croppar->temp_spring,croppar->temp_vern,
          croppar->trg.low,croppar->trg.high,
          croppar->pvd,
          croppar->psens,
          croppar->pb,
          croppar->ps,
          croppar->phus.low,croppar->phus.high,
          croppar->phuw.low,croppar->phuw.high,
          croppar->phu_par,
          croppar->basetemp.low,croppar->basetemp.high,
          croppar->fphuc,
          croppar->flaimaxc,
          croppar->fphuk,
          croppar->flaimaxk,
          croppar->fphusen,
          croppar->flaimaxharvest,
          croppar->laimin,croppar->laimax,
          croppar->hiopt,croppar->himin,
          croppar->shapesenescencenorm,
          par->respcoeff*param.k/croppar->cn_ratio.root,
          par->respcoeff*param.k/croppar->cn_ratio.so,
          par->respcoeff*param.k/croppar->cn_ratio.pool);
} /* of 'fprintpar_crop' */
