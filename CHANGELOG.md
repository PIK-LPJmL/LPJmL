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

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- Output `"nbp"` for net biosphere productivity and `"tws"` for total water storage added.
- `fail` added to mail type in `lpjsubmit_hpc`.
- Option `-map` added to `cdf2bin` and `cdf2clm` to read map from NetCDF file and write it into JSON metafile.
- `"sim_name"` and all other global attributes are written into JSON file for `cdf2bin` utility.

### Fixed

- Scaling of `"littertemp"` output fixed.
- Unit corrected for "rootmoist"` output and scaling corrected.
- Correct number of output files printed in `lpjfiles` utility if separate output files for each year are enabled.

### Changed

- `DEPTH_NAME` and `BNDS_NAME` excluded for search for variable in NetCDF file if no variable name is provided. 
- Check added that dimension must greater 1 for variable in NetCDF file.


## [5.9.1] - 2024-07-11

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Christoph Mueller (cmueller@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- Missing regridding of wateruse and livestock density input added in `regridlpj`.

### Changed

- Compile option `-Werror` added to stop compilation of `icx`/`gcc` after warning. This feature can be disabled by using the `-noerror` option of `configure.sh`.
- Filename and source is written into configuration file created by `regridlpj`.
- All missing output indices are printed in case that the `"outputvar"` array is too short.

### Fixed

- Incorrect scaling removed in utility `printharvest`.
- Uninitialized PFT pointer set to first PFT in `harvest_stand.c`.
- Uninitialized variable `n_attr` set to zero in utility `printclm`.
- Prescribed land cover is now read correctly if landcover map size differs from number of natural PFTs.
- Check for land use type `WP` corrected in `landusechange.c` (issue #354).
- Code fixed to remove all warnings if compiled with `icx`/`gcc`.
- `regridclm` now used for regridding of soil file in `regridlpj`.
- Missing seek to offset added in filesexist.c
- Misplaced deallocation of memory for unit corrected in `printclm`.


## [5.9.0] - 2024-07-08

### Contributors

- author: David Hötten (davidho@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de) 

### Added
- Added `GPL_HEAT` macro to change number of gridpoints per soil layer used for heat convection.
- Added `percolation_heattransfer` switch to disable convection/percolation heattransfer.
- Added `littertemp` output variable for litter temperature.
- Added fast implicit crank nicolson heat conduction scheme for non-phase change conditions `apply_heatconduction_of_a_day.c`.

### Changed 

- Replaced `soiltemp` soil temperature and snow and litter heat conduction routines with `update_soil_thermal_state.c`;
  `soiltemp` remains in the repository for POEM/FMS coupling.
- Use enthalpy (thermal energy) as state variable for soil thermal regime instead of temperature.
- Numeric heat conduction method is a finite element enthalpy scheme `apply_heatconduction_of_a_day.c`.
- Numeric heat convection method is improved by closing the energy balance for water infil and percolation. 
- Conduction through snow and litter cover is now part of the numerical method for the soil.
- Snow and litter temperatures are now calculated by interpolating air and top gridpoint temperature.
- Snow melt by snow temperature greater than 0 deg is removed.
- Litter heat insulation is now based on literature values for litter thermal conductivity.


## [5.8.17] - 2024-06-14

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- List of required modules on new PIK cluster added in `INSTALL`.
- `configure.sh` script recognizes new PIK cluster and sets `mpiicx`/`icx` compiler accordingly.
- New site-specific `Makefile.hpc2024` and Makefile.icx` for parallel/sequential compilation on new cluster added.
- New `lpjsubmit_hpc` slurm script added for new PIK cluster. `configure.sh` sets symbolic link of `lpjsubmit` to this script.

### Changed

- All occurrences of the deprecated function `MPI_Type_extent` replaced by `MPI_Type_get_extent` to allow compilation with OpenMPI.
- `*.tmp` files added to `.gitignore`.

### Fixed

- Functions `mpi_write.c`, `mpi_write_txt.c`, `mpi_read_socket.c` and `mpi_write_socket.c` changed to avoid `SEGV` on new PIK cluster.


## [5.8.16] - 2024-06-07

### Contributors

- author: Christoph Müller (cmueller@pik-potsdam.de)
- reviewer: Fabian Stenzel (stenzel@pik-potsdam.de), Luke Oberhagemann (lukeober@pik-potsdam.de)

### Changed

- .zenodo.org updated to be compliant with PIK publication rules:
  - added "active-development" and "Potsdam Institute for Climate Impact Research" to keywords
  - added Christoph as "contributor", which seems to be the only way to have a visible contact person


