/**************************************************************************************/
/**                                                                                \n**/
/**                   c  o  n  f  i  g  .  h                                       \n**/
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

#ifndef CONFIG_H /* Already included? */
#define CONFIG_H

/* Definition of datatypes */


typedef struct
{
  Filename filename; /**< Filename of output file */
  int id;
  Bool oneyear;
} Outputvar;

typedef struct
{
  Real lon_min;
  Real lon_max;
  Real lat_min;
  Real lat_max;
  Real lon_res;
  Real lat_res;
} Extension;

typedef struct
{
  const char *filename;       /**< LPJ configuration filename */
  char *arglist;              /**< command line */
  char *inputdir;             /**< input directory */
  char *outputdir;            /**< output directory */
  char *restartdir;           /**< restart directory */
  Filename temp_filename;
  Filename prec_filename;
  Filename cloud_filename;
  Filename wet_filename;
  Filename wind_filename;
  Filename tamp_filename;
  Filename tmax_filename;
  Filename humid_filename;
  Filename lightning_filename;
  Filename lwnet_filename;
  Filename swdown_filename;
  Filename popdens_filename;
  Filename human_ignition_filename;
  Filename co2_filename;
  Filename drainage_filename;
  Filename neighb_irrig_filename;
  Filename coord_filename;
  Filename soil_filename;
  Filename soilph_filename;
  Filename river_filename;
  Filename countrycode_filename;
  Filename regioncode_filename;
  Filename landuse_filename;
  Filename fertilizer_nr_filename;
  Filename no3deposition_filename;
  Filename nh4deposition_filename;
  char *restart_filename;
  Filename lakes_filename;
  Filename wateruse_filename;
  Filename elevation_filename;
  Filename reservoir_filename;
  Filename sdate_filename;
  Filename burntarea_filename;
  Filename landcover_filename;
  Filename runoff2ocean_filename; /**< map with indices of the ocean cells where the coast cells send their runoff into */
  Filename grassfix_filename;
  Filename grassharvest_filename;
#ifdef IMAGE
  Filename temp_var_filename; /**< file with CRU-derived temperature variation (K) */
  Filename prec_var_filename; /**< file with CRU-derived precipitation variation (%) */
  Filename prodpool_init_filename; /* file with initial product pool sizes */
  char *image_host;       /**< hostname for computer running the IMAGE model */
  int image_inport;       /**< port numbert for ingoing data */
  int image_outport;      /**< port number for outgoing data */
  int wait_image;         /**< time to wait for image connection (sec) */
#endif
  char *sim_name;         /**< Desciption of LPJ simulation */
  int sim_id;             /**< Simulation type */
  int *npft;              /**< number of PFTs in each PFT class */
  int nbiomass;           /**< number of biomass PFTs */
  unsigned int nsoil;     /**< number of soil types */
  Soilpar *soilpar;       /**< Soil parameter array */
  int ncountries;         /**< number of countries */
  Countrypar *countrypar; /**< country parameter array */
  int nregions;           /**< number of regions */
  Regionpar *regionpar;
  Outputvar *outputvars;
  int n_out;     /**< number of output files */
  int laimax_interpolate;
  Real laimax;        /**< maximum LAI for benchmark */
  Bool withdailyoutput; /**< with daily output (TRUE/FALSE) */
  int crop_index;
  int fdi;
  char *pft_index;
  char *layer_index;
  Bool crop_irrigation;
  int with_nitrogen;      /**< enable nitrogen cycle */
  Bool fertilizer_input; 
  Bool global_netcdf;     /**< enable global grid for NetCDF output */
  Bool landuse_restart;   /**< land use enabled in restart file */
  int wateruse;           /**< enable wateruse (NO_WATERUSE, WATERUSE, ALL_WATERUSE) */
  int sdate_option_restart; /**< sdate option in restart file */
  int landuse_year_const;       /**< year landuse is fixed for LANDUSE_CONST case */
  Bool intercrop;               /**< intercropping (TRUE/FALSE) */
  Bool istimber;
  Bool const_climate;           /**< constant climate */
  Bool const_deposition;        /**< constant N deposition */
  Bool remove_residuals;
  Bool residues_fire;   /**< use parameters for agricultural fires */
  Bool param_out;               /**< print LPJmL parameter */
  Bool check_climate; /**< check climate input data for NetCDF files */
  Verbosity scan_verbose;       /**< option -vv 2: verbosely print the read values during fscanconfig. default 1; 0 would supress even error messages */
  int compress;           /**< compress NetCDF output (0: no compression) */
  float missing_value;    /**< Missing value in NetCDF files */
  Variable *outnames;
  Outputmethod outputmethod;
  char *hostname;               /**< hostname to send data */
  int port;                     /**< port of socket connection */
#ifdef USE_MPI
  MPI_Comm comm; /**< MPI communicator */
  int offset;
#endif
  char *write_restart_filename; /**< filename of restart file */
  char *checkpoint_restart_filename; /**< filename of checkpoint restart file */
  Bool ischeckpoint;      /**< run from checkpoint file ? (TRUE/FALSE) */
  int checkpointyear;     /**< year stored in restart file */
  Pftpar *pftpar;         /**< PFT parameter array */
  int restartyear; /**< year restart file is written */
  int ntypes;    /**< number of PFT classes */
  int ngridcell; /**< number of grid cells */
  int startgrid; /**< index of first local grid cell */
  int firstgrid; /**< index of first grid cell */
  int nspinup;   /**< number of spinup years */
  int nspinyear; /**< cycle length during spinup (yr) */
  int lastyear;  /**< last simulation year (AD) */
  int firstyear; /**< first simulation year (AD) */
  int outputyear; /**< first year for output (AD) */
  Bool isfirstspinupyear; /**< set first year for climate in spinup (TRUE/FALSE) */
  int firstspinupyear;   /**< first year for climate in spinup */
  int total;     /**< total number of grid cells with valid soilcode */
  int nall;      /**< total number of grid cells */
  int rank;      /**< my rank */
  int ntask;     /**< number of parallel tasks */
  int count;     /**< number of grid cells with valid soilcode */
  int fire;      /**< fire disturbance enabled */
  int firewood;  /**< fire wood usage */
  int seed;      /**< initial seed for random number generator */
  Coord resolution;    /**< size of grid cell (deg) */
  Bool ispopulation;
  Bool river_routing;  /**< river routing enabled */
  Bool permafrost;     /**< permafrost module enabled */
  Bool new_phenology;	/**< new phenology enabled */
  Bool from_restart;   /**< reading from restart */
  int sdate_option;    /**< sowing date option (computed internally: 0, fixed: 1, prescribed: 2)*/
  int sdate_fixyear;    /**< year in which sowing dates shall be fixed */
  Bool initsoiltemp;
  Pnet *route;         /**< river routing network */
  Pnet *irrig_neighbour; /**< irrigation neighbour network */
  Pnet *irrig_back;      /**< back irrigation network */
  Pnet *irrig_res;
  Pnet *irrig_res_back;
  int withlanduse;
  Bool reservoir;
  int irrig_scenario; /**< irrigation scenario (NO:0, LIM:1, POT:2, ALL:3, IRRIG on RAINFED: 4) */
  Bool rw_manage;     /**< rain-water management enabled: reduced soil evaporation + rain-water harvesting */
  Bool pft_output_scaled; /**< PFT output grid scaled */
  int with_radiation; /**< input of radiation components (CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN) */
  Bool prescribe_burntarea;	/**< use input to prescribe burnt area to SPITFIRE? */
  int prescribe_landcover; /**< use input to prescribe land cover ? */

#ifdef IMAGE
  int start_imagecoupling; /**< year in which coupling to IMAGE starts
                              (e.g. 1970), set to 9999 if IMAGE is not used */
  Socket *in;  /**< socket for ingoing data */
  Socket *out; /**< socket for outgoing data */
#endif

} Config; /* LPJ configuration */

