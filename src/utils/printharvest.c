/**************************************************************************************/
/**                                                                                \n**/
/**                 p  r  i  n  t  h  a  r  v  e  s  t  .  c                       \n**/
/**                                                                                \n**/
/**     Program prints global harvest carbon for crops and managed land            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <sys/stat.h>
#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "crop.h"

#define NTYPES 3 /* number of PFT types: grass, tree, crop */
#define dflt_conf_filename "lpjml.conf" /* Default LPJ configuration file */
#define USAGE "Usage: %s [-h] [-outpath dir] [-inpath dir] [[-Dmacro[=value]] [-Idir] ...] [filename]\n"

static int findfile2(const Outputvar *output,int n,int id)
{
  int i;
  for(i=0;i<n;i++)
   if(output[i].id==id)
     return i;
  return NOT_FOUND;
} /* of 'findfile2' */

static char *shorten(char *dst,const char *src,int n)
{
  strncpy(dst,src,n);
  dst[n]='\0';
  return dst;
} /* of 'shorten' */

int main(int argc,char **argv)
{
  /* Create array of functions, uses the typedef of (*Fscanpftparfcn) in pft.h */
  Fscanpftparfcn scanfcn[NTYPES]={fscanpft_grass,fscanpft_tree,fscanpft_crop};
  Config config;         /* LPJ configuration */
  Real *area;
  Coord coord;
  String s;
  char *name;
  Intcoord intcoord;
  FILE *file;
  struct stat filestat;
  int i,j,n,index,cell,year;
  float harvest;
  Real harvest_total;
  Landfrac harvest_sum[2];
  initconfig(&config);
  if(readconfig(&config,dflt_conf_filename,scanfcn,NTYPES,NOUT,&argc,&argv,USAGE))
  {
    fprintf(stderr,"Syntax error found in configuration file.\n");
    return EXIT_FAILURE;
  }
  printf("Simulation: %s\n",config.sim_name);
  index=findfile2(config.outputvars,config.n_out,GRID);
  if(index==NOT_FOUND)
  {
    fprintf(stderr,"Error: No gridfile was written in simulation.\n");
    return EXIT_FAILURE;
  }
  file=fopen(config.outputvars[index].filename.name,"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",
            config.outputvars[index].filename.name,strerror(errno));
    return EXIT_FAILURE;
  }
  fstat(fileno(file),&filestat);
  n=filestat.st_size/sizeof(Intcoord);
  if(n==0)
  {
    fprintf(stderr,"Error: No coordinates written in grid file.\n");
    return EXIT_FAILURE;
  }
  area=newvec(Real,n);
  for(i=0;i<n;i++)
  {
    fread(&intcoord,sizeof(Intcoord),1,file);
    coord.lon=intcoord.lon*0.01;
    coord.lat=intcoord.lat*0.01;
    area[i]=cellarea(&coord,&config.resolution);
  }
  fclose(file);
  index=findfile2(config.outputvars,config.n_out,PFT_HARVEST);
  if(index==NOT_FOUND)
  {
    fprintf(stderr,"Error: No harvest file was written in simulation.\n");
    return EXIT_FAILURE;
  }
  file=fopen(config.outputvars[index].filename.name,"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",
            config.outputvars[index].filename.name,strerror(errno));
    return EXIT_FAILURE;
  }
  printf("Year");
  for(i=0;i<config.npft[CROP];i++)
    printf(" %-7s",shorten(s,config.pftpar[i+config.npft[GRASS]+config.npft[TREE]].name,7));
  printf(" %-7s %-7s Total\n","Pasture","Others");
  for(j=1;j<=2;j++)
  {
    printf("    ");
    for(i=0;i<config.npft[CROP];i++)
    {
      name=config.pftpar[i+config.npft[GRASS]+config.npft[TREE]].name;
      printf(" %-7s",(strlen(name)>7*j) ? shorten(s,name+7*j,7) : "");
    }
    printf("\n");
  }
  newlandfrac(harvest_sum,config.npft[CROP]);
  printf("----");
  for(i=0;i<config.npft[CROP];i++)
     printf(" -------");
  printf(" ------- ------- ------\n");
  for(year=config.firstyear;year<=config.lastyear;year++)
  {
    for(i=0;i<2;i++)
    {
      for(j=0;j<config.npft[CROP];j++)
      {
        harvest_sum[i].crop[j]=0; 
        for(cell=0;cell<n;cell++)
        {
          fread(&harvest,sizeof(harvest),1,file);
          harvest_sum[i].crop[j]+=harvest*area[cell];
        }
      }
      for(j=0;j<NGRASS;j++)
      {
        harvest_sum[i].grass[j]=0; 
        for(cell=0;cell<n;cell++)
        {
          fread(&harvest,sizeof(harvest),1,file);
          harvest_sum[i].grass[j]+=harvest*area[cell];
        }
      }
    }
    harvest_total=0;
    printf("%4d",year);
    for(i=0;i<config.npft[CROP];i++)
    {
      printf(" %7.2f",(harvest_sum[0].crop[i]+harvest_sum[1].crop[i])*1e-15);
      harvest_total+=harvest_sum[0].crop[i]+harvest_sum[1].crop[i];
    }
    for(i=0;i<NGRASS;i++)
    {
      printf(" %7.2f",(harvest_sum[0].grass[i]+harvest_sum[1].grass[i])*1e-15);
      harvest_total+=harvest_sum[0].grass[i]+harvest_sum[1].grass[i];
   }
    printf("%7.2f\n",harvest_total*1e-15);
  }
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
