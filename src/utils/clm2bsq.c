/**************************************************************************************/
/**                                                                                \n**/
/**                      c l m 2 b s q  .  c                                       \n**/
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

#undef USE_MPI
#include "lpj.h"

#define DATA_TYPE short
#define CLIMATE 0
#define LANDUSE 1

#define check_ptr(ptr) if((ptr)==NULL) failu("Error allocating memory\n")
#define check_file(file) if((file)==NULL) failu("Error opening file\n")

void failu(const char* str){
  fprintf(stderr, "%s\n", str);
  exit(-1);
}

void usage(char* progname){
  fprintf(stderr, "Use:\n%s ifile ofile type(0=climate, 1=landuse)\n", progname);
  exit(-1);
}

int main(int argc, char* argv[]){
  int i, m,p,y,offset;
  DATA_TYPE *rbuf, *wbuf, tmp;
  char* ifilename, *ofilename;
  FILE* ifile, *ofile;
  Header header;
  String headername;
  int swap, type,version;

  /* Parse command line */
  if (argc < 4) usage(argv[0]);
  ifilename = argv[1];
  ofilename = argv[2];
  type = atoi(argv[3]);
  
  /* Open files */
  ifile = fopen64(ifilename, "rb");
  check_file(ifile);
  ofile = fopen64(ofilename, "wb");
  check_file(ofile);

  version=READ_VERSION;
  freadheader(ifile, &header, &swap, headername, &version);
  offset = ftello64(ifile);
  
  wbuf=malloc(header.ncell*header.nbands*sizeof(DATA_TYPE));
  check_ptr(wbuf);
  if (header.order==YEARCELL){ /*BIP*/
    printf("Order: YEARCELL\n");
    rbuf=malloc(header.nbands*sizeof(DATA_TYPE));
    check_ptr(wbuf);
    if(swap&&sizeof(DATA_TYPE)==2)
       printf("Swapping byte order\n");
    for (y=0; y<header.nyear;++y){
      for (p=0; p<header.ncell; ++p){
    fseeko64(ifile, offset+p*header.nbands*header.nyear*sizeof(DATA_TYPE)+y*header.nbands*sizeof(DATA_TYPE), SEEK_SET);
    fread(rbuf, header.nbands, sizeof(DATA_TYPE), ifile);
    if (swap){
      if (sizeof(DATA_TYPE)==2){
        for(m=0; m<header.nbands;++m){
          tmp = swapshort(rbuf[m]);
          rbuf[m]=tmp;
        }
      }
      else
        fprintf(stderr, "Data needs to be byte swapped; not implemented for data type yet\n");
    }
    for(m=0; m<header.nbands;++m){
      wbuf[m*header.ncell+p]=rbuf[m];
    }
      }
      fwrite(wbuf, header.ncell*header.nbands,sizeof(DATA_TYPE), ofile);
    }
  }
  else if (header.order==CELLYEAR){/*mixed bip/bsq*/
    printf("Order: CELLYEAR\n");
    if(swap&&sizeof(DATA_TYPE)==2)
       printf("Swapping byte order\n");
    rbuf=malloc(header.ncell*header.nbands*sizeof(DATA_TYPE));
    while(fread(rbuf, header.ncell*header.nbands, sizeof(DATA_TYPE), ifile)){
      if (swap){
      if (sizeof(DATA_TYPE)==2){
        for(m=0; m<header.nbands*header.ncell;++m){
          tmp = swapshort(rbuf[m]);
          rbuf[m]=tmp;
        }
      }
      else
        fprintf(stderr, "Data needs to be byte swapped; not implemented for data type yet\n");
    }
      for (m=0; m<header.nbands; ++m)
    for(p=0; p<header.ncell; ++p){
      wbuf[m*header.ncell+p]=rbuf[p*header.nbands+m];
    }
      fwrite(wbuf, header.ncell*header.nbands,sizeof(DATA_TYPE),ofile);
    }     
  }
  free(rbuf);
  fclose(ifile);
  fclose(ofile);
  return 0;
}
