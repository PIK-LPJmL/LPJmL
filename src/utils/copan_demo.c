/**************************************************************************************/
/**                                                                                \n**/
/**                c  o  p  a  n  _  d  e  m  o  .  c                              \n**/
/**                                                                                \n**/
/**     Demonstration program for coupling LPJmL to COPAN                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define COPANDEMO_VERSION "0.9.003"

#define USAGE "Usage: %s [-port n] [-wait n]\n"

#define LANDUSE_NBANDS 64

static Bool readdata(Socket *socket,int ncell,int day,int count[],Type type[])
{
  int token;
  int index;
  int year;
  int j,k;
  float *data;
  short *sdata;
  readint_socket(socket,&token,1);
  if(token==END_DATA)
    return TRUE;
  if(token==FAIL_DATA)
  {
    fprintf(stderr,"LPJml stopped with error.\n");
    return TRUE;
  }
  if(token!=PUT_DATA)
  {
    fprintf(stderr,"Token for output data=%d is not PUT_DATA.\n",token);
    return TRUE;
  }
  if(readint_socket(socket,&index,1))
  {
    fprintf(stderr,"Error reading index of output\n");
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
  if(index==GLOBALFLUX)
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
      sdata=newvec(short,ncell);
      check(sdata);
      for(j=0;j<count[index];j++)
      {
        readshort_socket(socket,sdata,ncell);
        printf("%d/%d %d[%d]:",day,year,index,j);
        for(k=0;k<ncell;k++)
          printf(" %d",sdata[k]);
        printf("\n");
      }
      free(sdata);
    }
    else
    {
      data=newvec(float,ncell);
      check(data);
      for(j=0;j<count[index];j++)
      {
        readfloat_socket(socket,data,ncell);
        printf("%d/%d %d[%d]:",day,year,index,j);
        for(k=0;k<ncell;k++)
          printf(" %g",data[k]);
        printf("\n");
      }
      free(data);
    }
  }
  return FALSE;
} /* of 'readdata' */

static Bool readyeardata(Socket *socket,int ncell,int nday_out,int nmonth_out,int n_out,int counts[],Type type[])
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
        if(readdata(socket,ncell,dayofyear,counts,type))
          return TRUE;
      dayofyear++;
    }
    /* read monthly data */
    for(i=0;i<nmonth_out;i++)
      if(readdata(socket,ncell,dayofyear-1,counts,type))
        return TRUE;
  }
  /* read annual data */
  for(i=0;i<n_out-nday_out-nmonth_out;i++)
    if(readdata(socket,ncell,NDAYYEAR,counts,type))
      return TRUE;
  return FALSE;
} /* of 'readyeardata' */

