/**************************************************************************************/
/**                                                                                \n**/
/**                c  r  e  a  t  e  p  f  t  n  a  m  e  s  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates string vector for desctiption of PFT-specific             \n**/
/**     output in NetCDF files.                                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return NULL; }
#define checkptr2(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE; }

static char *prefix[2]={"rainfed ","irrigated "};

static Bool addpfts(char **pftnames,int *index,Bool iscft,int irr,const Pftpar *pftpar,int size)
{
  int i;
  for(i=0;i<size;i++)
  {
    if(iscft)
    {
      pftnames[*index]=malloc(strlen(pftpar[i].name)+strlen(prefix[irr])+1);
      checkptr2(pftnames[*index]);
      strcpy(pftnames[*index],prefix[irr]);
      strcat(pftnames[*index],pftpar[i].name);
    }
    else
    {
      pftnames[*index]=strdup(pftpar[i].name);
      checkptr2(pftnames[*index]);
    }
    (*index)++;
  }
  return FALSE;
} /* of addnames' */

static Bool addnames(char **pftnames,int *index,Bool iscft,int irr,const char **names,int size)
{
  int i;
  for(i=0;i<size;i++)
  {
    if(iscft)
    {
      pftnames[*index]=malloc(strlen(names[i])+strlen(prefix[irr])+1);
      checkptr2(pftnames[*index]);
      strcpy(pftnames[*index],prefix[irr]);
      strcat(pftnames[*index],names[i]);
    }
    else
    {
      pftnames[*index]=strdup(names[i]);
      checkptr2(pftnames[*index]);
    }
    (*index)++;
  }
  return FALSE;
} /* of addnames' */

