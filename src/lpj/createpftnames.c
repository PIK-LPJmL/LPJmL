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
                      int nbiomass,        /**< number of biomass types */
                      int nwft,            /**< number of WFTs */
                      int ncft,            /**< number of crop PFTs */
                      const Pftpar *pftpar /**< PFT parameter vector */
                     )                     /** \return string vector */
{
  int i,j,incr,size;
  char **pftnames;
  size=outputsize(index,npft,nbiomass,nwft,ncft);
  pftnames=newvec(char *,size);
  if(pftnames==NULL)
    return NULL;
  switch(index)
  {
    case SDATE: case HDATE:
    case SDATE2: case HDATE2: case SYEAR: case SYEAR2:
      for(i=0;i<ncft;i++)
      {
        pftnames[i]=strdup(pftpar[npft+i].name);
        checkptr(pftnames[i]);
      }
      for(i=0;i<ncft;i++)
      {
        pftnames[i+ncft]=malloc(strlen(pftpar[npft+i].name)+strlen("irrigated ")+1);
        checkptr(pftnames[i+ncft]);
        strcpy(pftnames[i+ncft],"irrigated ");
        strcat(pftnames[i+ncft],pftpar[npft+i].name);
      }
      break;
    case PFT_NPP: case PFT_GCGP: case MPFT_LAI:
      for(i=0;i<npft-nbiomass-nwft;i++)
      {
        pftnames[i]=strdup(pftpar[i].name);
        checkptr(pftnames[i]);
      }
      incr=npft-nbiomass-nwft;
      for(i=0;i<2;i++)
      {
        for(j=0;j<ncft;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(pftpar[npft+j].name)+strlen("irrigated ")+1);
            checkptr(pftnames[j+incr]);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],pftpar[npft+j].name);
          }
          else
          {
            pftnames[j+incr]=strdup(pftpar[npft+j].name);
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
#if defined IMAGE || defined INCLUDEWP
        pftnames[incr]=strdup((i) ? "irrigated woodplantation": "woodplantation");
        incr+=1;
#endif
      } 
      break;
    case PFT_HARVEST: case PFT_RHARVEST: case CFT_CONSUMP_WATER_G: case CFT_EVAP: case CFT_EVAP_B:
    case CFT_CONSUMP_WATER_B: case CFTFRAC: case CFT_AIRRIG: case CFT_FPAR: case CFT_IRRIG_EVENTS:
    case CFT_TRANSP: case CFT_TRANSP_B: case CFT_INTERC: case CFT_INTERC_B: case CFT_RETURN_FLOW_B:
    case CFT_CONV_LOSS_EVAP: case CFT_CONV_LOSS_DRAIN: case CFT_NIR:
    case PFT_HARVEST2: case PFT_RHARVEST2: case CFT_EVAP2: case CFT_TRANSP2: case CFT_INTERC2:
    case CFT_NIR2: case CFT_AIRRIG2: case CFTFRAC2:
      incr=0;
      for(i=0;i<2;i++)
      {
        for(j=0;j<ncft;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(pftpar[npft+j].name)+strlen("irrigated ")+1);
            checkptr(pftnames[j+incr]);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],pftpar[npft+j].name);
          }
          else
          {
            pftnames[j+incr]=strdup(pftpar[npft+j].name);
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
#if defined IMAGE || defined INCLUDEWP
        pftnames[incr]=strdup((i) ? "irrigated woodplantation": "woodplantation");
        incr+=1;
#endif
      }
      break;
    case GROWING_PERIOD:case CFT_TEMP:case CFT_PREC:
    case CFT_SRAD: case CFT_ABOVEGBM:
    case GROWING_PERIOD2: case CFT_TEMP2: case CFT_PREC2:
    case CFT_SRAD2: case CFT_ABOVEGBM2: case CFT_PET: case CFT_PET2:
      incr=0;
      for(i=0;i<2;i++)
      {
        for(j=0;j<ncft;j++)
          if(i)
          {
            pftnames[j+incr]=malloc(strlen(pftpar[npft+j].name)+strlen("irrigated ")+1);
            checkptr(pftnames[j+incr]);
            strcpy(pftnames[j+incr],"irrigated ");
            strcat(pftnames[j+incr],pftpar[npft+j].name);
          }
          else
          {
            pftnames[j+incr]=strdup(pftpar[npft+j].name);
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
      for(i=0;i<npft-nbiomass-nwft;i++)
      {
        pftnames[i+1]=strdup(pftpar[i].name);
        checkptr(pftnames[i+1]);
      }
      break;
    case PFT_MORT:
      for(i=0;i<npft-nbiomass-nwft;i++)
      {
        pftnames[i]=strdup(pftpar[i].name);
        checkptr(pftnames[i]);
      }
      break;
#ifdef IMAGE
    case WFT_VEGC:
      incr=0;
      for (j=0;j<npft;j++)
      {
        if (pftpar[j].type==TREE && pftpar[j].cultivation_type==WP)
        {
          pftnames[incr]=strdup(pftpar[j].name);
          incr++;
        }
      }
      break;
#endif
  } /* of switch */
  return pftnames;
} /* of 'createpftnames' */

void freepftnames(char **pftnames,int index,int npft,int nbiomass,int nwft,int ncft)
{
  int i,size;
  if(pftnames!=NULL)
  {
    size=outputsize(index,npft,nbiomass,nwft,ncft);
    for(i=0;i<size;i++)
      free(pftnames[i]);
    free(pftnames);
  }
} /* of 'freepftnames' */
