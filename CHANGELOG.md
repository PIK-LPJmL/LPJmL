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

