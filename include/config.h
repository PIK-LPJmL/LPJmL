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

struct config
{
  const char *filename;       /**< LPJ configuration filename */
  char *arglist;              /**< command line */
  char *inputdir;             /**< input directory */
  char *outputdir;            /**< output directory */
  char *restartdir;           /**< restart directory */
  char *cmd;                  /**< command line to process configuration file */
  char *filter;               /**< preprocessor for configuration file */
  int n_global;               /**< number of global attributes in output files */
  Attr *global_attrs;         /**< array of global attributes */
  char *json_filename;        /**< filename of preprocessed JSON file */
  Filename temp_filename;
  Filename prec_filename;
  Filename cloud_filename;
  Filename wet_filename;
  Filename wind_filename;
  Filename tamp_filename;
  Filename tmax_filename;
  Filename humid_filename;
  Filename tmin_filename;
  Filename lightning_filename;
  Filename lwnet_filename;
  Filename swdown_filename;
  Filename popdens_filename;
  Filename human_ignition_filename;
  Filename co2_filename;
  Filename drainage_filename;
  Filename extflow_filename;
  Filename neighb_irrig_filename;
  Filename coord_filename;
  Filename landfrac_filename;
  Filename soil_filename;
  Filename soilph_filename;
  Filename river_filename;
  Filename countrycode_filename;
  Filename landuse_filename;
  Filename fertilizer_nr_filename;
  Filename no3deposition_filename;
  Filename nh4deposition_filename;
  Filename manure_nr_filename;
  Filename with_tillage_filename;
  Filename residue_data_filename;
  char *restart_filename;
  Filename lakes_filename;
  Filename wateruse_filename;
  Filename elevation_filename;
  Filename reservoir_filename;
  Filename sdate_filename;
  Filename crop_phu_filename;
  Filename burntarea_filename;
  Filename landcover_filename;
#ifdef IMAGE
  Filename aquifer_filename;
  Filename wateruse_wd_filename;
#endif
  Filename grassharvest_filename;
  Filename lsuha_filename;
  Filename sowing_cotton_rf_filename;
  Filename harvest_cotton_rf_filename;
  Filename sowing_cotton_ir_filename;
  Filename harvest_cotton_ir_filename;
#ifdef IMAGE
  Filename temp_var_filename; /**< file with CRU-derived temperature variation (K) */
  Filename prec_var_filename; /**< file with CRU-derived precipitation variation (%) */
  Filename prodpool_init_filename; /* file with initial product pool sizes */
#ifdef COUPLED
  char *image_host;       /**< hostname for computer running the IMAGE model */
  int image_inport;       /**< port numbert for ingoing data */
  int image_outport;      /**< port number for outgoing data */
#endif
#endif
  int wait;               /**< time to wait for connection (sec) */
  Bool nopp;              /**< no piping thru preprocessor (TRUE/FALSE) */
  Bool pedantic;          /**< enables pedantic mode, stops on some warnings (TRUE/FALSE) */
  char *sim_name;         /**< Desciption of LPJ simulation */
  char *coupled_model;    /**< name of coupled model or NULL */
  int sim_id;             /**< Simulation type */
  int *npft;              /**< number of PFTs in each PFT class */
  int nbiomass;           /**< number of biomass PFTs */
  int nagtree;            /**< number of agricultural tree PFTs */
  int nwft;               /**< numer of WFTs */
  int ngrass;             /**< number of grass PFTs not biomass */
  int nwptype;
  int *cult_types;        /**< array of cultivation types to read from pft.js */
  int ncult_types;        /**< size of cult_types array */
  int nsoil;              /**< number of soil types */
  Soilpar *soilpar;       /**< Soil parameter array */
  int ncountries;         /**< number of countries */
  Countrypar *countrypar; /**< country parameter array */
  Outputvar *outputvars;
  char *compress_cmd;    /**< command for compressing output files */
  char *compress_suffix; /**< suffix for compressed output files */
  char csv_delimit;      /**< delimiter of CSV files */
  int n_out;     /**< number of output files */
  int laimax_manage;
  Real laimax;        /**< maximum LAI for benchmark */
  Bool withdailyoutput; /**< with daily output (TRUE/FALSE) */
  Bool flush_output;   /**< flush output after every simulation year (TRUE/FALSE) */
  Bool nofill;          /**< do not fille NetCDF files at creation (TRUE/FALSE) */
  int fdi;
  char *pft_index;
  char *layer_index;
  int with_nitrogen;      /**< enable nitrogen cycle */
  Bool crop_resp_fix;      /**< with fixed crop respiration (TRUE/FALSE) */
  int tillage_type;      /**< type of tillage NO_TILLAGE=0, TILLAGE=1, READ_TILLAGE=2 */
  int residue_treatment; /** residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */
  Bool fix_fertilization;   /**< simulation with fixed fertilizer application rate */
  Bool no_ndeposition;      /**< turn off atmospheric N deposition */
  int fertilizer_input;     /**< simulation with fertilizer input */
  Bool manure_input;       /**< simulation with manure input */
  Bool prescribe_lsuha;    /**< simulation with prescribed grassland livestock density from file */
  Bool global_netcdf;     /**< enable global grid for NetCDF output */
  Bool rev_lat;           /**< reverse lat coordinates in NetCDF output */
  Bool with_days;         /**< using days as a unit for monthly output */
  Type grid_type;         /**<  datatype for binary grid file */
  Bool landuse_restart;   /**< land use enabled in restart file */
  Bool separate_harvests;
  int wateruse;           /**< enable wateruse (NO_WATERUSE, WATERUSE, ALL_WATERUSE) */
  int sdate_option_restart;     /**< sdate option in restart file */
  int crop_option_restart;      /**< crop option in restart file */
  int landuse_year_const;       /**< year landuse is fixed for LANDUSE_CONST case */
  Bool intercrop;               /**< intercropping (TRUE/FALSE) */
  Bool grassonly;               /**< set all cropland including others to zero but keep managed grasslands */
  Bool luc_timber;              /***< land-use change timber */
  Bool storeclimate;           /**< store climate data in spin-up phase */
  Bool shuffle_spinup_climate;  /**< shuffle spinup climate */
  Bool fix_climate;             /**< fix climate after specified year */
  int fix_climate_year;         /**< year at which climate is fixed */
  int fix_climate_interval[2];  /**< interval for fixed climate */
  Bool fix_climate_shuffle;     /**< randomly shuffle climate */
  Bool fix_deposition;          /**< fix deposition after specified year */
  int fix_deposition_year;      /**< year at which deposition is fixed */
  int fix_deposition_interval[2]; /**< interval for fixed deposition */
  Bool fix_deposition_shuffle;  /**< randomly shuffle deposition */
  Bool fix_deposition_with_climate; /**< fix deposition same as climate */
  Bool fix_landuse;             /**< fix land use after specified year */
  int fix_landuse_year;         /**< year at which land use is fixed */
  Bool fix_co2;                 /**< fix CO2 after specified year */
  int fix_co2_year;             /**< year at which CO2 is fixed */
  Bool iscotton;                /**< cotton present in PFT parameter file */
  Bool fire_on_grassland;       /**< enable fires on grassland for Spitfire */
  Bool landfrac_from_file;      /**< land fraction read from file (TRUE/FALSE) */
  Bool residues_fire;           /**< use parameters for agricultural fires */
  Bool param_out;               /**< print LPJmL parameter */
  Bool ofiles;                  /**< list only all output files */
  Bool check_climate;           /**< check climate input data for NetCDF files */
  Bool others_to_crop;          /**< move PFT type others into PFT crop, cft_tropic for tropical, cft_temp for temperate */
  int cft_temp;
  int cft_tropic;
  Verbosity scan_verbose;       /**< option -vv 2: verbosely print the read values during fscanconfig. default 1; 0 would supress even error messages */
  int compress;           /**< compress NetCDF output (0: no compression) */
  float missing_value;    /**< Missing value in NetCDF files */
  Variable *outnames;
#ifdef USE_MPI
  MPI_Comm comm; /**< MPI communicator */
  int offset;
#endif
  char *write_restart_filename; /**< filename of restart file */
  char *checkpoint_restart_filename; /**< filename of checkpoint restart file */
  Bool ischeckpoint;      /**< run from checkpoint file ? (TRUE/FALSE) */
  int checkpointyear;     /**< year stored in restart file */
  char **pfttypes;        /**< array for PFT type names of size ntypes */
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
  int baseyear; /**< base year for NETCDF output (AD) */
  Bool absyear; /**< set absolute years in NetCDF output */
  Bool isfirstspinupyear; /**< set first year for climate in spinup (TRUE/FALSE) */
  int firstspinupyear;   /**< first year for climate in spinup */
  int total;     /**< total number of grid cells with valid soilcode */
  int nall;      /**< total number of grid cells */
  int rank;      /**< my rank */
  int ntask;     /**< number of parallel tasks */
  int count;     /**< number of grid cells with valid soilcode */
  int fire;      /**< fire disturbance enabled */
  int seed_start;      /**< initial seed for random number generator */
  Bool new_seed;
  Coord resolution;    /**< size of grid cell (deg) */
  Bool ispopulation;
  Bool river_routing;  /**< river routing enabled */
  Bool with_lakes;     /**< enable lakes (TRUE/FALSE) */
  Bool extflow;        /** external flow enabled */
  Bool permafrost;     /**< permafrost module enabled */
  Bool percolation_heattransfer; /**< water heat transfer enabled */
  Bool johansen;       /**< johansen enabled */
  Bool gsi_phenology;	/**< GSI phenology enabled (TRUE/FALSE) */
  Bool transp_suction_fcn; /**< transpiration reduction function enabled */
  Bool equilsoil;      /**< equilsoil is called */
  Bool from_restart;   /**< reading from restart */
  int soilpar_option;  /**< soil parameter option (NO_FIXED_SOILPAR, FIXED_SOILPAR, PRESCRIBED_SOILPAR) */
  int soilpar_fixyear; /**< year soil parameter should be fixed */
  int sdate_option;    /**< sowing date option (computed internally: 0, fixed: 1, prescribed: 2)*/
  int sdate_fixyear;    /**< year after which sowing dates are held constant (for both FIXED_SDATE or NO_FIXED_SDATE), also used to fix updating vernalization requirements */
  int crop_phu_option;    /**< crop phu option (old LPJmL4, semistatic internally computed, prescribed  */
  Bool initsoiltemp;
  Pnet *route;         /**< river routing network */
  Pnet *irrig_neighbour; /**< irrigation neighbour network */
  Pnet *irrig_back;      /**< back irrigation network */
  Pnet *irrig_res;
  Pnet *irrig_res_back;
  int withlanduse;
  Bool reservoir;
#ifdef COUPLING_WITH_FMS
  Bool nitrogen_coupled;
#endif
  int *landcovermap;        /**< landcover map */
  int landcovermap_size;    /**< size landcover map */
  int *landusemap;          /**< mapping of bands in land-use file to CFTs */
  int landusemap_size;      /**< size of landusmap */
  int *fertilizermap;
  int fertilizermap_size;
  int *cftmap;
  int cftmap_size;
  int *soilmap;
  int soilmap_size;
  int grazing;
  int grazing_others;
#ifdef IMAGE
  Bool groundwater_irrig;   /**< Irrigation from groundwater reservoir */
  Bool aquifer_irrig;       /**< Aquifer irrigation possible?*/
#endif
  int irrig_scenario;       /**< irrigation scenario (NO:0, LIM:1, POT:2, ALL:3, IRRIG on RAINFED: 4) */
  Bool rw_manage;           /**< rain-water management enabled: reduced soil evaporation + rain-water harvesting */
  Bool pft_output_scaled;   /**< PFT output grid scaled */
  char *json_suffix;        /**< suffix for JSON metafiles */
  int with_radiation;       /**< input of radiation components (CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN) */
  Bool prescribe_burntarea;	/**< use input to prescribe burnt area to SPITFIRE? */
  int prescribe_landcover; /**< use input to prescribe land cover ? */
  int* mowingdays;         /**< mowing days for grassland */
  int mowingdays_size;     /**< size of mowing days array */
  Bool npp_controlled_bnf;             /**< biological nitrogen fixation folowing Ma et al., 2022 */
  Seed seed;
  int start_coupling;      /**< year in which coupling to IMAGE/coupler starts */
#ifdef IMAGE
  Socket *in;  /**< socket for ingoing data */
  Socket *out; /**< socket for outgoing data */
#endif
  Socket *socket;         /**< socket for in- and outgoing data */
  char *coupled_host;     /**< hostname for computer running the IMAGE model */
  int coupler_port;       /**< port number for in- and outgoing data */
  int coupler_out;        /**< number of outgoing data streams */
  int coupler_in;         /**< number of ingoing data streams */
  int totalsize;          /**< size of shared output storage */
  int outputmap[NOUT];    /**< index into output storage */
  int outputsize[NOUT];   /**< number of bands for each output */
}; /* LPJ configuration */

