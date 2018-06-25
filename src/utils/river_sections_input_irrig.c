/**************************************************************************************/
/**                                                                                \n**/
/**     r i v e r _ s e c t i o n s _i n p u t _ i r r i g . c                     \n**/
/**                                                                                \n**/
/**     returns irrigation input-data                                              \n**/
/**     search for water in one of the 8 neighboring pixel as follows:             \n**/
/**     choose neighbor pixel with the most water input from other pixels          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define NCOLS 720
#define NROWS 360
#define WIDTH 50
#define XCORNER -18000
#define YCORNER 9000
#define USAGE "Usage: river_sections_input_irrig [-longheader] [-swap] re-ordered_grid numbered_grid_file vec_drain_river.bip outfile\n"

static void sort(int feld0[],int feld1[],int feld2[],int len)
{
  int i,j,min,tmp;

  for(i=0;i<len-1;i++)
 {
    min=i;
    for(j=i+1;j<len;j++)
      if(feld0[j]<feld0[min]) 
        min=j;
    
    tmp=feld0[min];
    feld0[min]=feld0[i];
    feld0[i]=tmp;

    tmp=feld1[min];
    feld1[min]=feld1[i];
    feld1[i]=tmp;

    tmp=feld2[min];
    feld2[min]=feld2[i];
    feld2[i]=tmp;    
  }
} /* of 'sort' */

