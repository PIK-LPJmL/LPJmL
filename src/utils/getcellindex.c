/**************************************************************************************/
/**                                                                                \n**/
/**                g  e  t  c  e  l  l  i  n  d  e  x  .  c                        \n**/
/**                                                                                \n**/
/**     Finds cell index in CLM grid file for given latitude/longitude             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE  "Usage: getcellindex [-search] gridfile [lat lon] ...\n"

int main(int argc,char **argv)
{
  int iarg,i,n,index;
  Bool issearch=FALSE;
  char *endptr;
  Coordfile file;
  Coord *coords,res,pos;
  float lon,lat;
  Real dist_min;
  Filename name;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-search"))
        issearch=TRUE;
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n"
                USAGE,argv[iarg]);
        return EXIT_FAILURE;
      }
    }
    else
      break;

  if(argc-iarg<1 || (argc-iarg-1) % 2)
  {
    fputs("Error: Missing arguments.\n"
          USAGE,stderr);
    return EXIT_FAILURE;
  }
  name.name=argv[iarg];
  name.fmt=CLM;
  file=opencoord(&name,TRUE);
  if(file==NULL)
    return EXIT_FAILURE;
  n=numcoord(file);
  getcellsizecoord(&lon,&lat,file);
  res.lon=lon;
  res.lat=lat;
  coords=newvec(Coord,n);
  if(coords==NULL)
  {
    printallocerr("coords");
    closecoord(file);
    return EXIT_FAILURE;
  }
  for(i=0;i<n;i++)
    if(readcoord(file,coords+i,&res))
    {
      fprintf(stderr,"Error reading grid at %d from '%s'.\n",i+1,argv[1]);
      free(coords);
      closecoord(file);
      return EXIT_FAILURE;
    }
  closecoord(file);
  if(argc==iarg+1)
  {
    puts("Lat     Lon");
    for(i=0;i<n;i++)
      printf("%10.8f %10.8f\n",coords[i].lat,coords[i].lon);
  }
  else
    for(i=iarg+1;i<argc;i+=2)
    {
      pos.lat=strtod(argv[i],&endptr);
      switch(*endptr)
      {
         case 'N': case '\0':
           break;
         case 'S':
           pos.lat= -pos.lat;
           break;
         default:
           fprintf(stderr,"Invalid argument '%s' for latitude.\n",argv[i]);
           free(coords);
           return EXIT_FAILURE;
      }
      pos.lon=strtod(argv[i+1],&endptr);
      switch(*endptr)
      {
         case 'E': case '\0':
           break;
         case 'W':
           pos.lon= -pos.lon;
           break;
         default:
           fprintf(stderr,"Invalid argument '%s' for longitude.\n",argv[i+1]);
           free(coords);
           return EXIT_FAILURE;
      }
      index=findcoord(&pos,coords,&res,n);
      if(index==NOT_FOUND)
      {
        if(issearch)
        {
          index=findnextcoord(&dist_min,&pos,coords,n);
          printf("%d: Coordinate ",index);
          printcoord(&pos);
          fputs(" mapped to ",stdout);
          printcoord(coords+index);
          printf(", distance=%g\n",dist_min);
        }
        else
          fprintf(stderr,"Coordinate not found for (%g, %g) in '%s'.\n",
                  pos.lat,pos.lon,argv[1]);
      }
      else
        printf("%d\n",index);
    }
  free(coords);
  return EXIT_SUCCESS;
} /* of 'main' */