typedef struct
{
  char *name;                                     /**< name of PFT type */
  Bool (*fcn)(LPJfile *,Pftpar *,const Config *); /**< pointer to PFT-specific scan function */
} Pfttype;

extern char *crop_phu_options[];
extern char *grazing_type[];

/* Declaration of functions */

#ifdef USE_MPI
extern void initmpiconfig(Config *,MPI_Comm);
#endif
extern void initconfig(Config *);
extern FILE* openconfig(Config *,int *,char***,const char*);
extern void freeconfig(Config *);
extern void fprintconfig(FILE *,int,int,const Config *);
extern Bool filesexist(Config,Bool);
extern long long outputfilesize(const Config *);
extern Variable *fscanoutputvar(LPJfile *,int,Verbosity);
extern Bool fscanpftpar(LPJfile *,const Pfttype [],Config *);
extern void fprintpftpar(FILE *,const Pftpar [],const Config *);
extern void fprintoutputvar(FILE *,const Variable *,int,int,int,const Config *);
extern void freeoutputvar(Variable *,int);
extern Bool fscanoutput(LPJfile *,int,int,Config *,int);
extern Bool readconfig(Config *,Pfttype [],int,int,int *,
                       char ***,const char *);
extern Bool fscanconfig(Config *,LPJfile *,Pfttype [],int,int);
extern Bool fscancultivationtypes(LPJfile *,const char *,int **,int *,Verbosity);
extern void fprintparam(FILE *,int,int,const Config *);
extern void fprintfiles(FILE *,Bool,Bool,const Config *);
extern Bool getextension(Extension *,const Config *);
extern void fprintincludes(FILE *,const char *,int,char **);
extern size_t getsize(int,const Config *);
extern int *fscanlandcovermap(LPJfile *,int *,const char *,int,const Config *);
extern void createconfig(const Config *);
extern void closeconfig(LPJfile *);

/* Definition of macros */

#define printconfig(npft,ncft,config) fprintconfig(stdout,npft,ncft,config)
#define ischeckpointrestart(config) ((config)->checkpoint_restart_filename!=NULL)
#define iswriterestart(config) ((config)->write_restart_filename!=NULL)
#define isreadrestart(config) ((config)->restart_filename!=NULL)
#define iscoupled(config) ((config).coupled_model!=NULL)
#ifdef USE_MPI
#define isroot(config) ((config).rank==0)
#else
#define isroot(config) TRUE
#endif
#define printparam(config,npft,ncft) fprintparam(stdout,config,npft,ncft)
#define printfiles(isinput,isoutput,config) fprintfiles(stdout,isinput,isoutput,config)
#define printincludes(name,argc,argv) fprintincludes(stdout,name,argc,argv)

#endif /* CONFIG_H */
