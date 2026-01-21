# Standardized LPJmL benchmark runs using lpjmlkit
#
# Usage example:
# Rscript simulate_default.R sim_path="/path/to/results"
# Rscript simulate_default.R sim_path="/path/to/results" ntasks=256
# Rscript simulate_default.R sim_path="/path/to/results" ntasks=256 \
#   blocking=64 model_path="../bin/lpjml" qos="short"
# Rscript simulate_default.R sim_path="/path/to/results" \
#   runs_filter="transient_full_pnv,transient_full_lu" wtime="10:00:00"
# Rscript simulate_default.R sim_path="/path/to/results" \
#   runs_filter="1,3,5" wtime="10:00:00"
#
# Arguments (use name=value format):
#   sim_path    : Path for simulation results (required)
#   ntasks      : Number of parallel tasks (default: 512)
#   blocking    : Cores per task to block (default: 128, or "unlimited")
#   model_path  : Path to model directory with binary and configs (default: .)
#   qos         : Quality of service (optional, default: "short")
#   runs_filter : Comma-separated list of simulation names or
#                 indices (default: all)
#   wtime       : Wall time for all runs (default: NULL, uses lpjmlkit defaults)

library(lpjmlkit)

# Parse command line arguments
args <- commandArgs(trailingOnly = TRUE)

# Function to parse named arguments
parse_args <- function(args) {
  parsed <- list(
    sim_path = NULL,
    ntasks = 512,
    blocking = 128,
    model_path = getwd(),
    qos = "standby",
    runs_filter = NULL,
    wtime = NULL
  )

  for (arg in args) {
    if (grepl("=", arg)) {
      parts <- strsplit(arg, "=", fixed = TRUE)[[1]]
      key <- parts[1]
      value <- parts[2]

      if (key %in% names(parsed)) {
        if (key == "ntasks") {
          parsed[[key]] <- as.integer(value)
        } else if (key == "blocking") {
          # Handle blocking as either integer or "unlimited"
          if (tolower(value) == "unlimited") {
            parsed[[key]] <- "unlimited"
          } else {
            parsed[[key]] <- as.integer(value)
          }
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

# Parse arguments
parsed_args <- parse_args(args)

# Check if required argument is provided
if (is.null(parsed_args$sim_path)) {
  stop(paste(
    "Usage: Rscript simulate_default.R sim_path=<path>",
    "[ntasks=512] [blocking=128] [model_path=.] [runs_filter=<names>] ",
    "[wtime=<time>]\n",
    "  sim_path    : Path where simulation results will be stored",
    "(required)\n",
    "  ntasks      : Number of tasks for parallel execution",
    "(optional, default: 512)\n",
    "  blocking    : Number of cores per task to be blocked or 'unlimited'",
    "(optional, default: 128)\n",
    "  model_path  : Path to model directory with binary and configs ",
    "(optional, default: .)\n",
    "  runs_filter : Comma-separated list of simulation names or indices ",
    "(optional, default: all)\n",
    "  wtime       : Wall time for all runs (optional, default: ",
    "uses lpjmlkit defaults)"
  ))
}

sim_path <- parsed_args$sim_path
ntasks <- parsed_args$ntasks
blocking <- parsed_args$blocking
model_path <- parsed_args$model_path
qos <- parsed_args$qos
runs_filter <- parsed_args$runs_filter
wtime <- parsed_args$wtime

runs <- tibble::tibble(
  sim_name = c(
    "spinup_full", "spinup_reduced", "transient_full_pnv",
    "transient_reduced_pnv", "transient_full_lu",
    "transient_reduced_lu"
  ),
  dependency = c(
    NA, NA, "spinup_full", "spinup_reduced", "spinup_full",
    "spinup_reduced"
  ),
  percolation_heattransfer = c(NA, FALSE, NA, FALSE, NA, FALSE),
  landuse = c(NA, NA, "no", "no", NA, NA),
  wateruse = c(NA, NA, FALSE, FALSE, NA, NA)
)

# Store filtered run indices/names if filter is specified
runs_to_submit <- NULL
if (!is.null(runs_filter)) {
  filter_values <- strsplit(runs_filter, ",")[[1]]
  filter_values <- trimws(filter_values) # Remove whitespace

  # Check if filter values are numeric indices or names
  if (all(grepl("^[0-9]+$", filter_values))) {
    # Numeric indices
    filter_indices <- as.integer(filter_values)
    if (any(filter_indices < 1 | filter_indices > nrow(runs))) {
      stop(paste("Index out of range. Valid indices are 1 to", nrow(runs)))
    }
    runs_to_submit <- runs$sim_name[filter_indices]
    message(paste(
      "Will submit filtered simulations:",
      paste(runs_to_submit, collapse = ", ")
    ))
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
    message(paste(
      "Will submit filtered simulations:",
      paste(runs_to_submit, collapse = ", ")
    ))
  }
}

outputvars_benchmark <- c(
  "vegc", "soilc", "litc", "vegn", "soiln", "soilnh4",
  "soilno3", "leaching", "n_immo", "n_mineralization",
  "n_volatilization", "n2_emis", "n2o_denit", "n2o_nit",
  "nuptake", "bnf", "firec", "flux_estabc", "gpp", "npp",
  "nbp", "rh", "evap", "transp", "interc", "runoff",
  "fpc", "pft_harvestc"
)

# Write configs for all runs (needed for dependency resolution)
configs <- write_config(
  x = runs,
  model_path = model_path,
  sim_path = sim_path,
  output_list = outputvars_benchmark,
  output_list_timestep = "annual"
)

# Filter configs to submit only the requested runs
if (!is.null(runs_to_submit)) {
  configs <- configs[configs$sim_name %in% runs_to_submit, ]
  message(paste("Submitting", nrow(configs), "simulation(s)"))
}

# Build submit_lpjml arguments
submit_args <- list(
  x = configs,
  model_path = model_path,
  sim_path = sim_path,
  ntasks = ntasks,
  blocking = blocking,
  sclass = qos
)

# Add wtime if specified
if (!is.null(wtime)) {
  submit_args$wtime <- wtime
}

submitted_runs <- do.call(submit_lpjml, submit_args)
