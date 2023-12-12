# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project follows a [GNU standard version numbering](https://www.gnu.org/prep/standards/html_node/Releases.html#index-version-numbers_002c-for-releases)
of `major.minor.patch` with
- `major` for substantial changes to code functionality,
- `minor` for additions of modules and functionality, and
- `patch` for technical changes and bugfixes without extending the functionality of the code.

- if applicable, categories to include in the CHANGELOG.md are:
  - Added
  - Changed
  - Deprecated
  - Removed
  - Fixed
  - Security

## [Unreleased]

## [5.8.1] - 2023-12-11

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Sebastion Ostberg (ostberg@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de), Sibyll Schaphoff (sibylls@pik-potsdam.de)

### Added

- tmin, tmax, humid GSWP3-W5E5 inputs added to `input.cjson`.
- missing parameters added in `lpjparam_non.cjson` and `pft_non.cjson`.

### Changed

- set gml.noaa data as default CO2 input.
- GSWP3-W5E5 climate is now the default.
- `regridlpj` utility script updated for new inputs

### Removed

- GLDAS climate input removed.

### Fixed

- scaling fixed for daily output and unit 1/second in fwriteoutput.c (issue #332).
- `#ifdef` and `#elif` statements fixed in `lpjml_config.cjson` and `input.cjson`.
- computation of root biomass based on carbon fixed in `nuptake_grass` and `nuptake_tree` (issue #330)
- fixed bug in `allocation_tree` (issue #330)


## [5.8.0] - 2023-11-30

### Contributors

- author: Jannes Breier (breier@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)
- dicussed by attendees of [LPJmL seminar](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/wikis/231123_lpjmlseminar) on Nov. 23rd 2023.

### Added

- new keyword `"cultivation_types"` added to specify an array of cultivation types read from the `pft.cjson` file:
```
  "cultivation_types" : ["none","biomass","annual crop"],
```
- wood plantation and agricultural tree PFTs added to `pft.cjson`.
- '"k_est"' establishment density array added for agricultural trees to `"countrypar"` in `manage_irrig_systems_with_dummy_laimax_data.cjson`.
- Makros to select different climate (CRU_MONTHLY, CRU_NETCDF, GLDAS) as well as land use, fertilizer and manure (OLD_LU, MADRAT) inputs added to `input.cjson`.
- Makro to disable nitrogen cycle (WITHOUT_NITROGEN) added to `lpjml_config.cjson`.
- Makros definition and description added to `lpjml_config.cjson`.

### Changed

- file ending from `js` to `cjson`.
- renamed `lpjml.cjson` to `lpjml_config.cjson`.
- extended `input.cjson` to include CRU, GSWP3-W5E5 and GLDAS climate inputs and LandInG, MADRAT and "Old" land use, fertilizer and manure inputs.
- Moved mappings from `input.cjson` to lpjml_config.cjson.
- Moved soil depths array and fuel bulk density factors from `soil.cjson` to `lpjparam.cjson`.

### Removed

- `lpjml_*js`, `param_*js`, `lpjparam_*js`, `pft_*.js`, `input_*.js` and `manage_*.js files` for specific projects.

## [5.7.10] - 2023-11-13

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jannes Breier (breier@pik-potsdam.de), Constanze Werner (cowerner@pik-potsdam.de), Sebastion Ostberg(ostberg@pik-potsdam.de), 
  Marie Hemmen (hemmen@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)

### Added

- name of grid file and `"terr_area"` output file written to output JSON files. A link to the corresponding JSON file is added.
- list of utilities added to `INSTALL` file.
- `-json` flag added to `cdf2clm` utility.
- last year of CO2 data is checked at run time and last year of simulation is changed accordingly.

### Changed

- `fscanfloat()` accepts now also integer input.

### Removed

- obsolete constant definitions in `conf.h`.

### Fixed

- opening of grid file in JSON metafile format fixed in `opencoord()`.
- default soil map is set for soil data in NetCDF format.
- missing carbon influx added to NBP calculation in `flux_sum.c`.

## [5.7.9] - 2023-10-23

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Constanze Werner (cowerner@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)

### Changed

- constant wood density has been replaced by PFT-specific parameter `"wood_density"`.

### Removed

- Unused flag `"biomass_grass_harvest"` removed from configuration.

### Fixed

- update of `pft->establish.nitrogen` corrected in `turnover_tree.c`.
- C and N in manure from grazing livestock goes to litter (as described in [Heinke et al. 2023](https://doi.org/10.5194/gmd-16-2455-2023) instead of going to soil pools directly.


## [5.7.8] - 2023-09-22

### Contributors

- author: Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Christoph Mueller (cmueller@pik-potsdam.de), Jannes Breier (breier@pik-potsdam.de)

### Fixed

- bug in `landusechange.c` where land transfers between rainfed and irrigated setaside stands were not taken into account when determining rainfed and irrigated areas to deforest/regrow
- false positive carbon balance errors in `turnover_tree.c` if the code is compiled with `CHECK_BALANCE` switch


## [5.7.7] - 2023-09-20

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jens Heinke (heinke@pik-potsdam.de), Sibyll Schaphoff (sibylls@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)

### Added

- grassland specific variables `deficit_lsu_ne` and `deficit_lsu_mp` added to `fprint_grassland.c` and therefore are printed by `lpjprint`.

### Removed

- obsolete definition of constants removed in `conf.h`.

### Fixed

- division by zero resulting in a floating point exception if lpjml is run with `-fpe` option fixed in `littersom.c` and `harvest_stand.c`.
- index in `"cft_nir"` output is now correctly calculated for `agriculture_tree`/`agrculture_grass` stands in `distribute_water.c`.
- `pft->npp_bnf` set to zero for the auto fertilization setting in the N uptake functions. Fertilization only applied if N deficit is greater than zero.

## [5.7.6] - 2023-09-11

### Added

- reallocation of N added in `allometry_tree.c` if tree height exceeds the maximum height.
- reproduction costs for nitrogen added to `turnover_tree.c`.
- missing output `"harvestc_agr"` updated and written to file.

### Changed

- check for `sum>1` for warnings has been replaced by `sum>1+epsilon` in `getlanduse()`.
- `ml.landfrac` scaling of output has been replaced by acual stand fraction.
- `output_gbw_*` functions have been merged into one `output_gbw` function.

### Removed

- misplaced `CFT_AIRRIG` update has been removed from `daily_woodplantation.c`.

### Fixed

- missing `#include "grass.h"` added in `sowingcft.c`.
- arguments of `setaside` calls in `annual_woodplantation.c` and `annual_biomass_tree.c` have been corrected.
- update of `LITFALLN` corrected in `timber_burn.c`.
- output written to correct `N2O_DENIT_*` file in `denitrification.c`.
- missing `reservoirfrac` added to scaling for outputs in `daily_agriculture_grass.c` and `daily_agriculture_tree.c`.


## [5.7.5] - 2023-08-23

### Added

- outputs:
  - IRRIG_STOR
  - RIVERVOL
  - SWC_VOL
 
### Changed:

- unit of output RES_STORAGE changed from hm3 to dm3 to be in line with IRRIG_STOR, RIVERVOL, LAKEVOL, SWC_VOL

### Fixed

- check for land-use fractions to not exceed 100% after scaling with `landfrac`, re-scale to 100%, print warning see [!173](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/merge_requests/173)
- add turnover before allocation in cultcftstand to prevent sporadic C balance errors when running with intercrops, see[!175](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/merge_requests/175)
- number of wet days not read if daily precipitation input is used. A warning is additionally printed to set `"random_prec"` to false.
- boolean `"river_routing"` is read before `"with_lakes"` flag to avoid uninitialized variable.


## [5.7.4] - 2023-08-17

### Changed

- changed quadratic soil evaporation function to sigmoid form and a minimum amount of evaporation of 5% of the available energy, following [Sun et al. 2013](http://dx.doi.org/10.1080/17538947.2013.783635), as described in [!168](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/merge_requests/168)

## [5.7.3] - 2023-08-16

### Fixed

- correct accounting of blue water transpiration on agriculture stands in outputs
- missing variable initialization for bioenergy grass
- landuse change from irrigated to rainfed setaside moved area to wrong target setaside


## [5.7.2] - 2023-08-11

### Changed:

- removed timestep attributes (monthly/annual) in all output descriptions that are not timestep specific

### Added 

- outputs:
  - PFT_WATER_DEMAND: PFT specific water demand
  - RD: dark respiration
  - NDEPOS: total N deposition as sum of NH4 and NO3 deposition

## [5.7.1] - 2023-08-10

### Fixed

- wrong use of `&f` instead of `f`, in `fscanparam.c`. Bug was introduced in version 5.6.26 and caused that parameter files could not be read properly

## [5.7.0] - 2023-08-10

### Added

- land fraction can now be read from file, with a new flag `"landfrac_from_file"` and a new input `"landfrac"`
- If setting `"landfrac_from_file : true"` cell areas in LPJmL are scaled down by land fraction from input file. If land fraction is zero, land fraction is set to `minlandfrac` and a warning is printed.
- new flag `"with_lakes"` defined
- new outputs `"terr_area"`, `"land_area"` and `"lake_area"` defined writing the terrestrial area (land and lakes),  land area and lake area of each cell
- new parameters `"minlandfrac"` and `"tinyfrac"` added to lpjparams.js
- option `-area filename` added to the `printglobal` utility to scale with the cell areas read from the file specified.

### Changed

- Lake fraction and land-use fractions are rescaled by the reciprocal of land fraction. If lake fraction is greater than one, lake fraction is set to one and a warning is printed.
- Lakes can be enabled without river routing by setting `"with_lakes" : true`. Variable `dmass_lake` is always written to restart file.
- lake fraction is read in utility `lpjprint` and printed
- flag `"shuffle_spinup_climate"` only read if `"nspinup"` is non-zero.
- Terrestrial area and harvest fraction are read in utility `printharvest`. Output format changed to CSV.

## [5.6.27] - 2023-08-08

### Removed

- removed large chunks of code in `allocation_daily_crop.c` that was inactive

### Fixed

- fixed output `PFT_VEGC` to not include `bm_inc.nitrogen` for crops in `daily_agriculture.c`
- fixed global flux summation for `flux.soil.carbon` and `flux.soil.nitrogen` (`+=` instead of `=`), bug was introduced in version 5.6.25

## [5.6.26] - 2023-08-07

### changed

- avoid daily leaf turnover for biomass grass and harvest leaf turnover as dead biomass at harvest event instead (reduces litter and increases harvest)
- introduce harvest dates depending on green/brown harvest instead of C threshold for biomass grass
- literature-based C/N ratios and fn_turnover for biomass grass depending on green/brown harvest configuration
- results and literature references can be found in issue [#275](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/275).

### Added

- introduced `#define` flag to change from green harvest (default) to brown harvest: `#define BMGR_BROWN`
- two options include green harvest (fresh biomass, high N content - usually used for biofuels) and brown harvest (dry biomass, high lignin content, low N content - usually used for combustion)
- brown harvest uses later harvest dates (defined in lpjparam.js) and higher N recovery (as lower fn_turnover defined in pft.js) than green harvest
- the `#define` flag is required to read the correct flag-specific parameter from the lpjparam.js and pft.js, see issue [#311](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/311)

## [5.6.25] - 2023-07-29

### Added

- New outputs added for N fluxes from timber extraction and for N pools in wood product pools
- New outputs added for `estab_storage` C and N
- Added missing N fluxes, `estab_storage`, and reservoirs to `globalflux` output
- Added new fluxes to man pages

### Fixed 

- Corrected/added units in comments in output.h
- Corrected units in `flux_sum` man page
- Corrected `N2O_DENIT` output, which now no longer also includes `N2O_NIT` values
- Corrected writing of mineral N pools to outputs, so that also mineral N in frozen soil layers is included
- Corrected writing of `VEGN` for crops, so that `bm_inc.nitrogen` is no longer included to avoid double accounting

### Removed

- Removed unecessary initialization of output variables to zero in `update_annual.c`, which is done in `initoutputdata.c`


## [5.6.24] - 2023-07-29

### Fixed

-  The number of bands is now correctly checked for the `read_residue_data` input.


## [5.6.23] - 2023-07-28

### Changed

- Instead of the soil code the index of the grid cell is written into the NetCDF file for `"grid"` output.


## [5.6.22] - 2023-07-27

### Changed

- Configuration files `lpjml_netcdf.js`, `lpjml_non.js`, `param_non.js`, and `lpjparam_non.js` updated to latest lpjml version.

### Removed

- Predefined daily outputs (`"d_lai"`-`"d_pet"`) and corresponding functions removed.

- Version numbers removed from man pages.

### Fixed

- Variable `issocket` always initialized to `FALSE` in `readfilename.c`.


## [5.6.21] - 2023-07-21

### Changed

- Flag `"nitrogen_coupled"` disabled for lpjml offline runs. Water stress is always coupled with nitrogen stress. Only if lpjml is compiled with `-DCOUPLING_WITH_FMS` flag is enabled.

- Flag `"new_trf"` renamed to `"transp_suction_fcn"`.


## [5.6.20] - 2023-07-13

### Added

- New flags `"fix_co2"` and `"fix_co2_year"` added.

### Changed

- Flag `"istimber"` renamed to `"luc_timber"`.

- Flag `"new_phenology"` renamed to `"gsi_phenology"`.

- Flag `"shuffle_climate"` renamed to `"shuffle_spinup_climate"`.

- The settings for fixed input data have been updated. For climate and N deposition input an interval can be specified from which data is taken after the specified year. In can specified whether random shuffling or cycling is used:

```java
"fix_climate" : false,                /* fix climate after specified year */
"fix_climate_year" : 1901,            /* year after climate is fixed */
"fix_climate_interval" : [1901,1930],
"fix_climate_shuffle" : true,          /* randomly shuffle climate in the interval */
"fix_deposition_with_climate" : false, /* fix N deposition same as climate */
"fix_deposition" : false,              /* fix N deposition after specified year */
"fix_deposition_year" : 1901,          /* year after deposition is fixed */
"fix_deposition_interval" : [1901,1930],
"fix_deposition_shuffle" : true,       /* randomly shuffle depositions in the interval */
"fix_landuse" : false,                 /* fix land use after specfied year */
"fix_landuse_year" : 1901,             /* year after land use is fixed */
"fix_co2" : false,                     /* fix atmospheric CO2  after specfied year */
"fix_co2_year" : 1901,                 /* year after CO2 is fixed */
```

### Removed

- Obsolete flag `"const_climate"` removed.


## [5.6.19] - 2023-07-06

### Added

- Help option `-h` added to `bin2cdf` and `clm2cdf` utility.

- Target `lpjcheck` added to `Makefile` to create only the `lpjcheck` utility.

### Changed

- The version of all man pages has been set to the LPJmL version number, date of last modification removed.

- The different versions of the utilities have been replaced by a reference to the LPJmL version.

### Removed

- All html files have been removed from the repository.

- Obsolete link `lpj` to `lpjml` removed.


## [5.6.18] - 2023-07-04

### Fixed

- Soil array `wpwp` is now correctly initialized with `soilpar->wpwp` for the `"prescribed_soilpar"` setting.


## [5.6.17] - 2023-06-30

### Fixed

- Replaced logical `or` by logical `and` in `survive.c`. This solves issue [#309](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/309).


## [5.6.16] - 2023-06-30

### Fixed

- Removed excess `pft->nind` in all harvest routines in `harvest_stand.c`.


## [5.6.15] - 2023-06-30

### Added

- Check for matching size of discharge queues read from restart file added to `initriver()` function in `initdrain.c`. If the size differs from the value calculated from the river lengths the queues will be resized and initialized to zero. Error message is additionally printed.


## [5.6.14] - 2023-06-08

### Removed

- Support for the deprecated `*.conf` configuration file format has been removed. Only JSON files are allowed for configuration files and metafiles for input.

- Man pages for the description of the `*.conf` file format removed.

### Changed

- Strings in JSON configuration files can now be longer than 256 characters.


## [5.6.13] - 2023-05-16

### Added

- Missing update of `"harvestc"` and `"harvestn"` output added in `annual_biomass_tree.c`, `annual_woodplantation.c`, `daily_agriculture_tree.c`, and `annual_agriculture_tree.c`.


## [5.6.12] - 2023-05-12

### Added

- Missing agsub litter pool added to `"litc"`, `"litn"`, `"mg_litc"`, `"litc_agr"`, `"mgrass_litc"`, `"mgrass_litn"` outputs and in global flux of `litc`.

- Output of `"litc_ag"` and `"litc_all"` now written to output file.

### Changed

- Item `ag` in struct `Litter` renamed to `agtop`.

- Functions  `litter_ag_grass()`, `litter_ag_nitrogen_tree()`, `litter_ag_sum()`, `litter_ag_sum_quick()`, `litter_ag_tree()`
  renamed to `litter_agtop_grass()`, `litter_agtop_nitrogen_tree()`, `litter_agtop_sum()`, `litter_agtop_sum_quick()`, `litter_agtop_tree()`.

- Index in `outnames` array in `fwriteoutput.c` corrected for `"mgrass_soilc"`, `"mgrass_soiln"`, `"mgrass_litc"`, `"mgrass_litn"`outputs.

## [5.6.11] - 2023-05-11

### Fixed

- Invalid access to pointers `soil->wsat-soil->wpwp` has been replaced by access to layer specific data `soil->wsat[l]-soil->wpwp[l]` in `infil_perc_rain.c` and `infil_perc_irr.c`. See issue [#303](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/303).


## [5.6.10] - 2023-05-10

### Added

- PFT specific output for biological nitrogen fixation with grid and PFT scaling in `npp_contr_biol_n_fixation.c` and `biologicalNfixation.c`.


### Changed

- Adjusted optimum temperature limits (`"temp_bnf_opt"`) of herbaceous PFTs for NPP controled biological nitrogen fixation in `pft.js`.


## [5.6.9] - 2023-05-10

### Added

- Missing `break` statement added in `harvest_stand.c`. See issue [#302](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/302)

- Missing deallocation of `decomp_litter_pft` added in `freesoil.c`.

- Missing deallocation of `with_tillage_filename` added in `freeconfig.c`.

### Changed

- If input comes from socket connection the existence of the file is not checked anymore.

- The memcpy arguments in `cpl.h` are now in the right order.

## [5.6.8] - 2023-04-28

### Added

- flag `"start_coupling"` added in configuration file. If lpjml is coupled to an external model via
```
  "coupled_model" : "model",
```
  this flag allows so set a year when input is received from the external model. Before that year input is read from input file.
  If flag is omitted or set to `null` coupling starts at the first year.

### Changed

- For input received from an external model a file name can additionally be specified where input is read from before the specified coupling year:
```
  "with_tillage" : { "fmt" : "clm", "socket" : true, "name" : "tillage.clm"},
```
- lpjcheck and lpjml checks for specified filenames if they are necessary for the coupled run

- JSON metafiles are also created for NetCDF output

## [5.6.7] - 2023-04-27

### Added

- CHANGELOG.md added see issue [#298](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/298)

### Changed

- leading zeros removed in `patch` identifier of the version number

## History

- all versions prior to 5.6.7 have not been documented by a changelog or similar but in the respective [merge requests](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/merge_requests) (internal link accessible at PIK only)

