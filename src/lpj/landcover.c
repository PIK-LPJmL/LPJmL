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
  int size;
};

Landcover initlandcover(int npft,            /**< number of natural PFTs */
                        const Config *config /**< LPJmL configuration */
                       )                     /** \return landcover data or NULL */
{
  Landcover landcover;
  int i;
  
  landcover=new(struct landcover);
  if(landcover==NULL)
  {
    printallocerr("landcover");
    return NULL;
  }
  if(opendata(&landcover->file,&config->landcover_filename,"landcover","1",LPJ_SHORT,0.01,config->landcovermap_size,TRUE,config))
  {
    free(landcover);
    return NULL;
  }
  landcover->size=config->ngridcell*getnnat(npft,config);
  if((landcover->frac=newvec(Real,landcover->size))==NULL)
  {
    printallocerr("frac");
    closeclimatefile(&landcover->file,isroot(*config));
    free(landcover);
    return NULL;
  }
  for(i=0;i<landcover->size;i++)
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
  Real *data;
  int i,j,count;
  for(i=0;i<landcover->size;i++)
    landcover->frac[i]=0;
  data=readdata(&landcover->file,NULL,grid,"landcover",year,config);
  if(data==NULL)
    return TRUE;
  count=0;
  for(i=0;i<config->ngridcell;i++)
    for(j=0;j<config->landcovermap_size;j++)
    {
      if(data[count]!=NOT_FOUND)
        landcover->frac[i*landcover->size/config->ngridcell+config->landcovermap[j]]+=data[count];
      count++;
    }
  free(data);
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
