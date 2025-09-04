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
#include <math.h>
#include "types.h"
#include "errmsg.h"
#include "swap.h"
#include "hash.h"
#include "bstruct.h"

#define USAGE "Usage: %s [-h] [-name key] [-first] [-json] [-noindent] [-invalid2null] [-d n]\n"\
              "       [-t] restartfile [first [last]]\n"
#define ERR_USAGE USAGE "\nTry \"%s --help\" for more information.\n"
#define getname(name) (name==NULL) ? "unnamed" : name

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
  const char *progname;
  char *endptr;
  char *key=NULL;
  int level,iarg,keylevel;
  Bool notend,isarray=FALSE,iskey=TRUE,stop=FALSE,setnull=FALSE;
  int firstcell,lastcell,last,cell,decimals=16,rc;
  Bool first=FALSE,islastcell=FALSE,isjson=FALSE,indent=TRUE,istable=FALSE;
  progname=strippath(argv[0]);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {   
        fputs("     ",stdout);
        rc=printf("%s (" __DATE__ ") Help",
                  progname);
        fputs("\n     ",stdout);
        repeatch('=',rc);
        printf("\n\nConvert restart file into YAML/JSON file for LPJmL %s\n\n",getversion());
        printf(USAGE,progname);
        printf("\nArguments:\n"
               "-h,--help     print this help text\n"
               "-name         key print only variables/array/structures with name key\n"
               "-first        stop at first occurrence of key specified by the -name option\n"
               "-json         output format is set to JSON, default ist YAML\n"
               "-noindent     disable indentations in JSON files, default is 2 spaces per level\n"
               "-invalid2null set all NaN and infinite values to null\n"
               "-d n          set the number of decimal places for float/double values, default is 16\n"
               "-t            print only table of defined object names\n"
               "restartfile   name of restart file\n"
               "start         index of first grid cell to be printed\n"
               "end           index of last grid cell to be printed\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n");
        return EXIT_SUCCESS;
      }   
      else if(!strcmp(argv[iarg],"-name"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-name'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        key=argv[++iarg];
        iskey=FALSE;
      }
      else if(!strcmp(argv[iarg],"-d"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Missing argument after option '-d'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        decimals=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-d'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(decimals<1)
        {
          fprintf(stderr,"Number of decimals=%d must be greater than zero.\n",decimals);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-invalid2null"))
        setnull=TRUE;
      else if(!strcmp(argv[iarg],"-noindent"))
        indent=FALSE;
      else if(!strcmp(argv[iarg],"-t"))
        istable=TRUE;
      else if(!strcmp(argv[iarg],"-first"))
        stop=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                ERR_USAGE,argv[iarg],progname,progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+1)
  {
    fprintf(stderr,"Missing argument(s).\n"
            ERR_USAGE,progname,progname);
    return EXIT_FAILURE;
  }
  file=bstruct_open(argv[iarg],TRUE);
  if(file==NULL)
    return EXIT_FAILURE;
  if(istable)
  {
    if(isjson)
    {
      printf("{\n\"filename\" : \"%s\",\n",argv[iarg]);
      bstruct_printnametable("table",file,TRUE);
      puts("\n}");
    }
    else
    {
      printf("%%YAML 1.2\n"
             "---\n"
             "filename: \"%s\"\n",
             argv[iarg]);
      bstruct_printnametable("table",file,FALSE);
      puts("...");
    }
    bstruct_finish(file);
    return EXIT_SUCCESS;
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
           "filename: \"%s\"\n",
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
      case BSTRUCT_END:
        notend=FALSE;
        break;
      case BSTRUCT_ENDSTRUCT: case BSTRUCT_ENDARRAY:
        level--;
        if(isjson && iskey)
        {
          putchar('\n');
          if(indent)
            repeatch(' ',2*(level-keylevel));
          putchar(data.token==BSTRUCT_ENDSTRUCT ? '}' : ']');
           first=FALSE;
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
      case BSTRUCT_BEGINSTRUCT: case BSTRUCT_BEGINARRAY: case BSTRUCT_BEGINARRAY1:
        if(key!=NULL && data.name!=NULL && !strcmp(data.name,key))
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
              puts(data.token==BSTRUCT_BEGINSTRUCT ? "{" : "[");
            else
              printf("\"%s\" : %c\n",data.name,data.token==BSTRUCT_BEGINSTRUCT ? '{' : '[');
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
              if(data.token!=BSTRUCT_BEGINSTRUCT && data.size==0)
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
          bstruct_finish(file);
          return EXIT_FAILURE;
        }
        if(firstcell>=data.size)
        {
          fprintf(stderr,"First cell %d is greater than upper number of cells %d.\n",firstcell,data.size-1);
          bstruct_finish(file);
          return EXIT_FAILURE;
        }
        fseek(bstruct_getfile(file),data.data.index[firstcell],SEEK_SET);
        break;
      default:
        if(key!=NULL && data.name!=NULL && !strcmp(data.name,key))
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
        if(data.token==BSTRUCT_FLOAT)
        {
          if(isnan(data.data.f))
          {
            fprintf(stderr,"Error: Value of '%s' is not a number (Nan).\n",getname(data.name));
            if(setnull)
              data.token=BSTRUCT_NULL;
          }
          else if(isinf(data.data.f))
          {
            fprintf(stderr,"Error: Value of '%s' is infinite.\n",getname(data.name));
            if(setnull)
              data.token=BSTRUCT_NULL;
          }
        }
        else if(data.token==BSTRUCT_DOUBLE)
        {
          if(isnan(data.data.d))
          {
            fprintf(stderr,"Error: Value of '%s' is not a number (Nan).\n",getname(data.name));
            if(setnull)
              data.token=BSTRUCT_NULL;
          }
          else if(isinf(data.data.d))
          {
            fprintf(stderr,"Error: Value of '%s' is infinite.\n",getname(data.name));
            if(setnull)
              data.token=BSTRUCT_NULL;
          }
        }
        bstruct_printdata(&data,decimals);
        if(!isjson)
          fputc('\n',stdout);
        if(key!=NULL && data.name!=NULL && !strcmp(data.name,key))
          iskey=FALSE;
        break;
    } /* of switch(token) */
    bstruct_freedata(&data);
  } /* of while */
  if(isjson)
  {
    if(data.token==BSTRUCT_END)
      puts("\n}");
    else
    {
      if(key==NULL)
        puts("\n]");
      puts("}");
    }
  }
  else
    puts("...");
  bstruct_finish(file);
  return EXIT_SUCCESS;
} /* of 'main' */
