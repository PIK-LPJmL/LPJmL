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
#define CONST_LANDUSE 2
#define ALL_CROPS 3
#define ONLY_CROPS 4
#define NO_FIXED_SDATE 0
#define FIXED_SDATE 1
#define PRESCRIBED_SDATE 2
#define NO_FIXED_SOILPAR 0
#define FIXED_SOILPAR 1
#define PRESCRIBED_SOILPAR 2
#define NO_IRRIGATION 0
#define LIM_IRRIGATION 1
#define POT_IRRIGATION 2
#define ALL_IRRIGATION 3
#ifdef IMAGE
#define GROUNDWATER_IRRIG 1
#define NO_GROUNDWATER_IRRIG 0 // Flag to allow for irrigation from groundwater reservoir (sustainable gw irrigation)
#define AQUIFER_IRRIG 1
#define NO_AQUIFER_IRRIG 0 //Flag to allow for irrigation from aquifers (according to input map for aquifers)
#endif
#define GRASS_HARVEST_OPTIONS 1
#define NO_GRASS_HARVEST_OPTIONS 0
#define GRASS_FIXED_PFT 1
#define NO_GRASS_FIXED_PFT 0
#define LAIMAX_CFT 0
#define LAIMAX_INTERPOLATE 1
#define CONST_LAI_MAX 2
#define LAIMAX_PAR 3
#define NO_RESERVOIR 0
#define RESERVOIR 1
#define NO_DRAINAGE 0
#define DRAINAGE 1
#define NO_EQUILSOIL 0
#define EQUILSOIL 1
#define PERM 1
#define NO_PERM 0
#define NEW_PERC 1
#define OLD_PERC 0
#define NO_NITROGEN 0
#define LIM_NITROGEN 1
#define UNLIM_NITROGEN 2
#define NO_WATERUSE 0
#define WATERUSE 1
#define ALL_WATERUSE 2
#define NO_RWMANAGEMENT 0
#define RWMANAGEMENT 1
#define NO_TILLAGE 0
#define TILLAGE 1
#define READ_TILLAGE 2
#define NO_RESIDUE_REMOVE 0
#define FIXED_RESIDUE_REMOVE 1
#define READ_RESIDUE_DATA 2
#define BMGR_GREEN 0
#define BMGR_BROWN 1
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
#define SOCK 7
#define RESTART 1
#define NO_RESTART 0
#define NO_FIRE 0
#define FIRE 1
#define SPITFIRE 2
#define SPITFIRE_TMAX 3
#define NESTEROV_INDEX 0
#define WVPD_INDEX 1
#define NO_PRESCRIBE_BURNTAREA 0
#define PRESCRIBE_BURNTAREA 1
#define NO_LANDCOVER 0
#define LANDCOVEREST 1
#define LANDCOVERFPC 2
#define OLD_PHENOLOGY 0
#define NEW_PHENOLOGY 1
#define OLD_TRF 0
#define NEW_TRF 1
#define OLD_CROP_PHU 0
#define NEW_CROP_PHU 1
#define PRESCRIBED_CROP_PHU 2
#define NO_POPULATION 0
#define POPULATION 1
#define NO_FIREWOOD 0
#define FIREWOOD 1
#define RANDOM_PREC 1
#define INTERPOLATE_PREC 0
#define NO_FERTILIZER 0
#define FERTILIZER 1
#define AUTO_FERTILIZER 2
#define NOUT 326            /* number of output files */
#define END -1              /* end marker for output files */
#define GRIDBASED 1         /* pft-specific outputs scaled by stand->frac */
#define PFTBASED 0          /* pft-specific outputs not scaled by stand->frac */
#define DAILY_IRRIG 1       /* daily outputs for irrigated crop */
#define DAILY_RAINFED 0     /* daily outputs for rain-fed crop */
#define CONST_PREC 2
#define ANNUAL -2
#define MONTHLY -3
#define DAILY -4
/* output*/
#define GRID 0
#define COUNTRY 1
#define REGION 2
#define GLOBALFLUX 3
#define TERR_AREA 4
#define LAKE_AREA 5
#define FPC 6
#define TRANSP 7
#define RUNOFF 8
#define DISCHARGE 9
#define EVAP 10
#define INTERC 11
#define SWC1 12
#define SWC2 13
#define FIREC 14
#define FIREF 15
#define VEGC 16
#define SOILC 17
#define LITC 18
#define FLUX_ESTABC 19
#define PFT_NPP 20
#define PFT_GCGP 21
#define HARVESTC 22
#define IRRIG 23
#define SDATE 24
#define PFT_HARVESTC 25
#define PFT_RHARVESTC 26
#define NPP 27
#define GPP 28
#define MG_VEGC 29
#define MG_SOILC 30
#define MG_LITC 31
#define AGB_TREE 32
#define INPUT_LAKE 33
#define RETURN_FLOW_B 34
#define EVAP_LAKE 35
#define ADISCHARGE 36
#define CFTFRAC 37
#define CFT_CONSUMP_WATER_G 38
#define CFT_CONSUMP_WATER_B 39
#define PROD_TURNOVER 40
#define DEFOREST_EMIS 41
#define NV_LAI 42
#define CFT_AIRRIG 43
#define HDATE 44
#define GROWING_PERIOD 45
#define CFT_PET 46
#define CFT_TRANSP 47
#define CFT_EVAP 48
#define CFT_INTERC 49
#define CFT_NIR 50
#define CFT_TEMP 51
#define CFT_PREC 52
#define WD_AQ 53
#define YDISCHARGE 54
#define TEMP 55
#define SUN 56
#define DAYLENGTH 57
#define EVAP_RES 58
#define PREC_RES 59
#define LITBURNC_WOOD 60
#define WFT_VEGC 61
#define NFIRE 62
#define FIREDI 63
#define FIREEMISSION_CO2 64
#define FAPAR 65
#define FLUX_FIREWOOD 66
#define RHARVEST_BURNTC 67
#define RHARVEST_BURNT_IN_FIELDC 68
#define TEMP_IMAGE 69
#define PREC_IMAGE 70
#define SUN_IMAGE 71
#define WET_IMAGE 72
#define FBURN 73
#define FTIMBER 74
#define TIMBER_HARVESTC 75
#define PRODUCT_POOL_FAST 76
#define PRODUCT_POOL_SLOW 77
#define LUC_IMAGE 78
#define MAXTHAW_DEPTH 79
#define SOILTEMP1 80
#define SOILTEMP2 81
#define SOILTEMP3 82
#define SOILTEMP4 83
#define SOILTEMP5 84
#define SOILTEMP6 85
#define RES_STORAGE 86
#define RES_DEMAND 87
#define TARGET_RELEASE 88
#define RES_CAP 89
#define SEASONALITY 90
#define PET 91
#define TRAD_BIOFUEL 92
#define AFRAC_WD_UNSUST 93
#define WD_UNSUST 94
#define RH_LITTER 95
#define WD_LOCAL 96
#define WD_NEIGHB 97
#define WD_RES 98
#define CONV_LOSS_EVAP 99
#define PREC 100
#define RAIN 101
#define SNOWF 102
#define MELT 103
#define SNOWRUNOFF 104
#define SWE 105
#define SOILC_LAYER 106
#define RUNOFF_SURF 107
#define RUNOFF_LAT 108
#define SEEPAGE 109
#define BURNTAREA 110
#define SOILC_SLOW 111
#define SWC3 112
#define SWC4 113
#define SWC5 114
#define ROOTMOIST 115
#define CFT_SRAD 116
#define CFT_ABOVEGBMC 117
#define GCONS_RF 118
#define GCONS_IRR 119
#define BCONS_IRR 120
#define WATERUSE_HIL 121
#define WATERAMOUNT 122
#define CFT_ABOVEGBMN 123
#define PFT_HARVESTN 124
#define PFT_RHARVESTN 125
#define RHARVEST_BURNTN 126
#define RHARVEST_BURNT_IN_FIELDN 127
#define FLUX_ESTABN 128
#define VEGN 129
#define SOILN 130
#define LITN 131
#define SOILNH4 132
#define SOILNO3 133
#define HARVESTN 134
#define SOILN_LAYER 135
#define SOILN_SLOW 136
#define PFT_NUPTAKE 137
#define NUPTAKE 138
#define LEACHING 139
#define N2O_DENIT 140
#define N2O_NIT 141
#define N2_EMIS 142
#define BNF 143
#define PFT_NDEMAND 144
#define N_MINERALIZATION 145
#define FIREN 146
#define N_IMMO 147
#define PFT_CLEAF 148
#define PFT_NLEAF 149
#define PFT_VEGC 150
#define PFT_VEGN 151
#define IRRIG_RW 152
#define TRANSP_B 153
#define CFT_TRANSP_B 154
#define UNMET_DEMAND 155
#define CFT_FPAR 156
#define EVAP_B 157
#define INTERC_B 158
#define CFT_EVAP_B 159
#define CFT_INTERC_B 160
#define CFT_RETURN_FLOW_B 161
#define CFT_IRRIG_EVENTS 162
#define CFT_CONV_LOSS_EVAP 163
#define CFT_CONV_LOSS_DRAIN 164
#define STOR_RETURN 165
#define CONV_LOSS_DRAIN 166
#define FPC_BFT 167
#define NEGN_FLUXES 168
#define NEGC_FLUXES 169
#define PFT_LAIMAX 170
#define PFT_CROOT 171
#define PFT_NROOT 172
#define PFT_CSAPW 173
#define PFT_NSAPW 174
#define PFT_CHAWO 175
#define PFT_NHAWO 176
#define HDATE2 177
#define SDATE2 178
#define PFT_HARVESTC2 179
#define PFT_RHARVESTC2 180
#define HARVESTC_AGR 181
#define CFT_PET2 182
#define CFT_TRANSP2 183
#define CFT_EVAP2 184
#define CFT_INTERC2 185
#define CFT_NIR2 186
#define CFT_TEMP2 187
#define CFT_PREC2 188
#define CFT_SRAD2 189
#define CFT_ABOVEGBMC2 190
#define CFTFRAC2 191
#define CFT_AIRRIG2 192
#define SYEAR 193
#define SYEAR2 194
#define PFT_HARVESTN2 195
#define PFT_RHARVESTN2 196
#define CFT_ABOVEGBMN2 197
#define N_VOLATILIZATION 198
#define PFT_NLIMIT 199
#define SOILNO3_LAYER 200
#define SOILNH4_LAYER 201
#define WSCAL 202
#define AGB 203
#define LAKEVOL 204
#define LAKETEMP 205
#define FIREEMISSION_CO 206
#define FIREEMISSION_CH4 207
#define FIREEMISSION_VOC 208
#define FIREEMISSION_TPM 209
#define FIREEMISSION_NOX 210
#define SOILTEMP 211
#define SWC 212
#define ALBEDO 213
#define PHEN_TMIN 214
#define PHEN_TMAX 215
#define PHEN_LIGHT 216
#define PHEN_WATER 217
#define VEGC_AVG 218
#define LITFALLC 219
#define LITFALLN 220
#define PFT_LAI 221
#define PFT_MORT 222
#define WATERUSECONS 223
#define WATERUSEDEM 224
#define WD_GW 225
#define LITC_ALL 226
#define LITC_AG 227
#define NPP_AGR 228
#define RH_AGR 229
#define SOILC_AGR 230
#define SOILC_AGR_LAYER 231
#define LITC_AGR 232
#define GROWING_PERIOD2 233
#define DELTA_NORG_SOIL_AGR 234
#define DELTA_NMIN_SOIL_AGR 235
#define DELTA_NVEG_SOIL_AGR 236
#define HUSUM 237
#define HUSUM2 238
#define CFT_SWC 239
#define MEANVEGCMANGRASS 240
#define CFT_RUNOFF 241
#define CFT_RUNOFF2 242
#define CFT_N2O_NIT 243
#define CFT_N2O_NIT2 244
#define PFT_NUPTAKE2 245
#define CFT_N2O_DENIT 246
#define CFT_N2O_DENIT2 247
#define CFT_N2_EMIS 248
#define CFT_N2_EMIS2 249
#define CFT_LEACHING 250
#define CFT_LEACHING2 251
#define CFT_C_EMIS 252
#define CFT_C_EMIS2 253
#define BNF_AGR 254
#define NFERT_AGR 255
#define NMANURE_AGR 256
#define NDEPO_AGR 257
#define NMINERALIZATION_AGR 258
#define NIMMOBILIZATION_AGR 259
#define NUPTAKE_AGR 260
#define NLEACHING_AGR 261
#define N2O_DENIT_AGR 262
#define N2O_NIT_AGR 263
#define NH3_AGR 264
#define N2_AGR 265
#define SEEDN_AGR 266
#define CELLFRAC_AGR 267
#define LITFALLC_AGR 268
#define LITFALLN_AGR 269
#define HARVESTN_AGR 270
#define MGRASS_LITC 271
#define MGRASS_SOILC 272
#define MGRASS_LITN 273
#define MGRASS_SOILN 274
#define CFT_NFERT 275
#define CFT_NFERT2 276
#define FLUX_AUTOFERT 277
#define LITFALLC_WOOD 278
#define LITFALLN_WOOD 279
#define RESPONSE_LAYER_AGR 280
#define RESPONSE_LAYER_NV 281
#define DECAY_WOOD_AGR 282
#define DECAY_WOOD_NV 283
#define DECAY_LEAF_AGR 284
#define DECAY_LEAF_NV 285
#define CSHIFT_FAST_NV 286
#define CSHIFT_SLOW_NV 287
#define LITBURNC 288
#define RH 289
#define PERC 290
#define WHC_NAT 291
#define WHC_GRASS 292
#define WHC_AGR 293
#define KS_NAT 294
#define KS_GRASS 295
#define KS_AGR 296
#define CFT_NHARVEST 297
#define UPTAKEN_MGRASS 298
#define FECESN_MGRASS 299
#define URINEN_MGRASS 300
#define YIELDN_MGRASS 301
#define NH3_MGRASS 302
#define NO3_LEACHING_MGRASS 303
#define N2_MGRASS 304
#define N2O_NIT_MGRASS 305
#define N2O_DENIT_MGRASS 306
#define UPTAKEC_MGRASS 307
#define FECESC_MGRASS 308
#define URINEC_MGRASS 309
#define YIELDC_MGRASS 310
#define RESPC_MGRASS 311
#define METHANEC_MGRASS 312
#define DELTAC_MGRASS 313
#define RA_MGRASS 314
#define RH_MGRASS 315
#define PFT_BNF 316
#define LAND_AREA 317
#define TIMBER_HARVESTN 318
#define PRODUCT_POOL_FAST_N 319
#define PRODUCT_POOL_SLOW_N 320
#define DEFOREST_EMIS_N 321
#define PROD_TURNOVER_N 322
#define FLUX_FIREWOOD_N 323
#define ESTAB_STORAGE_C 324
#define ESTAB_STORAGE_N 325
#define PFT_GCGP_COUNT 0
#define NDAY_MONTH 1
#define RANDOM_SEED 0

#endif
