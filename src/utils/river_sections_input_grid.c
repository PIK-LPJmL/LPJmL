/**************************************************************************************/
/**                                                                                \n**/
/**     r i v e r _ s e c t i o n s _i n p u t _ g r i d . c                       \n**/
/**                                                                                \n**/
/**     Re-orders grid cells according to river sections                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#undef USE_MPI

#include "lpj.h"

#define SWAP TRUE
#define LEN_MAX 7000     /* maximal length of the data-set */

int main(int argc,char **argv)
{
  FILE *ifp1,*ifp2;
  FILE *ofp,*ofp_ngrid;
  Header header;
  int i, j, k, l, c, c0;
  int ncell;
  int *rivers;
  int value;
  int swap_grid;
  double *lpjlon,*lpjlat;
  int *riversnew;
  int *sum;
  int version;
  short rbuf2[2];
  version=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    version=2;
    argc--;
    argv++;
  }
/* Parse command line */
/* e.g. "river_sections_input_grid lpjgrid_lonlat.clm riversections.bin output ngrid.txt" */
  if(argc!=5)
  {
    fprintf(stdout,"USAGE: river_sections_input_grid [-longheader] lpjgrid_lonlat.clm riversections.bin output ngrid.txt\n");
    return EXIT_FAILURE;
  }

/* Open in- & output file */
  if((ifp1=fopen(argv[1],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on data-file1 \n");
    return EXIT_FAILURE;
  }

  if((ifp2=fopen(argv[2],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on data-file2 \n");
    return EXIT_FAILURE;
  }

  if((ofp=fopen(argv[3],"wb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on output \n");
    return EXIT_FAILURE;
  }

  if((ofp_ngrid=fopen(argv[4],"w")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on output for ngrid.txt \n");
    return EXIT_FAILURE;
  }

  /* reading original grid file */
  if(freadheader(ifp1,&header,&swap_grid,LPJGRID_HEADER,&version))
  {
    fclose(ifp1);
    fprintf(stderr,"Invalid header in original grid file.\n");
    return EXIT_FAILURE;
  }

  /* write header to output file */
  fwriteheader(ofp,&header,LPJGRID_HEADER,version);

  ncell=header.ncell;

  lpjlat=newvec(double,ncell);
  lpjlon=newvec(double,ncell);
  rivers=newvec(int,ncell);
  riversnew=newvec(int,ncell);
  sum=newvec(int,ncell);

  for(i=0;i<ncell;i++)
  {
    /* read input file and make array for number, longitude and latitude */
    if(freadshort(rbuf2,2,swap_grid,ifp1)==2)
    {
      lpjlon[i]=rbuf2[0]*0.01;
      lpjlat[i]=rbuf2[1]*0.01;
      /* printf("%5.2f %5.2f\n",lpjlon[i],lpjlat[i]); */
    }
    if(freadint(rivers+i,1,SWAP,ifp2)==1)
    {
      /* printf("%d ",rivers[i]); */
    }
  }
  fclose(ifp1);
  fclose(ifp2);

  /* initialization */
  for(i=0;i<ncell;i++)
    riversnew[i]=sum[i]=-99;

  /* changing the number for seperate river-sections, beginning at 1 */
  c0=0;
  value=1;
  for(i=-1;i<ncell;i++)
  {
    for(j=0;j<ncell;j++)
    {
      if(rivers[j]==i)
      {
    riversnew[j]=value;
    c0+=1;
      }
    }
    if(c0>0)
    {
      sum[value]=c0;
      value+=1;
      c0=0;
    }
  }

  /* re-order the input, generate output with grid number of length < LEN_MAX */
  k=1;
  l=c=c0=0;
  while(sum[k]!=-99)
  {
    if(c==0 && sum[k]>LEN_MAX)
    {
      for(i=0;i<ncell;i++)
      {
    if(riversnew[i]==k)
        {
	  /* fprintf(ofp,"%d %d\n",(int)(lpjlon[i]*100.),(int)(lpjlat[i]*100.)); */
	  /* write binary file */
      rbuf2[0]=(short)(lpjlon[i]*100.);
      rbuf2[1]=(short)(lpjlat[i]*100.);
       fwrite(rbuf2,sizeof(short),2,ofp);
          c0++;
    }
      }
      k++;
    }
    else
    {
      while((c+sum[k])<=LEN_MAX && sum[k]!=-99)
      {
    for(i=0;i<ncell;i++)
        {
      if(riversnew[i]==k)
          {
	    /* fprintf(ofp,"%d %d\n",(int)(lpjlon[i]*100.),(int)(lpjlat[i]*100.)); */
	    /* write binary file */
        rbuf2[0]=(short)(lpjlon[i]*100.);
        rbuf2[1]=(short)(lpjlat[i]*100.);
         fwrite(rbuf2,sizeof(short),2,ofp);
        c0++;
      }
    }
    c+=sum[k];
    k++;
      }
    }
    printf("c=%d k=%d sum[k]=%d\n",c,k,sum[k]);

    /* write output of ngrid.txt */
    fprintf(ofp_ngrid,"%d\n",c0);

    l++;
    c=c0=0;
  }
  fclose(ofp);
  fclose(ofp_ngrid);

  return EXIT_SUCCESS;
} /* of 'main' */
