#include "lpj.h"

Bool fwritefwi(Bstruct file,const char *name,const FWIdata *data)
{
  bstruct_writebeginstruct(file,name);
  bstruct_writereal(file,"ffmc",data->ffmc);
  bstruct_writereal(file,"dmc",data->dmc);
  bstruct_writereal(file,"dc",data->dc);
  return bstruct_writeendstruct(file);
} /* of 'fwritefwi' */
