/**************************************************************************************/
/**                                                                                \n**/
/**                 d  r  a  i  n  a  g  e  .  c                                   \n**/
/**                                                                                \n**/
/**     Program returns vector of cells for drainage                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define NYEAR 1        /* default value for number of years */
#define FIRSTYEAR 0    /* default value for first year */
#define FIRSTCELL 0
#define ORDER CELLYEAR

typedef struct
{
  int lon,lat;
  Routing r;
} Drain;

/* Calculate distance (in m) between two points specified by latitude/longitude */
static double distHaversine(double lat1,
                            double lat2,
                            double lon1,
                            double lon2) 
{  /* Haversine version */
  double R = 6371000; /* earth's mean radius in m */
  double dLat  = sin((lat2-lat1)/2);
  double dLong = sin((lon2-lon1)/2);
  double a, b1, b2, c;
  double d;

  lat1 = cos(lat1);
  lat2 = cos(lat2);
  a = dLat * dLat + lat1 * lat2 * dLong * dLong;
  b1 = sqrt(a);
  b2 = sqrt(1-a);
  c = atan2(b1, b2);
  d = R * 2 * c;

  return d;
}

int main(int argc,char **argv)
{  
  FILE *ifp;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  int nodata;
  int cols, rows;
  float xcorner, ycorner;
  int ilat, ilon, i, j, c1, c2, c3, c4;  
  int value;
  Bool swap;
  Intcoord coord;
  Drain *drain;
  int *nr;
  float rbuf2;
  int width;
  int lon, lat, lonnew, latnew,version;
  version=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    version=2;
    argc--;
    argv++;
  } 
  /* header informations */
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=ORDER;
  header.firstcell=FIRSTCELL;
  header.nbands=2;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.datatype=LPJ_INT;
  header.scalar=1;

  /* Parse command line */
  /* e.g. "drainage re-ordered_grid DDM30.asc output" */
  if(argc!=4)
  {
    fprintf(stderr,"Missing argument(s).\n"
                   "Usage: drainage [-longheader] re-ordered_grid DDM30.asc outfile\n");
    return EXIT_FAILURE;
  }
 
  /* Open in- & output file */
  if((ifp=fopen(argv[1],"rb")) == NULL)
  {
    fprintf(stderr,"Error: File open failed on re-ordered grid file '%s': %s\n",
            argv[1],strerror(errno));
    return EXIT_FAILURE;
  }

  if((ofp=fopen(argv[3],"wb")) == NULL)
  {
    fprintf(stderr,"Error: File creation failed on output file '%s': %s\n",
            argv[3],strerror(errno));
    return EXIT_FAILURE;
  }

  if((mfp=fopen(argv[2],"r")) == NULL)
  {
    fprintf(stderr,"Error: File open failed on mask-file '%s': %s\n",
            argv[2],strerror(errno));
    return EXIT_FAILURE;
  } 
  /* read lines of mask file */
  if(fscanf(mfp,"%*s %d",&cols)!=1)
  {
    fprintf(stderr,"Error reading cols in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  /* printf("cols=%d\n",cols); */
  if(fscanf(mfp,"%*s %d",&rows)!=1)
  {
    fprintf(stderr,"Error reading rows in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  /* printf("rows=%d\n",rows); */
  if(fscanf(mfp,"%*s %f",&xcorner)!=1)
  {
    fprintf(stderr,"Error reading xcorner in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  if(fscanf(mfp,"%*s %f",&ycorner)!=1)
  {
    fprintf(stderr,"Error reading ycorner in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  if(fscanf(mfp,"%*s %f",&rbuf2)!=1)
  {
    fprintf(stderr,"Error reading cellsize in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  width=(int)(rbuf2*100);
  /* printf("cellsize=%d\n",width); */
  fscanf(mfp,"%*s %d",&nodata);
  /*printf("found cols %d rows %d xcorner %f ycorner %f rbuf2 %f nodata %d\n",
   *      cols, rows, xcorner, ycorner, rbuf2, nodata);
   */
  
  if(freadheader(ifp,&header_grid,&swap,LPJGRID_HEADER,&version))
  {
    fclose(ifp);
    fprintf(stderr,"Error reading header in grid file '%s.\n",argv[1]);
    return EXIT_FAILURE;
  }

  nr=newvec(int,header_grid.ncell);
  if(nr==NULL)
  {
    printallocerr("nr");
    return EXIT_FAILURE;
  }
  drain=newvec(Drain,header_grid.ncell);
  if(drain==NULL)
  {
    printallocerr("drain");
    return EXIT_FAILURE;
  }
  for(i=0;i<header_grid.ncell;i++)
  {
    /* initialisation of output array */  
    drain[i].r.index=-1;
    drain[i].r.len=0;
    nr[i]=i;
    /* read input file and make array for longitude and latitude */
    readintcoord(ifp,&coord,swap);
    drain[i].lon=coord.lon;
    drain[i].lat=coord.lat;
    /* printf("%d %d %d\n",nr[i],lpjlon[i],lpjlat[i]); */
  }
  fclose(ifp);

  c1=c2=c3=c4=0;
  for(ilat=rows-1;ilat>=0;ilat--)
  {
    /* printf("ilon= %d\n",ilon); */
    for(ilon=0;ilon<cols;ilon++)
    {
      /* read discrete values of mask file */
      if(fscanf(mfp,"%d",&value)!=1)
      {
        fprintf(stderr,"Error reading data in '%s' at (%d,%d).\n",argv[2],ilon,ilat);
        return EXIT_FAILURE;
      }
      /* printf("%d %d %d\n",ilat,ilon,value); */
      c1++;
      /* determine latitude and longitude of current position */
      lat=(int)(ycorner*100)+ilat*width+25;
      lon=(int)(xcorner*100)+ilon*width+25;
      /* printf("%d %d %d\n",c1,lon,lat); */
      
      for (i=0;i<header_grid.ncell;i++)
      {
        /* comparison if current position in input-file  */
        if (lat==drain[i].lat && lon==drain[i].lon) 
        {
          if(value==-9) 
            drain[i].r.index=-9;
          if(value==0)
          { 
            drain[i].r.index=-1;
            c4++;
            /* determine the distance between the two points*/
            drain[i].r.len=25000; 
            /* printf("%ld\n",d[i]); */
          }
  
          c2++;
          /* printf("%d %5.2f %5.2f\n",c2,lon,lat); */
          /* only values > 0 of interesting  */
          if(value>0)
          { 
            if(value<=8) 
            {
              if(value<=2)
              {
                if(value==1)
                {
                  latnew=lat;
                  lonnew=lon+width;
                  c3++;
                }
                else
                {
                  latnew=lat-width;
                  lonnew=lon+width;
                  c3++;
                }
              }
              else
              {
                if(value==4)
                {
                  latnew=lat-width;
                  lonnew=lon;
                  c3++;                  
                }
                else
                {
                  latnew=lat-width;
                  lonnew=lon-width;
                  c3++;                 
                }
              }
            }
            else
            {
              if(value<=32)
              {
                if(value==16) 
                {
                  latnew=lat;
                  lonnew=lon-width;
                  c3++;                  
                }
                else
                {
                  latnew=lat+width;
                  lonnew=lon-width;
                  c3++;          
                }
              }
              else
              {
                if(value==64) 
                {
                  latnew=lat+width;
                  lonnew=lon;
                  c3++;                
                }
                else
                {
                  latnew=lat+width;
                  lonnew=lon+width;
                  c3++;                 
                }   
              }
            }
    
            for(j=0;j<header_grid.ncell;j++) 
            {
              if(latnew==drain[j].lat && lonnew==drain[j].lon)
              {
                drain[i].r.index=nr[j];       
                /* determine the distance between the two points*/
                drain[i].r.len=(int)distHaversine((double)lat/100.*M_PI/180,(double)latnew/100.*M_PI/180,(double)lon/100.*M_PI/180,(double)lonnew/100.*M_PI/180);
                /* printf("%f\n",d[i]); */
              }
            }
          }    /* end of if value>0 */
        }      /* end of if lat==lpjlat && lon==lpjlon */  
      }        /* end of for (i=0;i<NCELL;i++) */
    }          /* end of ilon-loop */
  }            /* end of ilat-loop */
  fclose(mfp);
  printf("c1= %d c2= %d c3= %d c4= %d\n",c1,c2,c3,c4); 

  /* HEADER */
  header.ncell=header_grid.ncell;
  fwriteheader(ofp,&header,LPJDRAIN_HEADER,LPJDRAIN_VERSION);

  /* write data on the screen  */
  for(i=0;i<header.ncell;i++) 
  {
    printf("d=%d l=%d\n", drain[i].r.index,drain[i].r.len);
    /* write binary file */
    fwrite(&drain[i].r,sizeof(Routing),1,ofp);
  }
  fclose(ofp);
  free(nr);
  free(drain);

  return EXIT_SUCCESS;
} /* of 'main' */
