/***************************************************************************/
/**                                                                       **/
/**                  e f r .  h                                           **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Declaration of efr functions                                      **/
/**                                                                       **/
/**      written by Amandine Pastor, Wietse Franssen                      **/
/**     Wageningen univeristy, Earth system science                       **/
/**     Postbus 47                                                        **/
/**     6700AA Wageningen, Netherlands                                    **/                                                                  
/**                                                                       **/
/**     Last change: 08.04.2014                                           **/
/**                                                                       **/
/***************************************************************************/

#ifndef EFR_H
#define EFR_H
#define NYEAR_EFR 30 /* length of period to estimate EFR. Estimation of EFR should be done with natural flows, in restart period*/

typedef struct
{
  Real *mdischarge_buffer;          /* buffer for time series for EFR estimation in 1.000.000 m3 per day *//*Note Lotte: creates one array including all mdischarge values in period NYEAR_EFR, needed for discharge statistics*/
  Real discharge_q90;               /* discharge Q90 in reference period in 1.000.000 m3 per day */ /*discharge die 90% van de tijd overschreden wordt*/
  Real discharge_q75;               /* discharge Q75 in reference period in 1.000.000 m3 per day */
  Real discharge_q50;               /* discharge Q50 in reference period in 1.000.000 m3 per day */
  Real adischarge_mean;             /* mean annual discharge in reference period in 1.000.000 m3 per day */
  Real *mdischarge_mean;            /* mean monthly discharge in reference period in 1.000.000 m3 per day */
  Real *mefr_target;                /* monthly environmental flow target, based on specified method, in 1.000.000 m3 per day */
  Real aefr_target;                 /* long term annual environmental flow target in 1.000.000 m3 per day */
  Real *mefr_deficit;               /* long term monthly environmental flow deficit in 1.000.000 m3 per day, difference between efr_target and discharge */
  Real *mefr_threshold;             /* monthly environmental flow limit for withdrawal in liter per day */
  Real efr_buffer;                  /* buffer for unfulfilled EFR requests in liter per day, reset each month */
} Efrdata;

/*Nog uitzoeken verschil tussen threshold en target*/

extern Bool discharge_statistics(Cell *); //This is a declaration of the variable. The header is included by the source file that defines the variable. only one source file cnan define the variable,. 
extern Bool efr_determine(Cell *, int, int); // Declaration of function. It is defined in efr.c
extern Bool new_efrdata(Efrdata *, Bool);
extern Bool fwriteefrdata(FILE *,const Efrdata *);// const can be applied to the declaration of any variable to sp[ecify that its value will not be changed. 
extern Bool freadefrdata(FILE *,Efrdata *,Bool);

#endif
