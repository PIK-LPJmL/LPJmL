/**************************************************************************************/
/**                                                                                \n**/
/**                   c  m  p  r  e  s  t  a  r  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Utility compares two restart files                                         \n**/
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

#define USAGE "%s: [-q] restart1.lpj restart2.lpj\n"
#define printstack(stack,level) fprintstack(stdout,stack,level)

typedef struct
{
  int token;
  int size;
  int count;
  char *name;
} Stack;

static void fprintstack(FILE *file,const Stack stack[],int level)
{
  int i;
  for(i=0;i<level;i++)
  {
    if(stack[i].name!=NULL)
      fputs(stack[i].name,file);
    if(stack[i].token==BSTRUCT_BEGINARRAY || stack[i].token==BSTRUCT_BEGINARRAY1)
      fprintf(file,"[%d]",stack[i].count-1);
    else
      fputc('.',file);
  }
} /* of 'fprintstack' */

static Bool cmpdouble(double value1,double value2)
{
  if(isnan(value1) && !isnan(value2))
    return TRUE;
  if(isnan(value2) && !isnan(value1))
    return TRUE;
  if(isnan(value1) && isnan(value2))
    return FALSE;
  return value1!=value2;
} /* of 'cmpdouble' */

static Bool cmpfloat(float value1,float value2)
{
  if(isnan(value1) && !isnan(value2))
    return TRUE;
  if(isnan(value2) && !isnan(value1))
    return TRUE;
  if(isnan(value1) && isnan(value2))
    return FALSE;
  return value1!=value2;
} /* of 'cmpfloat' */

static void cmpbool(Bstruct file1,Bstruct file2,const char *name,const char *filename1,const char *filename2)
{
  Bool bool1,bool2;
  if(!bstruct_readbool(file1,name,&bool1))
  {
    if(!bstruct_readbool(file2,name,&bool2))
    {
      if(bool1!=bool2)
        fprintf(stderr,"%s=%s in '%s' differs from %s in '%s'.\n",
                name,bool2str(bool1),filename1,bool2str(bool2),filename2);
    }
  }
} /* of 'cmpbool' */

