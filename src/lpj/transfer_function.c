/**************************************************************************************/
/**                                                                                \n**/
/**    t  r  a  n  s  f  e  r  _  f  u  n  c  t  i  o  n  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates coefficients for the transfer function                 \n**/
/**     used in drain.c to calculate the release from each grid cell               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define TFACTORS_MAX (int)(3/TSTEP)  /* maximum number of transfer coefficients */
#define ISTEPS 100                   /* number of integration steps per TSTEP */

Real *transfer_function(Real rlength, /**< river length in m */
                        int *size     /**< number of transfer coefficients */
                       )              /** \return array of coefficents of transfer function */
{
  Real *transf; /* pointer to coefficients of transfer function */
  Real *data;
  int i,m,k;
  Real sum,ga,dt,n;
  Real u1,u2;

  data=newvec(Real,TFACTORS_MAX);
  if(data==NULL)
    return NULL;
  n=rlength>SEGLEN ? rlength/SEGLEN : 1.0;
  ga=tgamma(n);      /* value of the gamma function */
  dt=TSTEP/ISTEPS;   /* length of integration time step in d */

  sum=0.0;
  *size=0;
  u1=0.0;
  for(i=0;i<TFACTORS_MAX;i++)
  {
    data[i]=0.0;
    for(m=1;m<=ISTEPS;m++)
    {
      u2=1/ga/TAU*pow((i*TSTEP+m*dt)/TAU,(n-1))*exp(-(i*TSTEP+m*dt)/TAU);
      data[i]+=(u1+u2)/2*dt;
      u1=u2;
    }
    if(data[i]<0 || data[i]>1) /* is coefficient positive and lest than one? */
    {
      fprintf(stderr,"ERROR145: Invalid coefficient in transfer function: transf[%d] = %g\n",
              i,data[i]);
      free(data);
      return NULL;
    }
    sum+=data[i];
    (*size)++;
    if((sum>0.99) || ((sum>epsilon) && (data[i]<epsilon)))
      break;
  }

  transf=newvec(Real,*size);
  if(transf==NULL)
  {
    free(data);
    return NULL;
  }
  /* normalize and store coefficients */
  for(k=0;k<(*size);k++)
    transf[k]=data[k]/sum;
  free(data);
  return transf;
} /* of 'transfer_function' */
