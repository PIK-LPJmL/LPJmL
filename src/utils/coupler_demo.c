/**************************************************************************************/
/**                                                                                \n**/
/**                c  o  u  p  l  e  r  _  d  e  m  o  .  c                        \n**/
/**                                                                                \n**/
/**     Demonstration program for coupling LPJmL to external program               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define COUPLERDEMO_VERSION "0.9.003"

#define USAGE "Usage: %s [-port n] [-wait n] [landusefile]\n"

#define DFLT_FILENAME "/p/projects/lpjml/input/historical/input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"

#define LANDUSE_NBANDS 64
#define FERTILIZER_NBANDS 32

static Bool readsocket(Socket *socket,int day,int sizes[],int count[],Type type[])
{
  Token token;
  int index;
  int year,step;
  int j,k;
  float *data;
  short *sdata;
  if(receive_token_coupler(socket,&token,&index))
    return TRUE;
  if(token==END_DATA)
    return TRUE;
  if(token!=PUT_DATA)
  {
    fprintf(stderr,"Token for output data=%s is not PUT_DATA.\n",token_names[token]);
    return TRUE;
  }
  if(index<0 || index>=NOUT)
  {
    fprintf(stderr,"Invalid index %d of output, must be [0,%d],\n",index,NOUT-1);
    return TRUE;
  }
  if(count[index]<0)
  {
    fprintf(stderr,"No output defined for index %d.\n",index);
    return TRUE;
  }
  readint_socket(socket,&year,1);
#if COUPLER_VERSION == 4
  readint_socket(socket,&step,1);
#endif
  if(sizes[index]==0)
  {
    data=newvec(float,count[index]);
    check(data);
    printf("%d flux:",year);
    readfloat_socket(socket,data,count[index]);
    for(j=0;j<count[index];j++)
      printf(" %g",data[j]);
    printf("\n");
    free(data);
  }
  else
  {
    if(type[index]==LPJ_SHORT)
    {
      sdata=newvec(short,sizes[index]);
      check(sdata);
      for(j=0;j<count[index];j++)
      {
        readshort_socket(socket,sdata,sizes[index]);
        printf("%d/%d %d[%d]:",day,year,index,j);
        for(k=0;k<sizes[index];k++)
          printf(" %d",sdata[k]);
        printf("\n");
      }
      free(sdata);
    }
    else
    {
      data=newvec(float,sizes[index]);
      check(data);
      for(j=0;j<count[index];j++)
      {
        readfloat_socket(socket,data,sizes[index]);
        printf("%d/%d %d[%d]:",day,year,index,j);
        for(k=0;k<sizes[index];k++)
          printf(" %g",data[k]);
        printf("\n");
      }
      free(data);
    }
  }
  return FALSE;
} /* of 'readsocket' */

static Bool readyeardata(Socket *socket,int nday_out,int nmonth_out,int n_out,int sizes[],int counts[],Type type[])
{
  int month;
  int day,dayofyear;
  int i;
  dayofyear=1;
  for(month=0;month<12;month++)
  {
    /* read daily data */
    for(day=0;day<ndaymonth[month];day++)
    {
      for(i=0;i<nday_out;i++)
        if(readsocket(socket,dayofyear,sizes,counts,type))
          return TRUE;
      dayofyear++;
    }
    /* read monthly data */
    for(i=0;i<nmonth_out;i++)
      if(readsocket(socket,dayofyear-1,sizes,counts,type))
        return TRUE;
  }
  /* read annual data */
  for(i=0;i<n_out-nday_out-nmonth_out;i++)
    if(readsocket(socket,NDAYYEAR,sizes,counts,type))
      return TRUE;
  return FALSE;
} /* of 'readyeardata' */

