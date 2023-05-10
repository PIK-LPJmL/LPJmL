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

## [5.6.8] - 2023-04-28

### Added

- flag "start_coupling" added in configuration file. If lpjml is coupled to an external model via

  "coupled_model" : "model",

  this flag allows so set a year when input is received from the external model. Before that year input is read from input file.
  If flag is omitted or set to null coupling starts at the first year.

### Changed

- For input received from an external model a file name can additionally be specified where input is read from before the specified coupling year:

  "with_tillage" : { "fmt" : "clm", "socket" : true, "name" : "tillage.clm"},

- lpjcheck and lpjml checks for specified filenames if they are necessary for the coupled run

- JSON metafiles are also created for NetCDF output

## [5.6.7] - 2023-04-27

### Added

- CHANGELOG.md added see issue [#298](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/issues/298)

### Changed

- leading zeros removed in `patch` identifier of the version number

## History

- all versions prior to 5.6.7 have not been documented by a changelog or similar but in the respective [merge requests](https://gitlab.pik-potsdam.de/lpjml/LPJmL_internal/-/merge_requests) (internal link accessible at PIK only)

