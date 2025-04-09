/**************************************************************************************/
/**                                                                                \n**/
/**                   r  e  s  t  a  r  t  2  y  a  m  l  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Utility converts restart file into yaml file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "types.h"
#include "errmsg.h"
#include "swap.h"
#include "bstruct.h"

//#define DEBUG

#define USAGE "Usage: %s [-name key] [-first] restartfile [first [last]]\n"

#define readname2(file,name) if(readname(file,name)) {fprintf(stderr,"Error reading name of %s in '%s'.\n",bstruct_typenames[token],argv[1]); puts("..."); fclose(file); return EXIT_FAILURE; }

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
  long long filepos;
  char *endptr;
  char *string;
  char *key=NULL;
  int version,level,iarg,keylevel;
  Bool swap,notend,isarray=FALSE,iskey=TRUE,stop=FALSE;
  Byte token,b;
  short s;
  unsigned short us;
  int i,len;
  double d;
  float f;
  int firstcell,lastcell,last,cell;
  Bool first=FALSE,islastcell=FALSE;
  String name;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-name"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-name'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        key=argv[++iarg];
        iskey=FALSE;
      }
      else if(!strcmp(argv[iarg],"-first"))
        stop=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+1)
  {
    fprintf(stderr,USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[iarg],"rb");
  if(file==NULL)
  {
    printfopenerr(argv[iarg]);
    return EXIT_FAILURE;
  }
  firstcell=0;
  if(argc>iarg+1)
  {
    firstcell=strtol(argv[iarg+1],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for first grid.\n",argv[iarg+1]);
      return EXIT_FAILURE;
    }
    if(firstcell<0)
    {
      fprintf(stderr,"First cell must be >=0.\n");
      return EXIT_FAILURE;
    }
  }
  version=READ_VERSION;
  if(freadtopheader(file,&swap,BSTRUCT_HEADER,&version,TRUE))
  {
    fprintf(stderr,"ERROR154: Invalid header in restart file '%s'.\n",argv[1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(version!=BSTRUCT_VERSION)
  {
    fprintf(stderr,"ERROR154: Invalid version %d in restart file '%s', must be %d.\n",
            version,argv[1],BSTRUCT_VERSION);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(argc>iarg+2)
  {
    lastcell=strtol(argv[iarg+2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for last grid.\n",argv[iarg+2]);
      return EXIT_FAILURE;
    }
    islastcell=TRUE;
  }
  printf("%% YAML 1.2\n"
         "---\n"
         "filename: %s\n",
         argv[iarg]);
  level=0;
  cell=firstcell;
  notend=TRUE;
  keylevel=0;
  while(notend)
  {
    if(fread(&token,1,1,file)!=1)
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",cell,argv[iarg]);
      break;
    }
#ifdef DEBUG
    printf("%d,%d,%s:",cell,level,bstruct_typenames[token]);
#endif
    switch(token)
    {
      case BSTRUCT_ENDSTRUCT: case BSTRUCT_ENDARRAY:
        level--;
        if(key!=NULL && level==keylevel)
        {
          if(iskey && stop)
            notend=FALSE;
          iskey=FALSE;
        }
        /* if level reached 1 data for one cell has been read completely, increase cell counter */
        if(isarray && level==1)
        {
          cell++;
          if(cell>last)
            notend=FALSE;
        }
        break;
      case BSTRUCT_STRUCT: case BSTRUCT_ARRAY: case BSTRUCT_ARRAY1:
        readname2(file,name);
        if(key!=NULL && !strcmp(name,key))
        {
          iskey=TRUE;
          keylevel=level;
        }
        if(token==BSTRUCT_ARRAY || token==BSTRUCT_ARRAY1)
        {
          fseek(file,(token==BSTRUCT_ARRAY) ? sizeof(int) : 1,SEEK_CUR);
        }
        if(iskey)
        {
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
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
        }
        level++;
        break;
      case BSTRUCT_INDEXARRAY:
        isarray=TRUE;
        if(freadint(&len,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading size of index array in '%s'.\n",argv[iarg]);
          printf("...\n");
          fclose(file);
          return EXIT_FAILURE;
        }
        if(islastcell)
          last=lastcell;
        else
          last=len-1;
        if(last>=len)
        {
          fprintf(stderr,"Last cell %d is greater than upper number of cells %d.\n",last,len-1);
          fclose(file);
          return EXIT_FAILURE;
        }
        if(firstcell>=len)
        {
          fprintf(stderr,"First cell %d is greater than upper number of cells %d.\n",firstcell,len-1);
          fclose(file);
          return EXIT_FAILURE;
        }
        fseek(file,sizeof(long long)*firstcell,SEEK_CUR);
        freadlong(&filepos,1,swap,file);
        fseek(file,filepos,SEEK_SET);
        break;
      case BSTRUCT_ZERO:
        readname2(file,name);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("0\n");
        break;
      case BSTRUCT_BYTE:
        readname2(file,name);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        fread(&b,1,1,file);
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
        repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",b);
        break;
      case BSTRUCT_BOOL:
        readname2(file,name);
        fread(&b,1,1,file);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
        repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%s\n",bool2str(b));
        break;
      case BSTRUCT_SHORT:
        readname2(file,name);
        freadshort(&s,1,swap,file);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",s);
        break;
      case BSTRUCT_USHORT:
        readname2(file,name);
        freadushort(&us,1,swap,file);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",us);
        break;
      case BSTRUCT_INT:
        readname2(file,name);
        freadint(&i,1,swap,file);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%d\n",i);
        break;
      case BSTRUCT_FLOAT:
        readname2(file,name);
        freadfloat(&f,1,swap,file);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%g\n",f);
        break;
      case BSTRUCT_DOUBLE:
        readname2(file,name);
        freaddouble(&d,1,swap,file);
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("%g\n",d);
        break;
      case BSTRUCT_STRING:
        readname2(file,name);
        if(freadint(&len,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading string '%s' in '%s'.\n",name,argv[iarg]);
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
        if(key!=NULL && !strcmp(name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
        }
        if(!iskey)
          break;
        if(first)
          first=FALSE;
        else
          repeatch(' ',2*(level-keylevel));
        if(name[0]=='\0')
          fputs("- ",stdout);
        else
          printname(name);
        printf("\"%s\"\n",string);
        free(string);
        break;
      default:
        fprintf(stderr,"Invalid token %d in '%s'.\n",token,argv[iarg]);
        printf("...\n");
        fclose(file);
        return EXIT_FAILURE;
    } /* of switch(token) */
  } /* of while */
  printf("...\n");
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
