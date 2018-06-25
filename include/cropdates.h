/**************************************************************************************/
/**                                                                                \n**/
/**                 c  r  o  p  d  a  t  e  s  .  h                                \n**/
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

#ifndef CROPDATES_H /* Already included? */
#define CROPDATES_H

/* Definition of constants */

#define NODATA -999

/* Declaration of datatypes */

typedef struct
{
  int fall_sdate20;
  /* Definition: Day when temperature drops below a cft-specific threshold in autumn
   * Saved as 20-year running mean
   * Used for temperate cereals, rapeseeds
   * ABUSED for rice, maize, tropical cereals, pulses, temperate roots,
   * tropical roots, soybean, groundnuts to store current year's sowing date
   */
  int last_update_fall;
  /* Counts years since last non-zero value of fallsdate20 */

  int spring_sdate20;
  /* Definition: Day when spring temperature rises above cft-specific threshold in spring
   * Saved as 20-year running mean
   * Used for temperate cereals, rapeseeds, sunflowers, maize, rice (multicropping)
   */
  int last_update_spring;
  /* Counts years since last non-zero value of springsdate20 */

  int vern_date20;
  /* Definition: Day when temperature rises above a cft-specific threshold
   * Saved as 20-year running mean
   * Used for temperate cereals and rapeseeds to determine end of vernalization
   */
  int last_update_vern;
  /* Counts years since last non-zero value of verndate20 */

  int fallow;
  /* counts days to wait before next crop cycle of each cft to control multicropping */
  int fallow_irrig;
  /* counts days to wait before next crop cycle of each cft to control multicropping
   * for irrigated stand
   */

} Cropdates;

/* Declaration of functions */

extern Cropdates *init_cropdates(const Pftpar [],int,Real);
extern void calc_cropdates(const Pftpar *,const Climbuf *,Cropdates *,Real,
                           int,int);
extern void update_fallowdays(Cropdates *,Real,int,int);
extern void update_cropdates(Cropdates [],int);
extern void freecropdates(Cropdates *);
extern Bool fwritecropdates(FILE *,const Cropdates *,int);
extern Cropdates *freadcropdates(FILE *,int,Bool);
extern void fprintcropdates(FILE *,const Cropdates [],const Pftpar *,int);

/* Definitions of macros */

#endif
