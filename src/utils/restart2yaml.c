/**************************************************************************************/
/**                                                                                \n**/
/**                   r  e  s  t  a  r  t  2  y  a  m  l  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Utility converts restart file into yaml or JSON file                       \n**/
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
#include "hash.h"
#include "bstruct.h"

//#define DEBUG

#define USAGE "Usage: %s [-name key] [-first] [-json] [-noindent] restartfile [first [last]]\n"

static void printname(const char *name)
{
  /* to check for string N is necessary because N will be interpreted as FALSE in YAML 1.1 */
  fputprintable(stdout,name);
  fputs(":",stdout);
} /* of 'printname' */

int main(int argc,char **argv)
{
  Bstruct file;
  Bstruct_data data;
  char *endptr;
  char *key=NULL;
  int level,iarg,keylevel;
  Bool notend,isarray=FALSE,iskey=TRUE,stop=FALSE;
  int firstcell,lastcell,last,cell;
  Bool first=FALSE,islastcell=FALSE,isjson=FALSE,indent=TRUE;
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
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-noindent"))
        indent=FALSE;
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
    fprintf(stderr,"Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  file=bstruct_open(argv[iarg],TRUE);
  if(file==NULL)
    return EXIT_FAILURE;
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
  if(isjson)
    printf("{\n\"filename\" : \"%s\"",argv[iarg]);
  else
    printf("%%YAML 1.2\n"
           "---\n"
           "filename: %s\n",
           argv[iarg]);
  level=0;
  cell=firstcell;
  notend=TRUE;
  keylevel=0;
  while(notend)
  {
    if(bstruct_readdata(file,&data))
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",cell,argv[iarg]);
      break;
    }
#ifdef DEBUG
    printf("%d,%d,%s:",cell,level,bstruct_typenames[token]);
#endif
    switch(data.token)
    {
      case BSTRUCT_ENDSTRUCT: case BSTRUCT_ENDARRAY:
        level--;
        if(isjson && iskey)
        {
          putchar('\n');
          if(indent)
            repeatch(' ',2*(level-keylevel));
          putchar(data.token==BSTRUCT_ENDSTRUCT ? '}' : ']');
        }
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
        if(key!=NULL && !strcmp(data.name,key))
        {
          iskey=TRUE;
          keylevel=level;
        }
        if(iskey)
        {
          if(isjson)
          {
            if(!first)
              puts(",");
            first=TRUE;
            if(indent)
              repeatch(' ',2*(level-keylevel));
            if(data.name==NULL)
              puts(data.token==BSTRUCT_STRUCT ? "{" : "[");
            else
              printf("\"%s\" : %c\n",data.name,data.token==BSTRUCT_STRUCT ? '{' : '[');
          }
          else
          {
            if(first)
              first=FALSE;
            else
              repeatch(' ',2*(level-keylevel));
            if(data.name==NULL)
            {
              fputs("- ",stdout);
              first=TRUE;
            }
            else
            {
              printname(data.name);
              if(data.token!=BSTRUCT_STRUCT && data.size==0)
                puts(" []");
              else
                fputc('\n',stdout);
            }
          }
        }
        level++;
        break;
      case BSTRUCT_INDEXARRAY:
        isarray=TRUE;
        if(islastcell)
          last=lastcell;
        else
          last=data.size-1;
        if(last>=data.size)
        {
          fprintf(stderr,"Last cell %d is greater than upper number of cells %d.\n",last,data.size-1);
          bstruct_close(file);
          return EXIT_FAILURE;
        }
        if(firstcell>=data.size)
        {
          fprintf(stderr,"First cell %d is greater than upper number of cells %d.\n",firstcell,data.size-1);
          bstruct_close(file);
          return EXIT_FAILURE;
        }
        fseek(bstruct_getfile(file),data.data.index[firstcell],SEEK_SET);
        break;
      default:
        if(key!=NULL && !strcmp(data.name,key))
        {
          if(stop)
            notend=FALSE;
          iskey=TRUE;
          keylevel=level;
        }
        if(!iskey)
          break;
        if(isjson)
        {
          if(first)
            first=FALSE;
          else
            puts(",");
          if(indent)
            repeatch(' ',2*(level-keylevel));
          if(data.name!=NULL)
            printf("\"%s\" : ",data.name);
        }
        else
        {
          if(first)
            first=FALSE;
          else
            repeatch(' ',2*(level-keylevel));
          if(data.name==NULL)
            fputs("- ",stdout);
          else
          {
            printname(data.name);
            putchar(' ');
          }
        }
        bstruct_printdata(&data);
        if(!isjson)
          fputc('\n',stdout);
        if(key!=NULL && !strcmp(data.name,key))
          iskey=FALSE;
        break;
    } /* of switch(token) */
    bstruct_freedata(&data);
  } /* of while */
  if(isjson)
  {
    if(key==NULL)
      puts("\n]");
    puts("}");
  }
  else
    puts("...");
  bstruct_close(file);
  return EXIT_SUCCESS;
} /* of 'main' */
