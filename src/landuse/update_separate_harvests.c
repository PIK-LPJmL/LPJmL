/**************************************************************************************/
/**                                                                                \n**/
/**       u p d a t e _ s e p a r a t e _ h a r v e s t s . c                      \n**/
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

void update_separate_harvests(Output *output,      /**< pointer to output */
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
  index=(getlandusetype(pft->stand)==OTHERS) ? rothers(ncft) : pft->par->id-npft;
  if(crop->sh!=NULL)
  {
    if(output->syear[pft->par->id-npft+irrigation*ncft]>epsilon)
    {
      output->syear2[pft->par->id-npft+irrigation*ncft]=crop->sh->sowing_year;
      getoutputindex(output,SYEAR2,pft->par->id-npft+irrigation*ncft,config)=crop->sh->sowing_year;
    }
    else
    {
      output->syear[pft->par->id-npft+irrigation*ncft]=crop->sh->sowing_year;
      getoutputindex(output,SYEAR,pft->par->id-npft+irrigation*ncft,config)=crop->sh->sowing_year;
    }
    if(output->syear2[pft->par->id-npft+irrigation*ncft]>epsilon)
      getoutputindex(output,HDATE2,pft->par->id-npft+irrigation*ncft,config)=day;
    else
      getoutputindex(output,HDATE,pft->par->id-npft+irrigation*ncft,config)=day;
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_ABOVEGBMC,index+irrigation*(ncft+NGRASS),config),
                   &getoutputindex(output,CFT_ABOVEGBMC2,index+irrigation*(ncft+NGRASS),config),
                   (crop->ind.leaf.carbon+crop->ind.pool.carbon+crop->ind.so.carbon)*pft->nind);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_ABOVEGBMN,index+irrigation*(ncft+NGRASS),config),
                   &getoutputindex(output,CFT_ABOVEGBMN2,index+irrigation*(ncft+NGRASS),config),
                   (crop->ind.leaf.nitrogen+crop->ind.pool.nitrogen+crop->ind.so.nitrogen)*pft->nind);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_PET,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_PET2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->sh->petsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_NIR,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_NIR2,(index+irrigation*nirrig),config),
                   crop->sh->nirsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_TRANSP,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_TRANSP2,(index+irrigation*nirrig),config),
                   crop->sh->transpsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_EVAP,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_EVAP2,(index+irrigation*nirrig),config),
                   crop->sh->evapsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_INTERC,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_INTERC2,(index+irrigation*nirrig),config),
                   crop->sh->intercsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_PREC,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_PREC2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->sh->precsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,GROWING_PERIOD,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,GROWING_PERIOD2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->sh->lgp);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_SRAD,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_SRAD2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->sh->sradsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_AIRRIG,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_AIRRIG2,(index+irrigation*nirrig),config),
                   crop->sh->irrig_apply);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_TEMP,(index+irrigation*(ncft+NGRASS)),config),
                   &getoutputindex(output,CFT_TEMP2,(index+irrigation*(ncft+NGRASS)),config),
                   crop->sh->tempsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,HUSUM,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,HUSUM2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->husum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_RUNOFF,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_RUNOFF2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->sh->runoffsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_N2O_DENIT,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_N2O_DENIT2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->sh->n2o_denitsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_N2O_NIT,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_N2O_NIT2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->sh->n2o_nitsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_N2_EMIS,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_N2_EMIS2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->sh->n2_emissum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_LEACHING,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_LEACHING2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->sh->leachingsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_NFERT,(index+irrigation*nirrig),config),
                   &getoutputindex(output,CFT_NFERT2,(index+irrigation*nirrig),config),
                   crop->sh->nfertsum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
                   &getoutputindex(output,CFT_C_EMIS,(pft->par->id-npft+irrigation*ncft),config),
                   &getoutputindex(output,CFT_C_EMIS2,(pft->par->id-npft+irrigation*ncft),config),
                   crop->sh->c_emissum);
    separate_harvests(output->syear2[pft->par->id-npft+irrigation*ncft],
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
    if(getlandusetype(pft->stand)==AGRICULTURE)
    {
      getoutputindex(output,HDATE,pft->par->id-npft+irrigation*ncft,config)=day;
      getoutputindex(output,HUSUM,pft->par->id-npft+irrigation*ncft,config)=crop->husum;
    }
  }
} /* of 'update_separate_harvests' */
