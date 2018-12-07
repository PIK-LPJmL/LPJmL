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
  Header header;
  Landcover landcover;
  String headername;
  size_t offset;
  int i,version,len;
  
  landcover=new(struct landcover);
  if(landcover==NULL)
  {
    printallocerr("landcover");
    return NULL;
  }
  landcover->file.fmt=config->landcover_filename.fmt;
  if(config->landcover_filename.fmt==CDF)
  {
    if(opendata_netcdf(&landcover->file,config->landcover_filename.name,config->landcover_filename.var,NULL,config))
    {
      free(landcover);
      return NULL;
    }
    if(landcover->file.var_len!=npft-config->nbiomass)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR225: Number of bands=%d is not %d\n",(int)landcover->file.var_len,npft-config->nbiomass);
      closeclimate_netcdf(&landcover->file,isroot(*config));
      free(landcover);
      return NULL;
    }
    len=landcover->file.var_len*config->ngridcell;
  }
  else
  {
    if((landcover->file.file=openinputfile(&header,&landcover->file.swap,
                                           &config->landcover_filename,headername,
                                           &version,&offset,config))==NULL)

    {
      free(landcover);
      return NULL;
    }
    if(version==1)
      landcover->file.scalar=0.01;
    else
      landcover->file.scalar=header.scalar;
    landcover->file.firstyear=header.firstyear;
    landcover->file.nyear=header.nyear;
    if(landcover->file.version<=2)
      landcover->file.datatype=LPJ_SHORT;
    else
      landcover->file.datatype=header.datatype;
    if(header.nbands!=npft-config->nbiomass)
    {
      fclose(landcover->file.file);
      free(landcover);
      if(isroot(*config))
        fprintf(stderr,"ERROR225: Number of bands=%d is not %d\n",header.nbands,npft-config->nbiomass);
      return NULL;
    }
    landcover->file.var_len=header.nbands;
    landcover->file.size=header.ncell*header.nbands*typesizes[landcover->file.datatype];
    landcover->file.n=header.nbands*config->ngridcell;
    landcover->file.offset=(config->startgrid-header.firstcell)*header.nbands*
                           typesizes[landcover->file.datatype]+headersize(headername,version)+offset;
    len=landcover->file.n;
  }
  if((landcover->frac=newvec(Real,len))==NULL)
  {
    printallocerr("frac");
    fclose(landcover->file.file);
    free(landcover);
    return NULL;
  }
  for(i=0;i<len;i++)
    landcover->frac[i]=0;
  return landcover;
} /* of 'initpopdens' */

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
              "ERROR149: Cannot read landcover of year %d in readlandcover().\n",
              year+landcover->file.firstyear);
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
      fprintf(stderr,"ERROR184: Cannot seek to landcover fractions of year %d in readlandcover().\n", index+landcover->file.firstyear);
      return TRUE;
    }
    if(readrealvec(landcover->file.file,landcover->frac,0,landcover->file.scalar,landcover->file.n,landcover->file.swap,landcover->file.datatype))
    {
      fprintf(stderr,"ERROR184: Cannot read landcover fractions of year %d in readlandcover().\n",index+landcover->file.firstyear);
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
  if(landcover->file.fmt==CDF)
    closeclimate_netcdf(&landcover->file,isroot);
  else
    fclose(landcover->file.file);
  free(landcover->frac);
  free(landcover);
} /* of 'freelandcover' */
