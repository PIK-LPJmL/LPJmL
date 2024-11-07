/**************************************************************************************/
/**                                                                                \n**/
/**              c  h  e  c  k  u  n i  q  o  u  t  p  u  t  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Checks whether output filenames has been used more than once               \n**/
/**     Only NetCDF files can share filenames                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool checkuniqoutput(int npft,            /**< number of natural PFTs */
                     int ncft,            /**< number of crop PFTs */
                     const Config *config /**< LPJmL configuration */
                )                         /** \return TRUE on error */
{
  int i,j;
  for(i=0;i<config->n_out;i++)
  {
    if(config->outputvars[i].filename.fmt!=SOCK)
    {
      /* check whether filename has already been used */
      for(j=i-1;j>=0;j--)
        if(config->outputvars[j].filename.fmt!=SOCK && !strcmp(config->outputvars[j].filename.name,config->outputvars[i].filename.name))
        {
          if(config->outputvars[i].filename.fmt!=CDF || config->outputvars[j].filename.fmt!=CDF)
          {
            if(isroot(*config))
              fprintf(stderr,"ERROR265: Filename is identical for output '%s' and '%s' and file type is not NetCDF.\n",
                      config->outnames[config->outputvars[i].id].name,
                      config->outnames[config->outputvars[j].id].name);
            return TRUE;
          }
          else if(outputsize(config->outputvars[i].id,npft,ncft,config)>1 ||
                  outputsize(config->outputvars[j].id,npft,ncft,config)>1)
          {
            if(isroot(*config))
              fprintf(stderr,"ERROR265: Filename is identical for output '%s' and '%s' and number of bands>1.\n",
                      config->outnames[config->outputvars[i].id].name,
                      config->outnames[config->outputvars[j].id].name);
            return TRUE;
          }
          else if(getnyear(config->outnames,config->outputvars[i].id)!=getnyear(config->outnames,config->outputvars[j].id))
          {
            if(isroot(*config))
              fprintf(stderr,"ERROR265: Filename is identical for output '%s' and '%s' and time steps differ.\n",
                      config->outnames[config->outputvars[i].id].name,
                      config->outnames[config->outputvars[j].id].name);
            return TRUE;
          }
        }
    }
  }
  return FALSE;
} /* of 'checkuniqoutput' */
