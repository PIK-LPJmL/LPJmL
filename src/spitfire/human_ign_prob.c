/**************************************************************************************/
/**                                                                                \n**/
/**               h  u  m  a  n  _  i  g  n  _  p  r  o  b  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of datatype Human_ign_prob                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct human_ign_prob
{
  Climatefile file;
  Real *human_ign_prob;
}; /* Definition of opaque datatype Human_ign_prob */

Human_ign_prob inithuman_ign_prob(const Config *config /**< LPJ configuration */
                                 )                     /** \return pointer to human ignition probability
                                                            struct or NULL */
{
  Human_ign_prob human_ign_prob;
  int i;

  if(config->human_ign_prob_filename.name==NULL)
    return NULL;
  human_ign_prob=new(struct human_ign_prob);
  if(human_ign_prob==NULL)
    return NULL;
  if(opendata(&human_ign_prob->file,NULL,&config->human_ign_prob_filename,"human ignition probability density",NULL,LPJ_FLOAT,LPJ_SHORT,1.0,1,TRUE,config))
  {
    free(human_ign_prob);
    return NULL;
  }
  if(isroot(*config) && config->lastyear>human_ign_prob->file.firstyear+human_ign_prob->file.nyear-1)
    fprintf(stderr,"WARNING024: Last year in human_ign_prob data file=%d is less than last simulation year %d, data from last year used.\n",
            human_ign_prob->file.firstyear+human_ign_prob->file.nyear-1,config->lastyear);

  human_ign_prob->file.n=config->ngridcell;
  if((human_ign_prob->human_ign_prob=newvec(Real,human_ign_prob->file.n))==NULL)
  {
    printallocerr("nhuman_ign_prob");
    closeclimatefile(&human_ign_prob->file,isroot(*config));
    free(human_ign_prob);
    return NULL;
  }
  for(i=0;i<human_ign_prob->file.n;i++)
    human_ign_prob->human_ign_prob[i]=0;
  return human_ign_prob;
} /* of 'inithuman_ign_prob' */

/*
- called in lpj()
- opens the human_ign_prob input file (see also building file for the human_ign_prob Input (like cfts26_lu2clm.c)
- sets the human_ign_prob variables (see also manage.h)
*/

Bool readhuman_ign_prob(Human_ign_prob human_ign_prob,     /**< pointer to population data */
                 int year,            /**< year (AD) */
                 const Cell grid[],   /**< LPJ grid */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  return (readdata(&human_ign_prob->file,human_ign_prob->human_ign_prob,grid,"human ignition probability",year,config)==NULL);
} /* of 'readhuman_ign_prob' */

Real gethuman_ign_prob(const Human_ign_prob human_ign_prob,int cell)
{
  return human_ign_prob->human_ign_prob[cell];
}  /* of 'gethuman_ign_prob' */

void freehuman_ign_prob(Human_ign_prob human_ign_prob,Bool isroot)
{
  if(human_ign_prob!=NULL)
  {
    closeclimatefile(&human_ign_prob->file,isroot);
    free(human_ign_prob->human_ign_prob);
    free(human_ign_prob);
  }
} /* of 'freehuman_ign_prob' */
