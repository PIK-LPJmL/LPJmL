/**************************************************************************************/
/**                                                                                \n**/
/**                      g r d 2 b s q  .  c                                       \n**/
/**                                                                                \n**/
/**     transforms LPJ-output to image of BSQ-format                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{  
  int bytel[]={sizeof(float),sizeof(short),sizeof(double),sizeof(int),sizeof(char)};
  short rbuf;
  int i;
  void *data;
  float lon;
  float lat;
  float data0;
  short data1;
  double data2;
  int data3;
  char data4;
  FILE *ifp;
  FILE *ofp;
  FILE *gfp;
  float slat, elat, slon, elon;
  int npix,nrec,nyrs,type;
  int ilat,ilon;
  float res;
  int **lw;
  int ip,ny,sy,ey;
  
  if(argc!=15){
    fprintf(stdout,"Usage:\n");
    fprintf(stdout,"grd2bsq infilename outfilename gridfilename output-start-lon output-end-lon output-start-lat output-end-lat output-start-year(1) output-end-year(103) numberpixels resolution-in-degs number-records-per-pixel number-years data-type(float=0,short=1,double=2,int=3,char=4)\n");
    fprintf(stdout,"argc is: %d\n",argc);
    puts("TEST 1");
    exit(1);
  }

  puts("1");
  
  /* PARSE COMMAND LINE */
  if((ifp=fopen(argv[1],"rb"))==NULL){
    fprintf(stderr,"Warning: File open failed on input-file.\n");
    exit(1);
  }
  if((ofp=fopen(argv[2],"wb"))==NULL){
    fprintf(stderr,"Warning: File open failed on output-file.\n");
    exit(1);
  }
  if((gfp=fopen(argv[3],"rb"))==NULL){
    fprintf(stderr,"Warning: File open failed on grid-file.\n");
    exit(1);
  }

  slon=atof(argv[4]);

  elon=atof(argv[5]);

  slat=atof(argv[6]);

  elat=atof(argv[7]);

  sy=atoi(argv[8]);

  ey=atoi(argv[9]);

  npix=atoi(argv[10]);

  res=atof(argv[11]);

  nrec=atoi(argv[12]);

  nyrs=atoi(argv[13]);

  type=atoi(argv[14]);
  if(type<0 || type>4)
  {
     fprintf(stderr,"Type should be 0 (for float),1 (for short),2 (for double),3 (for int),4 (for char).\n");
     exit(99);
  }
  data=malloc(sizeof(double));
  puts("bef alloc");   
  
  lw=(int **)calloc((int)(180./res),sizeof(int *));
  for(i=0;i<(int)(180./res);i++) 
    lw[i]=(int *)calloc((int)(360./res),sizeof(int));

  /*--------------------------------------------------*/

  for(ilat=0;ilat<(int)(180./res);ilat++)
    for(ilon=0;ilon<(int)(360./res);ilon++)
      lw[ilat][ilon]=-1.;
   
  for(ip=0;ip<npix;ip++)
  {
    fread(&rbuf,sizeof(short),1,gfp);
    lon=rbuf/100.;
    printf("lon %.2f",lon);
    fread(&rbuf,sizeof(short),1,gfp);
    lat=rbuf/100.;
    printf("lat %.2f\n",lat);

    ilon=(int)((lon+180.)/res+0.01);
    ilat=(int)((lat+90.)/res+0.01);

    lw[ilat][ilon]=ip;
  }
  fclose(gfp);

  ny=(ey-sy)+1;

  for(i=0;i<ny*nrec;i++)
  {
    if(i%nrec==0)
      printf("%d\n",i+nrec);
    /* Reverse latitude order for easy displaying */
    for(ilat=(int)(180./res)-1;ilat>=0;ilat--)
    {
      for(ilon=0;ilon<(int)(360./res);ilon++)
      {
    
    lon=ilon*res-180.+res/2.;
    lat=ilat*res-90.+res/2.;

    if(lon>=slon && lon<=elon && lat>=slat && lat<=elat)
        {
      ip=lw[ilat][ilon];
      if(ip!=-1)
          {  /* data present */
        fseek(ifp,ip*bytel[type]+i*bytel[type]*npix+(sy-1)*bytel[type]*nrec*npix,SEEK_SET);
            fread(data,bytel[type],1,ifp);
            fwrite(data,bytel[type],1,ofp);
      }
      else
          { /* ocean or lake or fill */
            switch(type)
            {
              case 0:
            data0=-9;
            fwrite(&data0,sizeof(float),1,ofp); 
                break;
              case 1:
            data1=-9;
            fwrite(&data1,sizeof(short),1,ofp); 
                break;
              case 2:
            data2=-9;
            fwrite(&data2,sizeof(double),1,ofp); 
                break;
              case 3:
            data3=-9;
            fwrite(&data3,sizeof(int),1,ofp); 
                break;
              case 4:
            data4=-9;
            fwrite(&data4,sizeof(char),1,ofp); 
                break;
            }
      }
	} /* end: if in window */
      } /* end: lat loop over global grid */
    } /* end: lon loop over global grid */
  } /* end of for(...) */

  fclose(ifp);
  fclose(ofp);

  return EXIT_SUCCESS;
} /* of 'main' */
