/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  s  o  i  l  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function write soil variables into restart file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static Bool fwritepool(FILE *file,const char *name,const Pool *pool)
{
  writestruct(file,name);
  writestocks(file,"slow",&pool->slow);
  writestocks(file,"fast",&pool->fast);
  return writeendstruct(file);
} /* of 'fwritepool' */

static Bool fwritepoolpararray(FILE *file,const char *name,const Poolpar *pool,int size)
{
  int i;
  writearray(file,name,size);
  for(i=0;i<size;i++)
  {
    writestruct(file,NULL);
    writereal(file,"slow",pool[i].slow);
    writereal(file,"fast",pool[i].fast);
    writeendstruct(file);
  }
  return writeendarray(file);
} /* of 'fwritepoolpararray' */

Bool fwritesoil(FILE *file,       /**< pointer to restart file */
                const char *name, /**< name of object */
                const Soil *soil, /**< soil data to be written */
                int ntotpft       /**< total number of PFTs */
               )                  /** \return TRUE on error */
{
  int l;
  writestruct(file,name);
  writearray(file,"pool",LASTLAYER);
  forrootsoillayer(l)
  {
    fwritepool(file,NULL,soil->pool+l);
  }
  writeendarray(file);
  writearray(file,"c_shift",LASTLAYER);
  forrootsoillayer(l)
  {
    fwritepoolpararray(file,NULL,soil->c_shift[l],ntotpft);
  }
  writeendarray(file);
  if(fwritelitter(file,"litter",&soil->litter))
    return TRUE;
  writerealarray(file,"NO3",soil->NO3,LASTLAYER);
  writerealarray(file,"NH4",soil->NH4,LASTLAYER);
  writerealarray(file,"wsat",soil->wsat, NSOILLAYER);
  writerealarray(file,"wpwp",soil->wpwp, NSOILLAYER);
  writerealarray(file,"wfc",soil->wfc, NSOILLAYER);
  writerealarray(file,"whc",soil->whc, NSOILLAYER);
  writerealarray(file,"whcs",soil->whcs, NSOILLAYER);
  writerealarray(file,"wpwwps",soil->wpwps, NSOILLAYER);
  writerealarray(file,"wsats",soil->wsats, NSOILLAYER);
  writerealarray(file,"beta_soil",soil->beta_soil, NSOILLAYER);
  writerealarray(file,"bulkdens",soil->bulkdens, NSOILLAYER);
  writerealarray(file,"k_dry",soil->k_dry, NSOILLAYER);
  writerealarray(file,"Ks",soil->Ks, NSOILLAYER);
  writerealarray(file,"df_tillage",soil->df_tillage, NTILLLAYER);
  writerealarray(file,"w",soil->w,NSOILLAYER);
  writereal(file,"w_evap",soil->w_evap);
  writerealarray(file,"w_fw",soil->w_fw,NSOILLAYER);
  writereal(file,"snowpack",soil->snowpack);
  writereal(file,"snowheight",soil->snowheight);
  writereal(file,"snowfraction",soil->snowfraction);
  writerealarray(file,"temp",soil->temp,NSOILLAYER+1);
  writerealarray(file,"enth",soil->enth,NHEATGRIDP);
  writerealarray(file,"wi_abs_enth_adj",soil->wi_abs_enth_adj,NSOILLAYER);
  writerealarray(file,"soil_abs_enth_adj",soil->sol_abs_enth_adj,NSOILLAYER);
  writerealarray(file,"ice_depth",soil->ice_depth,NSOILLAYER);
  writerealarray(file,"ice_fw",soil->ice_fw,NSOILLAYER);
  writerealarray(file,"freeze_depth",soil->freeze_depth,NSOILLAYER);
  writerealarray(file,"ice_pwp",soil->ice_pwp,NSOILLAYER);
  writerealarray(file,"perc_energy",soil->perc_energy,NSOILLAYER);
  writeshortarray(file,"state",soil->state,NSOILLAYER);
  writereal(file,"mean_maxthaw",soil->mean_maxthaw);
  writereal(file,"alag",soil->alag);
  writereal(file,"amp",soil->amp);
  writereal(file,"rw_buffer",soil->rw_buffer);
  fwritepoolpararray(file,"k_mean",soil->k_mean,LASTLAYER);
  fwritepoolpararray(file,"decay_rate",soil->decay_rate,LASTLAYER);
  writestocks(file,"decomp_litter_mean",&soil->decomp_litter_mean);
  writestocksarray(file,"decomp_litter_pft",soil->decomp_litter_pft,ntotpft);
  writeint(file,"count",soil->count);
  writereal(file,"meanw1",soil->meanw1);
  return writeendstruct(file);
} /* of 'fwritesoil' */
