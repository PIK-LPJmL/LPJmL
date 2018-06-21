/**************************************************************************************/
/**                                                                                \n**/
/**       c  a  l  c  _   c  r  o  p  d  a  t  e  s  .  c                          \n**/
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

#define KEYDAY_TEMP_NHEMISPHERE 181 /* last day of summer month (Jun) when
                                        we test what has happend in one year */

static void update_date(int *date20,int date,int *last_update)
{
  if(*date20==0 || *last_update==-9)
    *date20=date;
  else
    *date20=(int)(((Real)*date20-(Real)*date20/20.+(Real)date/20.)+0.5);
  *last_update=0;
} /* of 'update_date' */

void calc_cropdates(const Pftpar par[],     /**< PFT parameter array for crops */
                    const Climbuf *climbuf, /**< Climate buffer */
                    Cropdates *cropdates,   /**< crop dates */
                    Real lat,               /**< Latitude (deg) */
                    int day,                /**< day (1..365) */
                    int ncft                /**< number of crop PFTs */
                   )
{
  int cft,adjust_date;
  const Pftcroppar *croppar;

  for(cft=0;cft<ncft;cft++)
  {

    croppar=par[cft].data;

    if(climbuf->temp[NDAYS-1]>croppar->temp_vern &&
       climbuf->temp[NDAYS-2]<=croppar->temp_vern)
    {
      adjust_date=(lat>0.0 && day<KEYDAY_TEMP_NHEMISPHERE) ? day+365 : day;
      update_date(&cropdates[cft].vern_date20,adjust_date,
                  &cropdates[cft].last_update_vern);
    }
  }
} /* of 'calc_cropdates' */

/*
- local function calc_cropdates() is called in update_daily()
- loop over all cfts except managed grass
  -> updating of fallow, the 3 cropdates and the related counters
- the fallow period is reduced by one day for each cft without/with irrigation
- on the COLDEST_DAY_N/SHEMISPHERE the variable fallow is set to 0

- different calculations of cropdates for single crop groups:
  - NO_CALC_SDATE:
    -> for crops: pulses, temperate roots, soybean
    -> sowing date is equal to initialised values
    -> fall_sdate20 is abused to save the constant sowing date
    -> temp_spring20/temp_vern20 are not used in this group & set to zero in
       init_cropdates.c
       -> the parameters temp_fall/temp_spring/temp_vern are set to 1000 in
      pft.par so that the temperature could never decrease/increase
      under/over these thresholds
    -> see also pft.par,init_cropdates.c

  - TEMP_WTYP_CALC_SDATE:
    -> for crops: temperate cereals, rapeseed
    -> sowing date is equal to determined fall_sdate20, if exist & is greater
       than harvest day
       -> in this case the crop is a winter crop (wtyp=TRUE)
       -> fall_sdate20 is determined as the mean of the experience of the last
      20 years when the temperature decreases under a crop specific
      temperature (defined in pft.par as temp_fall)
    -> otherwise the sowing date is equal to spring_sdate20, if exist
       -> in this case the crop is not a winter crop (wtyp=FALSE)
       -> spring_sdate20 is determined as the mean of the experience of the
      last 20 years when the temperature increases over a crop specific
      temperature (defined in pft.par as temp_spring)
    -> otherwise the crop is not sown (temperature always too low or too high)
    -> requires vern_date20 for determination of the vernalization days (vd)
       -> vern_date20 is determined as the mean of the experience of the last
      20 years when the temperature increases over a crop specific
      temperature (defined in pft.par as temp_vern)
    -> see also pft.par,cropdates.h,init_cropdates.c,sowing.c,phen_variety.c

  - TEMP_STYP_CALC_SDATE:
    -> for crop: sunflower
    -> sowing date is equal to spring_sdate20, if exist
       -> spring_sdate20 is determined as the mean of the experience of the
      last 20 years when the temperature increases over a crop specific
      temperature (defined in pft.par as temp_spring)
      or if the mean minimum temperature of the last 20 years is greater
      than the threshold (tropical zone), spring_sdate20 is set each day
      new and the crop is sown if possible (not already sown & no fallow)
    -> otherwise the crop is not sown (temperature always to low)
    -> temp_vern20 is not used in this group & set to zero in init_cropdates.c
       -> the parameters temp_fall/temp_vern are set to 1000 in pft.par so that
      the temperature could never decrease/increase under/over these
      thresholds
    -> see also pft.par,cropdates.h,init_cropdates.c,sowing.c

  - TEMP_PREC_CALC_SDATE:
    -> for crop: maize
    -> spring_sdate20 is determined like for TEMP_STYP_CALC_SDATE
    -> with irrigation sowing date is set to spring_sdate20
    -> without irrigation sowing date is set to fall_sdate20
       -> here fall_sdate20 is abused & determined as the day when
      spring_sdate20 is reached and the soilmoisture is greater than 0.7
    -> otherwise the crop is not sown (temperature, soilmoisture always to low)
    -> temp_vern20 is not used in this group & set to zero in init_cropdates.c
       -> the parameters temp_fall/temp_vern are set to 1000 in pft.par so that
      the temperature could never decrease/increases under/over these
      thresholds
    -> see also pft.par,cropdates.h,initcropdates.c,sowing.c

  - PREC_CALC_SDATE:
    -> for crops: tropical cereals, tropical roots, groundnuts
    -> with irrigation no date is determined, the crop is sown if possible
       (not already sown & no fallow)
    -> without irrgiation sowing date is set to fall_sdate20
       -> here fall_sdate20 is abused & determined as the day if the
      soilmoisture is greater than 0.4 for latitude smaller 45
       -> the crop could be sown, if it is not already sown and no fallow period
    -> otherwise the crop is not sown (soilmoisture always to low)
    -> temp_vern20 and temp_spring20 are not used in this group & set to zero
       in init_cropdates.c
       -> the parameters temp_fall/temp_spring/temp_vern are set to 1000 in
      pft.par so that the temperature could never decrease/increase
      under/over these thresholds
    -> see also pft.par,cropdates.h,initcropdates.c,sowing.c

  - MULTICROP:
    -> for crop: rice
    -> sowing date is equal to initialised values
    -> fall_sdate20 is abused to save the constant sowing date
    -> multicropping is possible in always warm regions
       -> therefor spring_sdate20 is determined when the mean minimum temperature
      of the last 20 years is greater than the threshold temp_spring
      (tropical zone), spring_sdate20 is set each day new and the crop is
      sown if possible (not already sown & no fallow)
    -> multicropping is controlled by the variable fallow, which is set to
       parameter fallow_days (see pft.par) on harvest and to zero on the
       COLDEST_DAY_N/SHEMISPHERE (see calc_cropdates.c)
    -> temp_vern20 is not used in this group & set to zero in init_cropdates.c
       -> the parameters temp_fall/temp_vern are set to 1000 in pft.par so that
      the temperature could never decrease/increase under/over these
      thresholds
    -> see also pft.par,cropdates.h,initcropdates.c,sowing.c


- in function update_date() the 20 year mean is determined and the corresponding
  last_update is set to 0
*/
