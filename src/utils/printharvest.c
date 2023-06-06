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

#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "crop.h"
#include "grassland.h"

#define NTYPES 3 /* number of PFT types: grass, tree, crop */

#define USAGE "Usage: %s [-pp cmd] [-outpath dir] [-inpath dir] [[-Dmacro[=value]] [-Idir] ...] filename\n"

#define fread_harvest(file,harvest) if(fread(harvest,sizeof(float),1,file)!=1) \
  { \
    fprintf(stderr,"Error: Unexpected end of file in '%s'.\n",config.outputvars[index].filename.name);\
    return EXIT_FAILURE; \
  }

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
  /* Create array of functions, uses the typedef of Pfttype in config.h */
  Pfttype scanfcn[NTYPES]=
  {
    {name_grass,fscanpft_grass},
    {name_tree,fscanpft_tree},
    {name_crop,fscanpft_crop}
  };
  Config config;         /* LPJ configuration */
  Real *area;
  Coord coord;
  Coordfile coordfile;
  String s;
  const char *name;
  Intcoord intcoord;
  Coord_netcdf cdf;
  FILE *file;
  int i,j,n,index,cell,year;
  float harvest,lon,lat;
  Real scale;
  Real harvest_total;
  Landfrac *harvest_sum;
  initconfig(&config);
  if(readconfig(&config,scanfcn,NTYPES,NOUT,&argc,&argv,USAGE))
  {
    fputs("Error occurred in processing configuration file.\n",stderr);
    return EXIT_FAILURE;
  }
  printf("Simulation: %s\n",config.sim_name);
  if(config.soil_filename.fmt==CDF)
  {
    cdf=opencoord_netcdf(config.soil_filename.name,
                         config.soil_filename.var,
                         TRUE);
    if(cdf==NULL)
      return EXIT_FAILURE;
    getresolution_netcdf(cdf,&config.resolution);
    closecoord_netcdf(cdf);
  }
  else
  {
    /* get resolution from grid file */
    coordfile=opencoord(&config.coord_filename,TRUE);
    if(coordfile==NULL)
      return EXIT_FAILURE;
    getcellsizecoord(&lon,&lat,coordfile);
    closecoord(coordfile);
    config.resolution.lon=lon;
    config.resolution.lat=lat;
  }
  index=findfile2(config.outputvars,config.n_out,GRID);
  if(index==NOT_FOUND)
  {
    fputs("Error: No gridfile was written in simulation.\n",stderr);
    return EXIT_FAILURE;
  }
  if(config.outputvars[index].filename.fmt!=RAW)
  {
    fprintf(stderr,"Error: grid file '%s' not in raw format.\n",
            config.outputvars[index].filename.name);
    return EXIT_FAILURE;
  }
  file=fopen(config.outputvars[index].filename.name,"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",
            config.outputvars[index].filename.name,strerror(errno));
    return EXIT_FAILURE;
  }
  n=getfilesizep(file)/sizeof(Intcoord);
  if(n==0)
  {
    fprintf(stderr,"Error: No coordinates written in grid file '%s'.\n",
            config.outputvars[index].filename.name);
    return EXIT_FAILURE;
  }
  area=newvec(Real,n);
  if(area==NULL)
  {
    printallocerr("area");
    return EXIT_FAILURE;
  }
  for(i=0;i<n;i++)
  {
    fread(&intcoord,sizeof(Intcoord),1,file);
    coord.lon=intcoord.lon*0.01;
    coord.lat=intcoord.lat*0.01;
    area[i]=cellarea(&coord,&config.resolution);
  }
  fclose(file);
  index=findfile2(config.outputvars,config.n_out,PFT_HARVESTC);
  if(index==NOT_FOUND)
  {
    fputs("Error: No harvest file was written in simulation.\n",stderr);
    return EXIT_FAILURE;
  }
  if(config.outputvars[index].filename.fmt!=RAW)
  {
    fprintf(stderr,"Error: harvest file '%s' not in raw format.\n",
            config.outputvars[index].filename.name);
    return EXIT_FAILURE;
  }
  file=fopen(config.outputvars[index].filename.name,"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",
            config.outputvars[index].filename.name,strerror(errno));
    return EXIT_FAILURE;
  }
  printf("Unit: %s\n",(config.ngridcell<100) ? "Gg" : "Pg");
  printf("Year");
  for(i=0;i<config.npft[CROP];i++)
    printf(" %-7s",shorten(s,config.pftpar[i+config.npft[GRASS]+config.npft[TREE]].name,7));
  for(i=0;i<config.nagtree;i++)
    printf(" %-7s",shorten(s,config.pftpar[i+config.npft[GRASS]+config.npft[TREE]-config.nagtree].name,7));
  for(i=0;i<NGRASS;i++)
    printf(" %-7s",shorten(s,grassland_names[i],7));
  for(i=0;i<NBIOMASSTYPE;i++)
    printf(" %-7s",shorten(s,biomass_names[i],7));
  for(i=0;i<config.nwptype;i++)
    printf(" %-7s",shorten(s,woodplantation_names[i],7));
  printf(" Total\n");
  for(j=1;j<=2;j++)
  {
    printf("    ");
    for(i=0;i<config.npft[CROP];i++)
    {
      name=config.pftpar[i+config.npft[GRASS]+config.npft[TREE]].name;
      printf(" %-7s",(strlen(name)>7*j) ? shorten(s,name+7*j,7) : "");
    }
    for(i=0;i<config.nagtree;i++)
    {
      name=config.pftpar[i+config.npft[GRASS]+config.npft[TREE]-config.nagtree].name;
      printf(" %-7s",(strlen(name)>7*j) ? shorten(s,name+7*j,7) : "");
    }
    for(i=0;i<NGRASS;i++)
    {
      name=grassland_names[i];
      printf(" %-7s",(strlen(name)>7*j) ? shorten(s,name+7*j,7) : "");
    }
    for(i=0;i<NBIOMASSTYPE;i++)
    {
      name=biomass_names[i];
      printf(" %-7s",(strlen(name)>7*j) ? shorten(s,name+7*j,7) : "");
    }
    for(i=0;i<config.nwptype;i++)
    {
      name=woodplantation_names[i];
      printf(" %-7s",(strlen(name)>7*j) ? shorten(s,name+7*j,7) : "");
    }
    printf("\n");
  }
  harvest_sum=newlandfrac(config.npft[CROP],config.nagtree);
  printf("----");
  for(i=0;i<config.npft[CROP]+config.nagtree+NGRASS+NBIOMASSTYPE+config.nwptype+1;i++)
    printf(" -------");
  printf("\n");
  for(year=config.firstyear;year<=config.lastyear;year++)
  {
    for(i=0;i<2;i++)
    {
      for(j=0;j<config.npft[CROP];j++)
      {
        harvest_sum[i].crop[j]=0;
        for(cell=0;cell<n;cell++)
        {
          fread_harvest(file,&harvest);
          harvest_sum[i].crop[j]+=harvest*area[cell];
        }
      }
      for(j=0;j<NGRASS;j++)
      {
        harvest_sum[i].grass[j]=0;
        for(cell=0;cell<n;cell++)
        {
          fread_harvest(file,&harvest);
          harvest_sum[i].grass[j]+=harvest*area[cell];
        }
      }
      harvest_sum[i].biomass_grass=0;
      for(cell=0;cell<n;cell++)
      {
        fread_harvest(file,&harvest);
        harvest_sum[i].biomass_grass+=harvest*area[cell];
      }
      harvest_sum[i].biomass_tree=0;
      for(cell=0;cell<n;cell++)
      {
        fread_harvest(file,&harvest);
        harvest_sum[i].biomass_tree+=harvest*area[cell];
      }
      if(config.nwptype)
        for(cell=0;cell<n;cell++)
        {
          fread_harvest(file,&harvest);
          harvest_sum[i].woodplantation+=harvest*area[cell];
        }
      for(j=0;j<config.nagtree;j++)
      {
        harvest_sum[i].ag_tree[j]=0;
        for(cell=0;cell<n;cell++)
        {
          fread_harvest(file,&harvest);
          harvest_sum[i].ag_tree[j]+=harvest*area[cell];
        }
      }
    }
    harvest_total=0;
    printf("%4d",year);
    scale=(config.ngridcell<100) ? 1e-9 : 1e-15;
    for(i=0;i<config.npft[CROP];i++)
    {
      printf(" %7.2f",(harvest_sum[0].crop[i]+harvest_sum[1].crop[i])*scale);
      harvest_total+=harvest_sum[0].crop[i]+harvest_sum[1].crop[i];
    }
    for(i=0;i<config.nagtree;i++)
    {
      printf(" %7.2f",(harvest_sum[0].ag_tree[i]+harvest_sum[1].ag_tree[i])*scale);
      harvest_total+=harvest_sum[0].ag_tree[i]+harvest_sum[1].ag_tree[i];
    }
    for(i=0;i<NGRASS;i++)
    {
      printf(" %7.2f",(harvest_sum[0].grass[i]+harvest_sum[1].grass[i])*scale);
      harvest_total+=harvest_sum[0].grass[i]+harvest_sum[1].grass[i];
    }
    printf(" %7.2f",(harvest_sum[0].biomass_grass+harvest_sum[1].biomass_grass)*scale);
    harvest_total+=harvest_sum[0].biomass_grass+harvest_sum[1].biomass_grass;
    printf(" %7.2f",(harvest_sum[0].biomass_tree+harvest_sum[1].biomass_tree)*scale);
    harvest_total+=harvest_sum[0].biomass_tree+harvest_sum[1].biomass_tree;
    if(config.nwptype)
    {
      harvest_total+=harvest_sum[0].woodplantation+harvest_sum[1].woodplantation;
      printf(" %7.2f",(harvest_sum[0].woodplantation+harvest_sum[1].woodplantation)*scale);
    }
    printf(" %7.1f\n",harvest_total*scale);
  }
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
