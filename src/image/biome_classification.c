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

/* PFT indices for biome classification */
enum
{
  TrBE,   /* tropical broadleaved evergreen tree */
  TrBEfl, /* tropical broadleaved evergreen tree floodtolerant */
  TrBR,   /* tropical broadleaved raingreen tree */
  TeNE,   /* temperate needleleaved evergreen tree */
  TeBE,   /* temperate broadleaved evergreen tree */
  TeBS,   /* temperate broadleaved summergreen tree */
  BoNE,   /* boreal needleleaved evergreen tree */
  BoBS,   /* boreal broadleaved summergreen tree */
  BoNS,   /* boreal needleleaved summergreen tree */
  TrH,    /* tropical C4 grass */
  TeH,    /* temperate C3 grass */
  PoH,    /* polar C3 grass */
  C3fl,   /* C3 graminoid flood tolerant */
  Sph,    /* Sphagnum moss */
  NPFT_BIOME /* number of PFTs used for biome classification */
};

/* PFT names corresponding to enum indices */
static const char *pftnames[NPFT_BIOME]=
{
  "tropical broadleaved evergreen tree",
  "tropical broadleaved evergreen tree floodtolerant",
  "tropical broadleaved raingreen tree",
  "temperate needleleaved evergreen tree",
  "temperate broadleaved evergreen tree",
  "temperate broadleaved summergreen tree",
  "boreal needleleaved evergreen tree",
  "boreal broadleaved summergreen tree",
  "boreal needleleaved summergreen tree",
  "Tropical C4 grass",
  "Temperate C3 grass",
  "Polar C3 grass",
  "C3 graminoid flood tolerant",
  "Sphagnum moss"
};

/* called for natural stands only */

int biome_classification(Real atemp,            /**< Annual average temperature (deg C) */
                         Real npp,              /**< NPP of cell considered */
                         const Stand *stand,    /**< Pointer to stand */
                         int npft,              /**< number of natural PFTs */
                         const Config *config   /**< LPJmL configuration */
                        )                       /**< returns biome */
{
  int bclass=MANAGED_LAND;
  Real *fpc;
  Real fpctreetotal, fpcboreal, fpctemperate, fpctropical, fpcgrass, fpctotal;
  Real fpcneedleleaved;
  Real maxshare=0;
  int p;
  const Pft *pft;
  int pftid[NPFT_BIOME]; /* PFT indices looked up by name */

  /* Look up PFT indices by name */
  for(p=0;p<NPFT_BIOME;p++)
    pftid[p]=findpftname(pftnames[p],config->pftpar,npft);

  fpc=newvec(Real,npft);
  check(fpc);
  for(p=0;p<npft;p++)
    fpc[p]=0.0;

  foreachpft(pft,p,&stand->pftlist)
  {
    fpc[pft->par->id]=pft->fpc;
    maxshare=pft->fpc>maxshare ? pft->fpc : maxshare;
  }

  fpctemperate=fpc[pftid[TeNE]]+fpc[pftid[TeBE]]+fpc[pftid[TeBS]];
  fpctropical=fpc[pftid[TrBE]]+fpc[pftid[TrBEfl]]+fpc[pftid[TrBR]];
  fpcboreal=fpc[pftid[BoNE]]+fpc[pftid[BoBS]]+fpc[pftid[BoNS]];
  fpctreetotal=fpctemperate+fpctropical+fpcboreal;
  fpcgrass=fpc[pftid[TeH]]+fpc[pftid[TrH]]+fpc[pftid[PoH]]+fpc[pftid[C3fl]]+fpc[pftid[Sph]];
  fpctotal=fpctreetotal+fpcgrass;
  fpcneedleleaved=fpc[pftid[TeNE]]+fpc[pftid[BoNE]]+fpc[pftid[BoNS]];

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
    if(((fpc[pftid[BoBS]]==maxshare || fpc[pftid[BoNE]]==maxshare) && fpctemperate>0.1)
      || (((fpc[pftid[TeNE]]==maxshare || fpc[pftid[TeBE]]==maxshare ||
            fpc[pftid[TeBS]]==maxshare) && fpcboreal>0.1)))
      bclass=TEMPERATE_MIXED_FOREST;
    else if(fpc[pftid[BoBS]]==maxshare)
      bclass=BOREAL_FOREST;                            /* boreal deciduous forest */
    else if(fpc[pftid[BoNE]]==maxshare)
      bclass=COOL_CONIFER_FOREST;                      /* boreal evergreen forest */
    else if(fpc[pftid[TeNE]]==maxshare)
      bclass=TEMPERATE_MIXED_FOREST;                   /* temperate coniferous forest */
    else if(fpc[pftid[TeBS]]==maxshare)
      bclass=TEMPERATE_DECIDUOUS_FOREST;               /* temperate deciduous forest */
    else if(fpc[pftid[TeBE]]==maxshare)
      bclass=WARM_MIXED_FOREST;                        /* temperate broadleaved evergreen forest */
    else if(fpctreetotal<0.9 && fpcboreal==0 && fpc[pftid[TrBR]]<0.6)
      bclass=SAVANNAH;                                 /* moist savannah */
    else if(fpc[pftid[TrBE]]==maxshare)
      bclass=TROPICAL_FOREST;                          /* tropical rain forest */
    else if(fpc[pftid[TrBR]]==maxshare)
      bclass=TROPICAL_WOODLAND;                        /* tropical deciduous forest */
  } /* end trees >=80% */
  else if(fpcboreal>0.4 && atemp<-2)
    bclass=WOODED_TUNDRA;
  else if(fpctemperate>0.2)
    bclass=SCRUBLAND; /* xeric shrubs */
  else if(fpctropical>=0.3 && fpcboreal==0) /* tottree <0.8 anyway */
    bclass=SAVANNAH; /* moist savannah */
  else if((fpcgrass>=0.2 && fpctropical>=0.05 && fpcneedleleaved<0.1) ||
    fpc[pftid[TrH]]>=0.9)
    bclass=SAVANNAH; /* dry savannah */
  else if(fpc[pftid[TeH]]>=0.8)
  {
    if(atemp<(-2))
      bclass=TUNDRA; /* arctic tundra */
    else
    {
      if(fpc[pftid[TrH]]>fpc[pftid[TeH]])
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
      if(fpc[pftid[TrH]]>fpc[pftid[TeH]])
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