int main(int argc,char **argv)
{
  Bstruct file1,file2;
  Bstruct_data data1;
  int level=1,ilevel=1,size,count=0,iarg,i,npft1,npft2,allcount=0,grid_index;
  int firstcell1=0,firstcell2=0;
  float f;
  double d;
  char *version1,*version2;
  Bool verb=TRUE,b;
  Stack stack[15];
  for(iarg=1;iarg<argc;iarg++)
  {
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-q"))
        verb=FALSE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  }
  if(argc<iarg+2)
  {
    fprintf(stderr,"Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  file1=bstruct_open(argv[iarg],TRUE);
  if(file1==NULL)
    return EXIT_FAILURE;
  file2=bstruct_open(argv[iarg+1],TRUE);
  if(file2==NULL)
  {
    bstruct_finish(file1);
    return EXIT_FAILURE;
  }
  /* switch off error messages from bstruct */
  bstruct_setout(file1,FALSE);
  bstruct_setout(file2,FALSE);
  /* compare header */
  if(bstruct_readbeginstruct(file1,"header"))
    fprintf(stderr,"No header found in '%s'.\n",argv[iarg]);
  else
  {
    if(bstruct_readbeginstruct(file2,"header"))
    {
      fprintf(stderr,"No header found in '%s'.\n",argv[iarg]);
      bstruct_readendstruct(file1,"header");
    }
    else
    {
      version1=bstruct_readstring(file1,"version");
      if(version1!=NULL)
      {
        version2=bstruct_readstring(file2,"version");
        if(version2!=NULL)
        {
          if(strcmp(version1,version2))
            fprintf(stderr,"Version %s in '%s' differs from version %s in '%s'.\n",
                    version1,argv[iarg],version2,argv[iarg+1]);
          free(version2);
        }
        free(version1);
      }
      else
        free(version1);
      if(!bstruct_readint(file1,"year",&npft1))
      {
        if(!bstruct_readint(file2,"year",&npft2))
        {
          if(npft1!=npft2)
            fprintf(stderr,"Year %d in '%s' differs from %d in '%s'.\n",
                    npft1,argv[iarg],npft2,argv[iarg+1]);
        }
      }
      if(!bstruct_readint(file1,"firstcell",&firstcell1))
      {
        if(!bstruct_readint(file2,"firstcell",&firstcell2))
        {
          if(firstcell1!=firstcell2)
            fprintf(stderr,"First cell %d in '%s' differs from %d in '%s'.\n",
                    firstcell1,argv[iarg],firstcell2,argv[iarg+1]);
        }
      }
      grid_index=firstcell1;
      if(!bstruct_readint(file1,"npft",&npft1))
      {
        if(!bstruct_readint(file2,"npft",&npft2))
        {
          if(npft1!=npft2)
            fprintf(stderr,"Number of PFTs %d in '%s' differs from %d in '%s'.\n",
                    npft1,argv[iarg],npft2,argv[iarg+1]);
        }
      }
      if(!bstruct_readint(file1,"ncft",&npft1))
      {
        if(!bstruct_readint(file2,"ncft",&npft2))
        {
          if(npft1!=npft2)
            fprintf(stderr,"Number of CFTs %d in '%s' differs from %d in '%s'.\n",
                    npft1,argv[iarg],npft2,argv[iarg+1]);
        }
      }
      if(!bstruct_readint(file1,"datatype",&npft1))
      {
        if(!bstruct_readint(file2,"datatype",&npft2))
        {
          if(npft1!=npft2)
          {
            fprintf(stderr,"Datatype %d in '%s' differs from %d in '%s'.\n",
                    npft1,argv[iarg],npft2,argv[iarg+1]);
            bstruct_finish(file1);
            bstruct_finish(file2);
            return EXIT_FAILURE;
          }
        }
      }
      cmpbool(file1,file2,"landuse",argv[iarg],argv[iarg+1]);
      cmpbool(file1,file2,"crop_phu_option",argv[iarg],argv[iarg+1]);
      cmpbool(file1,file2,"river_routing",argv[iarg],argv[iarg+1]);
      cmpbool(file1,file2,"separate_harvests",argv[iarg],argv[iarg+1]);
      bstruct_readendstruct(file1,"header");
      bstruct_readendstruct(file2,"header");
    }
  }
  /* switch on error messages from bstruct */
  bstruct_setout(file1,TRUE);
  bstruct_setout(file2,TRUE);
  /* compare grid */
  if(bstruct_readbeginarray(file1,"grid",&stack[0].size))
  {
    bstruct_finish(file1);
    bstruct_finish(file2);
    return EXIT_FAILURE;
  }
  if(bstruct_readbeginarray(file2,"grid",&size))
  {
    bstruct_finish(file1);
    bstruct_finish(file2);
    return EXIT_FAILURE;
  }
  if(size!=stack[0].size)
  {
    fprintf(stderr,"Size of grid array=%d in '%s' differs from %d in '%s'.\n",
            stack[0].size,argv[iarg],size,argv[iarg+1]);
    bstruct_finish(file1);
    bstruct_finish(file2);
    return EXIT_FAILURE;
  }
  stack[0].token=BSTRUCT_BEGINARRAY;
  stack[0].count=0;
  stack[0].name=strdup("grid");
  do
  {
    if(bstruct_readdata(file1,&data1))
    {
      fprintf(stderr,"Error reading '%s'.\n",argv[iarg]);
      bstruct_finish(file1);
      bstruct_finish(file2);
      return EXIT_FAILURE;
    }
    switch(data1.token)
    {
      case BSTRUCT_BEGINSTRUCT:
        stack[level].token=data1.token;
        stack[level].name=data1.name==NULL ? NULL : strdup(data1.name);
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(data1.name==NULL || bstruct_isdefined(file2,data1.name))
          {
            if(bstruct_readbeginstruct(file2,data1.name))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            ilevel++;
          }
          else
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            puts("not found");
          }
        }
        level++;
        break;
      case BSTRUCT_BEGINARRAY: case BSTRUCT_BEGINARRAY1:
        stack[level].token=data1.token;
        stack[level].name=data1.name==NULL ? NULL : strdup(data1.name);
        stack[level].size=data1.size;
        stack[level].count=0;
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(data1.name==NULL || bstruct_isdefined(file2,data1.name))
          {
            if(bstruct_readbeginarray(file2,data1.name,&size))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(size!=data1.size)
            {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(".size=%d<>%d\n",data1.size,size);
              if(bstruct_skiparray(file1))
                return EXIT_FAILURE;
              if(bstruct_skiparray(file2))
                return EXIT_FAILURE;
              break;
            }
            ilevel++;
          }
          else
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            puts("not found");
            count++;
          }
        }
        level++;
        break;
      case BSTRUCT_ENDSTRUCT:
        if(level==ilevel)
        {
          if(bstruct_readendstruct(file2,stack[level-1].name))
          {
            fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
            return EXIT_FAILURE;
          }
          ilevel--;
        }
        free(stack[level-1].name);
        level--;
        if(level==1)
          grid_index++;
        break;
      case BSTRUCT_ENDARRAY:
        if(level==ilevel)
        {
          if(bstruct_readendarray(file2,stack[level-1].name))
          {
            fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
            return EXIT_FAILURE;
          }
          ilevel--;
        }
        free(stack[level-1].name);
        level--;
        break;
      case BSTRUCT_TRUE: case BSTRUCT_FALSE:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%s<>not found\n",bool2str(data1.data.b));
          }
          else
          {
            if(bstruct_readbool(file2,data1.name,&b))
              return EXIT_FAILURE;
            if(data1.data.b!=b)
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%s<>%s\n",bool2str(data1.data.b),bool2str(b));
              }
            }
          }
        }
        break;
      case BSTRUCT_ZERO:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":0<>not found\n");
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(i!=0)
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":0<>%d\n",i);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_FZERO:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":0.0<>not found\n");
            count++;
          }
          else
          {
            if(bstruct_readfloat(file2,data1.name,&f))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(cmpfloat(0,f))
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":0<>%g\n",f);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_DZERO:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":0.0<>not found\n");
            count++;
          }
          else
          {
            if(bstruct_readdouble(file2,data1.name,&d))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(cmpdouble(0,d))
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":0<>%g\n",d);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_DOUBLE:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%g<>not found\n",data1.data.d);
            count++;
          }
          else
          {
            if(bstruct_readdouble(file2,data1.name,&d))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(cmpdouble(data1.data.d,d))
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%g<>%g\n",data1.data.d,d);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_FLOAT:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%g<>not found\n",data1.data.f);
            count++;
          }
          else
          {
            if(bstruct_readfloat(file2,data1.name,&f))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(cmpfloat(data1.data.f,f))
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%g<>%g\n",data1.data.f,f);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_INT:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found\n",data1.data.i);
            count++;
          }
          //  fprintf(stderr,"Object '%s' not found in '%s'.\n",data1.name,argv[iarg+1]);
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(data1.data.i!=i)
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%d<>%d\n",data1.data.i,i);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_SHORT:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found\n",data1.data.s);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(data1.data.s!=i)
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%d<>%d\n",data1.data.s,i);
              }
              count++;
            }
            allcount++;
          }
        }
        break;
      case BSTRUCT_USHORT:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found\n",data1.data.us);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(data1.data.us!=i)
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%d<>%d\n",data1.data.us,i);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_BYTE:
        if(stack[level-1].token==BSTRUCT_BEGINARRAY || stack[level-1].token==BSTRUCT_BEGINARRAY1)
          stack[level-1].count++;
        if(level==ilevel)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found\n",data1.data.b);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d\n",grid_index);
              return EXIT_FAILURE;
            }
            if(data1.data.b!=i)
            {
              if(verb)
              {
                printstack(stack,level);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%d<>%d\n",data1.data.b,i);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      default:
        break;
    } /* of  switch(data1.token) */
    bstruct_freedata(&data1);
  } while(data1.token!=BSTRUCT_END && level>0);
  if(count)
    printf("%d out of %d differences in values found.\n",count,allcount);
  else
    puts("Numerical values are identical.");
  bstruct_finish(file1);
  bstruct_finish(file2);
  return EXIT_SUCCESS;
} /* of 'main' */
