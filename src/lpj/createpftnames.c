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

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return NULL; }

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
  if(pftnames==NULL)
    return NULL;
  nnat=getnnat(npft,config);
  switch(index)
  {
    case SDATE: case HDATE: case HUSUM: case SYEAR: case CFT_SWC:
    case SDATE2: case HDATE2: case HUSUM2: case SYEAR2: case CFT_LEACHING:
    case CFT_N2O_DENIT : case CFT_N2O_NIT: case CFT_N2_EMIS: case CFT_C_EMIS:
    case CFT_N2O_DENIT2: case CFT_N2O_NIT2: case CFT_N2_EMIS2: case CFT_C_EMIS2:
    case CFT_LEACHING2: case CFT_RUNOFF: case CFT_RUNOFF2:
      for(i=0;i<ncft;i++)
      {
        pftnames[i]=strdup(config->pftpar[npft+i].name);
        checkptr(pftnames[i]);
      }
      for(i=0;i<ncft;i++)
      {
        pftnames[i+ncft]=malloc(strlen(config->pftpar[npft+i].name)+strlen("irrigated ")+1);
        checkptr(pftnames[i+ncft]);
        strcpy(pftnames[i+ncft],"irrigated ");
        strcat(pftnames[i+ncft],config->pftpar[npft+i].name);
      }
      break;
    case PFT_NPP: case PFT_GCGP: case PFT_NUPTAKE: case PFT_NDEMAND:
    case PFT_VEGC: case PFT_VEGN: case PFT_CLEAF: case PFT_NLEAF:
    case PFT_CROOT: case PFT_NROOT: case PFT_CSAPW: case PFT_NSAPW:
    case PFT_CHAWO: case PFT_NHAWO: case PFT_LAIMAX: case PFT_NLIMIT:
    case PFT_LAI: case PFT_NUPTAKE2:
      for(i=0;i<nnat;i++)
      {
        pftnames[i]=strdup(config->pftpar[i].name);
        checkptr(pftnames[i]);
      }
      incr=nnat;
      for(i=0;i<2;i++)
      {
        for(j=0;j<ncft;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(config->pftpar[npft+j].name)+strlen("irrigated ")+1);
            checkptr(pftnames[j+incr]);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],config->pftpar[npft+j].name);
          }
          else
          {
            pftnames[j+incr]=strdup(config->pftpar[npft+j].name);
            checkptr(pftnames[j+incr]);
          }
        incr+=ncft;
        pftnames[incr]=strdup((i) ? "irrigated others":  "others");
        checkptr(pftnames[incr]);
        pftnames[incr+1]=strdup((i) ? "irrigated pasture": "pasture");
        checkptr(pftnames[incr+1]);
        incr+=2;
        pftnames[incr]=strdup((i) ? "irrigated biomass grass": "biomass grass");
        checkptr(pftnames[incr]);
        pftnames[incr+1]=strdup((i) ? "irrigated biomass tree":  "biomass tree");
        checkptr(pftnames[incr+1]);
        incr+=2;
        if(config->nwptype)
        {
          pftnames[incr]=strdup((i) ? "irrigated woodplantation": "woodplantation");
          incr+=1;
        }
        for (j=0;j<config->nagtree;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(config->pftpar[npft-config->nagtree+j].name)+strlen("irrigated ")+1);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],config->pftpar[npft+j-config->nagtree].name);
          }
        else
          pftnames[j+incr]=strdup(config->pftpar[npft+j-config->nagtree].name);
        incr+=config->nagtree;

      } 
      break;
    case PFT_HARVESTC: case PFT_RHARVESTC: case CFT_CONSUMP_WATER_G: case CFT_EVAP: case CFT_EVAP_B:
    case CFT_CONSUMP_WATER_B: case CFTFRAC: case CFT_AIRRIG: case CFT_FPAR: case CFT_IRRIG_EVENTS:
    case CFT_TRANSP: case CFT_TRANSP_B: case CFT_INTERC: case CFT_INTERC_B: case CFT_RETURN_FLOW_B:
    case CFT_CONV_LOSS_EVAP: case CFT_CONV_LOSS_DRAIN: case CFT_NIR:
    case PFT_HARVESTN: case PFT_RHARVESTN: case CFT_NFERT: case CFT_NFERT2:
    case PFT_HARVESTC2: case PFT_RHARVESTC2: case CFT_EVAP2: case CFT_TRANSP2: case CFT_INTERC2:
    case CFT_NIR2: case CFT_AIRRIG2: case CFTFRAC2: case PFT_HARVESTN2: case PFT_RHARVESTN2:
      incr=0;
      for(i=0;i<2;i++)
      {
        for(j=0;j<ncft;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(config->pftpar[npft+j].name)+strlen("irrigated ")+1);
            checkptr(pftnames[j+incr]);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],config->pftpar[npft+j].name);
          }
          else
          {
            pftnames[j+incr]=strdup(config->pftpar[npft+j].name);
            checkptr(pftnames[j+incr]);
          }
        incr+=ncft;
        pftnames[incr]=strdup((i) ? "irrigated others":  "others");
        checkptr(pftnames[incr]);
        pftnames[incr+1]=strdup((i) ? "irrigated pasture": "pasture");
        checkptr(pftnames[incr+1]);
        incr+=2;
        pftnames[incr]=strdup((i) ? "irrigated biomass grass": "biomass grass");
        checkptr(pftnames[incr]);
        pftnames[incr+1]=strdup((i) ? "irrigated biomass tree":  "biomass tree");
        checkptr(pftnames[incr+1]);
        incr+=2;
        if(config->nwptype)
        {
          pftnames[incr]=strdup((i) ? "irrigated woodplantation": "woodplantation");
          incr+=1;
        }
        for (j=0;j<config->nagtree;j++)
          if(i)
          {
            pftnames[j+incr] = malloc(strlen(config->pftpar[npft-config->nagtree+j].name)+strlen("irrigated ")+1);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],config->pftpar[npft+j-config->nagtree].name);
          }
          else
            pftnames[j+incr]=strdup(config->pftpar[npft+j-config->nagtree].name);
        incr+=config->nagtree;

      }
      break;
    case GROWING_PERIOD:case CFT_TEMP:case CFT_PREC:
    case CFT_SRAD: case CFT_ABOVEGBMC: case CFT_ABOVEGBMN:
    case GROWING_PERIOD2: case CFT_TEMP2: case CFT_PREC2:
    case CFT_SRAD2: case CFT_ABOVEGBMC2:  case CFT_ABOVEGBMN2 :case CFT_PET: case CFT_PET2:
      incr=0;
      for(i=0;i<2;i++)
      {
        for(j=0;j<ncft;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(config->pftpar[npft+j].name)+strlen("irrigated ")+1);
            checkptr(pftnames[j+incr]);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],config->pftpar[npft+j].name);
          }
          else
          {
            pftnames[j+incr]=strdup(config->pftpar[npft+j].name);
            checkptr(pftnames[j+incr]);
          }
        incr+=ncft;
        pftnames[incr]=strdup((i) ? "irrigated others":  "others");
        checkptr(pftnames[incr]);
        pftnames[incr+1]=strdup((i) ? "irrigated pasture": "pasture");
        checkptr(pftnames[incr+1]);
        incr+=2;
      }
      break;
    case FPC:
      pftnames[0]=strdup("natural stand fraction");
      checkptr(pftnames[0]);
      for(i=0;i<nnat;i++)
      {
        pftnames[i+1]=strdup(config->pftpar[i].name);
        checkptr(pftnames[i+1]);
      }
      break;
    case PFT_MORT: case NV_LAI:
      for(i=0;i<nnat;i++)
      {
        pftnames[i]=strdup(config->pftpar[i].name);
        checkptr(pftnames[i]);
      }
      break;
    case WFT_VEGC:
      incr=0;
      for (j=0;j<npft;j++)
      {
        if (config->pftpar[j].type==TREE && config->pftpar[j].cultivation_type==WP)
        {
          pftnames[incr]=strdup(config->pftpar[j].name);
          incr++;
        }
      }
      break;
    case FPC_BFT:
       for(i=0;i<config->ngrass;i++)
       {
         pftnames[i]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("grassland ")+1);
         strcpy(pftnames[i],"grassland ");
         strcat(pftnames[i],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->ngrass;i++)
       {
         pftnames[i+config->ngrass]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("biomass tree ")+1);
         strcpy(pftnames[i+config->ngrass],"biomass tree ");
         strcat(pftnames[i+config->ngrass],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->nbiomass;i++)
         pftnames[i+2*config->ngrass]=strdup(config->pftpar[i+nnat].name);
       for(i=0;i<config->ngrass;i++)
       {
          pftnames[i+config->ngrass*2+config->nbiomass]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("grassland irrigated ")+1);
          strcpy(pftnames[i+config->ngrass*2+config->nbiomass],"grassland irrigated ");
          strcat(pftnames[i+config->ngrass*2+config->nbiomass],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->ngrass;i++)
       {
          pftnames[i+config->ngrass*3+config->nbiomass]=malloc(strlen(config->pftpar[i+nnat-config->ngrass].name)+strlen("biomass tree irrigated ")+1);
          strcpy(pftnames[i+config->ngrass*3+config->nbiomass],"biomass tree irrigated ");
          strcat(pftnames[i+config->ngrass*3+config->nbiomass],config->pftpar[i+nnat-config->ngrass].name);
       }
       for(i=0;i<config->nbiomass;i++)
       {
          pftnames[i+config->ngrass*4+config->nbiomass]=malloc(strlen(config->pftpar[i+nnat].name)+strlen("irrigated ")+1);
          strcpy(pftnames[i+config->ngrass*4+config->nbiomass],"irrigated ");
          strcat(pftnames[i+config->ngrass*4+config->nbiomass],config->pftpar[i+nnat].name);
       }
       break;
  }
  return pftnames;
} /* of 'createpftnames' */

void freepftnames(char **pftnames,int index,int npft,int ncft,const Config *config)
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
