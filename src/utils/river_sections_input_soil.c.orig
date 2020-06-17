/**************************************************************************************/
/**                                                                                \n**/
/**     r i v e r _ s e c t i o n s _i n p u t _ s o i l . c                       \n**/
/**                                                                                \n**/
/**     re-orders soil or other input-data of size char                            \n**/
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
  FILE *mfp,*soil_file;
  FILE *ofp;
  Header header_grid;
  char *soil;
  short rbuf2[2];
  int i,j,k,ncell,version;
  Bool swap_grid;
  int *lpjlon,*lpjlat,lon,lat;
  version=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    version=2;
    argc--;
    argv++;
  }
/* Parse command line */
/* z.B. "river_sections_input_soil re-ordered_grid_file lpjgrid_lonlat.clm input_data_of_size_char(no header) output" */
  if(argc!=5)
  {
    fprintf(stderr,"USAGE: river_sections_input_soil [-longheader] re-ordered_grid.clm lpjgrid_lonlat.clm input_data_of_size_char outfile\n");
    return EXIT_FAILURE;
  }

/* Open in- & output file */
  if((ifp=fopen(argv[1],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on re-ordered grid file \n");
    return EXIT_FAILURE;
  }

  if((mfp=fopen(argv[2],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on mask-file: %s\n",
           strerror(errno));
    return EXIT_FAILURE;
  }

  if((soil_file=fopen(argv[3],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on mask-file: %s\n",
           strerror(errno));
    return EXIT_FAILURE;
  }

  if((ofp=fopen(argv[4],"wb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on output filei: %s\n",
           strerror(errno));
    return EXIT_FAILURE;
  }

  /* reading original grid file */
  if(freadheader(mfp,&header_grid,&swap_grid,LPJGRID_HEADER,&version))
  {
    fclose(mfp);
    fprintf(stderr,"Invalid header in original grid file.\n");
    return EXIT_FAILURE;
  }

  ncell=header_grid.ncell;
  printf("ncell:%d\n",ncell);

  lpjlat=newvec(int,ncell);
  lpjlon=newvec(int,ncell);
  soil=newvec(char,ncell);

  for(i=0;i<ncell;i++)
  {
    if(freadshort(rbuf2,2,swap_grid,mfp)==2)
    {
      lpjlon[i]=rbuf2[0];
      lpjlat[i]=rbuf2[1];

      /* Reading soil input-file */
      fread(soil+i,sizeof(char),1,soil_file);
      /* printf("%d %d %d\n",i,lpjlon[i],lpjlat[i]); */
    }
    else
    {
      fprintf(stderr,"Error reading lpjgridori.\n");
      return EXIT_FAILURE;
    }
  }
  fclose(mfp);
  fclose(soil_file);
  version=READ_VERSION;
  if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,&version))
  {
    fclose(ifp);
    fprintf(stderr,"Invalid header in re-ordered grid file.\n");
    return EXIT_FAILURE;
  }

  for(j=0;j<header_grid.ncell;j++)
  {
    /* read input file for longitude and latitude */
    freadshort(rbuf2,2,swap_grid,ifp);
    lon=rbuf2[0];
    lat=rbuf2[1];
    /* printf("%d %d %d\n",j,lon,lat); */
    for(k=0;k<ncell;k++)
      if(lat==lpjlat[k] && lon==lpjlon[k])
      {
	/* printf("j=%d k=%d soil=%d\n",j,k,soil[k]); */
    fwrite(soil+k,sizeof(char),1,ofp);
    break;
      }
  }
  fclose(ifp);
  fclose(ofp);

  return EXIT_SUCCESS;
} /* of 'main' */
