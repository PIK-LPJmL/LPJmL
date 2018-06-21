/**************************************************************************************/
/**                                                                                \n**/
/**                o  p  e  n  _  n  e  t  c  d  f  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens NetCDF files for identical filenames only once              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>

typedef struct
{
  char *filename;
  int ncid;
  int count;
} Item;

static List *list=NULL;
#endif

int open_netcdf(const char *filename,int *ncid,Bool *isopen)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i,rc;
  Item *item;
  if(list==NULL)
    list=newlist();
  else
  {
    for(i=0;i<getlistlen(list);i++)
    {
      item=(Item *)getlistitem(list,i);
      if(!strcmp(item->filename,filename)) /* file already open? */
      {
        *ncid=item->ncid; /* yes, copy id */
        *isopen=TRUE;
        item->count++;
        return 0;
      }
    }
  }
  *isopen=FALSE;
  /* file not open now, open it */
  rc=nc_open(filename,NC_NOWRITE,ncid);
  if(rc)
    return rc;
  /* add open file to list */
  item=new(Item);
  if(item==NULL)
    return 1;
  item->filename=strdup(filename);
  if(item->filename==NULL)
  {
    free(item);
    return 1;
  }
  item->ncid=*ncid;
  item->count=1;
  addlistitem(list,item);
  return 0;
#else
  return 1;
#endif
} /* of 'open_netcdf' */

void free_netcdf(int ncid)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i;
  Item *item;
  if(list==NULL)
    nc_close(ncid);
  else
    for(i=0;i<getlistlen(list);i++)
    {
      item=(Item *)getlistitem(list,i);
      if(ncid==item->ncid)
      {
        item->count--;
        if(item->count==0) /* last open file? */
        {
          free(item->filename);
          nc_close(ncid);
          dellistitem(list,i);
        }
        break;
     }
   }
#endif
} /* of 'free_netcdf' */
