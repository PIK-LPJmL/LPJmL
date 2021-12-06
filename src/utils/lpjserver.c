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
#define dflt_filename "lpjml.js"

int main(int argc,char **argv)
{
  Socket *socket;
  float **data,*landuse;
  int count[NOUT];
  int total,year,index,i,j,k,count_max,n_out,nmonth,month,version;
  short **sdata;
  Config config;
  String line;
  const char *progname;
  const char *title[3];
  Pfttype scanfcn[NTYPES]=
  {
    {name_grass,fscanpft_grass},
    {name_tree,fscanpft_tree},
    {name_crop,fscanpft_crop}
  };
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
#if 0
  outsocket=connecttdt_socket("localhost",config.copan_inport);
  if(outsocket==NULL)
  {
    fprintf(stderr,"Error opening communication channel at port %d: %s\n",
            config.copan_inport,strerror(errno));
    return EXIT_FAILURE;
  }
#endif
  socket=opentdt_socket(config.copan_port,0);
  if(socket==NULL)
  {
    fprintf(stderr,"Error opening communication channel at port %d: %s\n",
            config.copan_port,strerror(errno));
    return EXIT_FAILURE;
  }
  /* Establish the connection */
  printconfig(config.npft[GRASS]+config.npft[TREE],config.npft[CROP],&config);
  readint_socket(socket,&version,1);
  printf("Version: %d\n",version);
  readint_socket(socket,&total,1);
  if(total<1 || total>config.ngridcell)
  {
    fprintf(stderr,"Invalid number of cells=%d\n",total);
    return EXIT_FAILURE;
  }
  //printf("Number of cells: %d\n",total);
  for(i=0;i<NOUT;i++)
    count[i]=-1;
  count_max=0;
  n_out=nmonth=0;
  for(i=0;i<config.n_out;i++)
  {
    if(config.outputvars[i].filename.fmt==SOCK)
    {
      if(config.outnames[config.outputvars[i].id].timestep==MONTHLY)
        nmonth++;
      n_out++;
      count[config.outputvars[i].id]=outputsize(config.outputvars[i].id,config.npft[TREE]+config.npft[GRASS],config.npft[CROP],&config);
      //printf("%s:%d\n",config.outnames[config.outputvars[i].id].name,count[config.outputvars[i].id]);
      if(count[config.outputvars[i].id]>count_max)
        count_max=count[config.outputvars[i].id];
    }
  }
  if(count[SDATE]!=-1 || count[HDATE]!=-1)
  {
    sdata=newmatrix(short,count[SDATE],total);
    check(sdata);
  }
  data=newmatrix(float,count_max,total);
  check(data);
  if(config.landuse_filename.fmt==SOCK)
  {
    landuse=newvec(float,config.nall*outputsize(PFT_HARVESTC,config.npft[TREE]+config.npft[GRASS],config.npft[CROP],&config)*2);
    index=outputsize(PFT_HARVESTC,config.npft[TREE]+config.npft[GRASS],config.npft[CROP],&config)*2;
    if(writeint_socket(socket,&index,1))
    {
      fprintf(stderr,"Error writing size of landuse data of output\n");
      return EXIT_FAILURE;
    }
  }
  for(year=config.firstyear;year<=config.lastyear;year++)
  {
    if(config.landuse_filename.fmt==SOCK)
    { 
      for(k=0;k<config.nall*outputsize(PFT_HARVESTC,config.npft[TREE]+config.npft[GRASS],config.npft[CROP],&config)*2;k++)
        landuse[k]=.0001;
        writefloat_socket(socket,landuse,config.nall*outputsize(PFT_HARVESTC,config.npft[TREE]+config.npft[GRASS],config.npft[CROP],&config)*2);
    }
    if(year>=config.outputyear)
    {
    for(j=0;j<nmonth;j++)
    {
      for(month=0;month<11;month++)
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
        printf("%s:",config.outnames[index].name);
        for(k=0;k<total;k++)
          printf(" %g",data[0][k]);
        printf("\n");
      }
    }

    for(i=0;i<n_out;i++)
    {
      if(readint_socket(socket,&index,1))
      {
        fprintf(stderr,"Error reading index of output 0\n");
        return EXIT_FAILURE;
      }
      if(index<0 || index>=NOUT)
      {
        fprintf(stderr,"Invalid index %d of output 0\n",index);
        return EXIT_FAILURE;
      }
      printf("index: %d\n",index);
      for(j=0;j<count[index];j++)
      {
        readfloat_socket(socket,data[j],total);
        printf("%s[%d]:",config.outnames[index].name,j);
        for(k=0;k<total;k++)
          printf(" %g",data[j][k]);
        printf("\n");
      }
    }
    }
  }
  close_socket(socket);
  return EXIT_SUCCESS;
} /* of 'main' */
