/**************************************************************************************/
/**                                                                                \n**/
/**                 s  t  o  r  e  c  l  i  m  a  t  e  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) {printallocerr(#ptr); return TRUE; }

Bool storeclimate(Climatedata *store,  /**< pointer to climate data to be stored */
                  Climate *climate,    /**< climate pointer data is read */
                  const Cell grid[],   /**< LPJ grid */
                  int firstyear,       /**< first year of climate to be read */
                  int nyear,           /**< number of years to be read */
                  const Config *config /**< LPJ configuration */
                 )                     /** \return TRUE on error */
{
  int year,j;
  long count;
  /**
  * allocate arrays for climate storage
  **/
  store->temp=newvec(Real,climate->file_temp.n*nyear);
  checkptr(store->temp);
  store->prec=newvec(Real,climate->file_prec.n*nyear);
  checkptr(store->prec);
  if(climate->data.tmax!=NULL)
  {
    store->tmax=newvec(Real,climate->file_tmax.n*nyear);
    checkptr(store->tmax);
  }
  else
    store->tmax=NULL;
  if(climate->data.humid!=NULL)
  {
    store->humid=newvec(Real,climate->file_humid.n*nyear);
    checkptr(store->humid);
  }
  else
    store->humid=NULL;
  if(climate->data.sun!=NULL)
  {
    store->sun=newvec(Real,climate->file_cloud.n*nyear);
    checkptr(store->sun);
  }
  else
    store->sun=NULL;
  if(climate->data.lwnet!=NULL)
  {
    store->lwnet=newvec(Real,climate->file_lwnet.n*nyear);
    checkptr(store->lwnet);
  }
  else
    store->lwnet=NULL;
  if(climate->data.swdown!=NULL)
  {
    store->swdown=newvec(Real,climate->file_swdown.n*nyear);
    checkptr(store->swdown);
  }
  else
    store->swdown=NULL;
  if(climate->data.wet!=NULL)
  {
    store->wet=newvec(Real,climate->file_wet.n*nyear);
    checkptr(store->wet);
  }
  else
    store->wet=NULL;
  if(climate->data.wind!=NULL)
  {
    store->wind=newvec(Real,climate->file_wind.n*nyear);
    checkptr(store->wind);
  }
  else
    store->wind=NULL;
  if(climate->data.tamp!=NULL)
  {
    store->tamp=newvec(Real,climate->file_tamp.n*nyear);
    checkptr(store->tamp);
  }
  else
    store->tamp=NULL;
  if(climate->data.burntarea!=NULL)
  {
    store->burntarea=newvec(Real,climate->file_burntarea.n*nyear);
    checkptr(store->burntarea);
  }
  else
    store->burntarea=NULL;
  if(climate->data.lightning!=NULL)
  {
    store->lightning=newvec(Real,climate->file_lightning.n);
    checkptr(store->lightning);
    for(j=0;j<climate->file_lightning.n;j++)
      store->lightning[j]=climate->data.lightning[j];
  }
  else
    store->lightning=NULL;
  for(year=firstyear;year<firstyear+nyear;year++)
  {
    if(getclimate(climate,grid,year,config))
      return TRUE;
    count=climate->file_temp.n*(year-firstyear);
    for(j=0;j<climate->file_temp.n;j++)
      store->temp[count++]=climate->data.temp[j];
    count=climate->file_prec.n*(year-firstyear);
    for(j=0;j<climate->file_prec.n;j++)
      store->prec[count++]=climate->data.prec[j];
    if(store->sun!=NULL)
    {
      count=climate->file_cloud.n*(year-firstyear);
      for(j=0;j<climate->file_cloud.n;j++)
        store->sun[count++]=climate->data.sun[j];
    }
    if(store->tmax!=NULL)
    {
      count=climate->file_tmax.n*(year-firstyear);
      for(j=0;j<climate->file_tmax.n;j++)
        store->tmax[count++]=climate->data.tmax[j];
    }
    if(store->humid!=NULL)
    {
      count=climate->file_humid.n*(year-firstyear);
      for(j=0;j<climate->file_humid.n;j++)
        store->humid[count++]=climate->data.humid[j];
    }
    if(store->lwnet!=NULL)
    {
      count=climate->file_lwnet.n*(year-firstyear);
      for(j=0;j<climate->file_lwnet.n;j++)
        store->lwnet[count++]=climate->data.lwnet[j];
    }
    if(store->swdown!=NULL)
    {
      count=climate->file_swdown.n*(year-firstyear);
      for(j=0;j<climate->file_swdown.n;j++)
        store->swdown[count++]=climate->data.swdown[j];
    }
    if(store->wet!=NULL)
    {
      count=climate->file_wet.n*(year-firstyear);
      for(j=0;j<climate->file_wet.n;j++)
        store->wet[count++]=climate->data.wet[j];
    }
    if(store->wind!=NULL)
    {
      count=climate->file_wind.n*(year-firstyear);
      for(j=0;j<climate->file_wind.n;j++)
        store->wind[count++]=climate->data.wind[j];
    }
    if(store->tamp!=NULL)
    {
      count=climate->file_tamp.n*(year-firstyear);
      for(j=0;j<climate->file_tamp.n;j++)
        store->tamp[count++]=climate->data.tamp[j];
    }
    if(store->burntarea!=NULL)
    {
      count=climate->file_burntarea.n*(year-firstyear);
      for(j=0;j<climate->file_burntarea.n;j++)
        store->burntarea[count++]=climate->data.burntarea[j];
    }
  }
  return FALSE;
} /* of 'storeclimate' */