/* Declaration of functions */

#ifdef USE_MPI
extern void initmpiconfig(Config *,MPI_Comm);
#endif
extern void initconfig(Config *);
extern FILE* openconfig(Config *,const char *,int *,char***,const char*);
extern void freeconfig(Config *);
extern void fprintconfig(FILE *,const Config *,int,int);
extern Bool filesexist(Config,Bool);
extern long long outputfilesize(const Config *);
extern Variable *fscanoutputvar(LPJfile *,int,Verbosity);
extern void fprintoutputvar(FILE *,const Variable *,int);
extern void freeoutputvar(Variable *,int);
extern Bool fscanoutput(LPJfile *,Config *,int);
extern Bool readconfig(Config *,const char *,Fscanpftparfcn [],int,int,int *,
                       char ***,const char *);
extern Bool fscanconfig(Config *,LPJfile *,Fscanpftparfcn [],int,int);
extern void fprintparam(FILE *,int,int,const Config *);
extern void fprintfiles(FILE *,Bool,const Config *);
extern Bool getextension(Extension *,const Config *);
extern void fprintincludes(FILE *,const char *,int,char **);
extern size_t getsize(int,const Config *);

/* Definition of macros */

#define printconfig(config,npft,ncft) fprintconfig(stdout,config,npft,ncft)
#define ischeckpointrestart(config) ((config)->checkpoint_restart_filename!=NULL)
#define iswriterestart(config) ((config)->write_restart_filename!=NULL)
#define isreadrestart(config) ((config)->restart_filename!=NULL)
#ifdef USE_MPI
#define isroot(config) ((config).rank==0)
#else
#define isroot(config) TRUE
#endif
#define printparam(config,npft,ncft) fprintparam(stdout,config,npft,ncft)
#define printfiles(isinput,config) fprintfiles(stdout,isinput,config)
#define printincludes(name,argc,argv) fprintincludes(stdout,name,argc,argv)

#endif /* CONFIG_H */
