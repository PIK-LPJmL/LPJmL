/**************************************************************************************/
/**                                                                                \n**/
/**                 d  r  a  i  n  a  g  e  .  c                                   \n**/
/**                                                                                \n**/
/**     Program returns vector of cells for drainage                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define NYEAR 1        /* default value for number of years */
#define FIRSTYEAR 0    /* default value for first year */
#define FIRSTCELL 0
#define ORDER CELLYEAR

/* set up direction mapping, change this if your DDM ASCII uses different values */
#define DDM_E 1 /* direction east */
#define DDM_SE 2 /* direction southeast */
#define DDM_S 4 /* direction south */
#define DDM_SW 8 /* direction southwest */
#define DDM_W 16 /* direction west */
#define DDM_NW 32 /* direction northwest */
#define DDM_N 64 /* direction north */
#define DDM_NE 128 /* direction northeast */
#define DDM_outflow 0 /* outflow cell in DDM */
#define DDM_internalsink -1 /* value in DDM given to internal sinks (as opposed to outflow to ocean), set to value not used in DDM file if not distinguished */

#define LPJ_outflow -1 /* value given to outflow cell in LPJ, use negative value because positive values are interpreted as a cell index */
#define LPJ_internalsink -1 /* value given to internal sink cell in LPJ, default: same as outflow */
#define LPJ_nodata -9 /* initialization in LPJ */

typedef struct
{
  Real lon,lat;
  Real ilon, ilat;
  Routing r;
} Drain;

/* Calculate distance (in m) between two points specified by latitude/longitude */
static double distHaversine(double lat1,
                            double lat2,
                            double lon1,
                            double lon2) 
{  /* Haversine version */
  double R = 6371000; /* earth's mean radius in m */
  double dLat  = sin((lat2-lat1)/2);
  double dLong = sin((lon2-lon1)/2);
  double a, b1, b2, c;
  double d;

  lat1 = cos(lat1);
  lat2 = cos(lat2);
  a = dLat * dLat + lat1 * lat2 * dLong * dLong;
  b1 = sqrt(a);
  b2 = sqrt(1-a);
  c = atan2(b1, b2);
  d = R * 2 * c;

  return d;
}

static int cellmatch(int ilon, int ilat, int ncell, Drain *drain)
{
  int i;
  for(i=0;i<ncell;i++)
  {
    if(fabs(ilat-drain[i].ilat)< 0.1 && fabs(ilon-drain[i].ilon) < 0.1)
      return i;
  }
  return LPJ_outflow;
} /* of 'cellmatch' */

static Bool newindex(int ilon, int ilat, int value, int *ilonnew, int *ilatnew)
{
  switch(value)
  {
    case DDM_outflow:
    case DDM_internalsink:
      /* outflow */
      *ilonnew=ilon;
      *ilatnew=ilat;
      return FALSE;
    case DDM_E:
      /* east */
      *ilatnew=ilat;
      *ilonnew=ilon+1;
      return FALSE;
    case DDM_SE:
      /* southeast */
      *ilatnew=ilat-1;
      *ilonnew=ilon+1;
      return FALSE;
    case DDM_S:
      /* south */
      *ilatnew=ilat-1;
      *ilonnew=ilon;
      return FALSE;
    case DDM_SW:
       /* southwest */
      *ilatnew=ilat-1;
      *ilonnew=ilon-1;
      return FALSE;
    case DDM_W:
      /* west */
      *ilatnew=ilat;
      *ilonnew=ilon-1;
      return FALSE;
    case DDM_NW:
      /* northwest */
      *ilatnew=ilat+1;
      *ilonnew=ilon-1;
      return FALSE;
    case DDM_N:
      /* north */
      *ilatnew=ilat+1;
      *ilonnew=ilon;
      return FALSE;
    case DDM_NE:
      /* northeast */
      *ilatnew=ilat+1;
      *ilonnew=ilon+1;
      return FALSE;
    default:
      return TRUE;
  } /* of switch */
} /* of 'newindex' */

