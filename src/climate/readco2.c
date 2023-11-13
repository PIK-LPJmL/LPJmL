/**************************************************************************************/
/**                                                                                \n**/
/**               r  e  a  d  c  o  2  .  c                                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads atmopsheric CO2 concentration from text file.               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readco2(Co2data *co2,             /**< pointer to co2 data */
             const Filename *filename, /**< file name */
             Config *config            /**< LPJmL configuration */
            )                          /** \return TRUE on error */
{
  FILE *file;
  int yr,yr_old,size;
  Bool iseof;
  Verbosity verbose;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(filename->fmt==FMS || (iscoupled(*config) && filename->issocket))
  {
    co2->data=NULL;
    co2->nyear=0;
    co2->firstyear=0;
    if(filename->issocket)
    {
      if(openinput_coupler(filename->id,LPJ_FLOAT,0,&size,config))
        return TRUE;
      co2->id=filename->id;
      if(size!=1)
      {
        if(verbose)
          fprintf(stderr,"ERROR149: Invalid number of bands=%d received from socket, must be 1.\n",size);
        return TRUE;
      }
    }
  }
  if(filename->fmt==TXT)
  {
    if(iscoupled(*config) && config->start_coupling<=config->firstyear-config->nspinup)
      return FALSE;
    if((file=fopen(filename->name,"r"))==NULL)
    {
      if(verbose)
        printfopenerr(filename->name);
      return TRUE;
    }
    initscan(filename->name);
    co2->data=newvec(Real,1);
    if(co2->data==NULL)
    {
      printallocerr("co2");
      fclose(file);
      return TRUE;
    }
    /**
    * find start year in co2-file
    **/
    if(ffscanint(file,&yr,"year",verbose) || ffscanreal(file,co2->data,"co2",verbose))
    {
      if(verbose)
        fprintf(stderr,"ERROR129: Cannot read CO2 data in first line of '%s'.\n",
                filename->name);
      free(co2->data);
      fclose(file);
      return TRUE;
    }
    co2->firstyear=yr;
    co2->nyear=1;
    yr_old=yr;
    while(!feof(file))
    {
      co2->data=(Real *)realloc(co2->data,sizeof(Real)*(co2->nyear+1));
      if(co2->data==NULL)
      {
        printallocerr("co2");
        fclose(file);
        return TRUE;
      }
      if(fscaninteof(file,&yr,"year",&iseof,isroot(*config)) || ffscanreal(file,co2->data+co2->nyear,"co2",verbose))

      {
        if(iseof)
          break;
        if(verbose)
          fprintf(stderr,"ERROR129: Cannot read CO2 data in line %d of '%s'.\n",
                  getlinecount(),filename->name);
        free(co2->data);
        fclose(file);
        return TRUE;
      }
      if(yr!=yr_old+1)
      {
        if(verbose)
          fprintf(stderr,"ERROR157: Invalid year=%d in line %d of '%s', must be %d.\n",
                  yr,getlinecount(),filename->name,yr_old+1);
        free(co2->data);
        fclose(file);
        return TRUE;
      }
      co2->nyear++;
      yr_old=yr;
    }
    fclose(file);
    if(config->fix_co2 && co2->firstyear+co2->nyear-1<min(config->lastyear,config->fix_co2_year))
    {
      if(config->firstyear>co2->firstyear+co2->nyear-1)
      {
        if(verbose)
          fprintf(stderr,"ERROR259: Last year=%d in CO2 data '%s' less than first simulation year %d.\n",
                  co2->firstyear+co2->nyear-1,filename->name,config->firstyear);
        return TRUE;
      }
      if(verbose)
        fprintf(stderr,"ERROR260: Last year=%d in CO2 data '%s' less than CO2 fix year %d, last year set to %d.\n",
                co2->firstyear+co2->nyear-1,filename->name,min(config->lastyear,config->fix_co2_year),co2->firstyear+co2->nyear-1);
      config->lastyear=co2->firstyear+co2->nyear-1;
    }
    if(!config->fix_co2 && co2->firstyear+co2->nyear-1<config->lastyear)
    {
      if(config->firstyear>co2->firstyear+co2->nyear-1)
      {
        if(verbose)
          fprintf(stderr,"ERROR259: Last year=%d in CO2 data '%s' less than first simulation year %d.\n",
                  co2->firstyear+co2->nyear-1,filename->name,config->firstyear);
        return TRUE;
      }
      if(verbose)
        fprintf(stderr,"ERROR260: Last year=%d in CO2 data '%s' less than last simulation year %d, last year set to %d.\n",
                co2->firstyear+co2->nyear-1,filename->name,config->lastyear,co2->firstyear+co2->nyear-1);
      config->lastyear=co2->firstyear+co2->nyear-1;
    }
  }
  else if(filename->fmt==SOCK && config->start_coupling>config->firstyear-config->nspinup)
  {
    if(verbose)
      fprintf(stderr,"ERROR149: No filename specified for CO2 data required for socket connection before coupling year %d, first simulatiomn year=%d.\n",
             config->start_coupling,config->firstyear-config->nspinup);
    return TRUE;
  }
  return FALSE;
} /* of 'readco2' */
