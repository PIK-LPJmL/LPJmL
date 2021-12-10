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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "lpj.h"

#define COPANDEMO_VERSION "0.9.001"

int main(int argc,char **argv)
{
  Socket *socket;
  float *data,*landuse;
  float co2;
  Intcoord *coords;
  int count[NOUT];
  int port;
  String line;
  const char *progname;
  const char *title[3];
  int i,j,k,token,n_out,n_in,total,index,year,version,n_out_1;
  progname=strippath(argv[0]);
  snprintf(line,STRING_LEN,                                               
           "%s C Version %s (" __DATE__ ")",progname,COPANDEMO_VERSION);
  title[0]=line;
  title[1]="COPAN demo for LPJmL";
  title[2]="(c)2008-2015 PIK Potsdam";
  banner(title,3,78);
  port=DEFAULT_COPAN_PORT;
  /* Establish the connection */
  socket=opentdt_socket(port,0);
  if(socket==NULL)
  {
    fprintf(stderr,"Error opening communication channel at port %d: %s\n",
            port,strerror(errno));
    return EXIT_FAILURE;
  }
  /* Get protocol version */
  readint_socket(socket,&version,1);
  if(version!=COPAN_COUPLER_VERSION)
  {
    fprintf(stderr,"Unsupported coupler version %d.\n",version);
    return EXIT_FAILURE;
  }
  printf("Version: %d\n",version);
  readint_socket(socket,&total,1);
  printf("Number of cells: %d\n",total);
  readint_socket(socket,&n_in,1);
  readint_socket(socket,&n_out,1);
  printf("Number of input streams: %d\n"
         "Number of output streams: %d\n",
         n_in,n_out);
  /* Send number of items per cell for each input data strean */
  for(i=0;i<n_in;i++)
  {
    readint_socket(socket,&token,1);
    if(token!=GET_DATA_SIZE)
    {
      fprintf(stderr,"Token=%d is not GET_DATA_SIZE.\n",token);
      return EXIT_FAILURE;
    }
    readint_socket(socket,&index,1);
    if(index<0 || index>=N_IN)
    {
      fprintf(stderr,"Invalid index %d of input\n",index);
      return EXIT_FAILURE;
    }
    switch(index)
    {
      case LANDUSE_DATA:
        /* send number of bands */
        index=64;
        landuse=newvec(float,total*64);
        break;
      case CO2_DATA:
        index=1;
        break;
      default:
        fprintf(stderr,"Invalid index %d of input\n",index);
        return EXIT_FAILURE;
    }
    writeint_socket(socket,&index,1);
  }
  /* Get number of items per cell for each output data strean */
  for(i=0;i<NOUT;i++)
    count[i]=-1;
  n_out_1=0;
  for(i=0;i<n_out;i++)
  {
    readint_socket(socket,&token,1);
    if(token!=PUT_DATA_SIZE)
    {
      fprintf(stderr,"Token=%d is not PUT_DATA_SIZE.\n",token);
      return EXIT_FAILURE;
    }
    readint_socket(socket,&index,1);
    if(index<0 || index>=NOUT)
    {
      fprintf(stderr,"Invalid index %d of output\n",index);
      return EXIT_FAILURE;
    }
    /* get number of bands for output */
    readint_socket(socket,count+index,1);
    if(index==GRID || index==COUNTRY || index==REGION)
      n_out_1++;
  }
  for(i=0;i<n_out_1;i++)
  {
    readint_socket(socket,&token,1);
    if(token!=PUT_DATA)
    {
      fprintf(stderr,"Token for input data=%d is not PUT_DATA.\n",token);
      return EXIT_FAILURE;
    }
    if(readint_socket(socket,&index,1))
    {
      fprintf(stderr,"Error reading index of input\n");
      return EXIT_FAILURE;
    }
    switch(index)
    {
      case GRID:
        coords=newvec(Intcoord,total);
        readshort_socket(socket,(short *)coords,total*2);
        for(j=0;j<total;j++)
          printf("%g %g\n",coords[j].lat*.01,coords[j].lon*.01);
        break;
      default:
      {
        fprintf(stderr,"Invalid index %d of input\n",index);
        return EXIT_FAILURE;
      }
    }
  }
  n_out-=n_out_1;
  data=newvec(float,total);
  /* main simulation loop */
  for(;;)
  {
    /* get input from LPJmL */
    for(i=0;i<n_in;i++)
    {
      readint_socket(socket,&token,1);
      if(token==END_DATA) /* Did we receive end token? */
        break;
      if(token!=GET_DATA)
      {
        fprintf(stderr,"Token for input data=%d is not GET_DATA.\n",token);
        return EXIT_FAILURE;
      }
      if(readint_socket(socket,&index,1))
      {
        fprintf(stderr,"Error reading index of input\n");
        return EXIT_FAILURE;
      }
      if(index<0 || index>=N_IN)
      {
        fprintf(stderr,"Invalid index %d of input\n",index);
        return EXIT_FAILURE;
      }
      switch(index)
      {
        case LANDUSE_DATA:
          readint_socket(socket,&year,1);
          for(j=0;j<total*64;j++)
            landuse[j]=0.001;
          writefloat_socket(socket,landuse,total*64);
          break;
        case CO2_DATA:
          co2=288.0;
          readint_socket(socket,&year,1);
          writefloat_socket(socket,&co2,1);
          break;
        default:
          fprintf(stderr,"Invalid index %d of input\n",index);
          return EXIT_FAILURE;
      }
    }
    if(token==END_DATA) /* Did we receive end token? */
      break;
    /* get output from LPJmL */
    for(i=0;i<n_out;i++)
    {
      readint_socket(socket,&token,1);
      if(token==END_DATA)
        break; /* Did we receive end token? */
      if(token!=PUT_DATA)
      {
        fprintf(stderr,"Token for output data=%d is not PUT_DATA.\n",token);
        return EXIT_FAILURE;
      }
      if(readint_socket(socket,&index,1))
      {
        fprintf(stderr,"Error reading index of output\n");
        return EXIT_FAILURE;
      }
      if(index<0 || index>=NOUT)
      {
        fprintf(stderr,"Invalid index %d of output\n",index);
        return EXIT_FAILURE;
      }
      if(count[index]<0)
      {
        fprintf(stderr,"No output define for index %d.\n",index);
        return EXIT_FAILURE;
      }
      readint_socket(socket,&year,1);
      for(j=0;j<count[index];j++)
      {
        readfloat_socket(socket,data,total);
        printf("%d %d[%d]:",year,index,j);
        for(k=0;k<total;k++)
          printf(" %g",data[k]);
        printf("\n");
      }
    }
    if(token==END_DATA) /* Did we receive end token? */
      break;
  } /* of for(;;) */
  printf("End of communication.\n");
  close_socket(socket);
  return EXIT_SUCCESS;
} /* of 'main' */