## [5.8.15] - 2024-05-30

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jannes Breier (breier@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)

### Added

- A preprocessed JSON configuration file can be written if the following setting is added to the `*.cjson` configuration file:
```java
"json_config_filename" : "output/lpjml_config_restart.json", /* Filename of preprocessed configuration file written or null to disable this feature */
```

### Fixed

- The right allocated storage for `config->coupled_model` is checked for `NULL` in `fscanconfig.c`.


## [5.8.14] - 2024-05-27

### Contributors

- author: Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Susanne Rolinski (rolinski@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)

### Fixed

- fixed PFT pool scaling in mix_veg_grass() and mix_veg_tree()


## [5.8.13] - 2024-05-23

### Contributors

- author: Jannes Breier (jannesbr@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de)

### Changed

- PFT parameter lai_sapl has been set back to its original value of 0.001 for grasses
(tropical, temperate, polar)


## [5.8.12] - 2024-05-07

### Contributors

- author: Sibyll Schaphoff (sibylls@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- added link to LPJmL Zenodo archive to README

### Changed

- changed `ifndef` usage in `lpjml_config.cjson` to `ifdef` to avoid confusion

### Fixed

- fixed handling of monthly and daily outputs in `iterateyear.c` to ensure that last annual timestep of outputs is written after `update_annual()` is called


## [5.8.11] - 2024-04-10

### Contributors

- author: Johanna Braun (jobraun@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), Constanze Werner (cowerner@pik-potsdam.de), Jannes Breier (breier@pik-potsdam.de)

### Added

- added four new outputs for managed land: deposition (`NDEPO_MG`), biological nitrogen fixation (`BNF_MG`), applied nitrogen (`NAPPLIED_MG`) and nitrogen establishment flux (`FLUX_ESTABN_MG`)


## [5.8.10] - 2024-04-04

### Contributors

- author: Jannes Breier (jannesbr@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), David Hoetten (davidho@pik-potsdam.de)


### Changed

- In outputvars.cjson, the `variable` field for output grid has been changed
back to `"grid"` (instead of `"soil"`). If NetCDF output is written the variable
name would be cell id, if raw output is written the variable name would be
the coordinates. `"grid"` is used as generic naming here, `long_name` is used
to describe this exception (`"grid information (raw: coordinates, cdf: cell_id)"`)


## [5.8.9] - 2024-03-14

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Jannes Breier (breier@pik-potsdam.de)

### Changed

- NetCDF output files are now in a more ISIMIP3 compliant format, datatype of time, lat, and lon is now double, `depth_bnds` added to layer specific output. Standard and long name can be specified in `outputvars.json`.
- Deprecated function `MPI_Extent` replaced by `MPI_Get_extent` in `mpi_write.c`.
- Functions `create_netcdf.c`/`create_pft_netcdf.c` and `create1_netcdf.c`/`create1_pft_netcdf.c` have been merged.
- Function `findcoord()` improved to find cell indices, returns now true if coord is within cell.
- `STYLESHEET` file converted to markup.
- Cell size in `regridclm` is set to cell size of target grid, warning is printed if cell size differs.
- Format and suffix of output files have been removed in `lpjml_config.cjson` and are now set by `"default_fmt"` and `"default_suffix"`:
```java
{ "id" : "npp", "file" : { "name" : "output/mnpp"}},
```
- `LPJINPATH` set to `/p/projects/lpjml/input/historical` in `regridlpj` if not defined.
- `"descr"` in `outputvars.cjson` renamed to `"long_name"`.

### Added

- More options added to `lpjml_config.cjson` to customize format of output:
```java
  "default_fmt" : "raw",     /* default format for output files: "raw","txt","cdf","clm","sock" */
  "default_suffix" : ".bin", /* default file suffix for output files */
  "grid_type" : "short",     /* set datatype of grid file ("short", "float", "double") */
  "flush_output" : false,    /* flush output to file every time step */
  "absyear" : false,         /* absolute years instead of years relative to baseyear (true/false) */
  "rev_lat" : false,         /* reverse order of latitudes in NetCDF output (true/false) */
  "with_days" : true,        /* use days as units for monthly output in NetCDF files */
  "nofill" : false,          /* do not fill NetCDF files at creation (true/false) */
  "baseyear" : 1901,         /* base year for output in NetCDF files */
```
- Global attributes for NetCDF output files can be set in the `lpjml_config.cjson` file:
```java
 "global_attrs" : {"institution" : "Potsdam Institute for Climate Impact Research",
                    "contact" : "", /* name and email address */
                    /* any name and string value can be added: "name" : "value", */
                    "comment" : ""  /* additional comments */
                   }, /* Global attributes for NetCDF output files */
```
- `"standard_name"` for output can be set in `outputvars.cjson`. If not defined standard name is set to variable name.
- Chunking defined for NetCDF files if compiled with `-DUSE_NETCDF4`.
- Option `-metafile` added to `regridclm` and `binsum`. If metafile contains name of grid file, the filename of the source grid can be omitted:
```bash
regridclm -metafile grid_new.clm temp.clm.json temp_new.clm
binsum -metafile mnpp.bin.json anpp.bin
```
- NetCDF files can be created with `bin2cdf` and `clm2cdf` from the JSON metafiles containing all global attributes:
```bash
bin2cdf -metafile soilc_layer.bin.json soilc_layer.nc
clm2cdf -metafile temp.clm.json temp.nc
```
- `"landcovermap"` added to define mapping of the prescribed FPC input to PFTs.
- `-hostfile` option added to `lpjrun`.
- New utilities added:
  * `cmpbin` - compares binary ouput files
  * `statclm` - prints minimum, maximum and average of clm files
  * `regriddrain` - regrids drainage file to new grid
- Option `-metafile` and `-f` added to `mathclm`.
- `cdf2clm` stores all global attributes of NetCDF file in JSON metafile if `-json` option is set.
- Option `-nopp` added to `lpjml`. This option disables preprocessing of the config file by `cpp`.
- Option `-ofiles` added to `lpjml` and `lpjcheck`  to print list of all available output files.
- Option `-pedantic` added to `lpjcheck` and `lpjml` to stop on warning.
- Option `-json` added to `regridclm` and `regridsoil` utilities to create additional JSON metafiles.
- Compile flag `-DSTRICT_JSON` added to enable more strict syntax checking of JSON files.
- Remark message in `fscankeywords.c` added if number instead of string is used in JSON file.

### Fixed

- Missing deallocation added in `freeconfig.c` and memory leak in `fscanpftpar.c` fixed.
- Missing argument after option handling fixed in `cdf2bin` and `countr2cdf`.
- Missing `$dir/` added to manure data in `regridlpj`.
- Correct default landuse, sowing data and crop PHU file set in `regridlpj`.


## [5.8.8] - 2024-03-07

### Contributors

- author: Christoph  Mueller (cmueller@pik-potsdam.de)
- code review: Boris Sakschewski (borissa@pik-potsdam.de), Sabine Mathesius (sabine.mathesius@pik-potsdam.de)

### Fixed

- missing family names added in CITATION.cff

### Changed

- renamed `double_harvest.c` to `separate_harvests.c` and `update_double_harvest.c` to `update_separate_harvests.c` and changed entries in Makefiles accordingly
- renamed functions `double_harvest()` to `separate_harvests()` and `update_double_harvest()` to `update_separate_harvests()` throughout the code
- renamed the `struct` `Double_harvest` to `Separate_harvest` and the variable of type `Separate_harvest` in `struct` `Pftcrop` from `dh` to `sh`
- renamed `double_harvest` flag in `struct` `Config` to `separate_harvests` 


## [5.8.7] - 2024-03-06

### Contributors

- author: Sibyll Schaphoff (sibylls@pik-potsdam.de)
- code review: Jens Heinke (heinke@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de)

### Added

- new output `PFT_WATER_SUPPLY` to calculate water stress PFT-specific.

### Changed

- output writing for the new output (`conf.h` , `outputvar.cjson` , `createpftnames.c` , `fwriteoutput.c` , `outputsize.c` , `water_stressed.c`).
- `isopen()` call removed (`fwriteoutput.c`).


## [5.8.6] - 2024-02-29

- author: Stephen Wirth (wirth@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de), Marie Hemmen (hemmen@pik-potsdam.de)
- code review: Johanna Braun (jobraun@pik-potsdam.de)

### Fixed

- Divsion by zero avoided for calculation of `nc_ratio` in `ndemand_tree.c` and `ndemand_grass.c` (issue #341).
- Default directory for input files to current directory set and directory created in `regridlpj`.
- Correct id 11 set for `"tamp"` input in `input.cjson` (issue #347).
- Correct filename used in error message in utility `mathclm`.
- Datatype of Version 4 CLM files correctly handled in `cutclm`, `catclm`, `mergeclm`, `regridclm`.

### Changed

- Hard coded CFT indices (e.g. `MAIZE`) have been replaced by their names.
- Datatype of `bmgr_harvest_day_nh` and ` bmgr_harvest_day_nh` changed to `int`.
- Function `freadseed()` used in `freadrestartheader.c`.
- First value of CO2 data file used for simulation years before first year of CO2 data.
- CO2 data replaced by TRENDY data in `input.cjson`.

### Added

- Man page for `regridclm` and `regridsoil` added.
- New keywords `"cft_fertday_temp"` and `"cft_fertday_tropic"` added in `lpjparam.cjson`to specify the CFT names where the fertilizer application dates are taken from for grasslands.
- Missing filename of tillage input added to `lpjfiles` utility.
- Option `-h` added to `lpjsubmit`. Options `-v` and `-l` added to `configure.sh`.
- New keyword `"error_limits"` defined in `lpjparam.cjson` to set maximum balance errors allowed in simulation:

```java
 "error_limits" :
    {
      "carbon" : 1.0,       /* maximum error in local carbon balance (gC/m2) */
      "nitrogen" : 0.2,     /* maximum error in local nitrogen balance (gN/m2) */
      "water_local" : 1.5,  /* maximum error in local water balance (mm) */
      "water_global" : 1e-3 /* maximum error in global water balance (mm) */
    },
```
### Removed

- Option `--propagate` removed from `srun` in `lpjsubmit_slurm`.
- Hard-coded parameter `k_l` removed in `littersom.c`.
- Parameter `"residues_in_soil_notillage"` removed.
- Parameter `"co2_p"`  removed.
- Keyword in `"till_startyear"` removed from `lpjml_config.cjson`.


## [5.8.5] - 2024-01-31

### Contributors

- author: Christoph Mueller (cmueller@pik-potsdam.de) and the entire LPJmL team
- code review: Boris Sakschewski (borissa@pik-potsdam.de), Fabian Stenzel (stenzel@pik-potsdam.de)

### Added

- added `.zenodo.json` for interaction with zenodo archives
- added CITATION.cff file

### Changed

- updated AUTHORS file


## [5.8.4] - 2024-01-22

### Contributors

- author: Fabian Stenzel (stenzel@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de), Sibyll Schaphoff (schaphoff@pik-potsdam.de)
- code review: Stephen Wirth, Constanze Werner (cowerner@pik-potsdam.de)

### Fixed:

- Removed lines 240 and 241 in landuse/annual_biomass_tree.c to get rid of the carbon balance error, which shows up in cells with significant bioenergy tree fractions in the landuse input.

## [5.8.3] - 2024-01-21

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jannes Breier (breier@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de)

### Added

- allowed range for coordinates checked in `readcoord()`.
- allowed formats (`"raw"`, `"clm"`, `"clm2"`) for input files checked in `openmetafile()`.
- Output scaling added to `writearea.c`.

### Changed

- Error messages changed for more clarity.

### Fixed

- NetCDF output for grid file corrected if cells are skipped.
- `var_len` correctly set for input with no time axis in `openclimate_netcdf.c`.


## [5.8.2] - 2023-12-21

### Contributors

- author: Johanna Braun (jobraun@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Christoph Mueller (cmueller@pik-potsdam.de)
- dicussed by attendees of [LPJmL seminar](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/wikis/231221_lpjmlseminar) on Dec. 21st 2023 and [Nov. 23rd 2023](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/wikis/231123_lpjmlseminar).

### Changed

- `wateruse` from "yes" to "all"
- `sowing_date_option` from "fixed_sdate" to "prescribed_sdate"
- `crop_phu_option` from "new" to "prescribed"
- `tillage_type` from "all" to "read"
- `residue_treatment` from  "fixed_residue_remove" to "read_residue_data"
- `grazing` from "default" to "livestock"
- `prescribe_lsuha` from FALSE to TRUE
- set `nspinup` to 420 instead of 390 for LU spinup
- updated paths to land management input files in `input.cjson`
- set all available [LandInG1.0](https://doi.org/10.5194/gmd-16-3375-2023) inputs as the `default` option


## [5.8.1] - 2023-12-11

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de), Christoph Mueller (cmueller@pik-potsdam.de), Sibyll Schaphoff (sibylls@pik-potsdam.de)

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
- code review: Jannes Breier (breier@pik-potsdam.de), Constanze Werner (cowerner@pik-potsdam.de), Sebastian Ostberg(ostberg@pik-potsdam.de), 
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

