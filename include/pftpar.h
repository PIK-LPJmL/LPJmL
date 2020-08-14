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

/* CROP must always last id because of ntypes, see also lpjml.c */
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
#define BIOENERGY_TROPICAL_TREE 11
#define BIOENERGY_TEMPERATE_TREE 12
#define BIOENERGY_C4_GRASS 13
#define WINTERWHEAT 14
#define SPRINGWHEAT 15
#define MAIZE 16
#define RICE1 17
#define RICE2 18
#define SOYBEAN 19
#define MILLET 20
#define SORGHUM 21
#define PEAS 22
#define SUGARBEET 23
#define CASSAVA 24
#define RAPESEED 25
#define SUNFLOWER 26
#define GROUNDNUT 27
#define SUGARCANE 28

#define NO_CALC_SDATE 0
#define PREC_CALC_SDATE 1
#define TEMP_WTYP_CALC_SDATE 2
#define TEMP_STYP_CALC_SDATE 3
#define TEMP_PREC_CALC_SDATE 4
#define MULTICROP 5

#define NGRASS 2 /* OTHERS + MANAGED GRASSLAND */
#define NBIOMASSTYPE 2 /* TREE + GRASS */
#define WIRRIG 2

#define ALLNATURAL -1 /* ensures summing daily outputs for all natural vegetation pfts */

#endif
