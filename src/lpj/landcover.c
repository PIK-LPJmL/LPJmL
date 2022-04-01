/**************************************************************************************/
/**                                                                                \n**/
/**                  l  a  n  d  c  o  v  e  r  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Implementation of landcover datatype.                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct landcover
{
  Climatefile file;
  Real *frac;
};

Landcover initlandcover(int npft,            /**< number of natural PFTs */
                        const Config *config /**< LPJmL configuration */
                       )                     /** \return landcover data or NULL */
{
  Landcover landcover;
  int i,len;
  
  landcover=new(struct landcover);
  if(landcover==NULL)
  {
    printallocerr("landcover");
    return NULL;
  }
  if(opendata(&landcover->file,&config->landcover_filename,"landcover","1",LPJ_SHORT,0.01,getnnat(npft,config),TRUE,config))
  {
    free(landcover);
    return NULL;
  }
  len=config->ngridcell*landcover->file.var_len;
  if((landcover->frac=newvec(Real,len))==NULL)
  {
    printallocerr("frac");
    closeclimatefile(&landcover->file,isroot(*config));
    free(landcover);
    return NULL;
  }
  for(i=0;i<len;i++)
    landcover->frac[i]=0;
  return landcover;
} /* of 'initlandcover' */

/*
- called in lpj()
- opens the landcover input file (see also building file for the landcover Input (like cfts26_lu2clm.c)
- sets the landcover variables (see also manage.h)
*/

Bool readlandcover(Landcover landcover, /**< landcover data */
                   const Cell grid[],   /**< LPJ cell array */
                   int year,            /**< simulation year (AD) */
                   const Config *config /**< LPJmL configuration */
                  )                     /** \return TRUE on error */
{
  char *name;
  int index;
  index = year - landcover->file.firstyear;
  if(index >= landcover->file.nyear)
    index = landcover->file.nyear - 1;
  /* read first year of file if iteration year is earlier */
  if(index<0)
    index=0;
  /* read always first year if there is only one year in the file */
  if(landcover->file.nyear == 1)
    index=0;
  if(landcover->file.fmt==CDF)
  {
    if(readdata_netcdf(&landcover->file,landcover->frac,grid,year,config))
    {
      fprintf(stderr,
              "ERROR149: Cannot read landcover of year %d from '%s'.\n",
              year+landcover->file.firstyear,config->landcover_filename.name);
      fflush(stderr);
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    /* read last year of file if iteration year is later */
  
    if(fseek(landcover->file.file, landcover->file.size * index + landcover->file.offset,SEEK_SET))
    {
      name=getrealfilename(&config->landcover_filename);
      fprintf(stderr,"ERROR184: Cannot seek to landcover fractions of year %d in '%s'.\n",
              index+landcover->file.firstyear,name);
      free(name);
      return TRUE;
    }
    if(readrealvec(landcover->file.file,landcover->frac,0,landcover->file.scalar,landcover->file.n,landcover->file.swap,landcover->file.datatype))
    {
      name=getrealfilename(&config->landcover_filename);
      fprintf(stderr,"ERROR184: Cannot read landcover fractions of year %d from '%s'.\n",
              index+landcover->file.firstyear,name);
      free(name);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'readlandcover */

Real *getlandcover(Landcover landcover, /**< landcover data */
                   int index            /**< cell index */
                  )                     /** \return land cover array of specified cell */
{
  return landcover->frac+index*landcover->file.var_len;
} /* of 'getlandcover' */

void freelandcover(Landcover landcover, /**< landcover data */
                   Bool isroot          /**< task is root task */
                  )
{
  if(landcover!=NULL)
  {
    closeclimatefile(&landcover->file,isroot);
    free(landcover->frac);
    free(landcover);
  }
} /* of 'freelandcover' */
