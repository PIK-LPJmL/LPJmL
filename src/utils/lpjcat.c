/**************************************************************************************/
/**                                                                                \n**/
/**                l  p  j  c  a  t  .  c                                          \n**/
/**                                                                                \n**/
/**     LPJ utility programme. Concatenates restart files.                         \n**/
/**     Program is obsolete for the MPI version of LPJ                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-o filename] lpjfile ...\n"

typedef struct
{
  char *filename;
  Bstruct file;
  long long *index;
  int firstcell;
  int ncell;
} Item;

static int compare(const void *a,const void *b)
{
  if(((const Item *)a)->firstcell<((const Item *)b)->firstcell)
    return -1;
  else if(((const Item *)a)->firstcell==((const Item *)b)->firstcell)
    return 0;
  else
    return 1;
} /* of 'compare' */

int main(int argc,char **argv)
{
  int i,j,k,ncell,count,cell,size,level;
  Item *item;
  long long *index;
  struct
  {
    char *version;
    int year;
    int ncell;
    int npft;
    int ncft;
    Real cellsize_lat;
    Real cellsize_lon;
    int datatype;
    Bool river_routing;
    Bool landuse;
    int sdate_option;
    Bool crop_phu_option;
    Bool separate_harvests;
  } header,header_first;
  Seed seed;
  long long filepos;
  char *s,*arglist,*pftname;
  char **pfts=NULL;
  char *outfile="out.lpj"; /* default name for restart file written */
  Bstruct_data data;
  Bstruct out;
  time_t t;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-o"))
      {
         if(argc==i+1)
         {
           fprintf(stderr,"Missing argument after option '-o'.\n"
                  USAGE,argv[0]);
           return EXIT_FAILURE;
         }

        outfile=argv[++i];
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[i],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  item=(Item *)malloc(sizeof(Item)*(argc-i));
  check(item);
  ncell=count=0;
  if(i==argc)
  {
    fprintf(stderr,"Filename missing.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  /* open all restart files for reading */
  for(;i<argc;i++)
  {
    item[count].filename=argv[i];
    item[count].file=bstruct_open(argv[i],TRUE);
    if(item[count].file==NULL)
    {
      continue;
    }
    /* read header */
    if(bstruct_readbeginstruct(item[count].file,"header"))
    {
      bstruct_finish(item[count].file);
      continue;
    }
    header.version=bstruct_readstring(item[count].file,"version");
    if(header.version==NULL)
    {
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readint(item[count].file,"year",&header.year))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readint(item[count].file,"firstcell",&item[count].firstcell))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readint(item[count].file,"npft",&header.npft))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readint(item[count].file,"ncft",&header.ncft))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readreal(item[count].file,"cellsize_lat",&header.cellsize_lat))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readreal(item[count].file,"cellsize_lon",&header.cellsize_lon))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readint(item[count].file,"datatype",&header.datatype))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readbool(item[count].file,"landuse",&header.landuse))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readint(item[count].file,"sdate_option",&header.sdate_option))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readbool(item[count].file,"crop_phu_option",&header.crop_phu_option))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readbool(item[count].file,"river_routing",&header.river_routing))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readbool(item[count].file,"separate_harvests",&header.separate_harvests))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readbeginarray(item[count].file,"pfts",&size))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(size!=header.npft+header.ncft)
    {
      fprintf(stderr,"Error: Size of PFT array=%d is not %d in file '%s'.\n",
              size,header.npft+header.ncft,argv[i]);
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(pfts==NULL)
    {
      pfts=newvec(char *,size);
      for(j=0;j<size;j++)
      {
        pfts[j]=bstruct_readstring(item[count].file,NULL);
      }
    }
    else
      for(j=0;j<size;j++)
      {
        pftname=bstruct_readstring(item[count].file,NULL);
        if(pftname!=NULL && pfts[j]!=NULL && strcmp(pftname,pfts[j]))
          fprintf(stderr,"WARNING042: PFT name '%s' differs from '%s' in file '%s'.\n",
                  pftname,pfts[j],argv[i]);

        free(pftname);
      }
    bstruct_readendarray(item[count].file,"pfts");
    if(freadseed(item[count].file,"seed",seed))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readendstruct(item[count].file,"header"))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    if(bstruct_readbeginarray(item[count].file,"grid",&item[count].ncell))
    {
      free(header.version);
      bstruct_finish(item[count].file);
      continue;
    }
    item[count].index=newvec(long long,item[count].ncell);
    check(item[count].index);
    if(bstruct_readindexarray(item[count].file,item[count].index,item[count].ncell))
    {
      free(header.version);
      free(item[count].index);
      bstruct_finish(item[count].file);
      continue;
    }
    if(count)
    {
      /* compare settings in header to first file */
      if(strcmp(header.version,header_first.version))
      {
        fprintf(stderr,"ERROR154: LPJ version %s differs from %s in file '%s'.\n",
                header.version,header_first.version,argv[i]);
        free(header.version);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      free(header.version);
      if(header.cellsize_lon!=header_first.cellsize_lon)
      {
        fprintf(stderr,"ERROR154: Cell size longitude %g different from %g in file '%s'.\n",
                header.cellsize_lon,header_first.cellsize_lon,argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.cellsize_lat!=header_first.cellsize_lat)
      {
        fprintf(stderr,"ERROR154: Cell size latitude %g different from %g in file '%s'.\n",
                header.cellsize_lat,header_first.cellsize_lat,argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.landuse!=header_first.landuse)
      {
        fprintf(stderr,"ERROR180: Land-use setting %s is different from %s in file '%s'.\n",
                bool2str(header.landuse),bool2str(header_first.landuse),argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.separate_harvests!=header_first.separate_harvests)
      {
        fprintf(stderr,"ERROR180: Separate harvest setting %s is different from %s in file '%s'.\n",
                bool2str(header.separate_harvests),bool2str(header_first.separate_harvests),argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.river_routing!=header_first.river_routing)
      {
        fprintf(stderr,"ERROR180: River routing setting %s is different from %s in file '%s'.\n",
                bool2str(header.river_routing),bool2str(header_first.river_routing),argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.sdate_option!=header_first.sdate_option)
      {
        fprintf(stderr,"ERROR180: Sowing date option setting %d is different from %d in file '%s'.\n",
                header.sdate_option,header_first.sdate_option,argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.crop_phu_option!=header_first.crop_phu_option)
      {
        fprintf(stderr,"ERROR180: Crop PHU option setting %s is different from %s in file '%s'.\n",
                bool2str(header.crop_phu_option),bool2str(header_first.crop_phu_option),argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.datatype!=header_first.datatype)
      {
        fprintf(stderr,"ERROR180: Datatype %s is different from %s in file '%s'.\n",
                typenames[header.crop_phu_option],typenames[header_first.crop_phu_option],argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.npft!=header_first.npft)
      {
        fprintf(stderr, "ERROR183: Number of natural PFTs=%d does not match %d present in file '%s'.\n",
                header.npft,header_first.npft,argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
      if(header.ncft!=header_first.ncft)
      {
        fprintf(stderr, "ERROR183: Number of crop PFTs=%d does not match %d present in file '%s'.\n",
                header.ncft,header_first.ncft,argv[i]);
        bstruct_finish(item[count].file);
        return EXIT_FAILURE;
      }
    }
    else
      header_first=header;
    ncell+=item[count].ncell;
    count++;
  } /*  for(;i<argc;i++) */
  if(count==0)
  {
    fprintf(stderr,"No restart file successfully read.\n");
    return EXIT_FAILURE;
  }
  /* sort files by first cell index */
  qsort(item,count,sizeof(Item),compare);
  for(i=1;i<count;i++)
  {
    if(item[i-1].firstcell+item[i-1].ncell!=item[i].firstcell)
    {
      fprintf(stderr,"Error: first cell in '%s'=%d not last cell+1=%d\n",
              item[i].filename,
              item[i].firstcell,
              item[i-1].firstcell+item[i-1].ncell);
      return EXIT_FAILURE;
    }
  }
  /* create concatenated restart file */
  out=bstruct_create(outfile);
  if(out==NULL)
    return EXIT_FAILURE;
  /* write header */
  bstruct_writebeginstruct(out,"header");
  bstruct_writestring(out,"version",header_first.version);
  free(header_first.version);
  time(&t);
  arglist=catstrvec(argv,argc);
  check(arglist);
  s=getsprintf("%s: %s",strdate(&t),arglist);
  check(s);
  bstruct_writestring(out,"history",s);
  free(arglist);
  free(s);
  bstruct_writeint(out,"year",header_first.year);
  bstruct_writeint(out,"firstcell",item[0].firstcell);
  bstruct_writeint(out,"npft",header_first.npft);
  bstruct_writeint(out,"ncft",header_first.ncft);
  bstruct_writereal(out,"cellsize_lat",header_first.cellsize_lat);
  bstruct_writereal(out,"cellsize_lon",header_first.cellsize_lon);
  bstruct_writeint(out,"datatype",header_first.datatype);
  bstruct_writebool(out,"landuse",header_first.landuse);
  bstruct_writeint(out,"sdate_option",header_first.sdate_option);
  bstruct_writebool(out,"crop_phu_option",header_first.crop_phu_option);
  bstruct_writebool(out,"river_routing",header_first.river_routing);
  bstruct_writebool(out,"separate_harvests",header_first.separate_harvests);
  if(pfts[i]!=NULL)
  {
    bstruct_writebeginarray(out,"pfts",size);
    for(i=0;i<size;i++)
    {
      bstruct_writestring(out,NULL,pfts[i]);
      free(pfts[i]);
    }
    free(pfts);
    bstruct_writeendarray(out);
  }
  fwriteseed(out,"seed",seed);
  bstruct_writeendstruct(out);
  /* define array with index vector and get position of first element of index vector */
  bstruct_writebeginindexarray(out,"grid",&filepos,ncell);
  index=newvec(long long,ncell);
  check(index);
  k=0;
  /* copy LPJ grids */
  for(i=0;i<count;i++)
  {
    for(cell=0;cell<item[i].ncell;cell++)
    {
      index[k++]=bstruct_getarrayindex(out);
      level=0;
      /* copy one cell */
      do
      {
        if(bstruct_readdata(item[i].file,&data))
        {
          fprintf(stderr,"Error reading cell %d in '%s'.\n",
                  cell,item[i].filename);
          return EXIT_FAILURE;
        }
        switch(data.token)
        {
          case BSTRUCT_BEGINARRAY: case BSTRUCT_BEGINARRAY1: case BSTRUCT_BEGINSTRUCT:
            level++;
            break;
          case BSTRUCT_ENDSTRUCT: case BSTRUCT_ENDARRAY:
            level--;
            break;
          default:
            break;
        }
        bstruct_writedata(out,&data);
        bstruct_freedata(&data);
      }while(level>0);
    }
    bstruct_finish(item[i].file);
  }
  free(item);
  bstruct_writeendarray(out);
  bstruct_writearrayindex(out,filepos,index,0,ncell);
  free(index);
  bstruct_finish(out);
  return EXIT_SUCCESS;
} /* of 'main' */
