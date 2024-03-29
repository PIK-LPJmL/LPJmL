/**************************************************************************************/
/**                                                                                \n**/
/**    b  i  o  m  e  _  c  l  a  s  s  i  f  i  c  a  t  i  o  n  .  c            \n**/
/**                                                                                \n**/
/**     extension of LPJ to classify biomes based on FPC values only               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/* called for natural stands only */

int biome_classification(Real atemp,            /**< Annual average temperature (deg C) */
                         Real npp,              /**< NPP of cell considered */
                         const Stand *stand,    /**< Pointer to stand */
                         int npft               /**< number of natural PFTs */
                        )                       /**< returns biome */
{
  int bclass=MANAGED_LAND;
  Real *fpc;
  Real fpctreetotal, fpcboreal, fpctemperate, fpctropical, fpcgrass, fpctotal;
  Real fpcneedleleaved;
  Real maxshare=0;
  int p;
  const Pft *pft;

  fpc=newvec(Real,npft);
  check(fpc);
  for(p=0;p<npft;p++)
    fpc[p]=0.0;

  foreachpft(pft,p,&stand->pftlist)
  {
    fpc[pft->par->id]=pft->fpc;
    maxshare=pft->fpc>maxshare ? pft->fpc : maxshare;
  }

  fpctemperate=fpc[TEMPERATE_NEEDLELEAVED_EVERGREEN_TREE]+
    fpc[TEMPERATE_BROADLEAVED_EVERGREEN_TREE]+
    fpc[TEMPERATE_BROADLEAVED_SUMMERGREEN_TREE];
  fpctropical=fpc[TROPICAL_BROADLEAVED_EVERGREEN_TREE]+
    fpc[TROPICAL_BROADLEAVED_RAINGREEN_TREE];
  fpcboreal=fpc[BOREAL_NEEDLELEAVED_EVERGREEN_TREE]+
    fpc[BOREAL_BROADLEAVED_SUMMERGREEN_TREE];
  fpctreetotal=fpctemperate+fpctropical+fpcboreal;
  fpcgrass=fpc[TEMPERATE_HERBACEOUS]+fpc[TROPICAL_HERBACEOUS];
  fpctotal=fpctreetotal+fpcgrass;
  fpcneedleleaved=fpc[TEMPERATE_NEEDLELEAVED_EVERGREEN_TREE]+
    fpc[BOREAL_NEEDLELEAVED_EVERGREEN_TREE]+
    fpc[BOREAL_BROADLEAVED_SUMMERGREEN_TREE];

  if(fpctotal<=0.1)
  {
    if(atemp<-2)
      bclass=TUNDRA;
    else
      bclass=HOT_DESERT;
  }
  else if(fpctreetotal>=0.8)
  {

    /* boreal/temperate mixed forest needs to be tested before boreal types */
    if(((fpc[BOREAL_BROADLEAVED_SUMMERGREEN_TREE]==maxshare ||               /* PFT7 dominant */
      fpc[BOREAL_NEEDLELEAVED_EVERGREEN_TREE]==maxshare)&&       /* or PFT 6 dominant */
      fpctemperate>0.1)                                          /* AND PFT 3,4,and 5 together > 0.1 */
      ||                                                         /* OR */
      (((fpc[TEMPERATE_NEEDLELEAVED_EVERGREEN_TREE]==maxshare||  /* PFT 3 dominant */
      fpc[TEMPERATE_BROADLEAVED_EVERGREEN_TREE]==maxshare||      /* or PFT 4 dominant */
      fpc[TEMPERATE_BROADLEAVED_SUMMERGREEN_TREE]==maxshare)&&   /* or PFT 5 dominant */
      fpcboreal>0.1))                                            /* AND PFT 6 or 7 >0.1 */
      )
      bclass=TEMPERATE_MIXED_FOREST;
    else if(fpc[BOREAL_BROADLEAVED_SUMMERGREEN_TREE]==maxshare)             /* PFT 7 dominant */
      bclass=BOREAL_FOREST;                            /* boreal deciduous forest */
    else if(fpc[BOREAL_NEEDLELEAVED_EVERGREEN_TREE]==maxshare)  /* PFT 6 dominant */
      bclass=COOL_CONIFER_FOREST;                        /* boreal evergreen forest */
    else if(fpc[TEMPERATE_NEEDLELEAVED_EVERGREEN_TREE]==maxshare)    /* PFT 3 dominant */
      bclass=TEMPERATE_MIXED_FOREST;                    /* temperate coniferous forest */
    else if(fpc[TEMPERATE_BROADLEAVED_SUMMERGREEN_TREE]==maxshare)   /* PFT 5 dominant */
      bclass=TEMPERATE_DECIDUOUS_FOREST;                     /* temperate deciduous forest */
    else if(fpc[TEMPERATE_BROADLEAVED_EVERGREEN_TREE]==maxshare)     /* PFT 4 dominant */
      bclass=WARM_MIXED_FOREST;         /*temperate broadleaved evergreen forest */
    else if(fpctreetotal<0.9 && fpcboreal==0 && 
      fpc[TROPICAL_BROADLEAVED_RAINGREEN_TREE]<0.6)             /* fpctreetotal>=0.8 anyway */
      bclass=SAVANNAH;                                 /* moist savannah */
    else if(fpc[TROPICAL_BROADLEAVED_EVERGREEN_TREE]==maxshare)        /* PFT 1 dominant */
      bclass=TROPICAL_FOREST;                           /* tropical rain forest */
    else if(fpc[TROPICAL_BROADLEAVED_RAINGREEN_TREE]==maxshare) /* PFT 2 dominant */
      bclass=TROPICAL_WOODLAND;                      /* tropical deciduous forest */
  } /* end trees >=80% */
  else if(fpcboreal>0.4 && atemp<-2)
    bclass=WOODED_TUNDRA;
  else if(fpctemperate>0.2)
    bclass=SCRUBLAND; /* xeric shrubs */
  else if(fpctropical>=0.3 && fpcboreal==0) /* tottree <0.8 anyway */
    bclass=SAVANNAH; /* moist savannah */
  else if((fpcgrass>=0.2 && (fpctropical)>=0.05 && fpcneedleleaved<0.1)|| /* PFT 8 or 9 dominant */
    fpc[TROPICAL_HERBACEOUS]>=0.9)
    bclass=SAVANNAH; /* dry savannah */
  else if(fpc[TEMPERATE_HERBACEOUS]>=0.8)             
  {
    if(atemp<(-2))
      bclass=TUNDRA; /* arctic tundra */
    else
    {
      if(fpc[TROPICAL_HERBACEOUS]>fpc[TEMPERATE_HERBACEOUS])
        bclass=GRASSLAND_STEPPE;
      else
        bclass=GRASSLAND_STEPPE;
    }
  }

  else if(fpctreetotal<=0.05)
  {
    if(atemp<(-2))
      bclass=TUNDRA; /* arctic tundra */
    else
      bclass=GRASSLAND_STEPPE; /* arid shrubland/steppe */
  }
  else
  {
    if(atemp<(-2))
      bclass=TUNDRA; /* arctic tundra */
    else
    {
      if(fpc[TROPICAL_HERBACEOUS]>fpc[TEMPERATE_HERBACEOUS])
        bclass=SCRUBLAND;
      else
        bclass=SCRUBLAND;
    }
  }

  /* Some IMAGE rules, formerly done in interface_image.f */
  if(npp<1.0)
	if(bclass==TUNDRA || (bclass==HOT_DESERT && atemp<=0.0))
	  bclass=IJS;
  if(bclass==HOT_DESERT && atemp<=0.0 && npp>=1.0)
	  bclass=TUNDRA;

  free(fpc);
  return bclass;
} /* of biome_classification' */
