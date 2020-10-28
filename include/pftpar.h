/**************************************************************************************/
/**                                                                                \n**/
/**                    p  f  t  p  a  r  .  h                                      \n**/
/**                                                                                \n**/
/**     Pft parameter header, should be included in data file                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef PFTPAR_H /* Already included? */
#define PFTPAR_H

/* Definition of constant for pft parameter */

#define EVERGREEN 0
#define RAINGREEN 1
#define SUMMERGREEN 2
#define ANY 3
#define CROPGREEN 4
#define NOPATHWAY 0
#define C3 1
#define C4 2
#define BROADLEAVED 0
#define NEEDLELEAVED 1
#define ANYLEAVED 2
#define GRASS 0
#define TREE 1
#define CROP 2
#define NONE 0
#define BIOMASS 1
#define ANNUAL_CROP 2
#define WP 3

#define NO_CALC_SDATE 0
#define PREC_CALC_SDATE 1
#define TEMP_WTYP_CALC_SDATE 2
#define TEMP_STYP_CALC_SDATE 3
#define TEMP_PREC_CALC_SDATE 4
#define MULTICROP 5

#define NGRASS 2 /* OTHERS + MANAGED GRASSLAND */
#define NBIOMASSTYPE 2 /* GRASS + TREE */
#if defined IMAGE || defined INCLUDEWP
#define NWPTYPE 1
#else
#define NWPTYPE 0
#endif
#define WIRRIG 2
#define ALLNATURAL -2 /* ensures summing daily outputs for all natural vegetation pfts */
#define ALLGRASSLAND -3 /* ensures summing daily outputs for all natural vegetation pfts */
#define ALLSTAND -4

/* the following constants are used by IMAGE */

#define TROPICAL_BROADLEAVED_EVERGREEN_TREE 0
#define TROPICAL_BROADLEAVED_RAINGREEN_TREE 1
#define TEMPERATE_NEEDLELEAVED_EVERGREEN_TREE 2
#define TEMPERATE_BROADLEAVED_EVERGREEN_TREE 3
#define TEMPERATE_BROADLEAVED_SUMMERGREEN_TREE 4
#define BOREAL_NEEDLELEAVED_EVERGREEN_TREE 5
#define BOREAL_BROADLEAVED_SUMMERGREEN_TREE 6
#define BOREAL_NEEDLELEAVED_SUMMERGREEN_TREE 7
#define TROPICAL_HERBACEOUS 8
#define TEMPERATE_HERBACEOUS 9
#define POLAR_HERBACEOUS 10

#define TEMPERATE_CEREALS 0
#define RICE 1
#define MAIZE 2
#define TROPICAL_CEREALS 3
#define PULSES  4
#define TEMPERATE_ROOTS 5
#define TROPICAL_ROOTS 6
#define OIL_CROPS_SUNFLOWER 7
#define OIL_CROPS_SOYBEAN 8
#define OIL_CROPS_GROUNDNUT 9
#define OIL_CROPS_RAPESEED 10
#define SUGARCANE 11

#endif
