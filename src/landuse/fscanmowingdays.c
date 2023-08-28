/**************************************************************************************/
/**                                                                                \n**/
/**     f  s  c  a  n  m  o  w  i  n  g  d  a  y  s  .  c                          \n**/
/**                                                                                \n**/
/** Function reads mowing days from configuration file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static const int mowingDays[] = {152, 335}; // mowing on fixed dates 1-june or 1-dec

Bool fscanmowingdays(LPJfile *file, /**< pointer to LPJ configuration file */
                     Config *config /**< LPJ configuration */
                    )               /** \return TRUE on error */
{
  LPJfile *array,*item;
  Verbosity verbose;
  int i;
  if(iskeydefined(file,"mowing_days"))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    array=fscanarray(file,&config->mowingdays_size,"mowing_days",verbose);
    if(array==NULL)
      return TRUE;
    config->mowingdays=newvec(int,config->mowingdays_size);
    if(config->mowingdays==NULL)
    {
      printallocerr("mowingdays");
      return TRUE;
    }
    for(i=0;i<config->mowingdays_size;i++)
    {
      item=fscanarrayindex(array,i);
      if(fscanint(item,config->mowingdays+i,NULL,FALSE,verbose))
      {
        free(config->mowingdays);
        return TRUE;
      }
      if(config->mowingdays[i]<1 || config->mowingdays[i]>NDAYYEAR)
      {
        if(verbose)
          fprintf(stderr,"ERROR241: Invalid value for mowing day=%d, must be in [1,%d].\n",
                  config->mowingdays[i],NDAYYEAR);
        free(config->mowingdays);
        return TRUE;
      }
    }
  }
  else
  {
    config->mowingdays=newvec(int,2);
    if(config->mowingdays==NULL)
    {
      printallocerr("mowingdays");
      return TRUE;
    }
    config->mowingdays_size=2;
    for(i=0;i<2;i++)
      config->mowingdays[i]=mowingDays[i];
  }
  return FALSE;
} /* of 'fscanmowingdays' */