int main(int argc,char **argv)
{
  Socket *socket;
  float *landuse;
  short *country,*region;
  char *endptr;
  float co2;
  Intcoord *coords;
  int count[NOUT];
  int nstep[NOUT];
  Type type[NOUT];
  Type type_in[N_IN];
  int port;
  int nmonth_out;
  int nday_out;
  String line;
  const char *progname;
  const char *title[4];
  int i,j,token,n_out,n_in,ncell,index,year,version,n_out_1,n_err,wait;
  struct coord
  {
    float lon,lat;
  } *fcoords;
  Bool islanduse=FALSE;
  port=DEFAULT_COPAN_PORT;
  wait=0;
  progname=strippath(argv[0]);
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
  snprintf(line,STRING_LEN,
           "%s C Version %s (" __DATE__ ")",progname,COPANDEMO_VERSION);
  title[0]=line;
  title[1]="COPAN demo for LPJmL";
  title[2]="(c) Potsdam Institute for Climate Impact Research (PIK),";
  title[3]="see COPYRIGHT file";
  banner(title,4,78);
  if(wait)
    printf("Waiting for LPJmL model for %d sec...\n",wait);
  else
    printf("Waiting for LPJmL model...\n");
  /* Establish the connection */
  socket=opentdt_socket(port,wait);
  if(socket==NULL)
  {
    fprintf(stderr,"Error opening communication channel at port %d.\n",
            port);
    return EXIT_FAILURE;
  }
  /* Get protocol version */
  readint_socket(socket,&version,1);
  if(version!=COPAN_COUPLER_VERSION)
  {
    fprintf(stderr,"Unsupported coupler version %d.\n",version);
    return EXIT_FAILURE;
  }
  readint_socket(socket,&ncell,1);
  printf("Number of cells: %d\n",ncell);
  readint_socket(socket,&n_in,1);
  readint_socket(socket,&n_out,1);
  printf("Number of input streams: %d\n"
         "Number of output streams: %d\n",
         n_in,n_out);
  /* Send number of items per cell for each input data stream */
  for(i=0;i<n_in;i++)
  {
    readint_socket(socket,&token,1);
    if(token==END_DATA)
    {
      fprintf(stderr,"Unexpected end token received.\n");
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token==FAIL_DATA)
    {
      fprintf(stderr,"LPJmL stopped with error.\n");
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token!=GET_DATA_SIZE)
    {
      fprintf(stderr,"Token=%d is not GET_DATA_SIZE.\n",token);
      return EXIT_FAILURE;
    }
    readint_socket(socket,&index,1);
    if(index<0 || index>=N_IN)
    {
      fprintf(stderr,"Invalid index %d of input, must be in [0,%d].\n",index,N_IN-1);
      readint_socket(socket,&index,1);
      index=COPAN_ERR;
      writeint_socket(socket,&index,1);
      return EXIT_FAILURE;
    }
    readint_socket(socket,(int *)(type_in+index),1);
    switch(index)
    {
      case LANDUSE_DATA:
        index=LANDUSE_NBANDS;
        landuse=newvec(float,ncell*LANDUSE_NBANDS);
        if(landuse==NULL)
        {
          printallocerr("landuse");
          index=COPAN_ERR;
        }
        islanduse=TRUE;
        break;
      case CO2_DATA:
        index=1;
        break;
      default:
        fprintf(stderr,"Unsupported index %d of input\n",index);
        index=COPAN_ERR;
    }
    /* send number of bands */
    writeint_socket(socket,&index,1);
    if(index==COPAN_ERR)
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
    readint_socket(socket,&token,1);
    if(token==END_DATA)
    {
      fprintf(stderr,"Unexpected end token received.\n");
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token==FAIL_DATA)
    {
      fprintf(stderr,"LPJmL stopped with error.\n");
      close_socket(socket);
      return EXIT_FAILURE;
    }
    if(token!=PUT_DATA_SIZE)
    {
      fprintf(stderr,"Token=%d is not PUT_DATA_SIZE.\n",token);
      return EXIT_FAILURE;
    }
    readint_socket(socket,&index,1);
    if(index<0 || index>=NOUT)
    {
      fprintf(stderr,"Invalid index %d of output, must be in[0,%d].\n",index,NOUT-1);
      readint_socket(socket,&index,1);
      readint_socket(socket,&index,1);
      readint_socket(socket,&index,1);
      index=COPAN_ERR;
      writeint_socket(socket,&index,1);
      n_err++;
      continue;
    }
    /* get number of steps per year for output */
    readint_socket(socket,nstep+index,1);
    /* get number of bands for output */
    readint_socket(socket,count+index,1);
    readint_socket(socket,(int *)(type+index),1);
    /* check for static output */
    if(index==GRID || index==COUNTRY || index==REGION)
      n_out_1++;
    switch(nstep[index])
    {
      case NMONTH:
        nmonth_out++;
        index=COPAN_OK;
        break;
      case NDAYYEAR:
        nday_out++;
        index=COPAN_OK;
        break;
      case 0 : case 1:
        index=COPAN_OK;
        break;
      default:
        index=COPAN_ERR;
        n_err++;
        fprintf(stderr,"Invalid number of steps %d for index %d, must be 1, 12, or 365.\n",nstep[index],index);
    }
    writeint_socket(socket,&index,1);
  }
  readint_socket(socket,&token,1);
  if(token==END_DATA)
  {
    fprintf(stderr,"Unexpected end token received.\n");
    close_socket(socket);
    return EXIT_FAILURE;
  }
  if(token==FAIL_DATA)
  {
    fprintf(stderr,"LPJmL stopped with error.\n");
    close_socket(socket);
    return EXIT_FAILURE;
  }
  if(token!=GET_STATUS)
  {
    fprintf(stderr,"Token %d is not GET_STATUS.\n",token);
    return EXIT_FAILURE;
  }
  if(islanduse)
    token=COPAN_OK;
  else
  {
    token=COPAN_ERR;
    writeint_socket(socket,&token,1);
    fprintf(stderr,"Landuse output is missing from LPJmL.\n");
    return EXIT_FAILURE;
  }
  writeint_socket(socket,&token,1);
  n_out-=n_err;
  /* read all static non time dependent outputs */
  region=country=NULL;
  coords=NULL;
  fcoords=NULL;
  for(i=0;i<n_out_1;i++)
  {
    readint_socket(socket,&token,1);
    if(token!=PUT_DATA)
    {
      fprintf(stderr,"Token for output data=%d is not PUT_DATA.\n",token);
      return EXIT_FAILURE;
    }
    if(readint_socket(socket,&index,1))
    {
      fprintf(stderr,"Error reading index of output.\n");
      return EXIT_FAILURE;
    }
    switch(index)
    {
      case GRID:
        if(type[GRID]==LPJ_SHORT)
        {
          coords=newvec(Intcoord,ncell);
          check(coords);
          readshort_socket(socket,(short *)coords,ncell*2);
          printf("Grid:\n");
          for(j=0;j<ncell;j++)
            printf("%g %g\n",coords[j].lat*.01,coords[j].lon*.01);
        }
        else
        {
          fcoords=newvec(struct coord,ncell);
          check(fcoords);
          readfloat_socket(socket,(float *)fcoords,ncell*2);
          printf("Grid:\n");
          for(j=0;j<ncell;j++)
            printf("%g %g\n",fcoords[j].lat,fcoords[j].lon);
        }
        break;
      case COUNTRY:
        country=newvec(short,ncell);
        check(country);
        readshort_socket(socket,country,ncell);
        break;
      case REGION:
        region=newvec(short,ncell);
        check(region);
        readshort_socket(socket,region,ncell);
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
      readint_socket(socket,&token,1);
      if(token==END_DATA) /* Did we receive end token? */
        break;
      if(token==FAIL_DATA)
      {
        fprintf(stderr,"LPJmL stopped with error.\n");
        close_socket(socket);
        return EXIT_FAILURE;
      }
      if(token!=GET_DATA)
      {
        fprintf(stderr,"Token for input data=%d is not GET_DATA.\n",token);
        return EXIT_FAILURE;
      }
      if(readint_socket(socket,&index,1))
      {
        fprintf(stderr,"Error reading index of input.\n");
        return EXIT_FAILURE;
      }
      if(index<0 || index>=N_IN)
      {
        fprintf(stderr,"Invalid index %d of input.\n",index);
        return EXIT_FAILURE;
      }
      readint_socket(socket,&year,1);
      switch(index)
      {
        case LANDUSE_DATA:
          for(j=0;j<ncell*LANDUSE_NBANDS;j++)
            landuse[j]=0.001;
          writefloat_socket(socket,landuse,ncell*LANDUSE_NBANDS);
          break;
        case CO2_DATA:
          co2=288.0;
          writefloat_socket(socket,&co2,1);
          break;
        default:
          fprintf(stderr,"Unsupported index %d of input.\n",index);
          return EXIT_FAILURE;
      }
    }
    if(token==END_DATA) /* Did we receive end token? */
      break;
    /* get output from LPJmL */
    if(readyeardata(socket,ncell,nday_out,nmonth_out,n_out,count,type))
      break;
  } /* of for(;;) */
  printf("End of communication.\n");
  close_socket(socket);
  return EXIT_SUCCESS;
} /* of 'main' */
