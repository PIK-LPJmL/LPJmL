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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static int findintcoord(Intcoord c,const Intcoord *c2,int n)
{
  int i;
  for(i=0;i<n;i++)
    if(c2[i].lon==c.lon && c2[i].lat==c.lat)
      return i;
  return NOT_FOUND;
} /* of 'findintcoord' */

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
  FILE *file;
  Header header,header2,header3,header4;
  Intcoord *c,*c2,*cnew;
  Routing *r;
  Bool swap;
  int version,index,from,to;
  int i,j,data_version,setversion;
  String id;
  setversion=READ_VERSION;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-longheader"))
        setversion=2;
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
            "Usage: %s [-longheader] coord_all.clm coord.clm drainage.clm coord_basin.clm\n",
            argv[1-i]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[1],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  version=setversion;
  if(freadheader(file,&header,&swap,LPJGRID_HEADER,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  c=newvec(Intcoord,header.ncell);
  if(c==NULL)
  {
    fclose(file);
    printallocerr("c");
    return EXIT_FAILURE;
  }
  for(i=0;i<header.ncell;i++)
    if(readintcoord(file,c+i,swap))
    {
      fclose(file);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
    }
  fclose(file);
  file=fopen(argv[2],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[2],strerror(errno));
    return EXIT_FAILURE;
  }
  version=setversion;
  if(freadheader(file,&header2,&swap,LPJGRID_HEADER,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[2]);
    fclose(file);
    return EXIT_FAILURE;
  }
  c2=newvec(Intcoord,header2.ncell);
  if(c2==NULL)
  {
    fclose(file);
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<header2.ncell;i++)
    if(readintcoord(file,c2+i,swap))
    {
      fclose(file);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[2]);
      return EXIT_FAILURE;
    }
  fclose(file);
  file=fopen(argv[3],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  data_version=setversion;
  if(freadanyheader(file,&header3,&swap,id,&data_version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[3]);
    return EXIT_FAILURE;
  }
  if(header3.ncell!=header.ncell)
  {
    fprintf(stderr,"Invalid number of cells %d in '%s', not %d.\n",
            header3.ncell,argv[3],header.ncell);
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
  cnew=newvec(Intcoord,header.ncell);
  if(cnew==NULL)
  {
    printallocerr("cnew");
    return EXIT_FAILURE;
  }
  for(i=0;i<header2.ncell;i++)
    cnew[i]=c2[i];
  file=fopen(argv[4],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[4],strerror(errno));
    return EXIT_FAILURE;
  }
  header4=header2;
  header4.ncell=header2.ncell;
  for(i=0;i<header4.ncell;i++)
  {
    index=findintcoord(cnew[i],c,header.ncell);
    if(index==NOT_FOUND)
    {
      fprintf(stderr,"Coordinate not found.\n");
      return EXIT_FAILURE;
    }
    for(j=0;;)
    {
      from=findroute(index,r+j,header.ncell-j);
      if(from==NOT_FOUND)
        break;
      from+=j;
      to=findintcoord(c[from],cnew,header4.ncell);
      if(to==NOT_FOUND) 
      {
        cnew[header4.ncell]=c[from];
        header4.ncell++;
      }
      j=from+1;
    }
    to=r[index].index;
    if(to>=0)
    {
       index=findintcoord(c[to],cnew,header4.ncell);
       if(index==NOT_FOUND)
       {
         cnew[header4.ncell]=c[to];
         header4.ncell++;
       }
     }
  }
  if(fwriteheader(file,&header4,LPJGRID_HEADER,version))
  {
    fprintf(stderr,"Error writing header in '%s'.\n",argv[4]);
    return EXIT_FAILURE;
  }
  if(fwrite(cnew,sizeof(Intcoord),header4.ncell,file)!=header4.ncell)
  {
    fprintf(stderr,"Error writing data in '%s'.\n",argv[4]);
    return EXIT_FAILURE;
  }
  fclose(file);
  if(header4.ncell-header2.ncell>0)
    printf("%d cells added.\n",header4.ncell-header2.ncell);
  else
    puts("No cells added.");
  return EXIT_SUCCESS;
} /* of 'main' */