int main(int argc,char **argv)
{  
  FILE *mfp;
  FILE *ofp;
  Filename grid_filename;
  Coordfile grid_file;
  Header header;
  int nodata;
  int cols, rows;
  float xcorner, ycorner;
  int ilat, ilon, i, j, c1, c2, c3, c4, c5, c6, progress;
  int value, next_value;
  int *ddm_data;
  Coord coord, resol;
  Drain *drain;
  int fmt=CLM;
  float rbuf2;
  int ilonnew, ilatnew;
  float lon,lat;
  if(argc>1 && !strcmp(argv[1],"-longheader"))
  {
    fmt=CLM2;
    argc--;
    argv++;
  } 
  /* header informations */
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=ORDER;
  header.firstcell=FIRSTCELL;
  header.nbands=2;
  header.datatype=LPJ_INT;
  header.scalar=1;

  ddm_data=NULL;

  /* Parse command line */
  /* e.g. "drainage re-ordered_grid DDM30.asc output" */
  if(argc!=4)
  {
    fprintf(stderr,"Missing argument(s).\n"
                   "Usage: drainage [-longheader] re-ordered_grid DDM30.asc outfile\n");
    return EXIT_FAILURE;
  }

  /* open grid file */
  grid_filename.fmt=fmt;
  grid_filename.name=argv[1];
  grid_file=opencoord(&grid_filename, TRUE);
  if(grid_file==NULL)
    return EXIT_FAILURE;
  getcellsizecoord(&lon,&lat,grid_file);
  resol.lon=lon;
  resol.lat=lat;
 
  /* Open in- & output file */
  if((ofp=fopen(argv[3],"wb")) == NULL)
  {
    fprintf(stderr,"Error: File creation failed on output file '%s': %s\n",
            argv[3],strerror(errno));
    return EXIT_FAILURE;
  }

  if((mfp=fopen(argv[2],"r")) == NULL)
  {
    fprintf(stderr,"Error: File open failed on mask-file '%s': %s\n",
            argv[2],strerror(errno));
    return EXIT_FAILURE;
  } 
  /* read lines of mask file */
  if(fscanf(mfp,"%*s %d",&cols)!=1)
  {
    fprintf(stderr,"Error reading cols in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  /* printf("cols=%d\n",cols); */
  if(fscanf(mfp,"%*s %d",&rows)!=1)
  {
    fprintf(stderr,"Error reading rows in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  /* printf("rows=%d\n",rows); */
  if(fscanf(mfp,"%*s %f",&xcorner)!=1)
  {
    fprintf(stderr,"Error reading xcorner in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  if(fscanf(mfp,"%*s %f",&ycorner)!=1)
  {
    fprintf(stderr,"Error reading ycorner in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  if(fscanf(mfp,"%*s %f",&rbuf2)!=1)
  {
    fprintf(stderr,"Error reading cellsize in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  if(rbuf2 <= 0)
  {
    fprintf(stderr,"Error: invalid cellsize %f in '%s'.\n", rbuf2, argv[2]);
    return EXIT_FAILURE;
  }
  /* printf("cellsize=%d\n",width); */
  fscanf(mfp,"%*s %d",&nodata);
  /*printf("found cols %d rows %d xcorner %f ycorner %f rbuf2 %f nodata %d\n",
   *      cols, rows, xcorner, ycorner, rbuf2, nodata);
   */
  
  /* check if resolution matches */
  if(resol.lon/rbuf2 > 1.0001 || resol.lon/rbuf2 < 0.9999 || resol.lat/rbuf2 > 1.0001 || resol.lat/rbuf2 < 0.9999)
  {
    fprintf(stderr,"Error: resolution mismatch between '%s' and '%s': %f different from (%f, %f).\n", argv[2], argv[1], rbuf2, resol.lon, resol.lat);
    return EXIT_FAILURE;
  }
  /* set resolution of output file to grid resolution */
  header.cellsize_lon = (float)resol.lon;
  header.cellsize_lat = (float)resol.lat;

  drain=newvec(Drain,numcoord(grid_file));
  if(drain==NULL)
  {
    printallocerr("drain");
    return EXIT_FAILURE;
  }
  for(i=0;i<numcoord(grid_file);i++)
  {
    /* initialisation of output array */  
    drain[i].r.index=LPJ_nodata; /* initialize with fill value */
    drain[i].r.len=0;
    /* read input file and make array for longitude and latitude */
    if(readcoord(grid_file, &coord, &resol))
    {
      fprintf(stderr, "Error: Unexpected end of grid file '%s' in cell %d", argv[1], i);
      closecoord(grid_file);
      fclose(ofp);
      fclose(mfp);
      return EXIT_FAILURE;
    }
    drain[i].lon= coord.lon;
    drain[i].ilon = ((coord.lon-resol.lon/2)-xcorner)/resol.lon; /* corresponds to ilon index in drainage data */
    drain[i].lat=coord.lat;
    drain[i].ilat = ((coord.lat-resol.lat/2)-ycorner)/resol.lat; /* corresponds to ilat index in drainage data */
    if(drain[i].ilon < -0.00001 || drain[i].ilon >=cols || drain[i].ilat < -0.00001 || drain[i].ilat >= rows)
    {
      fprintf(stderr, "Error: cell %d in '%s' (%.5f E, %.5f N) is outside the geographical extent of '%s' (%.5f - %.5f E, %.5f - %.5f N)\n.", i, argv[1], coord.lon, coord.lat, argv[2], xcorner, xcorner+rbuf2*cols, ycorner, ycorner+rbuf2*rows);
      closecoord(grid_file);
      fclose(ofp);
      fclose(mfp);
      return EXIT_FAILURE;
    }
  }

  ddm_data=newvec(int, cols*rows);
  if(ddm_data==NULL)
  {
    printallocerr("ddm_data");
    return EXIT_FAILURE;
  }

  c1=c2=c3=c4=c5=c6=0;
  progress = (int)(cols*rows*0.01);
  printf("Processing %d * %d cells in '%s'\n", cols, rows, argv[2]);
  printf("LPJmL grid '%s' has %d cells\n", argv[1], numcoord(grid_file));
  /* read full DDM data to be able to search for follow-up cells */
  for(ilat=rows-1;ilat>=0;ilat--)
  {
    /* printf("ilon= %d\n",ilon); */
    for(ilon=0;ilon<cols;ilon++)
    {
      /* read discrete values of mask file */
      if(fscanf(mfp,"%d",ddm_data+ilat*cols+ilon)!=1)
      {
        fprintf(stderr,"Error reading data in '%s' at (%d,%d).\n",argv[2],ilon,ilat);
        return EXIT_FAILURE;
      }
      /* printf("%d %d %d\n",ilat,ilon,value); */
    }
  }
  fclose(mfp);
  for(ilat=0;ilat<rows;ilat++)
  {
    for(ilon=0;ilon<cols;ilon++)
    {
      value=ddm_data[ilat*cols+ilon];
      c1++; /* running counter */
      if(c1 >= progress)
      {
        printf("%.1f%s... ", c1*100.0/(cols*rows), "%");
        progress+=(int)(cols*rows*0.01);
        fflush(stdout);
      }

      if(value == nodata)
        continue; /* no need to look for matching LPJ cell if fill value in DDM */
      c2++; /* counter for DDM cells without fill value */
      if(value == DDM_outflow || value == DDM_internalsink)
      {
        if((i = cellmatch(ilon, ilat, numcoord(grid_file), drain)) >= 0)
        {
          drain[i].r.index=(value==DDM_outflow ? LPJ_outflow : LPJ_internalsink);
          drain[i].r.len=0;
          c5++; /* counter for outflow cells */
          c3++; /* counter for DDM cells found in LPJ grid */
        } /* else
     fprintf(stdout, "Outflow cell (%d, %d) not found in LPJmL grid\n", ilon, ilat); */
      }
      ilonnew=ilon;
      ilatnew=ilat;
      if(value != DDM_outflow && value != nodata && value != DDM_internalsink)
      {
        if((i = cellmatch(ilon, ilat, numcoord(grid_file), drain)) < 0)
        {
          /* cell not in LPJ grid */
          // fprintf(stdout, "Intermediate cell (%d, %d) not found in LPJmL grid\n", ilon, ilat);
          continue; /* skip to next ilon iteration */
        }
        c3++;
        if(newindex(ilon, ilat, value, &ilonnew, &ilatnew))
        {
          fprintf(stderr, "Error: invalid drainage direction value %d in cell (%d, %d). Skipping cell\n", value, ilon, ilat);
          continue;
        }
        /* check for flow direction crossing date line */
        if(ilonnew < 0 && xcorner < -179.999999)
        {
          if(xcorner+cols*rbuf2 > 179.999999)
            ilonnew=cols-1;
        }
        if(ilonnew >= cols && xcorner+cols*rbuf2 > 179.999999)
        {
          if(xcorner < -179.999999)
            ilonnew = 0;
        }
        if(ilonnew < 0 || ilonnew >= cols || ilatnew < 0 || ilatnew >= rows)
        {
          fprintf(stderr, "Error: destination cell (%d, %d) outside geographical extent of DDM\n", ilonnew, ilatnew);
          next_value = nodata;
        }
        else
          next_value = ddm_data[ilatnew*cols+ilonnew]; /* drainage direction of following cell */

        while((j = cellmatch(ilonnew, ilatnew, numcoord(grid_file), drain)) < 0 && next_value != DDM_outflow && next_value != DDM_internalsink && next_value != nodata)
        {
          fprintf(stdout, "Destination cell (%d, %d) of (%d, %d) not found in LPJmL grid. Searching DDM further down-stream\n", ilonnew, ilatnew, ilon, ilat);
          /* destination cell not in LPJ grid (cellmatch() < 0), but destination cell in DDM (next_values > 0)
           * get next cell according to DDM
           */
          if(newindex(ilonnew, ilatnew, next_value, &ilonnew, &ilatnew))
          {
            fprintf(stderr, "Invalid drainage direction value %d in cell (%d, %d). Setting to nodata value\n", value, ilonnew, ilatnew);
            next_value = nodata;
            break;
          }
          /* check for flow direction crossing date line */
          if(ilonnew < 0 && xcorner < -179.999999)
          {
            if(xcorner+cols*rbuf2 > 179.999999)
              ilonnew=cols-1;
          }
          if(ilonnew >= cols && xcorner+cols*rbuf2 > 179.999999)
          {
            if(xcorner < -179.999999)
              ilonnew = 0;
          }
          if(ilonnew < 0 || ilonnew >= cols || ilatnew < 0 || ilatnew >= rows)
          {
            fprintf(stderr, "Error: destination cell (%d, %d) outside geographical extent of DDM\n", ilonnew, ilatnew);
            next_value = nodata;
          }
          else
            next_value = ddm_data[ilatnew*cols+ilonnew]; /* drainage direction of following cell */
        }
        /* j now has the index of an LPJ cell, or a following DDM cell is an outflow cell / nodata value */
        if(j>=0)
        {
          /* cell found */
          drain[i].r.index=j;
          drain[i].r.len=(int)distHaversine(drain[i].lat*M_PI/180,drain[j].lat*M_PI/180,drain[i].lon*M_PI/180,drain[j].lon*M_PI/180);
          c4++;
        }
        else
        {
          switch (next_value)
          {
            case DDM_outflow:
              drain[i].r.index = LPJ_outflow;
              c5++;
              break;
            case DDM_internalsink:
              drain[i].r.index = LPJ_internalsink;
              c5++;
              break;
            default:
              /* note: nodata should not happen, points to error in DDM */
              drain[i].r.index = LPJ_nodata;
          }
        }
      } /* end value > 0 */
    } /* end of ilon-loop */
  } /* end of ilat-loop */

  /* HEADER */
  header.ncell=numcoord(grid_file);
  fwriteheader(ofp,&header,LPJDRAIN_HEADER,LPJDRAIN_VERSION);

  /* write data into file */
  for(i=0;i<header.ncell;i++) 
  {
    //printf("cell %d: d=%d l=%d\n", i, drain[i].r.index,drain[i].r.len);
    /* write binary file */
    fwrite(&drain[i].r,sizeof(Routing),1,ofp);
    if(drain[i].r.index == LPJ_nodata)
      c6++;
  }
  fclose(ofp);
  closecoord(grid_file);
  printf("DDM cells with value:             %d\n"
         "Cells found in LPJmL grid:        %d\n"
         "Intermediate cells in LPJmL grid: %d\n"
         "Outflow cells in LPJmL grid:      %d\n"
         "Fill values left in LPJmL grid:   %d\n",c2,c3,c4, c5, c6);
  free(drain);
  free(ddm_data);
  return EXIT_SUCCESS;
} /* of 'main' */
