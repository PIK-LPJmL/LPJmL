/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  o  u  t  p  u  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads output file names and types from a configuration            \n**/
/*      file                                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanint2(file,var,name) if(fscanint(file,var,name,verbosity)) return TRUE;

static Bool isopenoutput(int id,const Outputvar output[],int n)
{
  /* checks whether output file has already been specified */
  int i;
  for(i=0;i<n;i++)
    if(id==output[i].id) /* id already set? */
      return TRUE; /* yes, return TRUE */
  return FALSE; /* not found */
} /* of 'isopenoutput' */

Bool fscanoutput(FILE *file,        /**< File pointer to text file */
                 Config *config,    /**< LPJ configuration */
                 int nout_max       /**< maximum number of output files */
                )                   /** \return TRUE on error */
{
  String name;
  Bool isdaily;
  int count,flag;
  Verbosity verbosity;
  verbosity=isroot(*config) ? config->scan_verbose : NO_ERR;
  config->outputvars=newvec(Outputvar,nout_max);
  if(config->outputvars==NULL)
  {
    printallocerr("outputvars");
    return TRUE;
  }
  count=0;
  isdaily=FALSE;
  fscanint2(file,&config->pft_output_scaled,"pft_output_scaled");
  while(count<=nout_max)
  {
    fscanint2(file,&flag,"out index");
    if(flag==END)  /* end marker read? */
      break;  
    else if(count==nout_max)
    {
      fprintf(stderr,"ERROR160: Invalid value=%d in line %d of '%s' for number of output files.\n",
              count,getlinecount(),getfilename());
      return TRUE;
    }
    else
    {
      if(readfilename(file,&config->outputvars[count].filename,config->outputdir,FALSE,verbosity))
        break;
      if(flag<0 || flag>=nout_max)
      {
        if(verbosity)
          fprintf(stderr,
                  "ERROR161: Invalid value=%d in line %d of '%s' for index of output file '%s'.\n",
                  flag,getlinecount(),getfilename(),name);
      }
      else if(isopenoutput(flag,config->outputvars,count))
      {
        if(verbosity)
          fprintf(stderr,"WARNING006: Output file is opened twice in line %d of '%s', will be ignored.\n",
                getlinecount(),getfilename());
      }
      else if(config->outputvars[count].filename.fmt==CLM2)
      {
        if(verbosity)
          fprintf(stderr,"ERRROR223: File format CLM2 is not supported for output file in line %d of '%s'.\n",getlinecount(),getfilename());
      }
      else
      {
        config->outputvars[count].id=flag;
        config->outputvars[count].oneyear=(strstr(config->outputvars[count].filename.name,"%d")!=NULL);
        if(config->outputvars[count].oneyear && checkfmt(config->outputvars[count].filename.name,'d'))
        {
          if(verbosity)
            fprintf(stderr,"ERRROR224: Invalid format specifier in filename '%s' in line %d of '%s'.\n",
                    config->outputvars[count].filename.name,getlinecount(),getfilename());
        }
        else if(config->outputvars[count].oneyear && (flag==GRID || flag==COUNTRY || flag==REGION))
        {
          if(verbosity)
            fprintf(stderr,"ERRROR225: One year output not allowed for grid, country or region in line %d of '%s'.\n",getlinecount(),getfilename());
        }
        else
        {
          if(flag>=D_LAI && flag<=D_PET)
            isdaily=TRUE;
          count++;
        }
      }
    }
  }
  if(config->sim_id==LPJML && isdaily)
  {
    fscanint2(file,&config->crop_index,"crop index");
    fscanint2(file,&config->crop_irrigation,"crop irrigation");
    if (config->crop_index == TROPICAL_HERBACEOUS) config->crop_index = TEMPERATE_HERBACEOUS; /* for managed grassland the key for daily output is C3_PERENNIAL_GRASS */
  }
  else
  {
    config->crop_index=-1;
    config->crop_irrigation=-1;
  }
  config->n_out=count;
  return FALSE;
} /* of 'fscanoutput' */
