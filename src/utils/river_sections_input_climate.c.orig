/**************************************************************************************/
/**                                                                                \n**/
/**  r i v e r _ s e c t i o n s _i n p u t _ c l i m a t e . c                    \n**/
/**                                                                                \n**/
/**     Re-orders climate input-data                                               \n**/
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
  FILE *climate_file;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  String headername;
  int i,j,k,l,ncell;
  short rbuf1[2];
  short rbuf2[NMONTH];
  int lon,lat,*lpjlon,*lpjlat;
  int swap,swap_grid,version,climate_version;
  int setversion;
  setversion=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    setversion=2;
    argc--;
    argv++;
  }
/* Parse command line */
/* z.B. "river_sections_input_climate re-ordered_grid.clm lpjgrid_lonlat.clm "climate".clm output" */
  if(argc!=5)
  {
    fprintf(stdout,"USAGE: river_sections_input_climate [-longheader] re-ordered_grid.clm lpjgrid_lonlat.clm climate.clm outfile.clm\n");
    exit(99);
  }

/* Open in- & output file */
  if((ifp=fopen(argv[1],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on re-ordered grid file\n");
    exit(1);
  }

  if((mfp=fopen(argv[2],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on original grid file\n");
    exit(1);
  }

  if((climate_file=fopen(argv[3],"rb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on climate-file\n");
    exit(1);
  }

  if((ofp=fopen(argv[4],"wb")) == NULL)
  {
    fprintf(stderr,"Warning: File open failed on output file\n");
    exit(1);
  }

  climate_version=setversion;
  if(freadanyheader(climate_file,&header,&swap,headername,&climate_version))
  {
    fclose(climate_file);
    fail(2,FALSE,"Invalid header in climate-infile.");
  }
  if(header.order!=CELLYEAR)
  {
    fclose(climate_file);
    fail(2,FALSE,"Order in climate-infile not CELLYEAR.");
  }
  ncell=header.ncell;

  lpjlat=newvec(int,ncell);
  lpjlon=newvec(int,ncell);

  /* reading original grid file */
  version=setversion;
  if(freadheader(mfp,&header_grid,&swap_grid,LPJGRID_HEADER,&version))
  {
    fclose(mfp);
    fail(23,FALSE,"Invalid header in original grid file.");
  }

  for(i=0;i<ncell;i++)
    if(freadshort(rbuf1,2,swap_grid,mfp)==2)
    {
      lpjlon[i]=rbuf1[0];
      lpjlat[i]=rbuf1[1];
      /* printf("%d %d %d\n",i,lpjlon[i],lpjlat[i]); */
    }
    else
    {
      fprintf(stderr,"Error reading lpjgridori.\n");
      exit(1);
    }
  fclose(mfp);

  /* reading re-ordered grid file */
  version=setversion;
  if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,&version))
  {
    fclose(ifp);
    fail(23,FALSE,"Invalid header in re-ordered grid file");
  }

  /* HEADER */
  header.ncell=header_grid.ncell;
  fwriteheader(ofp,&header,headername,climate_version);

  for(j=0;j<header.nyear;j++)
  {
    /* printf("j=%d\n",j); */
    fseek(ifp,headersize(LPJGRID_HEADER,version),SEEK_SET);
    for(k=0;k<header_grid.ncell;k++)
    {
      if(freadshort(rbuf1,2,swap_grid,ifp)==2)
      {
    lon=rbuf1[0];
    lat=rbuf1[1];
	/* printf("%d %d %d\n",k,lon,lat); */
      }
      else
      {
    fprintf(stderr,"Error reading lpjgrid.\n");
    exit(1);
      }

      for(l=0;l<ncell;l++)
      {
    if(lat==lpjlat[l] && lon==lpjlon[l])
        {
      fseek(climate_file,j*ncell*sizeof(short)*NMONTH+NMONTH*sizeof(short)*l+headersize(LPJ_CLIMATE_HEADER,climate_version),SEEK_SET);
      freadshort(rbuf2,NMONTH,swap,climate_file);
      fwrite(rbuf2,sizeof(short),NMONTH,ofp);
      break;
    }
      }
    }
  }
  fclose(ifp);
  fclose(ofp);

  fclose(climate_file);
  free(lpjlat);
  free(lpjlon);
  exit(0);
}
