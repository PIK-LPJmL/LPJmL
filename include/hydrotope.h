/***************************************************************************/
/**                                                                       **/
/**                         h y d r o t o p e . h                         **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran version         **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     hydrotopes data definitions for wetland code                      **/
/**     Thomas Kleinen, 27/06/2008                                        **/
/**                                                                       **/
/**     Last change: $Date:: 2019-01-15 09:01:03 +0100 (Di, 15 Jan 2019#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#ifndef HYDRO_H /* Already included? */
#define HYDRO_H

/* Definition of constants */

#define TOPMODEL FALSE
#define NHYD 10
#define HYD_AV_TIME 50
#define WTAB_THRES 0.1
#define WLAND_MIN_THRES 0.
#define WLAND_MAX_THRES 0.9 
#define CTI_THRES 5.5 
#define EFOLD_HYD 2.3

#define cfrac_oxic  0.50	/* carbon fraction acrotelm g/g  -- Malmer & Wallen (2004):    */
/*  0.48 at top, 0.52 at bottom acrotelm -->  mean value is 0.5 */
#define cfrac_anoxic   0.52	/* carbon fraction acrotelm g/g  -- Malmer & Wallen (2004, var. others */
#define density_oxic  3.5e4	/* density acrotelm g/m^3 -- Granberg (1999): rho upper 20cm: 0.02 g/cm^3  */
/*  rho 20-40cm: 0.05 g/cm^3, > 40 cm: 0.1-0.12 g/cm^3  */
/* for acrotelm -> 0.5 * 0.02 + 0.5 * 0.05 = 0.035 g/cm^3 = 3.5e4 g/m^3  */
#define density_anoxic   9.1e4	/* density catotelm g/m^3 -- Botch et al. (1995): 0.11 g/cm^3 mean val. Western Russia */
/* agrees with Granberg, Clymo (1983); Turunen (2002): 91 g/l = 0.091 g/cm^3 */

/* Definition of datatypes */

typedef struct
{
  Real wland_min;
  Real cti_max;
  Real lat_min;
  Real wtab_thres;
  Real cti_thres;
} Hydropar;

typedef struct
{
  Bool skip_cell;
  int changecount;
  Real cti_mean;
  Real cti_chi;
  Real cti_phi;
  Real cti_mu;

  Real wtable_mean;
  Real wtable_monthly;
  Real wtable_min;
  Real wtable_max;

  Real meanwater;
  Real wetland_area;
  Real wetland_area_runmean;
  Real wetland_cti;
  Real wetland_cti_runmean;

  Real wetland_wtable_current;
  Real wetland_wtable_monthly;
  Real wetland_wtable_max;
  Real wetland_wtable_mean;

  Real permafrost_modifier;
} Hydrotope;

/* Declaration of global variable */

extern Hydropar hydropar;

/* Declaration of functions */

extern Bool fscanhydropar(LPJfile *, Verbosity);
extern void fprinthydropar(FILE *);

#endif
