/**************************************************************************************/
/**                                                                                \n**/
/**       c  a  l  c  _   s  e  a  s  o  n  a  l  i  t  y  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

static double variation_coefficient(Real data[],int n)
{
  Real avg,dev=0,varcoe=0,sum=0;
  int i;
  double std=0;
  for (i=0;i<n;i++)
    sum+=data[i];
  avg=sum/n;
  for (i=0;i<n;i++)
    dev+=(data[i]-avg)*(data[i]-avg);
  std=sqrt(dev/(n-1));
  if (std>0 && avg>0) /*just if temperature and precipitation data appear in the cell*/
    varcoe=std/avg;
  return varcoe;
} /* of 'variation_coefficient' */

#define TEMPMIN 10 /*minimum temperature of coldest month (deg C) */

void calc_seasonality(Cell *grid,          /**< cell grid array */
                      int npft,            /**< Number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      const Config *config /**< LPJ configuration data */
                     )
{
  int m,n,monthbefore,cft,cell,i,c,earliestdayofmonth,earliest_sdate,earliest_smonth;
  Real earliest_sdatetemp,earliestbefore_sdatetemp,m_temp,monthbefore_temp,sum,max;
  int mm=0;
  Real var_temp=0, var_prec=0;
  Real mtemp20kelvin[NMONTH]={0}, p_pet20[NMONTH]={0};
  Pftcroppar *croppar;
  int *firstwintermonth,*firstspringmonth,*firstwetmonth;
  firstwintermonth=newvec(int,ncft);
  check(firstwintermonth);
  firstspringmonth=newvec(int,ncft);
  check(firstspringmonth);
  firstwetmonth=newvec(int,ncft);
  check(firstwetmonth);

  for (cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      for (n=0;n<NMONTH;n++) /*just need a monthly loop*/
      {
        mtemp20kelvin[n]= grid[cell].climbuf.mtemp20[n]+273.15; /*convert deg C to Kelvin*/
        p_pet20[n]      = (grid[cell].climbuf.mpet20[n] > 0) ? grid[cell].climbuf.mprec20[n]/grid[cell].climbuf.mpet20[n] : 0; /*calculate P/PET ratio if monthly PET is above zero*/
      }
      var_temp=variation_coefficient(mtemp20kelvin,NMONTH);
      var_prec=variation_coefficient(grid[cell].climbuf.mprec20,NMONTH);

      for (cft=0;cft<ncft;cft++)
      {
        croppar=config->pftpar[npft+cft].data;
        earliest_sdate=(grid[cell].coord.lat>=0) ? croppar->initdate.sdatenh : croppar->initdate.sdatesh;
        cvrtdaymonth(&earliestdayofmonth,&earliest_smonth,earliest_sdate);
        firstwintermonth[cft]=firstspringmonth[cft]=firstwetmonth[cft]=0;

        /*find first month when temperature exceeds (summer variety) or falls below (winter variety) a crop-specific threshold*/      

        /*first look for sowing month of winter variety*/
        if (croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE) /*winter and summer variety*/
        {
          for (m=0;m<NMONTH;m++)
          {
            monthbefore = (m==0) ? NMONTH-1 : m-1; 
            m_temp = grid[cell].climbuf.mtemp20[m];
            monthbefore_temp = grid[cell].climbuf.mtemp20[monthbefore];

            /*if temp of this month falls below temp_fall the first time and temp of last day of month before is not yet below temp_fall,*/
            /*this month is sowing month for winter variety, otherwise the month before is sowing month*/
            /*interpolate.c returns the daily climbuf value from monthly value*/
            if(m_temp < croppar->temp_fall && monthbefore_temp >= croppar->temp_fall)
            {
              firstwintermonth[cft] = 
                (interpolate(grid[cell].climbuf.mtemp20,monthbefore,ndaymonth[monthbefore]) < croppar->temp_fall) ? monthbefore : m;
              firstwintermonth[cft] = firstwintermonth[cft]+1; /*force firstwintermonth to run from 1-12*/
              break;
            }
          }
        }
        /*look for sowing month of summer variety*/
        for (m=0;m<NMONTH;m++)
        {
          monthbefore = (m==0) ? NMONTH-1 : m-1;
          m_temp = grid[cell].climbuf.mtemp20[m];
          monthbefore_temp = grid[cell].climbuf.mtemp20[monthbefore];

          if(m_temp > croppar->temp_spring && monthbefore_temp <= croppar->temp_spring)
          {
            firstspringmonth[cft] = 
              (interpolate(grid[cell].climbuf.mtemp20,monthbefore,ndaymonth[monthbefore]) > croppar->temp_spring) ? monthbefore : m;
            firstspringmonth[cft] = firstspringmonth[cft]+1; /*force firstspringmonth to run from 1-12*/
            break;
          }
        }
        /*force to sow in january if no spring month was found -> important for always warm and always cold regions*/
        if (firstspringmonth[cft]==0)
          firstspringmonth[cft]=DEFAULT_MONTH;

        /*find sowing month and calculate length of growing season depending on seasonality tpye*/
        grid[cell].ml.seasonality_type=NO_SEASONALITY;
        
        /*start calculating the sowing month depending on the seasonality type*/
        if (var_prec<=0.4 && var_temp<=0.010) /*no seasonality*/
        {
          grid[cell].ml.sowing_month[cft]=DEFAULT_MONTH; 
          grid[cell].ml.sowing_month[cft+ncft]=DEFAULT_MONTH;
        }

        if (var_prec>0.4 && var_temp<=0.010) /*precipitation seasonality only*/
          grid[cell].ml.seasonality_type=PREC;            
        if (var_prec>0.4 && (var_temp>0.010 && grid[cell].climbuf.mtemp_min20>TEMPMIN)) /*both seasonalities, but "weak" temperature seasonality (coldest month > 10 deg C)*/
          grid[cell].ml.seasonality_type=PRECTEMP;  
     
        if (grid[cell].ml.seasonality_type==PREC || grid[cell].ml.seasonality_type==PRECTEMP)
        {
          /*find first month of four wettest months (sum of their precipiation/PET ratios)*/ 
          max=sum=0;
          for (m=0;m<NMONTH;m++)  
          {       
            sum=0;
            for (i=0;i<MINLGP;i++)
            {
              mm = (m+i >= NMONTH) ? m+i-NMONTH : m+i;
              sum+= p_pet20[mm];
            }
            if (sum>max)
            {
              max=sum;
              firstwetmonth[cft]=m;
              firstwetmonth[cft]=firstwetmonth[cft]+1; /*force firstwetmonth to run from 1-12*/
            }
          }
          /*force to sow in january if no firstwetmonth was found*/
          if (firstwetmonth[cft]==0)
            firstwetmonth[cft]=DEFAULT_MONTH;

          grid[cell].ml.sowing_month[cft]=firstwetmonth[cft]; /*start of four wettest months*/
          grid[cell].ml.sowing_month[cft+ncft]=firstwetmonth[cft]; 
                    
          /*calculate length of growing season for just precipitation seasonality*/
          for (m=(grid[cell].ml.sowing_month[cft]-1);m<(grid[cell].ml.sowing_month[cft]-1+NMONTH);m++) /*m runs from 0 to 11, but sowing momth from 1 to 12*/
          {
            /*m can run from 11..22, mm is the reduced m to ask for climate data between 0 and 11*/
            mm = (m >= NMONTH || m == NMONTH-1) ? m-NMONTH : m; /*if month is in next year or month is december and month+1 will be in next year*/
            if (p_pet20[mm+1]<=max/MINLGP) /*last month with at least prec/pet ratio more than mean of four wettest months*/
            {
              grid[cell].ml.gs[cft]=m-grid[cell].ml.sowing_month[cft]+1;
              grid[cell].ml.gs[cft+ncft]=m-grid[cell].ml.sowing_month[cft]+1;
              break;
            }
          }  
        } /*of precipitation seasonality*/

        if (var_temp>0.010 && var_prec<=0.4) /*temperature seasonality only*/
           grid[cell].ml.seasonality_type=TEMP;
        if (var_temp>0.010 && (var_prec>0.4 && grid[cell].climbuf.mtemp_min20<=TEMPMIN)) /*both seasonalities but "strong" temperature seasonality (coldest month <= 10 deg C)*/  
           grid[cell].ml.seasonality_type=TEMPPREC;
         
        if (grid[cell].ml.seasonality_type==TEMP || grid[cell].ml.seasonality_type==TEMPPREC)
        {
          earliest_sdatetemp=interpolate(grid[cell].climbuf.mtemp20,earliest_smonth-1,earliestdayofmonth);
          earliestbefore_sdatetemp=interpolate(grid[cell].climbuf.mtemp20,earliest_smonth-1,earliestdayofmonth-1);
          if (croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE && (firstwintermonth[cft] > earliest_smonth 
            || (earliest_sdatetemp > croppar->temp_fall && earliestbefore_sdatetemp < croppar->temp_fall))) /*winter and summer variety exist and winter not too long*/
          {
            grid[cell].ml.sowing_month[cft] = firstwintermonth[cft];
            grid[cell].ml.sowing_month[cft+ncft] = firstwintermonth[cft];
                       
            /*calculate length of growing season for winter variety*/ 
            /*NOT NICE YET*/
            if (firstspringmonth[cft] > firstwintermonth[cft]) 
            {
              grid[cell].ml.gs[cft] = firstspringmonth[cft]-firstwintermonth[cft];
              grid[cell].ml.gs[cft+ncft] = firstspringmonth[cft]-firstwintermonth[cft];
            }
            else /*spring in next year*/
            {
              grid[cell].ml.gs[cft] = firstspringmonth[cft]+firstwintermonth[cft];
              grid[cell].ml.gs[cft+ncft] = firstspringmonth[cft]+firstwintermonth[cft];
            }
          }
          else /*other calc_method or winter too long*/ 
          {
            grid[cell].ml.sowing_month[cft] = firstspringmonth[cft];
            grid[cell].ml.sowing_month[cft+ncft] = firstspringmonth[cft];
            
            /*calculate length of growing season for spring variety*/
            c=0;
            for (m=(grid[cell].ml.sowing_month[cft]-1);m<(grid[cell].ml.sowing_month[cft]-1+NMONTH);m++) /*m runs from 0 to 11, but firstspringmonth from 1 to 12*/
            {
              mm = (m >= NMONTH) ? m-NMONTH : m;
              if (grid[cell].climbuf.mtemp20[mm] >= croppar->basetemp.low) 
              {
                c++;
                /*Twindow[mm]=1;*/
              }
              else if(c>0)
                break;
            } 
            grid[cell].ml.gs[cft]=c;
            grid[cell].ml.gs[cft+ncft]=c;
          }
        } /*of temperature seasonality*/   
      } /*of loop through cfts*/
    } /*of if(!grid[cell].skip*/
  } /*of for (cell=0;cell<config->ngridcell;cell++)*/
  free(firstwintermonth);
  free(firstspringmonth);
  free(firstwetmonth);
  
} /*of 'calc_seasonality' */

/*called in iterate.c */
