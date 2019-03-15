/**************************************************************************************/
/**                                                                                \n**/
/**                  c  o  n  f  .  h                                              \n**/
/**                                                                                \n**/
/**     Header for LPJ configuration file                                          \n**/
/**     Must be included by LPJmL configuration file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef CONF_H
#define CONF_H

#define LPJ 0          /* LPJ simulation with natural vegetation only */
#define LPJML 1        /* LPJ simulation with managed land */
#define LPJML_IMAGE 2  /* LPJ simulation with managed land coupled to IMAGE
                          model */
#define LPJML_FMS 3  /* LPJ simulation with managed land coupled to FMS */
#define LANDUSE 1
#define NO_LANDUSE 0
#define CONST_LANDUSE 3
#define ALL_CROPS 4
#define NO_FIXED_SDATE 0
#define FIXED_SDATE 1
#define PRESCRIBED_SDATE 2
#define NO_IRRIGATION 0
#define LIM_IRRIGATION 1
#define POT_IRRIGATION 2
#define ALL_IRRIGATION 3
#define GRASS_HARVEST_OPTIONS 1 
#define NO_GRASS_HARVEST_OPTIONS 0 
#define GRASS_FIXED_PFT 1 
#define NO_GRASS_FIXED_PFT 0 
#define LAIMAX_CFT 0
#define LAIMAX_INTERPOLATE 1
#define CONST_LAI_MAX 2
#define NO_RESERVOIR 0
#define RESERVOIR 1
#define NO_DRAINAGE 0
#define DRAINAGE 1
#define PERM 1
#define NO_PERM 0
#define NEW_PERC 1
#define OLD_PERC 0
#define NO_WATERUSE 0
#define WATERUSE 1
#define ALL_WATERUSE 2
#define NO_RWMANAGEMENT 0
#define RWMANAGEMENT 1
#define RADIATION_LWDOWN 3
#define RADIATION_SWONLY 2
#define RADIATION 1
#define CLOUDINESS 0
#define ALL -1
#define RAW 0
#define CLM 1
#define CLM2 2
#define TXT 3
#define FMS 4
#define META 5
#define CDF 6
#define RESTART 1
#define NO_RESTART 0
#define NO_FIRE 0
#define FIRE 1
#define SPITFIRE 2
#define SPITFIRE_TMAX 3
#define NO_PRESCRIBE_BURNTAREA 0
#define PRESCRIBE_BURNTAREA 1
#define NO_LANDCOVER 0
#define LANDCOVEREST 1
#define LANDCOVERFPC 2
#define OLD_PHENOLOGY 0
#define NEW_PHENOLOGY 1
#define NO_POPULATION 0
#define POPULATION 1
#define NO_FIREWOOD 0
#define FIREWOOD 1
#define RANDOM_PREC 1
#define INTERPOLATE_PREC 0
#define NOUT 215            /* number of output files */
#define END -1              /* end marker for output files */
#define GRIDBASED 1         /* pft-specific outputs scaled by stand->frac */
#define PFTBASED 0          /* pft-specific outputs not scaled by stand->frac */
#define DAILY_IRRIG 1       /* daily outputs for irrigated crop */
#define DAILY_RAINFED 0     /* daily outputs for rain-fed crop */
#define CONST_PREC 2
/*monthly and annual output*/
#define GRID 0
#define FPC 1
#define MNPP 2
#define MGPP 3
#define MRH 4
#define MTRANSP  5
#define MRUNOFF 6
#define MDISCHARGE 7
#define MEVAP 8
#define MINTERC 9
#define MSWC1  10
#define MSWC2 11
#define FIREC 12
#define FIREF 13
#define VEGC 14
#define SOILC 15
#define LITC 16
#define FLUX_ESTAB 17
#define PFT_NPP 18
#define PFT_GCGP 19
#define HARVEST 20
#define MIRRIG 21
#define SDATE 22
#define PFT_HARVEST 23
#define PFT_RHARVEST 24
#define COUNTRY 25
#define REGION 26
#define MG_VEGC 27
#define MG_SOILC 28
#define MG_LITC 29
#define APREC 30
#define INPUT_LAKE 31
#define MRETURN_FLOW_B 32
#define MEVAP_LAKE 33
#define ADISCHARGE 34
#define CFTFRAC 35
#define CFT_CONSUMP_WATER_G 36
#define CFT_CONSUMP_WATER_B 37
#define PROD_TURNOVER 38
#define DEFOREST_EMIS 39
#define AIRRIG 40
#define CFT_AIRRIG 41
#define HDATE 42
#define GROWING_PERIOD 43
#define CFT_PET 44
#define CFT_TRANSP 45
#define CFT_EVAP 46
#define CFT_INTERC 47
#define CFT_NIR 48
#define CFT_TEMP 49
#define CFT_PREC 50
/* daily outputs for single crops */
#define D_LAI 51
#define D_PHEN 52
#define D_CLEAF 53
#define D_CROOT 54
#define D_CSO 55
#define D_CPOOL 56
#define D_WDF 57
#define D_GROWINGDAY 58
#define D_PVD 59
#define D_PHU 60
#define D_FPHU 61
#define D_LAIMAXAD 62
#define D_LAINPPDEF 63
#define D_HUSUM 64
#define D_VDSUM 65
#define D_WSCAL 66
#define D_NPP 67
#define D_GPP 68
#define D_RD 69
#define D_RROOT 70
#define D_RSO 71
#define D_RPOOL 72
#define D_GRESP 73
#define D_TRANS 74
#define D_EVAP 75
#define D_PREC 76
#define D_PERC 77
#define D_IRRIG 78
#define D_W0 79
#define D_W1 80
#define D_WEVAP 81
#define D_HI 82
#define D_FHIOPT 83
#define D_HIMIND 84
#define D_FROOT 85
#define D_TEMP 86
#define D_SUN 87
#define D_PAR 88
#define D_DAYLENGTH 89
#define D_SWE 90
#define D_DISCHARGE 91
#define D_RUNOFF 92
#define D_RH 93
#define D_INTERC 94
#define D_ASSIM 95
#define D_ROT_MODE 96
#define D_PET 97 /* last daily output ID; Insert additional output before! */
#define MEVAP_RES 98
#define MPREC_RES 99
#define MFIREC 100
#define MNFIRE 101
#define MFIREDI 102
#define MFIREEMISSION_CO2 103
#define MFAPAR 104
#define FLUX_FIREWOOD 105
#define RHARVEST_BURNT 106
#define RHARVEST_BURNT_IN_FIELD 107
#define MTEMP_IMAGE 108
#define MPREC_IMAGE 109
#define MSUN_IMAGE 110
#define MWET_IMAGE 111
#define FBURN 112
#define FTIMBER 113
#define TIMBER_HARVEST 114
#define PRODUCT_POOL_FAST 115
#define PRODUCT_POOL_SLOW 116
#define LUC_IMAGE 117
#define MAXTHAW_DEPTH 118 
#define MSOILTEMP1 119
#define MSOILTEMP2 120
#define MSOILTEMP3 121
#define MSOILTEMP4 122
#define MSOILTEMP5 123
#define MSOILTEMP6 124
#define MRES_STORAGE 125
#define MRES_DEMAND 126
#define MTARGET_RELEASE 127
#define MRES_CAP 128
#define SEASONALITY 129
#define MPET 130
#define TRAD_BIOFUEL 131
#define AFRAC_WD_UNSUST 132
#define MWD_UNSUST 133
#define ACONV_LOSS_EVAP 134
#define ACONV_LOSS_DRAIN 135
#define MWD_LOCAL 136
#define MWD_NEIGHB 137
#define MWD_RES 138
#define MWD_RETURN 139
#define MCONV_LOSS_EVAP 140
#define MCONV_LOSS_DRAIN 141
#define MPREC 142
#define MRAIN 143
#define MSNOWF 144
#define MMELT 145
#define MSNOWRUNOFF 146
#define MSWE 147
#define SOILC_LAYER 148
#define MRUNOFF_SURF 149
#define MRUNOFF_LAT  150
#define MSEEPAGE 151
#define MBURNTAREA 152
#define SOILC_SLOW 153
#define MSWC3 154
#define MSWC4 155
#define MSWC5 156
#define MROOTMOIST 157
#define CFT_SRAD 158
#define CFT_ABOVEGBM 159
#define MGCONS_RF 160
#define MGCONS_IRR 161
#define MBCONS_IRR 162
#define AWATERUSE_HIL 163
#define MWATERAMOUNT 164
#define MIRRIG_RW 165
#define MTRANSP_B  166
#define CFT_TRANSP_B 167
#define MUNMET_DEMAND 168
#define CFT_FPAR 169
#define MEVAP_B 170
#define MINTERC_B 171
#define CFT_EVAP_B 172
#define CFT_INTERC_B 173
#define CFT_RETURN_FLOW_B 174
#define CFT_IRRIG_EVENTS 175
#define CFT_CONV_LOSS_EVAP 176
#define CFT_CONV_LOSS_DRAIN 177
#define MSTOR_RETURN 178
#define HDATE2 179
#define SDATE2 180
#define PFT_HARVEST2 181
#define PFT_RHARVEST2 182
#define GROWING_PERIOD2 183
#define CFT_PET2 184
#define CFT_TRANSP2 185
#define CFT_EVAP2 186
#define CFT_INTERC2 187
#define CFT_NIR2 188
#define CFT_TEMP2 189
#define CFT_PREC2 190
#define CFT_SRAD2 191
#define CFT_ABOVEGBM2 192
#define CFTFRAC2 193
#define CFT_AIRRIG2 194
#define SYEAR 195
#define SYEAR2 196
#define MALBEDO 197
#define MPHEN_TMIN 198
#define MPHEN_TMAX 199
#define MPHEN_LIGHT 200
#define MPHEN_WATER 201
#define MWSCAL 202
#define AGB 203
#define MLAKEVOL 204
#define MLAKETEMP 205
#define MFIREEMISSION_CO 206
#define MFIREEMISSION_CH4 207
#define MFIREEMISSION_VOC 208
#define MFIREEMISSION_TPM 209
#define MFIREEMISSION_NOX 210
#define MSOILTEMP 211
#define MSWC 212
#define ABURNTAREA 213
#define AGB_TREE 214
#define RANDOM_SEED 0

#endif
