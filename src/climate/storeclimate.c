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
  int year,j,index;
  long count;
  /**
  * allocate arrays for climate storage
  **/
  if (config->isanomaly)
    index = (config->delta_year>1) ? 3 : 1;
  else
    index = 0;
  store->temp=newvec(Real,climate->file_temp.n*nyear);
  checkptr(store->temp);
  store->prec=newvec(Real,climate->file_prec.n*nyear);
  checkptr(store->prec);
  if(climate->data[0].tmax!=NULL)
  {
    store->tmax=newvec(Real,climate->file_tmax.n*nyear);
    checkptr(store->tmax);
  }
  else
    store->tmax=NULL;
  if(climate->data[0].humid!=NULL)
  {
    store->humid=newvec(Real,climate->file_humid.n*nyear);
    checkptr(store->humid);
  }
  else
    store->humid=NULL;
  if(climate->data[0].tmin!=NULL)
  {
    store->tmin=newvec(Real,climate->file_tmin.n*nyear);
    checkptr(store->tmin);
  }
  else
    store->tmin=NULL;
  if(climate->data[0].sun!=NULL)
  {
    store->sun=newvec(Real,climate->file_cloud.n*nyear);
    checkptr(store->sun);
  }
  else
    store->sun=NULL;
  if(climate->data[0].lwnet!=NULL)
  {
    store->lwnet=newvec(Real,climate->file_lwnet.n*nyear);
    checkptr(store->lwnet);
  }
  else
    store->lwnet=NULL;
  if(climate->data[0].swdown!=NULL)
  {
    store->swdown=newvec(Real,climate->file_swdown.n*nyear);
    checkptr(store->swdown);
  }
  else
    store->swdown=NULL;
  if(climate->data[0].wet!=NULL)
  {
    store->wet=newvec(Real,climate->file_wet.n*nyear);
    checkptr(store->wet);
  }
  else
    store->wet=NULL;
  if(climate->data[0].wind!=NULL)
  {
    store->wind=newvec(Real,climate->file_wind.n*nyear);
    checkptr(store->wind);
  }
  else
    store->wind=NULL;
  if(climate->data[0].tamp!=NULL)
  {
    store->tamp=newvec(Real,climate->file_tamp.n*nyear);
    checkptr(store->tamp);
  }
  else
    store->tamp=NULL;
  if(climate->data[0].burntarea!=NULL)
  {
    store->burntarea=newvec(Real,climate->file_burntarea.n*nyear);
    checkptr(store->burntarea);
  }
  else
    store->burntarea=NULL;
  if(climate->data[0].no3deposition!=NULL)
  {
    store->no3deposition=newvec(Real,climate->file_no3deposition.n*nyear);
    checkptr(store->no3deposition);
  }
  else
    store->no3deposition=NULL;
  if(climate->data[0].nh4deposition!=NULL)
  {
    store->nh4deposition=newvec(Real,climate->file_nh4deposition.n*nyear);
    checkptr(store->nh4deposition);
  }
  else
    store->nh4deposition=NULL;
  if(climate->data[0].lightning!=NULL)
  {
    store->lightning=newvec(Real,climate->file_lightning.n);
    checkptr(store->lightning);
    for(j=0;j<climate->file_lightning.n;j++)
      store->lightning[j]=climate->data[0].lightning[j];
  }
  else
    store->lightning=NULL;
  for(year=firstyear;year<firstyear+nyear;year++)
  {
    if(getclimate(climate,grid,index,year,config))
      return TRUE;
    count=climate->file_temp.n*(year-firstyear);
    for(j=0;j<climate->file_temp.n;j++)
      store->temp[count++]=climate->data[index].temp[j];
    count=climate->file_prec.n*(year-firstyear);
    for(j=0;j<climate->file_prec.n;j++)
      store->prec[count++]=climate->data[index].prec[j];
    if(store->sun!=NULL)
    {
      count=climate->file_cloud.n*(year-firstyear);
      for(j=0;j<climate->file_cloud.n;j++)
        store->sun[count++]=climate->data[index].sun[j];
    }
    if(store->tmax!=NULL)
    {
      count=climate->file_tmax.n*(year-firstyear);
      for(j=0;j<climate->file_tmax.n;j++)
        store->tmax[count++]=climate->data[index].tmax[j];
    }
    if(store->humid!=NULL)
    {
      count=climate->file_humid.n*(year-firstyear);
      for(j=0;j<climate->file_humid.n;j++)
        store->humid[count++]=climate->data[index].humid[j];
    }
    if(store->tmin!=NULL)
    {
      count=climate->file_tmin.n*(year-firstyear);
      for(j=0;j<climate->file_tmin.n;j++)
        store->tmin[count++]=climate->data[index].tmin[j];
    }
    if(store->lwnet!=NULL)
    {
      count=climate->file_lwnet.n*(year-firstyear);
      for(j=0;j<climate->file_lwnet.n;j++)
        store->lwnet[count++]=climate->data[index].lwnet[j];
    }
    if(store->swdown!=NULL)
    {
      count=climate->file_swdown.n*(year-firstyear);
      for(j=0;j<climate->file_swdown.n;j++)
        store->swdown[count++]=climate->data[index].swdown[j];
    }
    if(store->wet!=NULL)
    {
      count=climate->file_wet.n*(year-firstyear);
      for(j=0;j<climate->file_wet.n;j++)
        store->wet[count++]=climate->data[index].wet[j];
    }
    if(store->wind!=NULL)
    {
      count=climate->file_wind.n*(year-firstyear);
      for(j=0;j<climate->file_wind.n;j++)
        store->wind[count++]=climate->data[index].wind[j];
    }
    if(store->tamp!=NULL)
    {
      count=climate->file_tamp.n*(year-firstyear);
      for(j=0;j<climate->file_tamp.n;j++)
        store->tamp[count++]=climate->data[index].tamp[j];
    }
    if(store->burntarea!=NULL)
    {
      count=climate->file_burntarea.n*(year-firstyear);
      for(j=0;j<climate->file_burntarea.n;j++)
        store->burntarea[count++]=climate->data[index].burntarea[j];
    }
    if(store->no3deposition!=NULL)
    {
      count=climate->file_no3deposition.n*(year-firstyear);
      for(j=0;j<climate->file_no3deposition.n;j++)
        store->no3deposition[count++]=climate->data[index].no3deposition[j];
    }
    if(store->nh4deposition!=NULL)
    {
      count=climate->file_nh4deposition.n*(year-firstyear);
      for(j=0;j<climate->file_nh4deposition.n;j++)
        store->nh4deposition[count++]=climate->data[index].nh4deposition[j];
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
    climate->data[0].temp[i]=store->temp[index++];
  index=year*climate->file_prec.n;
  for(i=0;i<climate->file_prec.n;i++)
    climate->data[0].prec[i]=store->prec[index++];
  if(store->tmax!=NULL)
  {
    index=year*climate->file_tmax.n;
    for(i=0;i<climate->file_tmax.n;i++)
      climate->data[0].tmax[i]=store->tmax[index++];
  }
  if(store->humid!=NULL)
  {
    index=year*climate->file_humid.n;
    for(i=0;i<climate->file_humid.n;i++)
      climate->data[0].humid[i]=store->humid[index++];
  }
  if(store->tmin!=NULL)
  {
    index=year*climate->file_tmin.n;
    for(i=0;i<climate->file_tmin.n;i++)
      climate->data[0].tmin[i]=store->tmin[index++];
  }
  if(store->sun!=NULL)
  {
    index=year*climate->file_cloud.n;
    for(i=0;i<climate->file_cloud.n;i++)
      climate->data[0].sun[i]=store->sun[index++];
  }
  if(store->lwnet!=NULL)
  {
    index=year*climate->file_lwnet.n;
    for(i=0;i<climate->file_lwnet.n;i++)
      climate->data[0].lwnet[i]=store->lwnet[index++];
  }
  if(store->swdown!=NULL)
  {
    index=year*climate->file_swdown.n;
    for(i=0;i<climate->file_swdown.n;i++)
      climate->data[0].swdown[i]=store->swdown[index++];
  }
  if(store->wet!=NULL)
  {
    index=year*climate->file_wet.n;
    for(i=0;i<climate->file_wet.n;i++)
      climate->data[0].wet[i]=store->wet[index++];
  }
  if(store->wind!=NULL)
  {
    index=year*climate->file_wind.n;
    for(i=0;i<climate->file_wind.n;i++)
      climate->data[0].wind[i]=store->wind[index++];
  }
  if(store->tamp!=NULL)
  {
    index=year*climate->file_tamp.n;
    for(i=0;i<climate->file_tamp.n;i++)
      climate->data[0].tamp[i]=store->tamp[index++];
  }
  if(store->burntarea!=NULL)
  {
    index=year*climate->file_burntarea.n;
    for(i=0;i<climate->file_burntarea.n;i++)
      climate->data[0].burntarea[i]=store->burntarea[index++];
  }
  if(store->no3deposition!=NULL)
  {
    index=year*climate->file_no3deposition.n;
    for(i=0;i<climate->file_no3deposition.n;i++)
      climate->data[0].no3deposition[i]=store->no3deposition[index++];
  }
  if(store->nh4deposition!=NULL)
  {
    index=year*climate->file_nh4deposition.n;
    for(i=0;i<climate->file_nh4deposition.n;i++)
      climate->data[0].nh4deposition[i]=store->nh4deposition[index++];
  }
} /* of 'restoreclimate' */

void moveclimate(Climate *climate,  /**< Pointer to climate data */
                 const Climatedata *store, /**< climate buffer */
                 int index,
                 int year           /**< year (AD) */
                 )                  /** \return void */
{
  climate->data[index].prec=store->prec+climate->file_prec.n*year;
  climate->data[index].temp=store->temp+climate->file_temp.n*year;
  if(climate->data[index].tmax!=NULL)
    climate->data[index].tmax=store->tmax+climate->file_tmax.n*year;
  if(climate->data[index].tmin!=NULL)
    climate->data[index].tmin=store->tmin+climate->file_tmin.n*year;
  if(climate->data[index].humid!=NULL)
    climate->data[index].humid=store->humid+climate->file_humid.n*year;
  if(climate->data[index].sun!=NULL)
    climate->data[index].sun=store->sun+climate->file_cloud.n*year;
  if(climate->data[index].lwnet!=NULL)
    climate->data[index].lwnet=store->lwnet+climate->file_lwnet.n*year;
  if(climate->data[index].swdown!=NULL)
    climate->data[index].swdown=store->swdown+climate->file_swdown.n*year;
  if(climate->data[0].wet!=NULL)
    climate->data[index].wet=store->wet+climate->file_wet.n*year;
  if(climate->data[0].wind!=NULL)
    climate->data[index].wind=store->wind+climate->file_wind.n*year;
  if(climate->data[0].tamp!=NULL)
    climate->data[index].tamp=store->tamp+climate->file_tamp.n*year;
  if(climate->data[0].burntarea!=NULL)
    climate->data[index].burntarea=store->burntarea+climate->file_burntarea.n*year;
  if(climate->data[0].no3deposition!=NULL)
    climate->data[index].no3deposition=store->no3deposition+climate->file_no3deposition.n*year;
  if(climate->data[0].nh4deposition!=NULL)
    climate->data[index].nh4deposition=store->nh4deposition+climate->file_nh4deposition.n*year;
} /* of 'moveclimate' */
