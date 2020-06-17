/**************************************************************************************/
/**                                                                                \n**/
/**  r i v e r _ s e c t i o n s _i n p u t _ c o u n t r y c o d e . c            \n**/
/**                                                                                \n**/
/**     Re-orders country code                                                     \n**/
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

int main(int argc,char **argv)
{
  FILE *ifp;
  FILE *mfp,*country_file;
  FILE *ofp;
  Header header_cow,header_grid;
  String headername;
  short *cow,*reg;
  short rbuf;
  short rbuf1;
  int i,j,k,ncell,version,country_version;
  int swap_cow,swap_grid;
  int *lpjlon,*lpjlat,lon,lat;
  int setversion;
  setversion=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    setversion=2;
    argc--;
    argv++;
  } 
/* Parse command line */
/* z.B. "river_sections_input_countrycode re-ordered_grid grid.bin country_file outfile" */
  if(argc!=5){
    fprintf(stdout,"USAGE: river_sections_input_countrycode [-longheader] reordered_grid lpjgrid_lonlat.clm country_file outfile\n");
    exit(99);
  }
 
/* Open in- & output file */
  if((ifp=fopen(argv[1],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on re-ordered grid file \n");
    exit(1);
  }

  if((mfp=fopen(argv[2],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on mask-file \n");
    exit(1);
  }   

  if((country_file=fopen(argv[3],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on country-file \n");
    exit(1);
  }   

  if((ofp=fopen(argv[4],"wb")) == NULL){
    fprintf(stderr,"Warning: File open failed on output file \n");
    exit(1);
  }

  /* reading header of original grid file */
  version=setversion;
  if(freadheader(mfp,&header_grid,&swap_grid,LPJGRID_HEADER,&version)){
    fclose(mfp);
    fail(23,FALSE,"Invalid header in original grid file.");
  }

  /* reading header of countrycodefile */
  country_version=setversion;
  if(freadanyheader(country_file,&header_cow,&swap_cow,headername,&country_version)){
    fclose(country_file);
    fail(24,FALSE,"Invalid header in country-infile.");
  }

  ncell=header_grid.ncell;
  printf("ncell:%d\n",ncell);

  lpjlat=newvec(int,ncell);
  lpjlon=newvec(int,ncell);
  cow=newvec(short,ncell);
  reg=newvec(short,ncell);

  for(i=0;i<ncell;i++){
    if(fread(&rbuf1,sizeof(short),1,mfp)==1){
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlon[i]=rbuf1;
      fread(&rbuf1,sizeof(short),1,mfp);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlat[i]=rbuf1;
      
      /* Reading countrycode input-file */
      fread(&rbuf,sizeof(short),1,country_file);
      if(swap_cow) rbuf=swapshort(rbuf);
      cow[i]=rbuf;
      fread(&rbuf,sizeof(short),1,country_file);
      if(swap_cow) rbuf=swapshort(rbuf);
      reg[i]=rbuf;
    }
    else{
      fprintf(stderr,"Error reading lpjgridori.\n");
      exit(1);
    }
  }
  fclose(mfp);
  fclose(country_file);
  version=setversion;

  if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,&version)){
    fclose(ifp);
    fail(23,FALSE,"Invalid header in re-ordered grid file.");
  }

  /* HEADER */
  fwriteheader(ofp,&header_cow,headername,country_version);

  for(j=0;j<header_grid.ncell;j++){
    /* read input file for longitude and latitude */
    fread(&rbuf1,sizeof(short),1,ifp);
    if(swap_grid) rbuf1=swapshort(rbuf1);
    lon=rbuf1;
    fread(&rbuf1,sizeof(short),1,ifp);
    if(swap_grid) rbuf1=swapshort(rbuf1);
    lat=rbuf1;
    /* printf("%d %d %d\n",j,lon,lat); */
    for(k=0;k<ncell;k++)
      if(lat==lpjlat[k] && lon==lpjlon[k]){
	/* printf("j=%d k=%d soil=%d\n",j,k,soil[k]); */
    fwrite(&cow[k],sizeof(short),1,ofp);
    fwrite(&reg[k],sizeof(short),1,ofp);
    break;
      }
  }
  fclose(ifp);
  fclose(ofp);

  exit(0);
}
