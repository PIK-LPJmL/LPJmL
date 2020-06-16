/**************************************************************************************/
/**                                                                                \n**/
/**                    a  d  d  d  r  a  i  n  .  c                                \n**/
/**                                                                                \n**/
/**     Program adds drainage basins to coordinate file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static int findroute(int index,const Routing *r,int n)
{
  int i;
  for(i=0;i<n;i++)
    if(index==r[i].index)
      return i;
  return NOT_FOUND;
} /* of 'findroute' */

int main(int argc,char **argv)
{
  Header header;
  Coordfile coordfile;
  FILE *file;
  Filename filename;
  String id,s;
  Coord *c,*c2,*cnew,res;
  float cellsize_lon,cellsize_lat;
  Routing *r;
  Bool swap,isforward,isshort;
  int index,from,to;
  int i,j,n1,n2,data_version;
  filename.fmt=CLM;
  isforward=FALSE;
  isshort=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-longheader"))
        filename.fmt=CLM2;
      else if(!strcmp(argv[i],"-downstream"))
        isforward=TRUE;
      else if(!strcmp(argv[i],"-short"))
        isshort=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",argv[i]);
        return EXIT_FAILURE;
      } 
    }
    else
      break;
  argc-=i-1;
  argv+=i-1;
  if(argc<5)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            "Usage: %s [-longheader] [-downstream] [-short] coord_all.clm coord.clm drainage.clm coord_basin.clm\n",
            argv[1-i]);
    return EXIT_FAILURE;
  }
  filename.name=argv[1];
  coordfile=opencoord(&filename,TRUE);
  if(coordfile==NULL)
    return EXIT_FAILURE;
  n1=numcoord(coordfile);
  c=newvec(Coord,n1);
  if(c==NULL)
  {
    printallocerr("c");
    return EXIT_FAILURE;
  }
  getcellsizecoord(&cellsize_lon,&cellsize_lat,coordfile);
  res.lon=cellsize_lon;
  res.lat=cellsize_lat;
  for(i=0;i<n1;i++)
    if(readcoord(coordfile,c+i,&res))
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
    }
  closecoord(coordfile);
  filename.name=argv[2];
  coordfile=opencoord(&filename,TRUE);
  if(coordfile==NULL)
    return EXIT_FAILURE;
  n2=numcoord(coordfile);
  c2=newvec(Coord,n2);
  if(c2==NULL)
  {
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<n2;i++)
    if(readcoord(coordfile,c2+i,&res))
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
    }
  closecoord(coordfile);
  file=fopen(argv[3],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  data_version=READ_VERSION;
  if(freadanyheader(file,&header,&swap,id,&data_version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[3]);
    return EXIT_FAILURE;
  }
  if(header.ncell!=n1)
  {
    fprintf(stderr,"Invalid number of cells %d in '%s', not %d.\n",
            header.ncell,argv[3],n1);
    return EXIT_FAILURE;
  }
  r=newvec(Routing,header.ncell);
  if(r==NULL)
  {
    fclose(file);
    printallocerr("r");
    return EXIT_FAILURE;
  }
  if(fread(r,sizeof(Routing),header.ncell,file)!=header.ncell)
  {
    fclose(file);
    fprintf(stderr,"Error reading routing in '%s'.\n",argv[3]);
    return EXIT_FAILURE;
  }
  if(swap)
    for(i=0;i<header.ncell;i++)
      r[i].index=swapint(r[i].index);
  fclose(file); 
  cnew=newvec(Coord,header.ncell);
  if(cnew==NULL)
  {
    printallocerr("cnew");
    return EXIT_FAILURE;
  }
  for(i=0;i<n2;i++)
    cnew[i]=c2[i];
  file=fopen(argv[4],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[4],strerror(errno));
    return EXIT_FAILURE;
  }
  header.datatype=(isshort) ? LPJ_SHORT : LPJ_FLOAT;
  header.ncell=n2;
  header.cellsize_lon=cellsize_lon;
  header.cellsize_lat=cellsize_lat;
  header.firstcell=0;
  header.firstyear=0;
  header.order=0;
  header.nyear=1;
  header.nbands=2;
  header.scalar=(isshort) ? 0.01 : 1;
  for(i=0;i<header.ncell;i++)
  {
    //printf("find %g %g\n",cnew[i].lon,cnew[i].lat);
    index=findcoord(cnew+i,c,n1);
    if(index==NOT_FOUND)
    {
      fprintf(stderr,"Coordinate %s not found.\n",sprintcoord(s,cnew+i));
      return EXIT_FAILURE;
    }
    for(j=0;;)
    {
      from=findroute(index,r+j,n1-j);
      if(from==NOT_FOUND)
        break;
      from+=j;
      to=findcoord(c+from,cnew,header.ncell);
      if(to==NOT_FOUND) 
      {
        /* add cell from upstream */
        cnew[header.ncell]=c[from];
        header.ncell++;
      }
      j=from+1;
    }
    if(isforward)  /* add cells from downstream? */
    {
      to=r[index].index;
      if(to>=0)
      {
         index=findcoord(c+to,cnew,header.ncell);
         if(index==NOT_FOUND)
         {
           cnew[header.ncell]=c[to];
           header.ncell++;
         }
      }
    }
  }
  if(fwriteheader(file,&header,LPJGRID_HEADER,LPJGRID_VERSION))
  {
    fprintf(stderr,"Error writing header in '%s'.\n",argv[4]);
    return EXIT_FAILURE;
  }
  if(isshort)
    for(i=0;i<header.ncell;i++)
      writecoord(file,cnew+i);
  else
    for(i=0;i<header.ncell;i++)
      writefloatcoord(file,cnew+i);
  fclose(file);
  if(header.ncell-n2>0)
    printf("%d cells added.\n",header.ncell-n2);
  else
    puts("No cells added.");
  return EXIT_SUCCESS;
} /* of 'main' */
