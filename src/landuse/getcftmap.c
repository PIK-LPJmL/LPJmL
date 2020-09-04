#include "lpj.h"

static int findpftname(const char *name,const Pftpar *pftpar,int ncft)
{
  int cft;
  for(cft=0;cft<ncft;cft++)
    if(!strcmp(name,pftpar[cft].name))
      return cft;
  return NOT_FOUND;
}

int *getcftmap(LPJfile *file,int npft,int ncft,const Config *config)
{
  LPJfile array,item;
  int *cftmap;
  Verbosity verbose;
  String s;
  int size,cft;
  if(iskeydefined(file,"cftmap"))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    if(fscanarray(file,&array,&size,FALSE,"cftmap",verbose))
      return NULL;
    if(size!=ncft)
    {
      if(verbose)
        fprintf(stderr,"ERROR240: Size of CFT map=%d differs from number of CFTs=%d\n",
                size,ncft);
      return NULL;
    }
    cftmap=newvec(int,ncft);
    for(cft=0;cft<ncft;cft++)
    {
      fscanarrayindex(&array,&item,cft,verbose);

      if(fscanstring(&item,s,NULL,FALSE,verbose))
      {
        free(cftmap);
        return NULL;
      }
      cftmap[cft]=findpftname(s,config->pftpar+npft,ncft);
      if(cftmap[cft]==NOT_FOUND)
      {
        if(verbose)
          fprintf(stderr,"ERROR241: CFT '%s' not found in CFT list.\n",s);
        free(cftmap);
        return NULL;
      }
    }
  }
  else
  {
    cftmap=newvec(int,ncft);
    for(cft=0;cft<ncft;cft++)
      cftmap[cft]=cft;
  }
  return cftmap;
}
