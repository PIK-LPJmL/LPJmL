/**************************************************************************************/
/**                                                                                \n**/
/**              o  u  t  p  u  t  n  a  m  e  s  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Checks whether outpiut filenames has been used more than once              \n**/
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

void outputnames(Outputfile *outfile, /**< output file array */
                 const Config *config /**< LPJmL configuration */
                )
{
  int i,j;
  for(i=0;i<config->n_out;i++)
    if(config->outputvars[i].filename.fmt==CDF)
    {
      outfile->files[config->outputvars[i].id].fp.cdf.state=ONEFILE;
      /* check whether filename has already been used */
      for(j=i-1;j>=0;j--)
        if(!strcmp(config->outputvars[j].filename.name,config->outputvars[i].filename.name))
        {
          if(config->outputvars[j].filename.fmt==CDF)
          {
            outfile->files[config->outputvars[i].id].fp.cdf.state=CLOSE;
            if( outfile->files[config->outputvars[j].id].fp.cdf.state==ONEFILE)
            {
              outfile->files[config->outputvars[j].id].fp.cdf.state=CREATE;
              outfile->files[config->outputvars[i].id].fp.cdf.root=&outfile->files[config->outputvars[j].id].fp.cdf;
            }
            else
            {
              outfile->files[config->outputvars[j].id].fp.cdf.state=APPEND;
              outfile->files[config->outputvars[i].id].fp.cdf.root=outfile->files[config->outputvars[j].id].fp.cdf.root;
            }
          }
          else if(isroot(*config))
            fprintf(stderr,"WARNING023: Filename is identical for %s and %s.\n",
                    config->outnames[config->outputvars[i].id].name,
                    config->outnames[config->outputvars[j].id].name);
          break;
        }
    }
    else if(isroot(*config))
    {
      for(j=i-1;j>=0;j--)
        if(!strcmp(config->outputvars[j].filename.name,config->outputvars[i].filename.name))
          fprintf(stderr,"WARNING023: Filename is identical for %s and %s.\n",
                  config->outnames[config->outputvars[i].id].name,
                  config->outnames[config->outputvars[j].id].name);
    }
} /* of 'outputnames' */
