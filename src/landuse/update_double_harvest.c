/**************************************************************************************/
/**                                                                                \n**/
/**       u p d a t e _ d o u b l e _ h a r v e s t . c                            \n**/
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
#include "crop.h"
#include "agriculture.h"

void update_double_harvest(Output *output,      /**< pointer to output */
                           Pft *pft,            /**< pointer to crop PFT */
                           Bool irrigation,     /**< stand irrigated? (TRUE,FALSE) */
                           int day,             /**< day of year */
                           int npft,            /**< number of natural PFTs */
                           int ncft,            /**< number of crop PFTs */
                           const Config *config /**< LPJmL configuration */
                          )
{
  Pftcrop *crop;
  int nnat,nirrig,index;
  crop=pft->data;
  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);
  index=(pft->stand->type->landusetype==OTHERS) ? rothers(ncft) : pft->par->id-npft;
  if(crop->dh!=NULL)
  {
    if(output->syear[pft->par->id-npft+irrigation*ncft]>epsilon)
    {
      output->syear2[pft->par->id-npft+irrigation*ncft]=crop->dh->sowing_year;
      getoutputindex(output,SYEAR2,pft->par->id-npft+irrigation*ncft,config)=crop->dh->sowing_year;
    }
    else
    {
      output->syear[pft->par->id-npft+irrigation*ncft]=crop->dh->sowing_year;
      getoutputindex(output,SYEAR,pft->par->id-npft+irrigation*ncft,config)=crop->dh->sowing_year;
    }
    if(output->syear2[pft->par->id-npft+irrigation*ncft]>epsilon)
      getoutputindex(output,HDATE2,pft->par->id-npft+irrigation*ncft,config)=day;
    else
      getoutputindex(output,HDATE,pft->par->id-npft+irrigation*ncft,config)=day;
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_ABOVEGBMC,index+irrigation*(ncft+NGRASS),config),
                   &getoutputindex(output,CFT_ABOVEGBMC2,index+irrigation*(ncft+NGRASS),config),
                   (crop->ind.leaf.carbon+crop->ind.pool.carbon+crop->ind.so.carbon)*pft->nind);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_ABOVEGBMN,index+irrigation*(ncft+NGRASS),config),
                   &getoutputindex(output,CFT_ABOVEGBMN2,index+irrigation*(ncft+NGRASS),config),
                   (crop->ind.leaf.nitrogen+crop->ind.pool.nitrogen+crop->ind.so.nitrogen)*pft->nind);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_PET,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_PET2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->dh->petsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_NIR,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_NIR2,(index+irrigation*nirrig),config),
                   crop->dh->nirsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_TRANSP,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_TRANSP2,(index+irrigation*nirrig),config),
                   crop->dh->transpsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_EVAP,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_EVAP2,(index+irrigation*nirrig),config),
                   crop->dh->evapsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_INTERC,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_INTERC2,(index+irrigation*nirrig),config),
                   crop->dh->intercsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_PREC,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_PREC2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->dh->precsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,GROWING_PERIOD,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,GROWING_PERIOD2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->dh->lgp);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_SRAD,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_SRAD2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->dh->sradsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_AIRRIG,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_AIRRIG2,(index+irrigation*nirrig),config),
                   crop->dh->irrig_apply);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_TEMP,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_TEMP2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->dh->tempsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,HUSUM,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,HUSUM2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->husum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_RUNOFF,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_RUNOFF2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->dh->runoffsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_N2O_DENIT,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_N2O_DENIT2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->dh->n2o_denitsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_N2O_NIT,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_N2O_NIT2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->dh->n2o_nitsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_N2_EMIS,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_N2_EMIS2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->dh->n2_emissum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_LEACHING,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_LEACHING2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->dh->leachingsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_NFERT,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_NFERT2,(index+irrigation*nirrig),config),
                   crop->dh->nfertsum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_C_EMIS,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_C_EMIS2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->dh->c_emissum);
    double_harvest(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,PFT_NUPTAKE,nnat+index+irrigation*nirrig,config),
                   &getoutputindex(output,PFT_NUPTAKE2,nnat+index+irrigation*nirrig,config),
                   crop->nuptakesum);
  }
  else
  {
    getoutputindex(output,CFT_ABOVEGBMC,index+irrigation*(ncft+NGRASS),config)=
      (crop->ind.leaf.carbon+crop->ind.pool.carbon+crop->ind.so.carbon)*pft->nind;
    getoutputindex(output,CFT_ABOVEGBMN,index+irrigation*(ncft+NGRASS),config)=
      (crop->ind.leaf.nitrogen+crop->ind.pool.nitrogen+crop->ind.so.nitrogen)*pft->nind;
    if(pft->stand->type->landusetype==AGRICULTURE)
    {
      getoutputindex(output,HDATE,pft->par->id-npft+irrigation*ncft,config)=day;
      getoutputindex(output,HUSUM,pft->par->id-npft+irrigation*ncft,config)=crop->husum;
    }
  }
} /* of 'update_double_harvest' */
