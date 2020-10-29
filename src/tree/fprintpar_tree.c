/**************************************************************************************/
/**                                                                                \n**/
/**           f  p  r  i  n  t  p  a  r  _  t  r  e  e  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints tree-specific PFT parameter                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "tree.h"

void fprintpar_tree(FILE *file,       /**< pointer to text file */
                    const Pftpar *par, /**< pointer to tree PFT parameter */
                    const Config *config /**< LPJmL configuration */
                  )
{
  int i;
  const Pfttreepar *partree;
  partree=par->data;
  fprintf(file,"leaftype:\t%s\n"
               "turnover:\t%g %g %g (yr)\n"
               "C:N ratio:\t%g %g %g\n"
               "max crownarea:\t%g (m2)\n"
               "sapling:\t%g %g %g %g (gC/m2/yr)\n"
               "k_latosa:\t%g\n"
               "allometry:\t%g %g %g %g\n",
          leaftype[partree->leaftype],
          1/partree->turnover.leaf,1/partree->turnover.sapwood,1/partree->turnover.root,
          1/partree->nc_ratio.leaf,1/partree->nc_ratio.sapwood,1/partree->nc_ratio.root,
          partree->crownarea_max,
          partree->sapl.leaf.carbon,partree->sapl.sapwood.carbon,
          partree->sapl.heartwood.carbon,partree->sapl.root.carbon,partree->k_latosa,
          partree->allom1,partree->allom2,partree->allom3,partree->allom4);
  if(config->with_nitrogen)
    fprintf(file,"rel. C:N ratio:\t%g %g\n",
            partree->ratio.sapwood,partree->ratio.root);
  if(par->phenology==SUMMERGREEN)
    fprintf(file,"aphen:\t\t%g %g\n",partree->aphen_min,partree->aphen_max);
  fprintf(file,"max height:\t%g (m)\n"
               "reprod cost:\t%g\n"
               "k_est:\t\t%g (1/m2)\n"
               "bark thickness:\t%g %g\n",
          partree->height_max,partree->reprod_cost,
          partree->k_est,partree->barkthick_par1,partree->barkthick_par2);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    fprintf(file,"scorch height:\t%g\n"
                 "crown length:\t%g\n"
                 "crown damage:\t%g %g\n",
            partree->scorchheight_f_param,partree->crownlength,
            partree->crown_mort_rck,partree->crown_mort_p);
  }
  fputs("fuel fraction:\t",file);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file,"%g ",partree->fuelfrac[i]);
  fputc('\n',file);
  if(par->cultivation_type!=NONE)
    fprintf(file,"rotation:\t%d (yr)\n"
                 "max. rotation:\t%d (yr)\n",
            partree->rotation,partree->max_rotation_length);
#if defined IMAGE || defined INCLUDEWP
  if(par->cultivation_type==WP)
    fprintf(file,"P_init:\t\t%g (1/m2)\n",partree->P_init);
#endif
} /* of 'fprintpar_tree' */
