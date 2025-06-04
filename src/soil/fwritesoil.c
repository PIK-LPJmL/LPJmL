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

static Bool fwritepoolpararray(Bstruct file,const char *name,const Poolpar *pool,int size)
{
  int i;
  bstruct_writebeginarray(file,name,size);
  for(i=0;i<size;i++)
  {
    bstruct_writebeginstruct(file,NULL);
    bstruct_writereal(file,"slow",pool[i].slow);
    bstruct_writereal(file,"fast",pool[i].fast);
    bstruct_writeendstruct(file);
  }
  return bstruct_writeendarray(file);
} /* of 'fwritepoolpararray' */

Bool fwritesoil(Bstruct file,     /**< pointer to restart file */
                const char *name, /**< name of object */
                const Soil *soil, /**< soil data to be written */
                int ntotpft       /**< total number of PFTs */
               )                  /** \return TRUE on error */
{
  int l;
  bstruct_writebeginstruct(file,name);
  bstruct_writebeginarray(file,"pool",LASTLAYER);
  forrootsoillayer(l)
  {
    fwritepool(file,NULL,soil->pool+l);
  }
  bstruct_writeendarray(file);
  bstruct_writebeginarray(file,"c_shift",LASTLAYER);
  forrootsoillayer(l)
  {
    fwritepoolpararray(file,NULL,soil->c_shift[l],ntotpft);
  }
  bstruct_writeendarray(file);
  if(fwritelitter(file,"litter",&soil->litter))
    return TRUE;
  bstruct_writerealarray(file,"NO3",soil->NO3,LASTLAYER);
  bstruct_writerealarray(file,"NH4",soil->NH4,LASTLAYER);
  bstruct_writerealarray(file,"CH4",soil->CH4,LASTLAYER);
  bstruct_writerealarray(file,"O2",soil->O2,LASTLAYER);
  bstruct_writerealarray(file,"wsat",soil->wsat, NSOILLAYER);
  bstruct_writerealarray(file,"wpwp",soil->wpwp, NSOILLAYER);
  bstruct_writerealarray(file,"wfc",soil->wfc, NSOILLAYER);
  bstruct_writerealarray(file,"whc",soil->whc, NSOILLAYER);
  bstruct_writerealarray(file,"whcs",soil->whcs, NSOILLAYER);
  bstruct_writerealarray(file,"wpwwps",soil->wpwps, NSOILLAYER);
  bstruct_writerealarray(file,"wsats",soil->wsats, NSOILLAYER);
  bstruct_writerealarray(file,"beta_soil",soil->beta_soil, NSOILLAYER);
  bstruct_writerealarray(file,"bulkdens",soil->bulkdens, NSOILLAYER);
  bstruct_writerealarray(file,"k_dry",soil->k_dry, NSOILLAYER);
  bstruct_writerealarray(file,"Ks",soil->Ks, NSOILLAYER);
  bstruct_writerealarray(file,"b",soil->b, NSOILLAYER);
  bstruct_writerealarray(file,"psi_sat",soil->psi_sat, NSOILLAYER);
  bstruct_writerealarray(file,"df_tillage",soil->df_tillage, NTILLLAYER);
  bstruct_writerealarray(file,"w",soil->w,NSOILLAYER);
  bstruct_writereal(file,"w_evap",soil->w_evap);
  bstruct_writerealarray(file,"w_fw",soil->w_fw,NSOILLAYER);
  bstruct_writereal(file,"snowpack",soil->snowpack);
  bstruct_writereal(file,"snowheight",soil->snowheight);
  bstruct_writereal(file,"snowfraction",soil->snowfraction);
  bstruct_writerealarray(file,"temp",soil->temp,NSOILLAYER+1);
  bstruct_writerealarray(file,"amean_temp",soil->amean_temp,NSOILLAYER+1);
  bstruct_writerealarray(file,"enth",soil->enth,NHEATGRIDP);
  bstruct_writerealarray(file,"wi_abs_enth_adj",soil->wi_abs_enth_adj,NSOILLAYER);
  bstruct_writerealarray(file,"soil_abs_enth_adj",soil->sol_abs_enth_adj,NSOILLAYER);
  bstruct_writerealarray(file,"ice_depth",soil->ice_depth,NSOILLAYER);
  bstruct_writerealarray(file,"ice_fw",soil->ice_fw,NSOILLAYER);
  bstruct_writerealarray(file,"freeze_depth",soil->freeze_depth,NSOILLAYER);
  bstruct_writerealarray(file,"ice_pwp",soil->ice_pwp,NSOILLAYER);
  bstruct_writerealarray(file,"perc_energy",soil->perc_energy,NSOILLAYER);
  bstruct_writeshortarray(file,"state",soil->state,NSOILLAYER);
  bstruct_writereal(file,"mean_maxthaw",soil->mean_maxthaw);
  bstruct_writereal(file,"alag",soil->alag);
  bstruct_writereal(file,"amp",soil->amp);
  bstruct_writereal(file,"rw_buffer",soil->rw_buffer);
  fwritepoolpararray(file,"k_mean",soil->k_mean,LASTLAYER);
  fwritepoolpararray(file,"decay_rate",soil->decay_rate,LASTLAYER);
  fwritestocks(file,"decomp_litter_mean",&soil->decomp_litter_mean);
  fwritestocksarray(file,"decomp_litter_pft",soil->decomp_litter_pft,ntotpft);
  bstruct_writeint(file,"count",soil->count);
  bstruct_writereal(file,"meanw1",soil->meanw1);
  bstruct_writereal(file,"wtable",soil->wtable);
  bstruct_writereal(file,"wa",soil->wa);
  bstruct_writebool(file,"iswetland",soil->iswetland);
  bstruct_writereal(file,"snowdens",soil->snowdens);
  bstruct_writereal(file,"fastfrac",soil->fastfrac);
  return bstruct_writeendstruct(file);
} /* of 'fwritesoil' */
