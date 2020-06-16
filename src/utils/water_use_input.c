/**************************************************************************************/
/**                                                                                \n**/
/**           w  a  t  e  r  _  u  s  e  _  i  n  p  u  t  .  c                    \n**/
/**                                                                                \n**/
/**     Program returns vector of cells for water use                              \n**/
/**     industrial_cons_use + domestic_cons_use + livestock_water_use              \n**/
/**     for the years 1901 to 2003 in mm downscaled to daily values                \n**/
/**     original data for industrial and domestic from 1901 to 2002                \n**/
/**     original data for livestock from 1961 to 2002                              \n**/
/**     data for 2003 equal to 2002 and data for livestock before 1961             \n**/
/**     equal to 1961                                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define NYEAR 103      /* default value for number of years */
#define FIRSTYEAR 1901 /* default value for first year */
#define FIRSTCELL 0
#define ORDER CELLYEAR
#define WIDTH 50
#define COLS 720
#define ROWS 360
#define XCORNER -18000
#define YCORNER -9000
#define SYEAR_LIVE 1961
#define NYEAR_LIVE 2002-SYEAR_LIVE+1
#define USAGE "Usage: water_use_input [-swap] [-longheader] infile_dom infile_ind infile_live re-ordered_grid outfile\n"

int main(int argc,char **argv)
{  
  FILE *ifp_dom,*ifp_ind,*ifp_live;
  FILE *ifp;
  FILE *ofp;
  Header header,header_grid;
  int ilat,ilon,i,y,ncell_grid;  
  Bool swap_grid,swap;
  int *data;
  float sum;
  int lon,lat,version; 
  float value_dom,value_ind,value_live;
  Intcoord *coords;  
  /* header informations */
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=ORDER;
  header.firstcell=FIRSTCELL;
  header.nbands=1;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.scalar=1000;
  header.datatype=LPJ_INT;
  swap=FALSE;
/* Parse command line */
/* e.g. "water_use_input input_dom input_ind input_live re-ordered_grid_file outfile" */
  version=READ_VERSION;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-swap"))
        swap=TRUE;
      if(!strcmp(argv[i],"-longheader"))
        version=2;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n" USAGE,argv[i]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  argc-=i;
  argv+=i; 
  if(argc!=5)
  {
    fprintf(stderr,"Invalid number of arguments.\n" USAGE);
    return EXIT_FAILURE;
  }
 
  /* Open in- & output file */
  if((ifp_dom=fopen(argv[0],"rb")) == NULL){
    fprintf(stderr,"Error: File open failed on infile domestic '%s': %s\n",
            argv[0],strerror(errno));
    return EXIT_FAILURE;
  } 

  if((ifp_ind=fopen(argv[1],"rb")) == NULL)
  {
    fprintf(stderr,"Error: File open failed on infile industrial '%s': %s\n",
            argv[1],strerror(errno));
    return EXIT_FAILURE;
  } 

  if((ifp_live=fopen(argv[2],"rb")) == NULL)
  {
    fprintf(stderr,"Error: File open failed on infile livestock '%s': %s\n",
            argv[2],strerror(errno));
    return EXIT_FAILURE;
  }

  if((ifp=fopen(argv[3],"rb")) == NULL)
  {
    fprintf(stderr,"Error: File open failed on re-ordered grid file '%s': %s\n",
            argv[3],strerror(errno));
    return EXIT_FAILURE;
  }

  if((ofp=fopen(argv[4],"wb")) == NULL)
  {
    fprintf(stderr,"Error: File creation failed on output file '%s': %s\n",
            argv[4],strerror(errno));
    return EXIT_FAILURE;
  }
  if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,&version)){
    fclose(ifp);
    fputs("Invalid header in re-ordered grid-file.",stderr);
    return EXIT_FAILURE;
  }
    
  ncell_grid=header_grid.ncell;
  printf("Number of cells: %d\n",ncell_grid);

  data=newvec(int,ncell_grid);
  coords=newvec(Intcoord,ncell_grid);

  for(i=0;i<ncell_grid;i++)
  {
    /* initialisation of output array */  
    data[i]=0;
    /* read input file */
    readintcoord(ifp,coords+i,swap_grid);
    /* printf("%d %d %d\n",nr[i],coords[i].lon,coords[i].lat); */
  }
  fclose(ifp);

  /* HEADER */
  header.ncell=ncell_grid;
  fwriteheader(ofp,&header,LPJWATERUSE_HEADER,LPJWATERUSE_VERSION);

  fseek(ifp_dom,0,SEEK_SET);
  fseek(ifp_ind,0,SEEK_SET);
  fseek(ifp_live,0,SEEK_SET);

  for(y=0;y<NYEAR-1;y++)
  {
    printf("year:%d\n",y);
    /* take the same values for LIVESTOCK from 1961 for the years before */
    if(y<=SYEAR_LIVE)
      fseek(ifp_live,0,SEEK_SET);

    for(ilat=ROWS-1;ilat>=0;ilat--)
    {
      for(ilon=0;ilon<COLS;ilon++)
      {
        /* printf("%d %d\n",ilat,ilon); */
        /* determine latitude and longitude of current position */
        lat=YCORNER+ilat*WIDTH+25;
        lon=XCORNER+ilon*WIDTH+25;
        /* printf("%5.2f %5.2f\n",lon,lat); */

        freadfloat(&value_dom,1,swap,ifp_dom);
        freadfloat(&value_ind,1,swap,ifp_ind);
        freadfloat(&value_live,1,swap,ifp_live);

        for(i=0;i<ncell_grid;i++)
        {
          /* comparison if current position in input-file  */
          if(lat==coords[i].lat && lon==coords[i].lon)
          {
            sum=0.0;
            if(value_dom>0)
              sum+=value_dom;
            if(value_ind>0)
              sum+=value_ind;
            if(value_live>0)
              sum+=value_live;
            /* downscaling of yearly to daily values */
            sum/=365;
            data[i]=(int)sum;
            break;
          }      /* end of if lat==lpjlat && lon==lpjlon */  
        }        /* end of for (i=0;i<NCELL;i++) */
      }          /* end of ilon-loop */
    }            /* end of ilat-loop */

    /* write binary file */
    fwrite(data,sizeof(int),ncell_grid,ofp);
    /* for(i=0;i<ncell_grid;i++) */
      /* printf("y:%d i:%d %d\n",y,i,data[i]); */
  } /* end of year-loop */

  /* take the same values from 2002 for 2003 */
  fwrite(data,sizeof(int),ncell_grid,ofp);
  for(i=0;i<ncell_grid;i++)
    printf("y:%d i:%d %d\n",y+1,i,data[i]);

  fclose(ofp);
  free(coords);
  free(data);

  fclose(ifp_dom);
  fclose(ifp_ind);
  fclose(ifp_live);

  return EXIT_SUCCESS;
} /* of 'main' */
