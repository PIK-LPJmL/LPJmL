Bool checkfilelength(FILE *file,const char *filename,size_t len,Bool isroot)
{
  if(getfilesizep(file)>len)
  {
    return TRUE;
  
