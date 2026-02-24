# Standardized LPJmL benchmark runs using lpjmlkit
#
# Usage example:
# Rscript simulate_default.R sim_path="/path/to/results"
# Rscript simulate_default.R sim_path="/path/to/results" ntasks=256
# Rscript simulate_default.R sim_path="/path/to/results" ntasks=256 \
#   blocking=64 model_path="../bin/lpjml" qos="standby"
# Rscript simulate_default.R sim_path="/path/to/results" \
#   runs_subset="transient_methane_pnv,transient_methane_lu" wtime="10:00:00"
# Rscript simulate_default.R sim_path="/path/to/results" \
#   runs_subset="1,3,5" wtime="10:00:00"
#
# Arguments (use name=value format):
#   sim_path    : Path for simulation results (required)
#   ntasks      : Number of parallel tasks (default: 512)
#   blocking    : Cores per task to block (default: 128, or "unlimited")
#   model_path  : Path to model directory with binary and configs (default: .)
#   qos         : Quality of service (optional, default: "standby")
#   runs_subset : Comma-separated list of simulation names or
#                 indices (default: all)
#   wtime       : Wall time for all runs (default: NULL, uses lpjmlkit defaults)
#
# (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file
# authors, and contributors see AUTHORS file
# This file is part of LPJmL and licensed under GNU AGPL Version 3
# or later. See LICENSE file or go to http://www.gnu.org/licenses/
# Contact: https://github.com/PIK-LPJmL/LPJmL
#
# =============================================================================

library(lpjmlkit)

# Source shared utilities
script_dir <- (function() {
  args <- commandArgs(trailingOnly = FALSE)
  file_arg <- grep("--file=", args, value = TRUE)
  if (length(file_arg) > 0) dirname(normalizePath(sub("--file=", "", file_arg)))
  else getwd()
})()
source(file.path(script_dir, "benchmark_utils.R"))

# =============================================================================
# Parse command line arguments
# =============================================================================

args <- commandArgs(trailingOnly = TRUE)

# Define defaults and type converters for this script
defaults <- list(
  sim_path = NULL,
  ntasks = 512,
  blocking = 128,
  model_path = getwd(),
  qos = "standby",
  runs_subset = NULL,
  wtime = NULL
)

type_converters <- list(
  ntasks = as.integer,
  blocking = function(x) {
    if (tolower(x) == "unlimited") "unlimited" else as.integer(x)
  }
)

parsed_args <- parse_args(args, defaults, type_converters)

# Check if required argument is provided
if (is.null(parsed_args$sim_path)) {
  stop(paste(
    "Usage: Rscript simulate_default.R sim_path=<path>",
    "[ntasks=512] [blocking=128] [model_path=.] [qos=standby]",
    "[runs_subset=<names>] [wtime=<time>]\n",
    "  sim_path    : Path where simulation results will be stored (required)\n",
    "  ntasks      : Number of tasks for parallel execution (default: 512)\n",
    "  blocking    : Number of cores per task to be blocked or 'unlimited'",
    "(default: 128)\n",
    "  model_path  : Path to model directory with binary and configs",
    "(default: .)\n",
    "  qos         : SLURM quality of service class (default: standby)\n",
    "  runs_subset : Comma-separated simulation names or indices",
    "(default: all)\n",
    "  wtime       : Wall time for all runs (default: 7 days for standby qos)"
  ))
}

sim_path <- parsed_args$sim_path
ntasks <- parsed_args$ntasks
blocking <- parsed_args$blocking
model_path <- parsed_args$model_path
qos <- parsed_args$qos
runs_subset <- parsed_args$runs_subset
wtime <- parsed_args$wtime

# =============================================================================
# Define simulation runs
# =============================================================================

