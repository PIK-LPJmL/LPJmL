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
             Bool isout                /**< enable error output */
            )                          /** \return TRUE on error */
{
  LPJfile file;
  int yr,yr_old;
  Bool iseof;
  file.isjson=FALSE;
  if(filename->fmt==FMS)
  {
    co2->data=NULL;
    co2->nyear=0;
    co2->firstyear=0;
  }
  else if(filename->fmt==TXT)
  {
    if((file.file.file=fopen(filename->name,"r"))==NULL)
    {
      if(isout)
        printfopenerr(filename->name);
      return TRUE;
    }
    initscan(filename->name);
    co2->data=newvec(Real,1);
    if(co2->data==NULL)
    {
      printallocerr("co2");
      fclose(file.file.file);
      return TRUE;
    }
    /**
    * find start year in co2-file
    **/
    if(fscanint(&file,&yr,"year",FALSE,isout ? ERR : NO_ERR) || fscanreal(&file,co2->data,"co2",FALSE,isout ? ERR : NO_ERR))
    {
      if(isout)
        fprintf(stderr,"ERROR129: Cannot read CO2 data in first line of '%s'.\n",
                filename->name);
      free(co2->data);
      fclose(file.file.file);
      return TRUE;
    }
    co2->firstyear=yr;
    co2->nyear=1;
    yr_old=yr;
    while(!feof(file.file.file))
    {
      co2->data=(Real *)realloc(co2->data,sizeof(Real)*(co2->nyear+1));
      if(co2->data==NULL)
      {
        printallocerr("co2");
        fclose(file.file.file);
        return TRUE;
      }
      if(fscaninteof(file.file.file,&yr,"year",&iseof,isout) || fscanreal(&file,co2->data+co2->nyear,"co2",FALSE,isout ? ERR : NO_ERR))

      {
        if(iseof)
          break;
        if(isout)
          fprintf(stderr,"ERROR129: Cannot read CO2 data in line %d of '%s'.\n",
                  getlinecount(),filename->name);
        free(co2->data);
        fclose(file.file.file);
        return TRUE;
      }
      if(yr!=yr_old+1)
      {
        if(isout)
          fprintf(stderr,"ERROR157: Invalid year=%d in line %d of '%s'.\n",
                  yr,getlinecount(),filename->name);
        free(co2->data);
        fclose(file.file.file);
        return TRUE;
      }
      co2->nyear++;
      yr_old=yr;
    }
    fclose(file.file.file);
  }
  else
    return TRUE;
  return FALSE;
} /* of 'readco2' */
