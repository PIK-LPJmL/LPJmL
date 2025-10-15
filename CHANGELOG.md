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

- author: Werner von Bloh (bloh@pik-potsdam.de)

### Added

- Option `-with_timing` added to `configure.sh` to enable timing, corresponding man page updated.
- Flag `-DUSE_TIMING` enables now timing for several LPJmL functions for performance analysis.
- Macros `timing_start()` and `timing_stop()` defined to allow timing of specific functions.

### Changed

- Flag `-DUSE_TIMING` removed from default compilation flags. Has to be enabled by `./configure.sh -with_timing`.


## [5.10.1] - 2025-09-30

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de)
- code review: Maik Billing (billing@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Added

- Variable `phen` added to output in `fprintpft.c`.
- Compile option `NRECOVERY_COST` added to `INSTALL` and man page of `configure.sh`.
- Check for valid `swc_bnf` parameters added in `fscanpftpar.c`.
- Wirth et al. (2024)  article added to `REFERENCES`.
- New parameter `"tscal_b"` added in `lpjparam.cjson`.
- New virtual function `getb()` added to type `Pft`.

### Changed

- 2 boolean flags `phen_to_one` and `rainyseason` added to tree parameter. These are used in `phenology_gsi.c` instead of string compare with tree names.
- Unused argument `aet_layer` removed from `nitrogen_stress()` function.
- `phi_bnf` parameter calculated from `swc_bnf` parameter instead of reading from `pft.cjson` file in `fscanpftpar.c`.
- Hard-coded parameter in temperature dependent calculation of `b` replaced by global parameter `tscal_b`.
- Calculation of `b` in `setupannual_grid.c` replaced by call to new virtual PFT function `getb()`.

### Removed

- Parameter `phi_bnf` removed from `pft.cjson`.
- Variable `b` removed from restart file.
- Unused macro `RESTART_VERSION` removed from `header.h`.

### Fixed

- Arguments of `npp` function corrected in `daily_woodplantation.c`. The current version leads to a too strong reduction in NPP.
- `nuptake_crop.c` changed to compile with `-DDEBUG_N` option.
- `update_daily_cell.c` changed to compile with `-DDEBUG` option.
- `daily_agriculture_tree.c` changed to compile with `-DDEBUG2` option.
- Typo in error message in `bstruct_writearrayindex.c` corrected.
- Error message corrected in `bstruct_wopen.c`.
- Error number corrected in `bstruct_finish.c`.
- Argument `temp` added to `f_turnover_tree` function to compile with `-DNRECOVERY_COST` option.


## [5.10.0] - 2025-08-14

### Contributors

- author: Jens Heinke (heinke@pik-potsdam.de), Sibyll Schaphoff (sibylls@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- option to specify year after which deforestation is no longer feeding product pools but biomass is burnt instead: `luc_burn_startyear` (disabled)
- option to specify costs for N recovery upon turnover (pre-compiler switch `NRECOVERY_COST`, disabled)

### Changed

- parameter changes: 
  - `alphaa` from 0.5 to 0.7 for natural vegetation; 
  - `theta` from 0.9 to 0.95; 
  - `fn_turnover` PFT1: 0.5, PFT2: 0.5, PFT3: 0.7, PFT4: 0.3, PFT5: 0.3, PFT6: 0.7, PFT7: 0.3, PFT8: 0.3; updated values from [Sophia et al. 2024](https://doi.org/10.5194/bg-21-4169-2024) for all other PFTs except crops
  - `cnratio_leaf.low/median/high` for all PFTs updated from TRY data
  - `b` (ratio of dark respiration Rd to maximum photosynthetic capacity Vcmax) set to 0.031 for C3 and C4 plants (value for 25°C, from [Wang et al. 2020](https://doi.org/10.1111/gcb.14980), consistent with TRY data)
  - `aphen_min` from 60 to 90 and `aphen_max` from 245 to 180
  - `nfixpot` from 0.01 to 0.5 (middle of proposed range in [Yu and Zhuang 2020](https://doi.org/10.5194/bg-17-3643-2020))
  - `turnover` parameter for leaves&roots set to 2 for TeNE and 3 BoNE 
  - temperature boundary between temperate and boreal zone changed from -2°C to -5°C
- acclimation of `b` to mean vegetation period temperature (following [Wang et al. 2020](https://doi.org/10.1111/gcb.14980))
- calculation of structural leaf N content (leaf N independent of Vcmax) based on `ncleaf.low` in both `ndemand_xx` and `vmaxlimit_xx` (was `ncleaf.medium` in `ndemand_xx`)
- scaling of vmax with f_LAI removed
- N recovery from turnover directly added to `bm_inc.nitrogen`
- N uptake now separated for uptake of NH4 and NO3 with parameters from  [Craig et al. 2025](https://doi.org/10.1111/nph.70140) (median of values)
- trees are burnt (instead of added to the litter) when running with `luc_timber=FALSE`
- beginning of vegetative period for tropical raingreen trees set at beginning of 6 month period with highest precipitation sum
- leaf shedding for deciduous trees induced after `aphen_max` or when `phen<0.1` after `aphen_min`
- `phen` permanently set to 1 for tropical evergreen trees

### Removed

### Fixed

- corrected conversion of vmax in computation of N demand, which was incorrectly scaled with `24/daylength`
- corrected temperature sensitivity of N demand (`k_temp` changed from 0.02 to 0.0693)
- corrected `f_NCplant` to comply with equation from [Smith et al. 2014](https://doi.org/10.5194/bg-11-2027-2014)
- corrected phenology of trees:
  - leaf turnover of raingreen trees at leaf shedding (same as for summergreen trees, had continuous daily leaf turnover like evergreen trees)
  - `phen` set to zero at leaf shedding until restart of phenological cycle to prevent phen>1 without leaf carbon 


## [5.9.28] - 2025-07-31

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Maik Billing (billing@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de), Sebastian Ostberg(ostberg@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de)

### Added

- Option `-h` added to utilities `addheader`, `cdf2bin`, `regridclm`, `regridsoil` and `printclm` to print help text.
- Notice added in `configure.sh` that `make clean; make all` has to be performed after change in configuration.
- Support for short datatype added in `cmpbin` utility.
- Shell script `allbin2cdf` added to convert all binary raw output files in a directory to NetCDF files.
- Option `-nounit` added to `bin2cdf` to set unit if unit is defined in the metafile as an empty string.

### Changed

- If option `-ncell 0` is used in `addheader` utility then the number of cells is calculated from the file size of the binary file.
- Utilities `grid2clm` and `cft2clm` are replaced by alias to `addheader`.
- File `default.md` converted from DOS to Linux format.
- If scaling factor is set as an option in `bin2cdf` then this scaling factor is used instead that one defined in the JSON metafile.
- Man pages updated.

### Fixed

- Missing `free(axis->comment)` added in `freeaxis()` to avoid memory leak.
- Missing deallocation of list added in `open_config.c`.
- Function `nc_close()` replaced by `closeclimate_netcdf()` in `cdf2clm.c` and `cdf2bin.c` to avoid memory leak.
- Argument for error message corrected in `getcellindex.c` and `getcountry.c`.
- Typo in man page of `configure.sh` fixed.
- Comment for `pft` initialized in `initsetting_netcdf.c`.
- Access to undefined `pft_name.comment` removed in `bin2cdf.c` to avoid SEGV.
- Short data handled correctly for metafile input in `bin2cdf`.
- Missing check for sum operator added in utility `mathclm`. Without this check number of bands in output file is always set to 1. 
- Man page for `mathclm` corrected for option `-v`.


## [5.9.27] - 2025-07-15

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), Sibyll Schaphoff (sibylls@pik-potsdam.de)

### Added

- Utility `restart2yaml` added to convert the new restart files into human readable YAML or JSON files.
- Utility `json2restart` added to convert JSON files into restart files.
- alias `restart2json` added for `restart2yaml -json`.
- Output of time spent in reading and writing the restart file added.
- Option `-print_noread` added to `lpjml` to print variable names not read from restart file.
- Check added that PFT names in restart file match PFT names in LPJmL configuration file.
- Unit tests added for bstruct library functions.

### Changed

- Restart file format changed to include metadata. A binary JSON-like structure has been implemented. The order of the objects read can be different from the order objects have been written, but performance of reading can be degraded.
- Utility `lpjcat` updated to support the new restart file format.
- Preprint's doi:10.5194/egusphere-2023-2946 changed to the final BNF paper doi:10.5194/gmd-17-7889-2024 in `.zenodo.json`.


## [5.9.26] - 2025-07-14

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: David Hötten (davidho@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Changed

- New functions `setupannual_grid`, `initmonthly_grid()`, `updatedaily_grid()` and `updateannual_grid()` defined to minimize lines of code in `iterateyear.c` and `iterate.c` and minimize duplicate code in `lpj_poem.c`.
- Parts of code in `iterateyear.c` has been moved to `update_daily_cell.c` and `update_monthly_grid.c`.
- `lpj_climber4.c` renamed to `lpj_poem.c`.
- `update_daily.c` renamed to `update_daily_cell.c`.
- `update_monthly.c` renamed to `update_monthly_grid.c`.
- `AFTER_STRESS` debugging output in `update_daily_cell.c` (former `updatedaily.c`) moved after N update really happened.

### Fixed

- Macro `isequilyear()` has been defined for calling equilibration function in a consistent way (issue #375).


## [5.9.25] - 2025-05-22

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), Fabian Stenzel (stenzel@pik-potsdam.de), Sebastian Ostberg(ostberg@pik-potsdam.de)

### Changed

- Axis names and missing values for NetCDF output are read from JSON file `netcdf.cjson` instead of being hard coded.
- Warnings in `addheader` replaced by terminating errors.
- Function `openinputfile()` returns an error if cell size differs in JSON metafile.
- Function `openmetafile()` returns an error if binary file is empty.
- Output format for cell size updated in `fprintheader.c`, `fprintoutputjson.c`, `fprintjson.c`, and `openinputfile.c`.

### Added

- New utility `splitclm` added to copy specific bands from a clm file.
- Environment variables `LPJNOPP` and `LPJPEDANTIC` added. If set to `true`, preprocessor is disabled and pedantic checking is enabled, respectively.
- Info about `--help` option added in case of error in command line arguments.
- Option `-config` added to `cdf2clm`, `bin2cdf`, `country2cdf` and `clm2cdf` in order to read axis names and missing values from a JSON file. File `netcdf.json` added as a template.
- Option `-json` added to `arr2clm`, `asc2clm`, `txt2grid`, `txt2clm`, `getcountry`, and `cdf2coord` in order to create additional JSON metafiles.
- Option `-inpath` added to `regridlpj` in order to set directory where data can be found.
- Options `-zero` and `-search` added to `regridlpj`.
- Option `-double` added to `txt2grid` in order to create grid files with coordinate values of type double.
- Option `-timestep` added to `addheader`, `txt2clm` and `bin2cdf`.
- Option `sum`, `tsum` and `tmean` to `mathclm` added.
- Check for correct setting of nstep and nbands added in `txt2clm`.

### Fixed

- Misspelled `-fsanitize=address` option corrected in `Makefile.mpich`.
- Missing `free(cmd)` added in `fprintincludes.c`.
- Parsing of options corrected in `fprintincludes.c`.
- Utility `bin2cdf` fixed for timestep>1.


## [5.9.24] - 2025-05-15

### Contributors

- author: David Hötten (davidho@pik-potsdam.de)
- code review: Sebastian Ostberg (ostberg@pik-potsdam.de), Maik Billing (billing@pik-potsdam.de)

### Added

- Merge request template ``.gitlab/merge_request_templates/default.md`` that includes several checklists.


## [5.9.23] - 2025-05-08

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jens Heinke (heinke@pik-potsdam.de), Fabian Stenzel(stenzel@pik-potsdam.de)

### Changed

- Option `-check` of `configure.sh` enables now run-time checks of memory leaks and memory access out of bounds and undefined variables for gcc and icx compiler. Optimization is not disabled. Use options `-debug -check` to disable.
- Datatype `List` used in `fprintfiles.c`.
- All allocated memory is now freed in utilities.
- Prescribed landcover input file changed in `input.cjson` to a file with the same number of natural PFTs.
- All remaining unsafe calls to `sprintf()` replaced by calls to `snprinf()` or `getsprinf()`.

### Fixed

- Check for missing values for integer input corrected in `readclimate_netcdf.c`.
- Memory leak fixed in `filesexist.c`.
- Calculation of global area fixed in `printglobal.c`.
- Check for null attribute pointer added in `fprintjson.c`.
- Uninitialized grid name initialized in `mathclm.c`.
- Call to `snprintf()` replaced by `getsprintf()` in `reservoir2cdf.c`.
- Datatype for index and writing the header corrected in `lpjcat.c`.
- Functions for qsort(), bisect(), and leftmostzero() changed to avoid run-time errors with `-check` option.
- Pointer set to NULL  if `k_est`is NULL in `initmanage.c`.
- Missing argument added in opening aquifer file for IMAGE.
- Scaling of coordinates fixed in `joingrid.c`.
- Fraction pointer initialized to NULL in `freadresdata.c` to avoid SEGV in `lpjprint.c` if reservoir data is read from restart file.
- Index set correctly for outflow cells in `regriddrain.c`.
- Check for maximum discharge length corrected in `printdrain.c`.
- Number of years of clm file set to 1 in `grid2clm.c`.
- Function `fscanlandcovermap.c` corrected (issue https://github.com/PIK-LPJmL/LPJmL/issues/48).
- Access out of bounds for `soil->freezdepth` fixed in `soiltemp.c`.
- Format specifier changed in `cat2bsq.c` and `cutclm.c` to compile without warnings using the clang compiler (issue #372).


## [5.9.22] - 2025-03-14

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Hester Biemans (hester.biemans@wur.nl)
- code review: Susanne Rolinski (rolinski@pik-potsdam.de), Marie Hemmen (hemmen@pik-potsdam.de)

### Changed

- `input_netcdf.cjson` changed to the default dataset of `input.cjson` but in NetCDF format.
- Code changed to compile under Windows OS.
- New function `getsprintf()` added to allocate and print formatted output into string. Function replaces call to `snprintf()` function.
- Default settings for `GIT_HASH` and `GIT_REPO` added.
- Duplicate filenames removed from list of input/output filenames in utility `lpjfiles`.

### Added

- Option `-int` added to utility `cdf2clm`.
- Option `-latlon` added to utility `cdf2coord` in order to change the order of the CLM grid file.
- Utility `cdf2reservoir` added to convert NetCDF reservoir file into CLM file.
- Utility `reservoir2cdf` added to convert CLM reservoir file into a NetCDF file using the soil code NetCDF file.
- Macro `NETCDF_INPUT` added in `lpjml_config.cjson` to enable NetCDF input.
- Reservoir, irrigation neighbor and drainage data can now be in NetCDF format:
```java
"drainage" :           { "fmt" : "cdf", "var" : "index", "name" : "cru_netcdf/drainage.nc"},
"neighb_irrig" :       { "fmt" : "cdf", "var" : "index", "name" : "cru_netcdf/neighb_irrig.nc"},
"river" :              { "fmt" : "cdf", "var" : "riverlen", "name" : "cru_netcdf/drainage.nc"},
"reservoir" :          { "fmt" : "cdf", "var" : "year", "name" : "cru_netcdf/reservoir.nc"},
"capacity_reservoir" : { "fmt" : "cdf", "var" : "capacity", "name" : "cru_netcdf/reservoir.nc"},
"area_reservoir" :     { "fmt" : "cdf", "var" : "area", "name" : "cru_netcdf/reservoir.nc"},
"inst_cap_reservoir" : { "fmt" : "cdf", "var" : "inst_cap", "name" : "cru_netcdf/reservoir.nc"},
"height_reservoir" :   { "fmt" : "cdf", "var" : "height", "name" : "cru_netcdf/reservoir.nc"},
"purpose_reservoir" :  { "fmt" : "cdf", "var" : "purpose", "name" : "cru_netcdf/reservoir.nc"},
```

### Fixed

- River length is now correctly read in `initdrain.c` for data in NetCDF format.
- Man page of `cvrtclm` corrected.
- Missing check for open NetCDF file added to `openclimate.c`.


## [5.9.21] - 2025-03-13

## [5.9.21] - 2025-01-31

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jannes Breier (breier@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- `"coupled_host"` and `"coupled_port"` added to LPJmL configuration to specify where the coupled model is running and which port is used for communication.


## [5.9.20] - 2025-03-13

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Sebastian Ostberg (ostberg@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de)

### Added

- Check for correct number of time steps added in `openclimate_netcdf.c`.
- Option `timestep` added to `setclm`.

### Changed

- Function `freegrid()` is calling `freecell()` to avoid redundant code.
- References in man pages updated.
- `-v` option of `cdf2clm` prints leap days setting for daily time step.
- Missing file `CHANGELOG.md` added to tar and zip file.
- Longitude and latitude boundaries excluded as variables in `cdf2grid`.
- Ids for sowing date and crop PHU input added to `couplerpar.h`.
- Length of GIT repository output limited in `copyright.c`.

### Fixed

- Memory leaks in utility `lpjprint` closed.
- Handling of NaN as missing value corrected in reading NetCDF files.
- Doubled initialization of standtype array corrected in `lpj_clinber4.c`.
- Option `type` corrected in `setclm`.
- Handling of different endianness corrected in `setclm`.
- If version is changed with `setclm` only version is updated in the clm file.
- Check that CO2 data is coming completely from socket corrected in `readco2.c`.
- Number of years are now calculated correctly from number of days in case of leap days if number of leap days reaches 365.
- Check for identical filenames corrected in `cutclm.c`.
- Missing `else` added in `readclimate()`.


## [5.9.19] - 2025-03-12

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Fabian Stenzel (stenzel@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de)

### Added

- Option `-partition` added to `lpjsubmit` script.

### Changed

- Man page of lpjsubmit refers only to slurm commands.
- If `icc` compiler is found, this compiler is used.
- `enablefpc.c` updated to compile under Mac OS.

### Removed

- Support for old PIK cluster removed.
- Support for LoadLeveler batch queueing system removed.
- Support for AIX OS removed.
- Alias for `lpjml` and obsolete `lpjml.sh` removed in `lpj_paths.sh`.


## [5.9.18] - 2025-01-31

### Contributors

- author: Christoph Müller (cmueller@pik-potsdam.de)
- code review: Sebastian Ostberg (ostberg@pik-potsdam.de), Marie Hemmen (hemmen@pik-potsdam.de)

### Fixed

- `basetemp` and `hlimit` parameters fixed to fit to standard growing season inputs (issue #346).
- `laimax` values corrected from 5 to 7 for `temperate cereals`, `rice`, and `tropical cereals`


## [5.9.17] - 2025-01-29

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Sebastion Ostberg (ostberg@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)
- code review: Marie Hemmen (hemmen@pik-potsdam.de), Jens Heinke (heinke@pik-potsdam.de)

### Added

- Option `-cmd` added to `lpjsubmit` in order to execute a command before lpjml is called.
- Command `module list` added to slurm script to show all loaded modules.

### Removed

- Obsolete `#ifdef WITHOUT_NITROGEN` check removed from `pft.cjson`.

### Fixed

- `ALLOM3` parameter corrected to 4.0 for oil palm in `pft.cjson` (issue #369).
- Missing `break` added in `convert_water.c`.


## [5.9.16] - 2024-11-15

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jens Heinke (heinke@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Added

- GIT repository and GIT hash printed in the LPJmL banner and in output of NetCDF and JSON files.
- Options `-repo` and `-hash` added to `lpjml` to print GIT repository and hash.
- The JSON file created by `createconfig` contains now name and hash of GIT repository.
- Command `lpjml -v` prints GIT repository and hash.
- Target `hash` added to Makefile to update hash after committing changes to the GIT repository.

### Changed

- `LPJ_VERSION` macro in `lpj.h` replaced by function `getversion()`.

### Removed

- Obsolete header columns cereal and maize removed from `fprintcountrypar.c`.

### Fixed

- Missing dependency on `gebuild.c` for target `lpjml` added in `src/Makefile`.
- `xiar` replaced by `ar` in `Makefile.hpc2024` in order to compile with `intel/oneAPI/2025.0.0`.
- Typos in error messages in `filesexist.c` and `cdf2soil.c`fixed.
- Missing check for open file added to `joingrid.c`.


## [5.9.15] - 2024-11-07

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- Option `-scale` added to `bin2cdf` in order to scale output.
- Option `-notime` added to `bin2cdf` in order to omit time axis.
- `time_bnds`, `lat_bnds`, and `lon_bnds` arrays added to NetCDF output for lpjml runs and the `bin2cdf` utility.
- Check for identical output filenames added in `fscanoutput.c`.

### Changed

- If `"with_days"` is set to true in the lpjml configuration file, time axis of yearly output is also set in units of days instead of years.
- Time in NetCDF output is always set in the middle between the lower and upper time boundaries.

### Removed

- Unused file `cflux_sum.c` removed.
- Conversion of obsolete file `manage.par` removed from utility `manage2js`.

### Fixed

- Separate output for each year by setting `"name" : "filename_%d"` is now written correctly (issue #360).
- Number of bands set to 1 in metafile of grid NetCDF output.
- Name of longitude/latitude variables correctly derived in `getlatlon_netcdf.c`.
- Misspelled option `-netdcdf4` corrected to `-netcdf4` in `clm2cdf.c`.
- `lpj_climber4.c` updated to compile without errors.
- Writing different variables into one NetCDF file fixed in `create_netcdf.c`. Only NetCDF outputs with one band and same time step can be written to one file.
- Formatting of man pages corrected.
- Missing `shift 1` for `-crumonthly` case added in `regridlpj`.


## [5.9.14] - 2024-09-30

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Stephen Wirth (wirth@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Fixed

- `pft->vscal` not set to `NDAYYEAR` in `update_annual.c`. (issue #364).
- `freezefrac2soil.c` corrected to avoid division by zero.
- size of option array for `sowing_date_option` and `crop_phu_option` now correctly specified in `fscanconfig.c`.


## [5.9.13] - 2024-09-26

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Sebastian Ostberg (ostberg@pik-potsdam.de), Marie Hemmen (hemmen@pik-potsdam.de)

### Changed

- `USE_NETCDF4` compile option replaced by `"netcdf4"` boolean flag in the LPJmL configuration file. If set compression of NetCDF4 files can be enabled and PFT names are written as strings instead of character arrays.
- Formatting of man pages harmonized.
- If lpjml is compiled without `-DUSE_NETCDF` flag then `fscanconfig.c` and `fscanoutput.c` return with an error for NetCDF files specified for input/output.

### Added

- Option `-netcdf4` added to `bin2cdf`, `clm2cdf`, `country2cdf`, and `drainage2cdf` utility to enable NetCDF4 format.
- Boolean flag `"netcdf4"` added to LPJmL configuration file in order to enable NetCDF4 format.
- Option `-compress` added to `drainage2cdf` utility to enable file compression.
- Man page for `drainage2cdf` added.
- Check for valid compression value added for lpjml and utilities.
- Missing option `"global_netcdf"` added to LPJmL config file.

### Removed

- Obsolete man page for `writeregioncode()` removed.

### Fixed

- Missing deallocation of memory added in `cpl_init.c`, `fscanlandcovermap.c`, `fscanoutput.c`, `writearea.c`, `create_pft_netcdf.c`, `newgrid.c`, `fscanagtreemap.c`, `celldata.c` in case of error.
- Syntax error fixed in `update_daily.c` in IMAGE coupling.
- Typo in `README` corrected.
- `establishmentpft.c`, `cdf2coord.c` and `cdf2grid.c` modified to compile without warnings using gcc.
- Debug flag in `send_token_coupler.c` and `openoutput_coupler.c` corrected to `DEBUG_COUPLER`.
- Code changed to compile without errors/warnings for `-DIMAGE -DCOUPLED` setting.
- Calculation of offsets and counts corrected in `readintdata_netdf.c` and missing loop over bands added.


## [5.9.12] - 2024-09-24

### Contributors

- author: Christoph Müller (cmueller@pik-potsdam.de)
- code review: Werner von Bloh (bloh@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- added SLURM option `--cpus-per-task=1` to `lpjsubmit` template `bin/lpjsubmit_hpc` to avoid that mpirun uses several CPUs per process


## [5.9.11] - 2024-09-23

### Contributors

- author: Stephen Wirth (wirth@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)
- code review: Alja Vrieling (alja.vrieling@vortech.nl), Johanna Braun (jobraun@pik-potsdam.de)

### Added

- tree.fruit biomass is now explicitly handled in timber_harvest() and pools scaled accordingly in annual_woodplantation.c even though these are currently always zero anyways.

### Fixed

- added missing scaling of `bm_inc.nitrogen` after part of it was added to litter in harvest of wood plantations as in issue #358


## [5.9.10] - 2024-09-23

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Removed

- Option `"no"` for nitrogen setting removed, only `"lim"` and `"unlim"` allowed. Corresponding parameter files `lpjparam_non.cjson` and `pft_non.cjson` removed.
- Removed commented-out code in `allocation_tree.c`

### Fixed

- Missing check for zero added in `allocation_grass.c` to avoid division by zero.


## [5.9.9] - 2024-09-04

### Contributors

- author: Christoph Müller (cmueller@pik-potsdam.de)
- code review: Stephen Wirth (wirth@pik-potsdam.de), Fabian Stenzel (stenzel@pik-potsdam.de)

### Added

- new options `PRESCRIBED_SDATE_ALL_RAINFED` and `PRESCRIBED_SDATE_ALL_IRRIG` to allow using the same sowing dates based on the rainfed or irrigated seasons respectively
- new options `PRESCRIBED_CROP_PHU_ALL_RAINFED` and `PRESCRIBED_CROP_PHU_ALL_IRRIG` to allow using the same PHU requirements based on the rainfed or irrigated seasons respectively

### Changed

- refactored `crop_option_restart` to `crop_phu_option_restart` in struct `Config` and `crop_option` to `crop_phu_option` in struct `Restartheader` for greater clarity
- refactored options `"new"` to `"vbussel15"` and `"old"` to `"bondau07"` for `crop_phu_options` where `"vbussel15"` is an implementation based on [van Bussel et al. 2015](http://dx.doi.org/10.1111/geb.12351) 

### Removed

- removed unnecessary check for file of prescribed `sdates` in `fileexist.c`

### Fixed

- fixed missing initialization of pointer `map` in cft2bin.c that caused abortion of compilation with `-Werror` otherwise


## [5.9.8] - 2024-09-02

### Contributors

- author: Christoph Müller (cmueller@pik-potsdam.de)
- code review: Susanne Rolinski (rolinski@pik-potsdam.de), Fabian Stenzel (stenzel@pik-potsdam.de)

### Changed

- included latest reference in .zenodo.json for syncing github and gitlab repositories. Future transfer of code between gitlab and github should be smoother now as both are at the same commit history now


## [5.9.7] - 2024-08-30

### Contributors

- author: David Hoetten (davidho@pik-potsdam.de)
- code review: Sibyll Schaphoff (sibylls@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de)

### Changed

- order of freezing of different water soil water components changed: free water freezes last to simply mathematical description of model


## [5.9.6] - 2024-08-29

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: David Hoetten (davidho@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)

### Added

- Utility `cdf2grid` added to convert grid files in NetCDF format into grid CLM files.
- Option `-json` and `-raw` added to `cdf2coord` utility to write additional JSON metafile and to write without CLM header.

### Changed

- Check added that dimension must be greater 1 for variable in NetCDF file in `cdf2coord.c`, `input_netcdf.c` and `coord_netcdf.c`.
- Datatype of longitude/latitude array in `coord_netcdf.c` changed from float to double for consistency, variable name correctly printed in error messages.
- Intel and clang compiler recognized in `printflags.c`.

### Fixed

- Test for `null` corrected for `"output"` in `fscanoutput.c`.
- Missing `break` added in `switch` statement in `receiver_coupler.c`.
- Code changed to compile successfully without `-DSAFE`, `-DUSE_NETCDF`, and `-DUDUNITS` compilation flag.
- Unit for `"estab_storage_n"` corrected to `"gN/m2"` in `outputvars.cjson`.
- Scaling set to 1 if datatype is not short in `cdf2coord.c` and `cdf2clm`.


## [5.9.5] - 2024-07-19

### Contributors

- author: Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Christoph Mueller (cmueller@pik-potsdam.de), Werner von Bloh (bloh@pik-potsdam.de)

### Fixed

- Fix a regression for `NBP` output in `fwriteoutput` introduced in version 5.9.4


## [5.9.4] - 2024-07-18

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de)
- code review: Jens Heinke (heinke@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Added

- Warning added if time step of output is longer than output interval.
- Cycle length of spinup and number of spinup years checked for positive values in `fscanconfig.c`.
- File size of CLM input files is checked for consistency with header and lpjml is stopped accordingly.
- Utility `drainage2cdf` added to convert CLM drainage file into a NetCDF file using the soil code NetCDF file (issue #355).

### Changed

- Setting `"const_lai_max"` renamed to `"laimax_const"` for consistency.
- Keyword `"laimax_interpolate"` renamed to `"laimax_manage"`.
- Region-specific fractions for residue burning replaced by global parameter `"bifratio"` and `"fuelratio"`.
- `lpjml` now terminates with an error message instead of a warning on invalid country codes to avoid invalid access to country-specific parameters.
- Country code files can now have only one band. Files with 2 bands are still supported, but region code ignored.
- Filename and source is written into configuration file created by `regridlpj`.
- `null` allowed for `"global_attrs"`, `"inpath"`, `"outpath"`, `"restartpath"`, `"output"`, and `"checkpoint_filename"` to disable the feature.

### Removed

- `"laimax_interpolate"`, `"firewood"`, `"black_fallow"`, `"till_fallow"`, `"cropsheatfrost"`, and `"grassland_fixed_pft"` settings and corresponding code removed (issue #350).
- Obsolete function `daily_setaside.c` removed. Function is not necessary any more for coupling to POEM.
- Obsolete constants removed in `conf.h`.
- Unused parameter `"residue_rate"`, `"residue_pool"`, `"residue_cn"`, `"residue_fbg"` removed.
- Functions for reading region-specific parameters removed.
- Obsolete region parameter file `manage_reg.cjson` removed
- Obsolete outputs `"region"`, `"flux_firewood"`, `"flux_firewood_n"` removed.
- Region definitions removed from `managepar.h`.

### Fixed

- Typos in error messages corrected in `bin2cdf.c` and `clm2cdf.c`.
- Output of turnover corrected in `fprintpar_grass.c`.
- Output of options fixed in `openconfig.c` if environment variable `LPJOPTIONS` is set.
- Calculation of average fixed in `statclm`.
- `res_remove` calculation fixed for residue fires in `harvest_crop.c` to close carbon and nitrogen balance.
- Missing update of `FLUX_ESTABN_MG` added in `turnover_tree.c`.
- Function `initdrain()`corrected to handle river routing files in NetCDF format.


## [5.9.3] - 2024-07-18

### Contributors

- author: Werner von Bloh (bloh@pik-potsdam.de), Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Fabian Stenzel (stenzel@pik-potsdam.de), David Hötten (davidho@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Added

- Output `"nbp"` for net biosphere productivity and `"tws"` for total water storage added.
- `fail` added to mail type in `lpjsubmit_hpc`.
- Option `-map` added to `cdf2bin` and `cdf2clm` to read map from NetCDF file and write it into JSON metafile.
- `"sim_name"` and all other global attributes are written into JSON file for `cdf2bin` utility.

### Fixed

- Scaling of `"littertemp"` output fixed.
- Unit corrected for `"rootmoist"` output and scaling corrected.
- Correct number of output files printed in `lpjfiles` utility if separate output files for each year are enabled.

### Changed

- `DEPTH_NAME` and `BNDS_NAME` excluded for search for variable in NetCDF file if no variable name is provided.
- Check added that dimension must be greater 1 for variable in NetCDF file.


## [5.9.2] - 2024-07-18

### Contributors

- author: Sebastian Ostberg (ostberg@pik-potsdam.de)
- code review: Jens Heinke (heinke@pik-potsdam.de), Christoph Müller (cmueller@pik-potsdam.de)

### Added

- `fire_grass` burns grass leaf biomass based on fire resistence parameter (function did nothing before)
- Burning of fruits added to `fire_tree`, although stands with fruit trees currently do not experience fire

### Changed

- Unit and description of `FIREF` output changed from fire return interval to fire fraction to harmonize output between GlobFirM and SpitFire

### Fixed

- Added missing update of FPCs after fire before establishment


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
- New site-specific `Makefile.hpc2024` and `Makefile.icx` for parallel/sequential compilation on new cluster added.
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

