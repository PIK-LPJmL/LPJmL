/**************************************************************************************/
/**                                                                                \n**/
/**     r i v e r _ s e c t i o n s _i n p u t _ l a n d u s e . c                 \n**/
/**                                                                                \n**/
/**     re-orders landuse input-data                                               \n**/
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
  FILE *landuse_file;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  String headername;
  int i,j,k,l,m,ncell;  
  short rbuf1;
  short *rbuf2;
  int lon,lat,*lpjlon,*lpjlat; 
  int swap,swap_grid,version,landuse_version;
  int setversion;
  setversion=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    setversion=2;
    argc--;
    argv++;
  } 
/* Parse command line */
/* z.B. "river_sections_input_landuse re-ordered_grid_file lpjgrid_lonlat.clm "luc".clm output" */
  if(argc!=5){
    fprintf(stdout,"USAGE: river_sections_input_landuse [-longheader] reordered_grid-file lpjgrid_lonlat.clm luc.clm outfile\n");
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

  if((landuse_file=fopen(argv[3],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on landuse-file \n");
    exit(1);
  }   

  if((ofp=fopen(argv[4],"wb")) == NULL){
    fprintf(stderr,"Warning: File open failed on output file \n");
    exit(1);
  }

  landuse_version=setversion;
  if(freadanyheader(landuse_file,&header,&swap,headername,&landuse_version)){
    fclose(landuse_file);
    fail(3,FALSE,"Invalid header in landuse-infile.");
  }
  if(header.order!=CELLYEAR){
    fclose(landuse_file);
    fail(3,FALSE,"Order in landuse-infile not CELLYEAR.");
  }
  ncell=header.ncell;

  lpjlat=newvec(int,ncell);
  lpjlon=newvec(int,ncell);
  rbuf2=newvec(short,header.nbands);

  /* reading original grid file */
  version=setversion;
  if(freadheader(mfp,&header_grid,&swap_grid,LPJGRID_HEADER,&version)){
    fclose(mfp);
    fail(23,FALSE,"Invalid header in original grid file.");
  }  

  for(i=0;i<ncell;i++){
    if(fread(&rbuf1,sizeof(short),1,mfp)==1){
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlon[i]=rbuf1;
      fread(&rbuf1,sizeof(short),1,mfp);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlat[i]=rbuf1;
      /* printf("%d %d %d\n",i,lpjlon[i],lpjlat[i]); */
    }
    else{
      fprintf(stderr,"Error reading lpjgridori.\n");
      exit(1);
    }
  }
  fclose(mfp);
  version=setversion;
  if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,&version)){
    fclose(ifp);
    fail(23,FALSE,"Invalid header in re-ordered grid file.");
  }

  /* HEADER */
  header.ncell=header_grid.ncell;
  fwriteheader(ofp,&header,headername,landuse_version);

  for(j=0;j<header.nyear;j++){
    /* printf("j=%d\n",j); */
    fseek(ifp,headersize(LPJGRID_HEADER,version),SEEK_SET);    
    for(k=0;k<header_grid.ncell;k++) {
      if(fread(&rbuf1,sizeof(short),1,ifp)==1){
    if(swap_grid) rbuf1=swapshort(rbuf1);
    lon=rbuf1;
    fread(&rbuf1,sizeof(short),1,ifp);
    if(swap_grid) rbuf1=swapshort(rbuf1);
    lat=rbuf1;
	/* printf("%d %d %d\n",k,lon,lat); */
      }
      else{
    fprintf(stderr,"Error reading lpjgrid.\n");
    exit(1);
      }

      for(l=0;l<ncell;l++){
    if(lat==lpjlat[l] && lon==lpjlon[l]){
      fseek(landuse_file,j*ncell*sizeof(short)*header.nbands+header.nbands*sizeof(short)*l+headersize(headername,landuse_version),SEEK_SET);
      fread(rbuf2,sizeof(short),header.nbands,landuse_file);
      if(swap)
        for(m=0;m<header.nbands;m++)
          rbuf2[m]=swapshort(rbuf2[m]);
      /* for(m=0;m<header.nbands;m++)
         printf("%d\n",rbuf2[m]); */
      fwrite(rbuf2,sizeof(short),header.nbands,ofp);
      break;
    }
      }
    }
  }
  fclose(ifp);
  fclose(ofp);
  fclose(landuse_file);
  free(lpjlat);
  free(lpjlon);
  free(rbuf2);

  exit(0);
}