int main(int argc,char **argv)
{
  FILE *file;
  Header header;
  Socket *socket;
  float *landuse;
  float *fertilizer;
  short *country,*region;
  char *endptr;
  float co2;
  Intcoord *coords;
  int sizes[NOUT];
  int count[NOUT];
  int nstep[NOUT];
  Type type[NOUT];
  int sizes_in[N_IN];
  Type type_in[N_IN];
  Type datatype;
  int port;
  int nmonth_out;
  int nday_out;
  int nbands;
  int firstgrid;
  int ncell_in;
  int status;
  Bool swap;
  String line;
  const char *progname;
  const char *title[4];
  char *filename;
  Token token;
  int i,j,n_out,n_in,ncell,index,year,version,n_out_1,n_err,wait;
  struct coord
  {
    float lon,lat;
  } *fcoords;
  port=DEFAULT_COUPLER_PORT;
  wait=0;
  progname=strippath(argv[0]);
  filename=DFLT_FILENAME;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-port"))
      {
        if(i==argc-1)
        {
          fprintf(stderr,"Argument missing after '-port' option.\n"
                USAGE,progname);
          return EXIT_FAILURE;
        }
        port=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-port'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(port<0 || port>USHRT_MAX)
        {
          fprintf(stderr,"Invalid port number %d.\n",port);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-wait"))
      {
        if(i==argc-1)
        {
          fprintf(stderr,"Argument missing after '-wait' option.\n"
                USAGE,progname);
          return EXIT_FAILURE;
        }
        wait=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-wait'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(wait<0)
        {
          fprintf(stderr,"Invalid value for wait %d, muset be >=0.\n",wait);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[i],progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(i<argc)
    filename=argv[i];
  snprintf(line,STRING_LEN,
           "%s C Version %s (" __DATE__ ")",progname,COUPLERDEMO_VERSION);
  title[0]=line;
  title[1]="Coupler demo for LPJmL";
  title[2]="(c) Potsdam Institute for Climate Impact Research (PIK),";
  title[3]="see COPYRIGHT file";
  banner(title,4,78);
  if(wait)
    printf("Waiting for LPJmL model for %d sec...\n",wait);
  else
    printf("Waiting for LPJmL model...\n");
  /* Establish the connection */
  socket=connect_coupler(port,wait);
  if(socket==NULL)
    return EXIT_FAILURE;
#if COUPLER_VERSION == 4
  if(receive_token_coupler(socket,&token,&index))
  {
    close_socket(socket);
    return EXIT_FAILURE;
  }
  if(token!=PUT_INIT_DATA)
  {
    fprintf(stderr,"Unexpected token %s received, must be PUT_INIT_DATA.\n",
            token_names[token]);
    close_socket(socket);
    return EXIT_FAILURE;
  }
  readint_socket(socket,(int *)&datatype,1);
  readint_socket(socket,&firstgrid,1);
  readint_socket(socket,&ncell_in,1);
  printf("Index of first cell: %d\n",firstgrid);
  printf("Total number of cells: %d\n",ncell_in);
#else
  firstgrid=0;
#endif
  readint_socket(socket,&ncell,1);
  printf("Number of cells: %d\n",ncell);
  readint_socket(socket,&n_in,1);
  readint_socket(socket,&n_out,1);
  printf("Number of input streams: %d\n"
         "Number of output streams: %d\n",
         n_in,n_out);
  /* Send number of items per cell for each input data stream */
  landuse=fertilizer=NULL;
  for(i=0;i<n_in;i++)
  {
    if(receive_token_coupler(socket,&token,&index))
    {
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token!=GET_DATA_SIZE)
    {
      fprintf(stderr,"Token=%s is not GET_DATA_SIZE.\n",token_names[token]);
      return EXIT_FAILURE;
    }
    if(index<0 || index>=N_IN)
    {
      fprintf(stderr,"Invalid index %d of input, must be in [0,%d].\n",index,N_IN-1);
      readint_socket(socket,&index,1);
      index=COUPLER_ERR;
      writeint_socket(socket,&index,1);
      return EXIT_FAILURE;
    }
    readint_socket(socket,(int *)(type_in+index),1);
#if COUPLER_VERSION == 4
    readint_socket(socket,sizes_in+index,1);
#else
    sizes_in[index]=ncell;
#endif
    switch(index)
    {
      case LANDUSE_DATA:
        file=fopen(filename,"rb");
        if(file==NULL)
        {
          fprintf(stderr,"Error opening landuse file '%s': %s.\n",filename,strerror(errno));
          nbands=COUPLER_ERR;
          break;
        }
        version=READ_VERSION;
        if(freadheader(file,&header,&swap,LPJ_LANDUSE_HEADER,&version,TRUE))
        {
          fprintf(stderr,"Error reading header of landuse file '%s'.\n",filename);
          nbands=COUPLER_ERR;
          break;
        }
        nbands=header.nbands;
        landuse=newvec(float,sizes_in[index]*nbands);
        if(landuse==NULL)
        {
          printallocerr("landuse");
          nbands=COUPLER_ERR;
        }
        break;
      case FERTILIZER_DATA:
        nbands=FERTILIZER_NBANDS;
        fertilizer=newvec(float,sizes_in[index]*FERTILIZER_NBANDS);
        if(fertilizer==NULL)
        {
          printallocerr("fertilizer");
          nbands=COUPLER_ERR;
        }
        break;
      case CO2_DATA:
        nbands=1;
        break;
      default:
        fprintf(stderr,"Unsupported index %d of input\n",index);
        nbands=COUPLER_ERR;
    }
    /* send number of bands */
    writeint_socket(socket,&nbands,1);
    if(nbands==COUPLER_ERR)
      return EXIT_FAILURE;
  }
  /* Get number of items per cell for each output data stream */
  for(i=0;i<NOUT;i++)
    count[i]=-1;
  n_out_1=0;
  nmonth_out=0;
  nday_out=0;
  n_err=0;
  for(i=0;i<n_out;i++)
  {
    if(receive_token_coupler(socket,&token,&index))
    {
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token==END_DATA)
    {
      fprintf(stderr,"Unexpected end token received.\n");
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token!=PUT_DATA_SIZE)
    {
      fprintf(stderr,"Token=%s is not PUT_DATA_SIZE.\n",token_names[token]);
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(index<0 || index>=NOUT)
    {
      fprintf(stderr,"Invalid index %d of output, must be in [0,%d].\n",index,NOUT-1);
#if COUPLER_VERSION == 4
      readint_socket(socket,&index,1);
#endif
      readint_socket(socket,&index,1);
      readint_socket(socket,&index,1);
      readint_socket(socket,&index,1);
      index=COUPLER_ERR;
      writeint_socket(socket,&index,1);
      n_err++;
      continue;
    }
#if COUPLER_VERSION == 4
    /* get number of cells per year for output */
    readint_socket(socket,sizes+index,1);
#else
    if(index==GLOBALFLUX)
      sizes[index]=0;
    else
      sizes[index]=ncell;
#endif
    /* get number of steps per year for output */
    readint_socket(socket,nstep+index,1);
    /* get number of bands for output */
    readint_socket(socket,count+index,1);
    readint_socket(socket,(int *)(type+index),1);
    /* check for static output */
    if(nstep[index]==0)
      n_out_1++;
    else
      switch(nstep[index])
      {
        case NMONTH:
          nmonth_out++;
          index=COUPLER_OK;
          break;
        case NDAYYEAR:
          nday_out++;
          index=COUPLER_OK;
          break;
        case 1:
          index=COUPLER_OK;
          break;
        default:
          index=COUPLER_ERR;
          n_err++;
          fprintf(stderr,"Invalid number of steps %d for index %d, must be 1, 12, or 365.\n",nstep[index],index);
      }
    writeint_socket(socket,&index,1);
  }
  if(receive_token_coupler(socket,&token,&index))
  {
    close_socket(socket);
    return EXIT_FAILURE;
  }
  if(token==END_DATA)
  {
    fprintf(stderr,"Unexpected end token received.\n");
    close_socket(socket);
    return EXIT_FAILURE;
  }
  if(token!=GET_STATUS)
  {
    fprintf(stderr,"Token %s is not GET_STATUS.\n",token_names[token]);
    close_socket(socket);
    return EXIT_FAILURE;
  }
  if(landuse!=NULL)
    index=COUPLER_OK;
  else
  {
    index=COUPLER_ERR;
    writeint_socket(socket,&index,1);
    fprintf(stderr,"Landuse output is missing from LPJmL.\n");
    close_socket(socket);
    return EXIT_FAILURE;
  }
  writeint_socket(socket,&index,1);
  n_out-=n_err;
  /* read all static non time dependent outputs */
  region=country=NULL;
  coords=NULL;
  fcoords=NULL;
  for(i=0;i<n_out_1;i++)
  {
    if(receive_token_coupler(socket,&token,&index))
    {
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token!=PUT_DATA)
    {
      close_socket(socket);
      fprintf(stderr,"Token for output data=%s is not PUT_DATA.\n",token_names[token]);
      return EXIT_FAILURE;
    }
    switch(index)
    {
      case GRID:
        if(type[GRID]==LPJ_SHORT)
        {
          coords=newvec(Intcoord,sizes[index]);
          check(coords);
          readshort_socket(socket,(short *)coords,sizes[index]*2);
          printf("Grid:\n");
          for(j=0;j<sizes[index];j++)
            printf("%g %g\n",coords[j].lat*.01,coords[j].lon*.01);
        }
        else
        {
          fcoords=newvec(struct coord,sizes[index]);
          check(fcoords);
          readfloat_socket(socket,(float *)fcoords,sizes[index]*2);
          printf("Grid:\n");
          for(j=0;j<sizes[index];j++)
            printf("%g %g\n",fcoords[j].lat,fcoords[j].lon);
        }
        break;
      case COUNTRY:
        country=newvec(short,sizes[index]);
        check(country);
        readshort_socket(socket,country,sizes[index]);
        break;
      case REGION:
        region=newvec(short,sizes[index]);
        check(region);
        readshort_socket(socket,region,sizes[index]);
        break;
      default:
        fprintf(stderr,"Unsupported index %d of output.\n",index);
        return EXIT_FAILURE;
    }
  }
  /* reduce the number of output streams by the number of static streams */
  n_out-=n_out_1;
  /* main simulation loop */
  for(;;)
  {
    /* send input to LPJmL */
    for(i=0;i<n_in;i++)
    {
      if(receive_token_coupler(socket,&token,&index))
      {
        close_socket(socket);
        return EXIT_FAILURE;
      }
      if(token==END_DATA) /* Did we receive end token? */
        break;
      if(token!=GET_DATA)
      {
        fprintf(stderr,"Token for input data=%s is not GET_DATA.\n",token_names[token]);
        close_socket(socket);
        return EXIT_FAILURE;
      }
      readint_socket(socket,&year,1);
      if(index<0 || index>=N_IN)
      {
        fprintf(stderr,"Invalid index %d of input.\n",index);
#if COUPLER_VERSION == 4
        status=COUPLER_ERR;
        writeint_socket(socket,&status,1);
#else
        close_socket(socket);
        return EXIT_FAILURE;
#endif
      }
      else
      switch(index)
      {
        case LANDUSE_DATA:
          if(year<header.firstyear)
            year=header.firstyear;
          else if(year>=header.firstyear+header.nyear)
            year=header.firstyear+header.nyear-1;
          fseek(file,headersize(LPJ_LANDUSE_HEADER,version)+((year-header.firstyear)*header.ncell*header.nbands+firstgrid*header.nbands)*typesizes[header.datatype],SEEK_SET);
          if(readfloatvec(file,landuse,header.scalar,sizes_in[index]*header.nbands,swap,header.datatype))
          {
            fprintf(stderr,"Error reading landuse file '%s': %s.\n",filename,strerror(errno));
#if COUPLER_VERSION == 4
            status=COUPLER_ERR;
            writeint_socket(socket,&status,1);
#else
            close_socket(socket);
            return EXIT_FAILURE;
#endif
          }
          else
          {
#if COUPLER_VERSION == 4
            status=COUPLER_OK;
            writeint_socket(socket,&status,1);
#endif
#ifdef DEBUG
            for(i=0;i<sizes_in[index];i++)
            {
              printf("cell %d:",i);
              for(j=0;j<header.nbands;j++)
                printf(" %g",landuse[i*header.nbands+j]);
              printf("\n");
            }
#endif
            writefloat_socket(socket,landuse,sizes_in[index]*header.nbands);
          }
          break;
        case FERTILIZER_DATA:
          for(j=0;j<sizes_in[index]*FERTILIZER_NBANDS;j++)
            fertilizer[j]=1;
#if COUPLER_VERSION == 4
          status=COUPLER_OK;
          writeint_socket(socket,&status,1);
#endif
          writefloat_socket(socket,fertilizer,sizes_in[index]*FERTILIZER_NBANDS);
          break;
        case CO2_DATA:
          co2=288.0;
#if COUPLER_VERSION == 4
          status=COUPLER_OK;
          writeint_socket(socket,&status,1);
#endif
          writefloat_socket(socket,&co2,1);
          break;
        default:
          fprintf(stderr,"Unsupported index %d of input.\n",index);
#if COUPLER_VERSION == 4
          status=COUPLER_ERR;
          writeint_socket(socket,&status,1);
#else
          close_socket(socket);
          return EXIT_FAILURE;
#endif
      }
    }
    if(token==END_DATA) /* Did we receive end token? */
      break;
    /* get output from LPJmL */
    if(readyeardata(socket,nday_out,nmonth_out,n_out,sizes,count,type))
      break;
  } /* of for(;;) */
  printf("End of communication.\n");
  close_socket(socket);
  return EXIT_SUCCESS;
} /* of 'main' */