int main(int argc,char **argv)
{  
  FILE *ifp, *ifp_nr;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  int ilat,ilon,ilat_n[8],ilon_n[8];
  int i,j,k,ncell,cell;
  int swap_grid;
  int neighb[8];
  short rbuf;
  short river[NROWS][NCOLS];
  int cell_n;
  int *lon,*lat,lat_n,lon_n,count,grid_version;
  Bool swap;
  grid_version=READ_VERSION; 
  swap=FALSE;
  for(i=1;i<argc;i++)
   if(argv[i][0]=='-')
   {
     if(!strcmp(argv[1],"-longheader"))
       grid_version=2;
     else if(!strcmp(argv[1],"-swap"))
       swap=TRUE;
     else
     {
       fprintf(stderr,"Invalid option '%s'.\n"
               USAGE,argv[i]);
       return EXIT_FAILURE;
     }
   }
   else
     break;
  argc-=i-1;
  argv+=i-1;
  header.nyear=0;
  header.firstyear=0;
  header.order=0;
  header.firstcell=0;
  header.nbands=1;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.datatype=LPJ_INT;
  header.scalar=1;
/* Parse command line */
/* e.g. "river_sections_input_irrig re-ordered_grid_file numbered_grid_file vec_drain_river.bip outfile" */
  if(argc!=5)
  {
    fputs("Missing argument(s).\n"
          USAGE,stderr);
    return EXIT_FAILURE;
  }

  /* Open in- & output file */
  if((ifp=fopen(argv[1],"rb")) == NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }

  if((ifp_nr=fopen(argv[2],"r")) == NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[2],strerror(errno));
    return EXIT_FAILURE;
  }

  if((mfp=fopen(argv[3],"rb")) == NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }   

  if((ofp=fopen(argv[4],"wb")) == NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[4],strerror(errno));
    return EXIT_FAILURE;
  }

  for(i=0;i<NROWS;i++)
  {
    for(j=0;j<NCOLS;j++)
    {
      fread(&rbuf,sizeof(short),1,mfp);
      if(swap)
        rbuf=swapshort(rbuf);
      river[i][j]=rbuf;
    }
    /* printf("i:%d %d\n",i,river[i][180]); */
  }
  fclose(mfp);

  /* HEADER */
  if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,&grid_version))
  {
    fclose(ifp);
    fprintf(stderr,"Invalid header in re-ordered grid file '%s'.",argv[1]);
    return EXIT_FAILURE;
  }

  header.ncell=header_grid.ncell;
  if(header_grid.datatype!=LPJ_SHORT)
  {
    fclose(ifp);
    fprintf(stderr,"Datatype in re-ordered grid file '%s' is not short.",argv[1]);
    return EXIT_FAILURE;
  }
  /* write header */
  fwriteheader(ofp,&header,LPJNEIGHB_IRRIG_HEADER,LPJNEIGHB_IRRIG_VERSION);

  count=0;
  while(fscanf(ifp_nr,"%d",&ncell)==1)
  {
    lat=newvec(int,ncell);
    lon=newvec(int,ncell);

    for(cell=0;cell<ncell;cell++)
    {
      if(fread(&rbuf,sizeof(short),1,ifp)==1)
      {
    if(swap_grid)
          rbuf=swapshort(rbuf);
    lon[cell]=rbuf;
    fread(&rbuf,sizeof(short),1,ifp);
    if(swap_grid)
          rbuf=swapshort(rbuf);
    lat[cell]=rbuf;
	/* printf("c:%d %d %d\n",cell,lon[cell],lat[cell]); */
      }
      else
      {
    fprintf(stderr,"Error reading grid.\n");
        return EXIT_FAILURE; 
      }
    }

    for(cell=0;cell<ncell;cell++)
    {
      /* determine ilatitude and ilongitude of current position */
      ilat=(YCORNER+25-lat[cell])/WIDTH-1;
      ilon=(lon[cell]-XCORNER+25)/WIDTH-1;
      /* printf("A c:%d lon:%d %d lat:%d %d\n",cell,ilon,lon[cell],ilat,lat[cell]); */

      for(j=0;j<8;j++)
      {
    neighb[j]=0;
    ilat_n[j]=ilat;
    ilon_n[j]=ilon;
      }

      if((ilat-1)>=0 && (ilon-1)>=0 && river[ilat-1][ilon-1]>0)
      {
    neighb[0]=river[ilat-1][ilon-1];
    ilat_n[0]=ilat-1;
    ilon_n[0]=ilon-1;
      }

      if((ilat-1)>=0 && river[ilat-1][ilon]>0)
      {
    neighb[1]=river[ilat-1][ilon];
    ilat_n[1]=ilat-1;
    ilon_n[1]=ilon;
      }

      if((ilat-1)>=0 && (ilon+1)<NCOLS && river[ilat-1][ilon+1]>0)
      {
    neighb[2]=river[ilat-1][ilon+1];
    ilat_n[2]=ilat-1;
    ilon_n[2]=ilon+1;
      }
      
      if((ilon-1)>=0 && river[ilat][ilon-1]>0)
      {
    neighb[3]=river[ilat][ilon-1];
    ilat_n[3]=ilat;
    ilon_n[3]=ilon-1;
      }

      if((ilon+1)<NCOLS && river[ilat][ilon+1]>0)
      {
    neighb[4]=river[ilat][ilon+1];
    ilat_n[4]=ilat;
    ilon_n[4]=ilon+1;
      }
      
      if((ilat+1)<NROWS && (ilon-1)>=0 && river[ilat+1][ilon-1]>0)
      {
    neighb[5]=river[ilat+1][ilon-1];
    ilat_n[5]=ilat+1;
    ilon_n[5]=ilon-1;
      }

      if((ilat+1)<NROWS && river[ilat+1][ilon]>0)
      {
    neighb[6]=river[ilat+1][ilon];
    ilat_n[6]=ilat+1;
    ilon_n[6]=ilon;
      }

      if((ilat+1)<NROWS && (ilon+1)<NCOLS && river[ilat+1][ilon+1]>0)
      {
    neighb[7]=river[ilat+1][ilon+1];
    ilat_n[7]=ilat+1;
    ilon_n[7]=ilon+1;
      }
      
      sort(neighb,ilat_n,ilon_n,8);

      cell_n=-9;
      for(j=7;j>=0;j--)
      {
    lat_n=YCORNER+25-WIDTH*(ilat_n[j]+1);
    lon_n=XCORNER-25+WIDTH*(ilon_n[j]+1);
	/* printf("C c:%d lon:%d %d lat:%d %d\n",cell,ilon_n[j],lon_n,ilat_n[j],lat_n); */
    for(k=0;k<ncell;k++)
      if(lat_n==lat[k] && lon_n==lon[k])
          {
        cell_n=k;
        break;
      }
    if(cell_n>=0) break;
      }

      /* printf("c:%d c_n:%d\n",cell,cell_n); */
      if(cell_n==-9) cell_n=cell;
      cell_n+=count;

      fwrite(&cell_n,sizeof(int),1,ofp); 
    } /* of cell loop */
    free(lat);
    free(lon);
    count+=ncell;
  } /* end of while*/
  printf("processed %d cells\n",count);
  fflush(stdout);
  fclose(ifp);
  fclose(ifp_nr);
  fclose(ofp);

  return EXIT_SUCCESS;
} /* of 'main' */
