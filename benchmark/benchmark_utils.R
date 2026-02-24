# Shared utility functions and constants for LPJmL benchmark scripts
#
# This file is sourced by:
#   - simulate_default.R
#   - benchmark_default.R
#
# (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file
# authors, and contributors see AUTHORS file
# This file is part of LPJmL and licensed under GNU AGPL Version 3
# or later. See LICENSE file or go to http://www.gnu.org/licenses/
# Contact: https://github.com/PIK-LPJmL/LPJmL

# =============================================================================
# Output variable definitions
# =============================================================================

# Base outputs for all benchmark runs
outputvars_base <- c(
  "grid", "terr_area", "land_area", "lake_area", "vegc", "soilc",
  "litc", "vegn", "soiln", "soilnh4", "soilno3", "leaching", "n_immo",
  "n_mineralization", "n_volatilization", "n2_emis", "n2o_denit",
  "n2o_nit", "nuptake", "bnf", "firec", "flux_estabc", "gpp", "npp",
  "nbp", "rh", "evap", "transp", "interc", "runoff", "fpc"
)

# Additional outputs for landuse runs
outputvars_landuse <- c("cftfrac", "pft_harvestc")

# Additional outputs for methane runs
# TODO: Update once methane outputs are finalized
outputvars_methane <- c("ch4_emissions", "ch4_rice_em", "ch4_sink", "wetfrac")

# Combined output lists for each run type
outputvars_pnv <- outputvars_base
outputvars_lu <- c(outputvars_base, outputvars_landuse)
outputvars_pnv_methane <- c(outputvars_pnv, outputvars_methane)
outputvars_lu_methane <- c(outputvars_lu, outputvars_methane)

# =============================================================================
# Benchmark settings name mapping
# =============================================================================

# Mapping from lpjmlstats default_settings names to actual output file names
name_mapping <- c(
  "mleaching" = "leaching",
  "mn_immo" = "n_immo",
  "mn_mineralization" = "n_mineralization",
  "mn_volatilization" = "n_volatilization",
  "mn2_emis" = "n2_emis",
  "mn2o_denit" = "n2o_denit",
  "mn2o_nit" = "n2o_nit",
  "mnuptake" = "nuptake",
  "mbnf" = "bnf",
  "flux_estab" = "flux_estabc",
  "mgpp" = "gpp",
  "mnpp" = "npp",
  "anbp" = "nbp",
  "mrh" = "rh",
  "mevap" = "evap",
  "mtransp" = "transp",
  "minterc" = "interc",
  "mrunoff" = "runoff"
)

# =============================================================================
# Argument parsing
# =============================================================================

#' Parse command line arguments in name=value format
#'
#' @param args Character vector of command line arguments
#' @param defaults Named list of default values for expected arguments
#' @param type_converters Named list of functions to convert argument types
#' @return Named list of parsed arguments
parse_args <- function(args, defaults, type_converters = list()) {
  parsed <- defaults

  for (arg in args) {
    if (grepl("=", arg)) {
      parts <- strsplit(arg, "=", fixed = TRUE)[[1]]
      key <- parts[1]
      value <- parts[2]

      if (key %in% names(parsed)) {
        # Apply type converter if specified
        if (key %in% names(type_converters)) {
          parsed[[key]] <- type_converters[[key]](value)
        } else {
          parsed[[key]] <- value
        }
      } else {
        warning(paste("Unknown argument:", key))
      }
    }
  }

  parsed
}

# =============================================================================
# Benchmark settings creation
# =============================================================================

#' Create benchmark settings with mapped variable names
#'
#' @param base_settings The default_settings from lpjmlstats
#' @param name_mapping Named vector mapping old names to new names
#' @return Modified settings list with renamed variables
create_bm_settings <- function(base_settings, name_mapping) {
  var_names <- names(base_settings)
  for (old_name in names(name_mapping)) {
    pattern <- paste0("^", old_name, "$")
    var_names <- gsub(pattern, name_mapping[old_name], var_names)
  }
  # Handle pft_harvest prefix replacement
  var_names <- gsub("^pft_harvest.pft", "pft_harvestc", var_names)
  names(base_settings) <- var_names
  base_settings
}

#' Get benchmark settings for a specific run type
#'
#' @param base_settings The default_settings from lpjmlstats
#' @param run_type One of: "pnv_no_methane", "pnv_methane",
#'                         "lu_no_methane", "lu_methane"
#' @param time_avg_map If TRUE, add TimeAvgMapWithAbs metric to all variables
#' @return Settings list appropriate for the run type
get_bm_settings <- function(base_settings, run_type, time_avg_map = FALSE) {
  # Create base settings with mapped names
  bm_settings <- create_bm_settings(base_settings, name_mapping)

  # Remove pft_harvestc for PNV runs

  if (grepl("^pnv", run_type)) {
    pft_harvest_vars <- grep("^pft_harvestc", names(bm_settings), value = TRUE)
    bm_settings <- bm_settings[!names(bm_settings) %in% pft_harvest_vars]
  }

  # Add methane-specific settings for methane runs (not no_methane)
  if (grepl("_methane$", run_type) && !grepl("no_methane", run_type)) {
    # Use firec as template for methane variables
    template <- bm_settings$firec
    bm_settings$ch4_emissions <- template
    bm_settings$ch4_sink <- template
    bm_settings$wetfrac <- template
    # ch4_rice_em only for landuse runs (all zeros in PNV)
    if (grepl("^lu", run_type)) {
      bm_settings$ch4_rice_em <- template
    }
  }

  # Add TimeAvgMapWithAbs metric to all variables if requested
  if (time_avg_map) {
    bm_settings <- lapply(bm_settings, function(x) c(x, "TimeAvgMapWithAbs"))
  }

  bm_settings
}

#' Determine run type from simulation name
#'
#' @param sim_name Simulation name (e.g., "transient_no_methane_pnv")
#' @return Run type string: "pnv_no_methane", "pnv_methane",
#'                          "lu_no_methane", or "lu_methane"
get_run_type <- function(sim_name) {
  is_pnv <- grepl("_pnv", sim_name)
  is_methane <- !grepl("no_methane", sim_name)
  paste0(if (is_pnv) "pnv" else "lu", "_", if (is_methane) "methane" else "no_methane")
}
