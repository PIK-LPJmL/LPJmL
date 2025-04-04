/**************************************************************************************/
/**                                                                                \n**/
/**                   r  e  s  t  a  r  t  2  y  a  m  l  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Utility converts restarrt file into yaml file                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define readname2(file,name) if(readname(file,name)) {fprintf(stderr,"Error reading name of %s in '%s'.\n",typenames[token],argv[1]); puts("..."); fclose(file); return EXIT_FAILURE; }

static Bool readname(FILE *file,String name)
{
  /* Function reads name of object */
  Byte len;
  if(fread(&len,1,1,file)!=1)
    return TRUE;
  if(fread(name,1,len,file)!=len)
    return TRUE;
  name[len]='\0';
  return FALSE;
} /* of 'readname' */

static void printname(const char *name)
{
  fputprintable(stdout,name);
  fputs(": ",stdout);
}

int main(int argc,char **argv)
{
  FILE *file;
  long long *index;
  char *endptr;
  char *string;
  Header header;
  int version,level;
  Bool swap;
  Byte token,b;
  short s;
  unsigned short us;
  int i,len;
  double d;
  float f;
  int firstcell,last,cell;
  Bool first=FALSE;
  String name;
  Restartheader restartheader;
  if(argc<2)
  {
    fprintf(stderr,"Usage: %s restartfile [first [last]]\n",argv[0]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[1],"rb");
  if(file==NULL)
  {
    printfopenerr(argv[1]);
    return EXIT_FAILURE;
  }
  firstcell=0;
  if(argc>2)
  {
    firstcell=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for first grid.\n",argv[2]);
      return EXIT_FAILURE;
    }
    if(firstcell<0)
    {
      fprintf(stderr,"First cell must be >=0.\n");
      return EXIT_FAILURE;
    }
  }
  version=READ_VERSION;
  if(freadheader(file,&header,&swap,RESTART_HEADER,&version,TRUE))
  {
    fprintf(stderr,"ERROR154: Invalid header in restart file '%s'.\n",argv[1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(version!=RESTART_VERSION)
  {
    fprintf(stderr,"ERROR154: Invalid version %d in restart file '%s', must be %d.\n",
            version,argv[1],RESTART_VERSION);
    fclose(file);
    return EXIT_FAILURE;
  }
  last=header.ncell-1;
  if(argc>3)
  {
    last=strtol(argv[3],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for last grid.\n",argv[3]);
      return EXIT_FAILURE;
    }
  }
  if(last>header.ncell)
  {
    fprintf(stderr,"Last cell %d is greater than upper number of cells %d.\n",last,header.ncell-1);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(firstcell>header.ncell)
  {
    fprintf(stderr,"First cell %d is greater than upper number of cells %d.\n",firstcell,header.ncell-1);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(freadrestartheader(file,&restartheader,swap))
  {
    fprintf(stderr,"ERROR154: Invalid restart header in restart file '%s'.\n",argv[1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  index=newvec(long long,header.ncell);
  if(index==NULL)
  {
    printallocerr("index");
    fclose(file);
    return EXIT_FAILURE;
  }
  if(freadlong(index,header.ncell,swap,file)!=header.ncell)
  {
    fprintf(stderr,"Error reading index vector in '%s.\n",argv[1]);
    free(index);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(fseek(file,index[firstcell],SEEK_SET))
  {
    fprintf(stderr,"Error seeking to cell %d in '%s'.\n",firstcell,argv[1]);
    free(index);
    fclose(file);
    return EXIT_FAILURE;
  }
  free(index);
  printf("%% YAML 1.2\n"
         "---\n"
         "filename: %s\n"
         "year: %d\n"
         "firstcell: %d\n"
         "cellsize_lon: %g\n"
         "cellsize_lat: %g\n"
         "datatype: %s\n"
         "ntotpft: %d\n"
         "seed:\n",
         argv[1],header.firstyear,header.firstcell,
         header.cellsize_lon,header.cellsize_lat,typenames[header.datatype],header.nbands);
  for(i=0;i<NSEED;i++)
    printf("  - %d\n",restartheader.seed[i]);
  puts("grid:");
  level=1;
  cell=first;
  while(cell<=last)
  {
    if(fread(&token,1,1,file)!=1)
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",cell,argv[1]);
      break;
    }
    switch(token)
    {
      case LPJ_ENDSTRUCT: case LPJ_ENDARRAY:
        level--;
        /* if level reached 1 data for one cell has been read completely, increase cell counter */
        if(level==1)
          cell++;
        break;
      case LPJ_STRUCT: case LPJ_ARRAY:
        readname2(file,name);
        if(token==LPJ_ARRAY)
          fseek(file,sizeof(int),SEEK_CUR);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
        {
          fputs("- ",stdout);
          first=TRUE;
        }
        else
        {
          printname(name);
          fputc('\n',stdout);
        }
        level++;
        break;
      case LPJ_ZERO:
        readname2(file,name);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("0\n");
        break;
      case LPJ_BYTE:
        readname2(file,name);
        fread(&b,1,1,file);
        if(first)
          first=FALSE;
        else
        repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",b);
        break;
      case LPJ_BOOL:
        readname2(file,name);
        fread(&b,1,1,file);
        if(first)
          first=FALSE;
        else
        repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%s\n",bool2str(b));
        break;
      case LPJ_SHORT:
        readname2(file,name);
        freadshort(&s,1,swap,file);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",s);
        break;
      case LPJ_USHORT:
        readname2(file,name);
        freadushort(&us,1,swap,file);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",us);
        break;
      case LPJ_INT:
        readname2(file,name);
        freadint(&i,1,swap,file);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",i);
        break;
      case LPJ_FLOAT:
        readname2(file,name);
        freadfloat(&f,1,swap,file);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%g\n",f);
        break;
      case LPJ_DOUBLE:
        readname2(file,name);
        freaddouble(&d,1,swap,file);
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%g\n",d);
        break;
      case LPJ_STRING:
        readname2(file,name);
        if(freadint(&len,1,swap,file))
        {
          fprintf(stderr,"Error reading string '%s' in '%s'.\n",name,argv[1]);
          printf("...\n");
          fclose(file);
          return EXIT_FAILURE;
        }
        string=malloc(len+1);
        if(string==NULL)
        {
          printallocerr("string");
          printf("...\n");
          fclose(file);
          return EXIT_SUCCESS;
        }
        fread(string,1,len,file);
        string[len]='\0';
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*level);
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%s\n",string);
        free(string);
        break;
      default:
        fprintf(stderr,"Invalid token %d in '%s'.\n",token,argv[1]);
        printf("...\n");
        fclose(file);
        return EXIT_FAILURE;
    } /* of switch(token) */
  } /* of while */
  printf("...\n");
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
