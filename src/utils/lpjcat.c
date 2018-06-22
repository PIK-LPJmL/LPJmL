/**************************************************************************************/
/**                                                                                \n**/
/**                l  p  j  c  a  t  .  c                                          \n**/
/**                                                                                \n**/
/**     LPJ utility programme. Concatenates Restart files to stdout                \n**/
/**     Program is obsolete for the MPI version of LPJ                             \n**/
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

#define USAGE "Usage: %s [-o filename] lpjfile ...\n"

typedef struct
{
  FILE *file;
  Header header;
} Item;

static int compare(const Item *a,const Item *b)
{
  if(a->header.firstcell<b->header.firstcell)
    return -1;
  else if(a->header.firstcell==b->header.firstcell)
    return 0;
  else
    return 1;
} /* of 'compare' */

int main(int argc,char **argv)
{
  int i,len,header_offset,o_offset,offset,j,ncell,swap,count;
  Item *item;
  int ptr,version;
  void *data;
  Header header;
  Restartheader restartheader;
  struct stat filestat;
  FILE *out;
  out=stdout;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-o"))
      {
        out=fopen(argv[++i],"wb");
        if(out==NULL)
        {
          fprintf(stderr,"Error creating '%s': %s\n",argv[i],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[i],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  item=(Item *)malloc(sizeof(Item)*(argc-i));
  ncell=count=0;
  for(;i<argc;i++)
  {
    item[count].file=fopen(argv[i],"rb");
    if(item[count].file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[i],strerror(errno));
      continue;
    }
    version=READ_VERSION;
    if(freadheader(item[count].file,&item[count].header,&swap,RESTART_HEADER,&version))
    {
      fprintf(stderr,"Error reading header of file '%s'\n",argv[i]);
      continue;
    }
    if(version!=RESTART_VERSION)
    {
      fprintf(stderr,"Invalid version of file '%s'\n",argv[i]);
      continue;
    }
    if(swap)
    {
      fprintf(stderr,"Unsupported byte order in file '%s'\n",argv[i]);
      continue;
    }
    if(freadrestartheader(item[count].file,&restartheader,swap))
    {
      fprintf(stderr,"Error reading header of file '%s'\n",argv[i]);
      continue;
    }
    /*printf("Filename: %s\n"
           "First year: %d\n"
           "Number of years:%d\n"
           "First cell: %d\n"
           "Number of cells:%d\n",
           argv[i],item[count].header.firstyear,item[count].header.nyear,
           item[count].header.firstcell,item[count].header.ncell); */
    ncell+=item[count].header.ncell;
    count++;
  }
  qsort(item,count,sizeof(Item),(int(*)(const void *,const void *))compare);
  header.firstyear=item[0].header.firstyear;
  for(i=1;i<count;i++)
  {
    if(item[i-1].header.firstcell+item[i-1].header.ncell!=item[i].header.firstcell) 
      fprintf(stderr,"Warning: first cell in %d=%d not last cell+1=%d\n",i,
              item[i].header.firstcell,
              item[i-1].header.firstcell+item[i-1].header.ncell);
    if(item[i].header.firstyear!=header.firstyear)
      fprintf(stderr,"Warning: year=%d in %d differs from %d\n",
              item[i].header.firstyear,i,header.firstyear);
  }
  header.firstcell=item[0].header.firstcell;
  header.firstyear=item[0].header.firstyear;
  header.ncell=ncell;
  fwriteheader(out,&header,RESTART_HEADER,RESTART_VERSION);
  fwrite(&restartheader,sizeof(Restartheader),1,out);
  header_offset=strlen(RESTART_HEADER)+sizeof(Restartheader)+sizeof(int)+sizeof(Header);
  o_offset=header_offset;
  offset=0;
  for(i=0;i<count;i++)
  { 
    fstat(fileno(item[i].file),&filestat);
    len=filestat.st_size-header_offset-item[i].header.ncell*sizeof(int);
    fseek(out,o_offset,SEEK_SET);
    o_offset+=item[i].header.ncell*sizeof(int);
    for(j=0;j<item[i].header.ncell;j++)
    {
      fread(&ptr,sizeof(ptr),1,item[i].file);
      ptr+=(ncell-item[i].header.ncell)*sizeof(int)+offset;
      fwrite(&ptr,sizeof(ptr),1,out);
    }
    fseek(out,offset+header_offset+ncell*sizeof(int),SEEK_SET);
    offset+=len;
    data=malloc(len);
    fread(data,len,1,item[i].file);
    fwrite(data,len,1,out);
    free(data);
    fclose(item[i].file);
  }
  return EXIT_SUCCESS;
} /* of 'main' */
