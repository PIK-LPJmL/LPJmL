/**************************************************************************************/
/**                                                                                \n**/
/**          c  f  t  s  2  6  -  l  u  2  c  l  m  .  c                           \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/**     Converts data files into LPJ data files                                    \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include <sys/stat.h>

#define CFTS26_LUC2CLM_VERSION "1.0.001"
#define ORDER CELLYEAR
#define NYEAR 103  /* default value for number of years */
#define FIRSTYEAR 1901 /* default value for first year */
#define FIRSTCELL 0
#define NCELL 67420
#define NBANDS_CFT 26
#define NBANDS_OUT 24
#define NBANDS_LU 5
#define USAGE    "Usage: cfts26_luc2clm [-h] [-firstyear first] [-lastyear last]\n"\
                 "       [-nyear n] [-firstcell] [-ncell] [-nbands] [-swap]\n"\
                 "       [-yearcell] cfts26-file lu5-file clmfile\n"

typedef short Data[NBANDS_CFT];
typedef short Data_out[NBANDS_OUT];

int main(int argc,char **argv)
{
  FILE *file;
  Data *data;
  Data_out *data_out;
  Header header;
  int i,j,n;
  long int k,m;
  short rbuf,rbuf0,rbuf1;
  Bool swap;
  struct stat filestat;
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=ORDER;
  header.firstcell=FIRSTCELL;
  header.ncell=NCELL;
  header.nbands=NBANDS_CFT;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.datatype=LPJ_SHORT;
  header.scalar=0.001;
  swap=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("cfts26_luc2clm " CFTS26_LUC2CLM_VERSION " (" __DATE__ ") - convert cfts26 and lu5 data files to\n"
               "       clm data files for lpj C version\n");
        printf(USAGE
               "-h               print this help text\n" 
               "-firstyear first first year in data file (default is %d)\n"
               "-lastyear last   last year in data file\n"
               "-nyear n         number of years in data file (default is %d)\n"
               "-firstcell       first cell in data file (default is %d)\n"
               "-ncell           number of cells in data file (default is %d)\n"
               "-nbands          number of bands in data/output file (default is %d)\n"
               "-swap            change byte order in data file\n"
               "-yearcell        does not revert order in data file\n"
               "cfts26-file      filename of cfts26 data file\n"
               "lu5-file         filename of lu5 data file\n"
               "clmfile          filename of clm data file\n",
               FIRSTYEAR,NYEAR,FIRSTCELL,NCELL,NBANDS_CFT);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[i],"-nyear"))
        header.nyear=atoi(argv[++i]);
      else if(!strcmp(argv[i],"-firstyear"))
        header.firstyear=atoi(argv[++i]);
      else if(!strcmp(argv[i],"-lastyear"))
        header.nyear=atoi(argv[++i])-header.firstyear+1;
      else if(!strcmp(argv[i],"-firstcell"))
        header.firstcell=atoi(argv[++i]);
      else if(!strcmp(argv[i],"-ncell"))
        header.ncell=atoi(argv[++i]);
      else if(!strcmp(argv[i],"-nbands"))
        header.nbands=atoi(argv[++i]);
      else if(!strcmp(argv[i],"-swap"))
        swap=TRUE;
      else if(!strcmp(argv[i],"-yearcell"))
        header.order=YEARCELL;
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n",argv[i]);
        fprintf(stderr,USAGE);
        return EXIT_FAILURE;
      }
    }
    else 
      break;
  if(argc<i+3)
  {
    fprintf(stderr,"Filenames missing.\n");
    fprintf(stderr,USAGE);
    return EXIT_FAILURE;
  }

  /* read lu26 file */
  file=fopen(argv[i],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
  fstat(fileno(file),&filestat);
  n=filestat.st_size/header.nyear/sizeof(Data);
  printf("Number of cells in cft-file: %d\n",n);
  data=(Data *)malloc(n*sizeof(Data)*header.nyear);
  if(data==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  data_out=(Data_out *)malloc(n*sizeof(Data_out)*header.nyear);
  if(data_out==NULL)
  {
    printallocerr("data_out");
    return EXIT_FAILURE;
  }
  fread(data,sizeof(Data),n*header.nyear,file);
  if(swap)
    for(m=0;m<n*header.nyear;m++)
      for(j=0;j<header.nbands;j++)
        data[m][j]=swapshort(data[m][j]);

  fclose(file);

  /* open lc5 file */
  file=fopen(argv[i+1],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fstat(fileno(file),&filestat);
  n=filestat.st_size/header.nyear/sizeof(short)/NBANDS_LU;
  printf("Number of cells in lu-file: %d\n",n);  

  for(m=0;m<n*header.nyear;m++)
  {
    fseek(file,sizeof(short)+m*NBANDS_LU*sizeof(short),SEEK_SET);
    fread(&rbuf,sizeof(short),1,file);
    if(swap)
      rbuf=swapshort(rbuf); 

    for(j=0;j<header.nbands;j++)
    {
      if(data[m][j]<0) data[m][j]=0;
      if(rbuf>=0)
        data[m][j]=(short)((float)data[m][j]*(float)rbuf/1000+0.5);
    }
  }
  fclose(file);

  /* put managed grass behind the other crops */
  for(m=0;m<n*header.nyear;m++)
  {
    /* without irrigation */
    rbuf0=data[m][0];
    rbuf1=data[m][1];
    for(j=0;j<header.nbands/2-2;j++)
      data_out[m][j]=data[m][j+2];
    data_out[m][header.nbands/2-2]=rbuf0+rbuf1;
    
     /* with irrigation */ 
    rbuf0=data[m][header.nbands/2];
    rbuf1=data[m][header.nbands/2+1];
    for(j=header.nbands/2-1;j<header.nbands-3;j++)
      data_out[m][j]=data[m][j+3];
    data_out[m][header.nbands-3]=rbuf0+rbuf1;
  }

  file=fopen(argv[i+2],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[i+2],strerror(errno));
    return EXIT_FAILURE;
  }
  header.ncell=n;
  header.nbands=NBANDS_OUT;
  fwriteheader(file,&header,LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION);
  if(header.order==CELLYEAR)
  {
    for(m=0;m<header.nyear;m++)
      for(k=0;k<n;k++)
        fwrite(data_out+k*header.nyear+m,sizeof(Data_out),1,file);    
  }
  else
    fwrite(data_out,sizeof(Data_out),header.nyear*n,file);

  fclose(file);
  free(data);
  free(data_out);
  return EXIT_SUCCESS;
} /* of 'main' */

/* 
- combines the two input files glo.cfts26.1901-2003.bin.aix and 
  glo.lu5.1901-2003.bin.aix to one input file (luc2003.clm)
- combines the fractions of managed C3 and C4 grass
  -> LPJ computes the establishment of the grass type
  -> instead of 26 cfts only 24 cfts yet
- changes the order of the cfts
  -> puts managed grass to the end of the other cfts
- first crops without irrigation and then crops with irrigation
- in LPJ no separat parameters for managed grass
  -> managed grass has the same id like natural grass
  -> number of plant functional types: npft=9
  -> number of crop functional types: ncft=11
  -> number of managed grass: NGRASS=1
  -> crop identification number from 9 to 19
  -> order in luc-inputfile: 
     11 crops without irrigation 
     => positions = id-npft
     1 fraction for managed grass
     => position = ncft
     11 crops with irrigation
     => positions = id-npft+ncft+NGRASS
     1 fraction for irrigated managed grass
     => position = id-npft+ncft+2*NGRASS
- changes the order CELLYEAR or YEARCELL 
*/