char **createpftnames(int index,           /**< output index */
                      int npft,            /**< number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      const Config *config /**< LPJmL configuration */
                     )                     /** \return string vector */
{
  int i,j,incr,size,nnat;
  char **pftnames;
  size=outputsize(index,npft,ncft,config);
  pftnames=newvec(char *,size);
  checkptr(pftnames);
  nnat=getnnat(npft,config);
  incr=0;
  switch(index)
  {
    case SDATE: case HDATE: case HUSUM: case SYEAR: case CFT_SWC:
    case SDATE2: case HDATE2: case HUSUM2: case SYEAR2: case CFT_LEACHING:
    case CFT_N2O_DENIT : case CFT_N2O_NIT: case CFT_N2_EMIS: case CFT_C_EMIS:
    case CFT_N2O_DENIT2: case CFT_N2O_NIT2: case CFT_N2_EMIS2: case CFT_C_EMIS2:
    case CFT_LEACHING2: case CFT_RUNOFF: case CFT_RUNOFF2:
      if(addpfts(pftnames,&incr,TRUE,0,config->pftpar+npft,ncft))
        return NULL;
      if(addpfts(pftnames,&incr,TRUE,1,config->pftpar+npft,ncft))
        return NULL;
      break;
    case PFT_NPP: case PFT_GCGP: case PFT_NUPTAKE: case PFT_NDEMAND:
    case PFT_VEGC: case PFT_VEGN: case PFT_CLEAF: case PFT_NLEAF:
    case PFT_CROOT: case PFT_NROOT: case PFT_CSAPW: case PFT_NSAPW:
    case PFT_CHAWO: case PFT_NHAWO: case PFT_LAIMAX: case PFT_NLIMIT:
    case PFT_LAI: case PFT_NUPTAKE2:
      if(addpfts(pftnames,&incr,FALSE,0,config->pftpar,nnat))
        return NULL;
      for(i=0;i<2;i++)
      {
        if(addpfts(pftnames,&incr,TRUE,i,config->pftpar+npft,ncft))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,grassland_names,NGRASS))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,biomass_names,NBIOMASSTYPE))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,woodplantation_names,config->nwptype))
          return NULL;
        if(addpfts(pftnames,&incr,TRUE,i,config->pftpar+npft-config->nagtree,config->nagtree))
          return NULL;
      } 
      break;
    case PFT_HARVESTC: case PFT_RHARVESTC: case CFT_CONSUMP_WATER_G: case CFT_EVAP: case CFT_EVAP_B:
    case CFT_CONSUMP_WATER_B: case CFTFRAC: case CFT_AIRRIG: case CFT_FPAR: case CFT_IRRIG_EVENTS:
    case CFT_TRANSP: case CFT_TRANSP_B: case CFT_INTERC: case CFT_INTERC_B: case CFT_RETURN_FLOW_B:
    case CFT_CONV_LOSS_EVAP: case CFT_CONV_LOSS_DRAIN: case CFT_NIR: case CFT_NHARVEST:
    case PFT_HARVESTN: case PFT_RHARVESTN: case CFT_NFERT: case CFT_NFERT2:
    case PFT_HARVESTC2: case PFT_RHARVESTC2: case CFT_EVAP2: case CFT_TRANSP2: case CFT_INTERC2:
    case CFT_NIR2: case CFT_AIRRIG2: case CFTFRAC2: case PFT_HARVESTN2: case PFT_RHARVESTN2:
      for(i=0;i<2;i++)
      {
        if(addpfts(pftnames,&incr,TRUE,i,config->pftpar+npft,ncft))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,grassland_names,NGRASS))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,biomass_names,NBIOMASSTYPE))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,woodplantation_names,config->nwptype))
          return NULL;
        if(addpfts(pftnames,&incr,TRUE,i,config->pftpar+npft-config->nagtree,config->nagtree))
          return NULL;
      }
      break;
    case GROWING_PERIOD:case CFT_TEMP:case CFT_PREC:
    case CFT_SRAD: case CFT_ABOVEGBMC: case CFT_ABOVEGBMN:
    case GROWING_PERIOD2: case CFT_TEMP2: case CFT_PREC2:
    case CFT_SRAD2: case CFT_ABOVEGBMC2:  case CFT_ABOVEGBMN2 :case CFT_PET: case CFT_PET2:
      for(i=0;i<2;i++)
      {
        if(addpfts(pftnames,&incr,TRUE,i,config->pftpar+npft,ncft))
          return NULL;
        if(addnames(pftnames,&incr,TRUE,i,grassland_names,NGRASS))
          return NULL;
      }
      break;
    case FPC:
      pftnames[incr]=strdup("natural stand fraction");
      checkptr(pftnames[incr]);
      incr++;
      if(addpfts(pftnames,&incr,FALSE,0,config->pftpar,nnat))
        return NULL;
      break;
    case PFT_MORT: case NV_LAI:
      if(addpfts(pftnames,&incr,FALSE,0,config->pftpar,nnat))
        return NULL;
      break;
    case WFT_VEGC:
      incr=0;
      for (j=0;j<npft;j++)
      {
        if (config->pftpar[j].type==TREE && config->pftpar[j].cultivation_type==WP)
        {
          pftnames[incr]=strdup(config->pftpar[j].name);
          checkptr(pftnames[incr]);
          incr++;
        }
      }
      break;
    case FPC_BFT:
       for(i=0;i<config->ngrass;i++)
       {
         pftnames[i]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("rainfed grassland ")+1);
         checkptr(pftnames[i]);
         strcpy(pftnames[i],"rainfed grassland ");
         strcat(pftnames[i],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->ngrass;i++)
       {
         pftnames[i+config->ngrass]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("rainfed biomass tree ")+1);
         checkptr(pftnames[i+config->ngrass]);
         strcpy(pftnames[i+config->ngrass],"rainfed biomass tree ");
         strcat(pftnames[i+config->ngrass],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->nbiomass;i++)
       {
         pftnames[i+2*config->ngrass]=malloc(strlen(config->pftpar[i+nnat].name)+strlen("rainfed ")+1);
         checkptr(pftnames[i+2*config->ngrass]);
         strcpy(pftnames[i+2*config->ngrass],"rainfed ");
         strcat(pftnames[i+2*config->ngrass],config->pftpar[i+nnat].name);
       }
       for(i=0;i<config->ngrass;i++)
       {
         pftnames[i+config->ngrass*2+config->nbiomass]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("irrigated grassland ")+1);
         checkptr(pftnames[i+2*config->ngrass+config->nbiomass]);
         strcpy(pftnames[i+config->ngrass*2+config->nbiomass],"irrigated grassland ");
         strcat(pftnames[i+config->ngrass*2+config->nbiomass],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->ngrass;i++)
       {
         pftnames[i+config->ngrass*3+config->nbiomass]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("irrigated biomass tree ")+1);
         checkptr(pftnames[i+3*config->ngrass+config->nbiomass]);
         strcpy(pftnames[i+config->ngrass*3+config->nbiomass],"irrigated biomass tree ");
         strcat(pftnames[i+config->ngrass*3+config->nbiomass],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->nbiomass;i++)
       {
         pftnames[i+config->ngrass*4+config->nbiomass]=malloc(strlen(config->pftpar[i+nnat].name)+strlen("irrigated ")+1);
         checkptr(pftnames[i+4*config->ngrass+config->nbiomass]);
         strcpy(pftnames[i+config->ngrass*4+config->nbiomass],"irrigated ");
         strcat(pftnames[i+config->ngrass*4+config->nbiomass],config->pftpar[i+nnat].name);
       }
       break;
    default:
      fprintf(stderr,"ERROR248: No PFT names defined for output '%s'.\n",config->outnames[index].name);
      pftnames=NULL;
  }
  return pftnames;
} /* of 'createpftnames' */

void freepftnames(char **pftnames,
                  int index,           /**< output index */
                  int npft,            /**< number of natural PFTs */
                  int ncft,            /**< number of crop PFTs */
                  const Config *config /**< LPJmL configuration */
                 )
{
  int i,size;
  if(pftnames!=NULL)
  {
    size=outputsize(index,npft,ncft,config);
    for(i=0;i<size;i++)
      free(pftnames[i]);
    free(pftnames);
  }
} /* of 'freepftnames' */
