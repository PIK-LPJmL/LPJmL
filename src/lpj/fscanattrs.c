#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}

static int findglobal(const char *name,const Global *global,int n_global)
{
  int i;
  for(i=0;i<n_global;i++)
    if(!strcmp(name,global[i].name))
      return i;
  return NOT_FOUND;
} 

Bool fscanattrs(LPJfile *file,Config *config)
{
  String name;
  LPJfile array,item,s;
  Verbosity verbose;
  int i,index,size;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(iskeydefined(file,"global_attrs"))
  {
    if(fscanarray(file,&array,&size,FALSE,"global_attrs",verbose))
      return TRUE;
    for(i=0;i<size;i++)
    {
      fscanarrayindex(&array,&item,i,verbose);
      if(fscanstruct(&item,&s,NULL,verbose))
        return TRUE;
      if(fscanstring(&s,name,"name",FALSE,verbose))
        return TRUE;
      index=findglobal(name,config->global_attrs,config->n_global);
      if(index==NOT_FOUND)
      {
        config->global_attrs=realloc(config->global_attrs,(config->n_global+1)*sizeof(Global));
        checkptr(config->global_attrs);
        index=config->n_global;
        config->n_global++;
        config->global_attrs[index].name=strdup(name);
        checkptr(config->global_attrs[index].name);
        config->global_attrs[index].value=NULL; 
      }
      if(fscanstring(&s,name,"value",FALSE,verbose))
        return TRUE;
      free(config->global_attrs[index].value);
      config->global_attrs[index].value=strdup(name); 
      checkptr(config->global_attrs[index].value);
    }
  }
  return FALSE;
} /* of 'fscanattrs' */
