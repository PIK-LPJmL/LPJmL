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

#define USAGE  "Usage: getcellindex gridfile [lat lon] ...\n"

int main(int argc,char **argv)
{ 
  int i,n,index;
  char *endptr;
  Coordfile file;
  Coord *coords,res,pos;
  float lon,lat;
  Filename name; 
  if(argc<2 || (argc-2) % 2)
  {
    fputs("Missing arguments.\n"
          USAGE,stderr);
    return EXIT_FAILURE;
  }
  name.name=argv[1];
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
    return EXIT_FAILURE;
  }
  for(i=0;i<n;i++)
    if(readcoord(file,coords+i,&res))
    {
      fprintf(stderr,"Error reading grid at %d from '%s'.\n",i+1,argv[1]);
      return EXIT_FAILURE;
    }
  closecoord(file);
  if(argc==2)
  {
    puts("Lat     Lon");
    for(i=0;i<n;i++)
      printf("%10.8f %10.8f\n",coords[i].lat,coords[i].lon);
  }
  else
    for(i=2;i<argc;i+=2)
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
           return EXIT_FAILURE;
      }
      index=findcoord(&pos,coords,n);
      if(index==NOT_FOUND)
        fprintf(stderr,"Coordinate not found for (%g, %g) in '%s'.\n",
                pos.lat,pos.lon,argv[1]);
      else
        printf("%d\n",index);
    }
  return EXIT_SUCCESS;
} /* of 'main' */