void restoreclimate(Climate *climate,         /**< pointer to climate data */
                    const Climatedata *store, /**< pointer to climate data */
                    int year                  /**< year (AD) */
                    )                         /** \return void*/
{
  int i;
  long index;
  index=year*climate->file_temp.n;
  for(i=0;i<climate->file_temp.n;i++)
    climate->data.temp[i]=store->temp[index++];
  index=year*climate->file_prec.n;
  for(i=0;i<climate->file_prec.n;i++)
    climate->data.prec[i]=store->prec[index++];
  if(store->tmax!=NULL)
  {
    index=year*climate->file_tmax.n;
    for(i=0;i<climate->file_tmax.n;i++)
      climate->data.tmax[i]=store->tmax[index++];
  }
  if(store->humid!=NULL)
  {
    index=year*climate->file_humid.n;
    for(i=0;i<climate->file_humid.n;i++)
      climate->data.humid[i]=store->humid[index++];
  }
  if(store->sun!=NULL)
  {
    index=year*climate->file_cloud.n;
    for(i=0;i<climate->file_cloud.n;i++)
      climate->data.sun[i]=store->sun[index++];
  }
  if(store->lwnet!=NULL)
  {
    index=year*climate->file_lwnet.n;
    for(i=0;i<climate->file_lwnet.n;i++)
      climate->data.lwnet[i]=store->lwnet[index++];
  }
  if(store->swdown!=NULL)
  {
    index=year*climate->file_swdown.n;
    for(i=0;i<climate->file_swdown.n;i++)
      climate->data.swdown[i]=store->swdown[index++];
  }
  if(store->wet!=NULL)
  {
    index=year*climate->file_wet.n;
    for(i=0;i<climate->file_wet.n;i++)
      climate->data.wet[i]=store->wet[index++];
  }
  if(store->wind!=NULL)
  {
    index=year*climate->file_wind.n;
    for(i=0;i<climate->file_wind.n;i++)
      climate->data.wind[i]=store->wind[index++];
  }
  if(store->tamp!=NULL)
  {
    index=year*climate->file_tamp.n;
    for(i=0;i<climate->file_tamp.n;i++)
      climate->data.tamp[i]=store->tamp[index++];
  }
  if(store->burntarea!=NULL)
  {
    index=year*climate->file_burntarea.n;
    for(i=0;i<climate->file_burntarea.n;i++)
      climate->data.burntarea[i]=store->burntarea[index++];
  }
} /* of 'restoreclimate' */

void moveclimate(Climate *climate,  /**< Pointer to climate data */
                 const Climatedata *store, /**< climate buffer */
                 int year           /**< year (AD) */
                 )                  /** \return void */
{
  climate->data.prec=store->prec+climate->file_prec.n*year;
  climate->data.temp=store->temp+climate->file_temp.n*year;
  if(climate->data.tmax!=NULL)
    climate->data.tmax=store->tmax+climate->file_tmax.n*year;
  if(climate->data.humid!=NULL)
    climate->data.humid=store->humid+climate->file_humid.n*year;
  if(climate->data.sun!=NULL)
    climate->data.sun=store->sun+climate->file_cloud.n*year;
  if(climate->data.lwnet!=NULL)
    climate->data.lwnet=store->lwnet+climate->file_lwnet.n*year;
  if(climate->data.swdown!=NULL)
    climate->data.swdown=store->swdown+climate->file_swdown.n*year;
  if(climate->data.wet!=NULL)
    climate->data.wet=store->wet+climate->file_wet.n*year;
  if(climate->data.wind!=NULL)
    climate->data.wind=store->wind+climate->file_wind.n*year;
  if(climate->data.tamp!=NULL)
    climate->data.tamp=store->tamp+climate->file_tamp.n*year;
  if(climate->data.burntarea!=NULL)
    climate->data.burntarea=store->burntarea+climate->file_burntarea.n*year;
} /* of 'moveclimate' */
