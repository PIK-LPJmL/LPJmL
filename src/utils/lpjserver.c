/**************************************************************************************/
/**                                                                                \n**/
/**                l  p  j  s  e  r  v  e  r  .  c                                 \n**/
/**                                                                                \n**/
/**     Live view for LPJmL Version 3.5.003 runs using socket connection           \n**/
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
#include "tree.h"
#include "crop.h"
#include "grass.h"

#define NTYPES 3
#define LPJSERVER_VERSION "0.9.005"
#define USAGE "%s [LPJargs ...]"
#define dflt_filename "lpjml.conf"

int main(int argc,char **argv)
{
  Socket *socket;
  float **data;
  struct
  {
    FILE *file;
    int count;
  } files[NOUT];
  int total,year,index,i,j,count_max,sub,nmonth,month;
  char id[3];
  Intcoord *coords;
  short *svec,**sdata;
  Bool isgrid,isregion,iscountry;
  Config config;
  String line;
  const char *progname;
  const char *title[3];
  time_t start,end;
  Flux flux;
  Fscanpftparfcn scanfcn[NTYPES]={fscanpft_grass,fscanpft_tree,fscanpft_crop};
  progname=strippath(argv[0]);
  config.rank=0;
  config.ntask=1;
  snprintf(line,STRING_LEN,                                               
           "%s C Version %s (" __DATE__ ")",progname,LPJSERVER_VERSION);
  title[0]=line;
  title[1]="Save output data for LPJmL Version " LPJ_VERSION;
  title[2]="(c)2008-2015 PIK Potsdam";
  banner(title,3,78);
  if(readconfig(&config,dflt_filename,scanfcn,NTYPES,NOUT,&argc,&argv,USAGE))
    fail(READ_CONFIG_ERR,FALSE,"Error opening config");
  socket=open_socket(config.port,0);
  if(socket==NULL)
  {
    fprintf(stderr,"Error opening communication channel at port %d: %s\n",
            config.port,strerror(errno));
    return EXIT_FAILURE;
  }
  /* Establish the connection */
  printconfig(&config,config.npft[GRASS]+config.npft[TREE],
              config.npft[CROP]);
  read_socket(socket,id,3);
  if(strncmp(id,"LPJ",3))
  {
    fprintf(stderr,"Invalid id opening communication channel\n");
    return EXIT_FAILURE;
  }
  readint_socket(socket,&total,1);
  if(total<1 || total>config.ngridcell)
  {
    fprintf(stderr,"Invalid number of cells=%d\n",total);
    return EXIT_FAILURE;
  }
  isgrid=isregion=iscountry=FALSE;
  for(i=0;i<NOUT;i++)
    files[i].file=NULL;
  count_max=0;
  sub=nmonth=0;
  for(i=0;i<config.n_out;i++)
  {
    switch(config.outputvars[i].id)
    {
      case GRID:
        isgrid=TRUE;
        sub++;
        break;
      case REGION:
        isregion=TRUE;
        sub++;
        break;
      case COUNTRY:
        iscountry=TRUE;
        sub++;
        break;
      default:
        if(ismonthlyoutput(config.outputvars[i].id))
        {
          nmonth++;
          sub++;
        }
        break;
    }
    files[config.outputvars[i].id].file=fopen(config.outputvars[i].filename.name,"wb");
    files[config.outputvars[i].id].count=outputsize(config.outputvars[i].id,config.npft[TREE]+config.npft[GRASS],config.nbiomass,config.npft[CROP]);
    if(files[config.outputvars[i].id].count>count_max)
      count_max=files[config.outputvars[i].id].count;
  }
  if(!isgrid)
  {
    fprintf(stderr,"Grid output is missing.\n");
    return EXIT_FAILURE;
  }
  coords=newvec(Intcoord,total);
  readshort_socket(socket,(short *)coords,2*total);
  fwrite(coords,sizeof(Intcoord),total,files[GRID].file);
  free(coords);
  if(iscountry)
  {
    svec=newvec(short,total);
    readshort_socket(socket,svec,total);
    fwrite(svec,sizeof(short),total,files[COUNTRY].file);
    free(svec);
  }
  if(isregion)
  {
    svec=newvec(short,total);
    readshort_socket(socket,svec,total);
    fwrite(svec,sizeof(short),total,files[REGION].file);
    free(svec);
  }
  if(files[SDATE].file!=NULL || files[HDATE].file!=NULL)
  {
    sdata=newmatrix(short,files[SDATE].count,total);
    check(sdata);
  }
  data=newmatrix(float,count_max,total);
  check(data);
  time(&start);
  for(year=config.outputyear;year<=config.lastyear;year++)
  {
    for(j=0;j<nmonth;j++)
    {
      for(month=0;month<12;month++)
      {
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
        readfloat_socket(socket,data[0],total);
        fwrite(data[0],sizeof(float),total,files[index].file);
      }
    }

    for(i=0;i<config.n_out-sub;i++)
    {
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
      if(index==SDATE || index==HDATE)
        for(j=0;j<files[index].count;j++)
        {
          readshort_socket(socket,sdata[j],total);
          fwrite(sdata[j],sizeof(short),total,files[index].file);
        }
      else
        for(j=0;j<files[index].count;j++)
        {
          readfloat_socket(socket,data[j],total);
          fwrite(data[j],sizeof(float),total,files[index].file);
        }
    }
    readdouble_socket(socket,(Real *)&flux,sizeof(Flux)/sizeof(Real));
    if((year-config.outputyear) % 20==0)
      printf("\nYear NEP     fire    estab   harvest total\n"
               "---- ------- ------- ------- ------- -------\n");
    printf("%4d %7.3f %7.3f %7.3f %7.3f %7.3f\n",
           year,flux.nep*1e-15,flux.fire*1e-15,flux.estab*1e-15,
           flux.harvest*1e-15,(flux.nep-flux.fire-flux.harvest+flux.estab)*1e-15);
  }
  time(&end);
  printf("Frame rate: %.1f (1/sec)\n",
         (config.lastyear-config.outputyear+1)/(float)(end-start));
  for(i=0;i<NOUT;i++)
   if(files[i].file!=NULL)
     fclose(files[i].file);
  close_socket(socket);
  return EXIT_SUCCESS;
} /* of 'main' */
