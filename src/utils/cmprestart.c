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

typedef struct
{
  char *version;
  int year;
  int firstcell;
  int datatype;
  int npft;
  int ncft;
  Bool landuse;
  Bool crop_phu_option;
  Bool river_routing;
  Bool separate_harvests;
} Restartheader;

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

static Bool readheader(Bstruct bstr,Restartheader *header,const char *filename)
{
  header->version=NULL;
  if(bstruct_readbeginstruct(bstr,"header"))
  {
    fprintf(stderr,"No header found in '%s'.\n",filename);
    return TRUE;
  }
  header->version=bstruct_readstring(bstr,"version");
  if(header->version==NULL)
  {
    fprintf(stderr,"No version found in '%s'.\n",filename);
    return TRUE;
  }
  if(bstruct_readint(bstr,"year",&header->year))
  {
    fprintf(stderr,"Cannot read year in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readint(bstr,"firstcell",&header->firstcell))
  {
    fprintf(stderr,"Cannot read firstcell in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readint(bstr,"npft",&header->npft))
  {
    fprintf(stderr,"Cannot read npft in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readint(bstr,"ncft",&header->ncft))
  {
    fprintf(stderr,"Cannot read ncft in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readint(bstr,"datatype",&header->datatype))
  {
    fprintf(stderr,"Cannot read datatype in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readbool(bstr,"landuse",&header->landuse))
  {
    fprintf(stderr,"Cannot read landuse in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readbool(bstr,"crop_phu_option",&header->crop_phu_option))
  {
    fprintf(stderr,"Cannot read crop_phu_option in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readbool(bstr,"river_routing",&header->river_routing))
  {
    fprintf(stderr,"Cannot read river_routing  in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  if(bstruct_readbool(bstr,"separate_harvests",&header->separate_harvests))
  {
    fprintf(stderr,"Cannot read separate harvests  in '%s'.\n",filename);
    free(header->version);
    return TRUE;
  }
  return bstruct_readendstruct(bstr,"header");
} /* of 'readheader' */

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

int main(int argc,char **argv)
{
  Bstruct file1,file2;
  Bstruct_data data1;
  int level1=1; /* nesting level in first file */
  int level2=1; /* nesting level in second file */
  int size,count=0,iarg,i,allcount=0,grid_index;
  float f;
  double d;
  Bool verb=TRUE,b;
  Stack stack[15];
  Restartheader header1,header2;
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
  if(readheader(file1,&header1,argv[iarg]))
  {
    bstruct_finish(file1);
    bstruct_finish(file2);
    return EXIT_FAILURE;
  }
  if(readheader(file2,&header2,argv[iarg+1]))
  {
    bstruct_finish(file1);
    bstruct_finish(file2);
    return EXIT_FAILURE;
  }
  if(strcmp(header1.version,header2.version))
    fprintf(stderr,"Version %s in '%s' differs from version %s in '%s'.\n",
            header1.version,argv[iarg],header2.version,argv[iarg+1]);
  free(header1.version);
  free(header2.version);
  if(header1.year!=header2.year)
    fprintf(stderr,"Year %d in '%s' differs from %d in '%s'.\n",
             header1.year,argv[iarg],header2.year,argv[iarg+1]);
  if(header1.firstcell!=header2.firstcell)
    fprintf(stderr,"First cell %d in '%s' differs from %d in '%s'.\n",
            header1.firstcell,argv[iarg],header2.firstcell,argv[iarg+1]);
  grid_index=header1.firstcell;
  if(header1.npft!=header2.npft)
     fprintf(stderr,"Number of PFTs %d in '%s' differs from %d in '%s'.\n",
             header1.npft,argv[iarg],header2.npft,argv[iarg+1]);
  if(header1.ncft!=header2.ncft)
    fprintf(stderr,"Number of CFTs %d in '%s' differs from %d in '%s'.\n",
                    header1.ncft,argv[iarg],header2.ncft,argv[iarg+1]);
  if(header1.datatype!=header2.datatype)
  {
    fprintf(stderr,"Datatype %d in '%s' differs from %d in '%s'.\n",
                    header1.datatype,argv[iarg],header2.datatype,argv[iarg+1]);
    bstruct_finish(file1);
    bstruct_finish(file2);
    return EXIT_FAILURE;
   }
  if(header1.landuse!=header2.landuse)
    fprintf(stderr,"Setting landuse=%s in '%s' differs from %s in '%s'.\n",
            bool2str(header1.landuse),argv[iarg],bool2str(header2.landuse),argv[iarg+1]);
  if(header1.crop_phu_option!=header2.crop_phu_option)
    fprintf(stderr,"Setting crop_phu_option=%s in '%s' differs from %s in '%s'.\n",
            bool2str(header1.crop_phu_option),argv[iarg],bool2str(header2.crop_phu_option),argv[iarg+1]);
  if(header1.river_routing!=header2.river_routing)
    fprintf(stderr,"Setting river_routing=%s in '%s' differs from %s in '%s'.\n",
            bool2str(header1.river_routing),argv[iarg],bool2str(header2.river_routing),argv[iarg+1]);
  if(header1.separate_harvests!=header2.separate_harvests)
    fprintf(stderr,"Setting separate_harvests=%s in '%s' differs from %s in '%s'.\n",
            bool2str(header1.separate_harvests),argv[iarg],bool2str(header2.separate_harvests),argv[iarg+1]);
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
        stack[level1].token=data1.token;
        stack[level1].name=data1.name==NULL ? NULL : strdup(data1.name);
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(data1.name==NULL || bstruct_isdefined(file2,data1.name))
          {
            if(bstruct_readbeginstruct(file2,data1.name))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading struct %s in '%s'\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            level2++;
          }
          else
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(" not found in '%s'\n",argv[iarg+1]);
          }
        }
        level1++;
        break;
      case BSTRUCT_BEGINARRAY: case BSTRUCT_BEGINARRAY1:
        stack[level1].token=data1.token;
        stack[level1].name=data1.name==NULL ? NULL : strdup(data1.name);
        stack[level1].size=data1.size;
        stack[level1].count=0;
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(data1.name==NULL || bstruct_isdefined(file2,data1.name))
          {
            if(bstruct_readbeginarray(file2,data1.name,&size))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading array %s in '%s'\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(size!=data1.size)
            {
                printstack(stack,level1);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(".size=%d<>%d\n",data1.size,size);
              if(bstruct_skiparray(file1))
                return EXIT_FAILURE;
              if(bstruct_skiparray(file2))
                return EXIT_FAILURE;
              break;
            }
            level2++;
          }
          else
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(" not found in '%s'\n",argv[iarg+1]);
            count++;
          }
        }
        level1++;
        break;
      case BSTRUCT_ENDSTRUCT:
        if(level1==level2)
        {
          if(bstruct_readendstruct(file2,stack[level1-1].name))
          {
            fprintf(stderr,"Comparison stopped in cell %d reading end of struct %s in '%s'.\n",
                    grid_index,bstruct_getname(stack[level1-1].name),argv[iarg+1]);
            return EXIT_FAILURE;
          }
          level2--;
        }
        free(stack[level1-1].name);
        level1--;
        if(level1==1)
          grid_index++;
        break;
      case BSTRUCT_ENDARRAY:
        if(level1==level2)
        {
          if(bstruct_readendarray(file2,stack[level1-1].name))
          {
            fprintf(stderr,"Comparison stopped in cell %d reading end of array %s in '%s'.\n",
                    grid_index,bstruct_getname(stack[level1-1].name),argv[iarg+1]);
            return EXIT_FAILURE;
          }
          level2--;
        }
        free(stack[level1-1].name);
        level1--;
        break;
      case BSTRUCT_TRUE: case BSTRUCT_FALSE:
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%s<>not found in '%s'\n",bool2str(data1.data.b),argv[iarg+1]);
          }
          else
          {
            if(bstruct_readbool(file2,data1.name,&b))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading boolean %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(data1.data.b!=b)
            {
              if(verb)
              {
                printstack(stack,level1);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%s<>%s\n",bool2str(data1.data.b),bool2str(b));
              }
            }
          }
        }
        break;
      case BSTRUCT_ZERO:
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":0<>not found in '%s'\n",argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading int %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(i!=0)
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":0.0<>not found in '%s'\n",argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readfloat(file2,data1.name,&f))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading float %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(cmpfloat(0,f))
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":0.0<>not found in '%s'\n",argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readdouble(file2,data1.name,&d))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading double %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(cmpdouble(0,d))
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%g<>not found in '%s'\n",data1.data.d,argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readdouble(file2,data1.name,&d))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading double %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(cmpdouble(data1.data.d,d))
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%g<>not found in '%s'\n",data1.data.f,argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readfloat(file2,data1.name,&f))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading float %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(cmpfloat(data1.data.f,f))
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found in '%s'\n",data1.data.i,argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading int %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(data1.data.i!=i)
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found in '%s'\n",data1.data.s,argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading int %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(data1.data.s!=i)
            {
              if(verb)
              {
                printstack(stack,level1);
                if(data1.name!=NULL)
                  fputs(data1.name,stdout);
                printf(":%d<>%d\n",data1.data.s,i);
              }
              count++;
            }
          }
          allcount++;
        }
        break;
      case BSTRUCT_USHORT:
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found in '%s'\n",data1.data.us,argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading int %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(data1.data.us!=i)
            {
              if(verb)
              {
                printstack(stack,level1);
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
        if(stack[level1-1].token==BSTRUCT_BEGINARRAY || stack[level1-1].token==BSTRUCT_BEGINARRAY1)
          stack[level1-1].count++;
        if(level1==level2)
        {
          if(!bstruct_isdefined(file2,data1.name))
          {
            printstack(stack,level1);
            if(data1.name!=NULL)
              fputs(data1.name,stdout);
            printf(":%d<>not found in '%s'\n",data1.data.b,argv[iarg+1]);
            count++;
          }
          else
          {
            if(bstruct_readint(file2,data1.name,&i))
            {
              fprintf(stderr,"Comparison stopped in cell %d reading int %s in '%s'.\n",
                      grid_index,bstruct_getname(data1.name),argv[iarg+1]);
              return EXIT_FAILURE;
            }
            if(data1.data.b!=i)
            {
              if(verb)
              {
                printstack(stack,level1);
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
  } while(data1.token!=BSTRUCT_END && level1>0);
  if(count)
    printf("%d out of %d differences in values found.\n",count,allcount);
  else
    puts("Numerical values are identical.");
  bstruct_finish(file1);
  bstruct_finish(file2);
  return EXIT_SUCCESS;
} /* of 'main' */