runs <- tibble::tibble(
  sim_name = c(
    "spinup_methane", "spinup_no_methane", "transient_methane_pnv",
    "transient_no_methane_pnv", "transient_methane_lu",
    "transient_no_methane_lu"
  ),
  dependency = c(
    NA, NA, "spinup_methane", "spinup_no_methane", "spinup_methane",
    "spinup_no_methane"
  ),
  with_methane = c(NA, FALSE, NA, FALSE, NA, FALSE),
  landuse = c(NA, NA, "no", "no", NA, NA),
  wateruse = c(NA, NA, FALSE, FALSE, NA, NA)
)

# =============================================================================
# Apply runs_subset filter if specified
# =============================================================================

runs_to_submit <- NULL
if (!is.null(runs_subset)) {
  filter_values <- strsplit(runs_subset, ",")[[1]]
  filter_values <- trimws(filter_values)

  # Check if filter values are numeric indices or names
  if (all(grepl("^[0-9]+$", filter_values))) {
    # Numeric indices
    filter_indices <- as.integer(filter_values)
    if (any(filter_indices < 1 | filter_indices > nrow(runs))) {
      stop(paste("Index out of range. Valid indices are 1 to", nrow(runs)))
    }
    runs_to_submit <- runs$sim_name[filter_indices]
  } else {
    # Simulation names
    if (!all(filter_values %in% runs$sim_name)) {
      invalid <- filter_values[!filter_values %in% runs$sim_name]
      stop(paste(
        "No matching simulations found for:",
        paste(invalid, collapse = ", ")
      ))
    }
    runs_to_submit <- filter_values
  }
  message(paste(
    "Will submit filtered simulations:",
    paste(runs_to_submit, collapse = ", ")
  ))
}

# =============================================================================
# Split runs by type and write configs
# =============================================================================

# Split runs into categories (spinups + transients for each methane setting)
# no_methane runs: spinup_no_methane + all transient_no_methane_*
runs_no_methane <- runs[grepl("no_methane", runs$sim_name), ]
# methane runs: spinup_methane + all transient_methane_*
runs_methane <- runs[!grepl("no_methane", runs$sim_name), ]

# Define which variables need monthly timestep
monthly_vars <- c("gpp", "evap", "transp", "rh", "npp", "vegc")

# Create timestep vector for no_methane runs (LU output list)
timesteps_lu <- ifelse(outputvars_lu %in% monthly_vars, "monthly", "annual")

# Write configs for no_methane runs (use base outputs, no methane outputs)
# PNV transients use outputvars_pnv, LU transients use outputvars_lu
# Spinups don't have benchmarkable outputs, but need consistent config
configs_no_methane <- write_config(
  x = runs_no_methane,
  model_path = model_path,
  sim_path = sim_path,
  output_list = outputvars_lu,  # LU outputs are superset of PNV
  output_list_timestep = timesteps_lu
)

# Create timestep vector for methane runs (LU + methane output list)
timesteps_lu_methane <- ifelse(outputvars_lu_methane %in% monthly_vars, "monthly", "annual")

# Write configs for methane runs (include methane outputs)
configs_methane <- write_config(
  x = runs_methane,
  model_path = model_path,
  sim_path = sim_path,
  output_list = outputvars_lu_methane,  # LU + methane is the full set
  output_list_timestep = timesteps_lu_methane
)

# Combine configs
configs <- rbind(configs_no_methane, configs_methane)

# Filter configs to submit only the requested runs
if (!is.null(runs_to_submit)) {
  configs <- configs[configs$sim_name %in% runs_to_submit, ]
  message(paste("Submitting", nrow(configs), "simulation(s)"))
}

# =============================================================================
# Submit simulations
# =============================================================================

submit_args <- list(
  x = configs,
  model_path = model_path,
  sim_path = sim_path,
  ntasks = ntasks,
  blocking = blocking,
  sclass = qos
)

if (!is.null(wtime)) {
  submit_args$wtime <- wtime
}

submitted_runs <- do.call(submit_lpjml, submit_args)
