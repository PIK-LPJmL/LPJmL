/**************************************************************************************/
/**                                                                                \n**/
/**                 p  r  i  n  t  d  r  a  i  n  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define COUNT_SIZE 25

static int getdrain(FILE *file,int *index,int pos,char *headername,
                    int version,Bool swap,Bool *iserror)
{
  struct
  {
    int index,len;
  } buf;
  if(fseek(file,headersize(headername,version)+sizeof(buf)*pos,SEEK_SET))
  {
    *iserror=TRUE;
    return 0;
  }
  if(fread(&buf,sizeof(buf),1,file)!=1)
  {
    *iserror=TRUE;
    return 0;
  }
  *iserror=FALSE;
  if(swap)
  {
    *index=swapint(buf.index);
    return swapint(buf.len);
  }
  else
  {
    *index=buf.index;
    return buf.len;
  }
} /* of 'getdrain' */
  
int main(int argc,char **argv)
{
  FILE *drainage;
  Header header;
  String headername;
  int cell,version;
  int i,count,len,lmin,lmax,count_max;
  int vec[COUNT_SIZE];
  Bool swap,iserror;
  version=READ_VERSION;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    version=2;
    argc--;
    argv++;
  }
  if(argc<2)
  {
    fputs("Error: Filename missing.\n",stderr);
    return EXIT_FAILURE;
  } 
  drainage=fopen(argv[1],"rb");
  if(drainage==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  if(freadanyheader(drainage,&header,&swap,headername,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[1]);
    return EXIT_FAILURE;
  }
  lmin=1000000;
  lmax=count_max=0;
  for(cell=0;cell<header.ncell;cell++)
  {
    count=0;
    len=getdrain(drainage,vec,cell,headername,version,swap,&iserror);
    if(iserror)
    {
      fprintf(stderr,"Error reading river route at %d.\n",cell+1); 
      return EXIT_FAILURE;
    }
    if(vec[0]>0)
    {
      if(lmin>len)
        lmin=len;
      if(lmax<len)
        lmax=len;
    }
    while(v>len && vec[count]>=0)
    {
      if(count==COUNT_SIZE)
      {
        fprintf(stderr,"River route too long, must be < %d.\n",COUNT_SIZE);
        return EXIT_FAILURE;
      }  
      len+=getdrain(drainage,vec+count+1,vec[count],headername,version,swap,&iserror);
      if(iserror)
      {
        fprintf(stderr,"Error reading river route at %d.\n",vec[count]+1); 
        return EXIT_FAILURE;
      }
      count++;
    }
    if(count>count_max)
      count_max=count;
    printf("%5d, %7d:",cell,len);
    for(i=0;i<=count;i++)
      printf(" %5d",vec[i]);
    printf("\n");
  } /* of for '(cell=0;..)' */
  printf("min,max: %d %d\n",lmin,lmax);
  printf("count_max=%d\n",count_max);
  return EXIT_SUCCESS;
} /* of 'main' */
