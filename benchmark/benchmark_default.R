# Standardized LPJmL benchmark comparison using lpjmlstats
#
# Modes:
# 1. Auto-detect: baseline_sim_path contains transient_* subdirectories
# 2. Direct: baseline_sim_path points to a simulation output directory
#
# Usage examples:
# Rscript benchmark_default.R baseline_sim_path="/path/to/baseline" \
#   under_test_sim_paths="/path/to/test1"
# Rscript benchmark_default.R baseline_sim_path="/path/to/baseline" \
#   under_test_sim_paths="/path/to/test1" \
#   runs_subset="transient_no_methane_pnv,transient_no_methane_lu"
# Rscript benchmark_default.R baseline_sim_path="/path/to/baseline/sim1" \
#   under_test_sim_paths="/path/to/test1/sim1" output_dir="/path/to/reports"
#
# Arguments:
#   baseline_sim_path    : Path to baseline simulation (required)
#   under_test_sim_paths : Comma-separated test paths (required)
#   runs_subset          : Comma-separated sim names (auto-detect mode only)
#   output_dir           : Directory for PDF reports (default: current directory)
#
# (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file
# authors, and contributors see AUTHORS file
# This file is part of LPJmL and licensed under GNU AGPL Version 3
# or later. See LICENSE file or go to http://www.gnu.org/licenses/
# Contact: https://github.com/PIK-LPJmL/LPJmL

library(lpjmlkit)
library(lpjmlstats)

# Source shared utilities
script_dir <- (function() {
  args <- commandArgs(trailingOnly = FALSE)
  file_arg <- grep("--file=", args, value = TRUE)
  if (length(file_arg) > 0) dirname(normalizePath(sub("--file=", "", file_arg)))
  else getwd()
})()
source(file.path(script_dir, "benchmark_utils.R"))

# =============================================================================
# Parse arguments
# =============================================================================

parsed_args <- parse_args(
  commandArgs(trailingOnly = TRUE),
  list(baseline_sim_path = NULL, under_test_sim_paths = NULL,
       runs_subset = NULL, output_dir = getwd(), time_avg_map = "false"),
  type_converters = list(time_avg_map = function(x) tolower(x) == "true")
)

if (is.null(parsed_args$baseline_sim_path) ||
    is.null(parsed_args$under_test_sim_paths)) {
  stop(paste(
    "Usage: Rscript benchmark_default.R baseline_sim_path=<path>",
    "under_test_sim_paths=<paths> [options]\n",
    "  baseline_sim_path    : Path to baseline simulation (required)\n",
    "  under_test_sim_paths : Comma-separated test paths (required)\n",
    "  runs_subset          : Comma-separated sim names (auto-detect only)\n",
    "  output_dir           : Directory for PDF reports (default: .)\n",
    "  time_avg_map         : Add TimeAvgMapWithAbs metric (true/false, default: false)\n"
  ))
}

baseline_sim_path <- parsed_args$baseline_sim_path
under_test_sim_paths <- strsplit(parsed_args$under_test_sim_paths, ",", fixed = TRUE)[[1]]
runs_subset <- parsed_args$runs_subset
output_dir <- parsed_args$output_dir
time_avg_map <- parsed_args$time_avg_map

# Create output directory if needed
if (!dir.exists(output_dir)) dir.create(output_dir, recursive = TRUE)

# =============================================================================
# Helper functions
# =============================================================================

# Extract version from path like "/path/to/6.0.3/output" -> "6.0.3"
# Looks for version pattern (vX.Y.Z or X.Y.Z) in path components
extract_version <- function(path) {
  path <- sub("/$", "", path)
  parts <- strsplit(path, "/")[[1]]
  # Look for version pattern like 6.0.3 or v6.0.3
  version_pattern <- "^v?[0-9]+\\.[0-9]+\\.[0-9]+$"
  version_parts <- grep(version_pattern, parts, value = TRUE)
  if (length(version_parts) > 0) {
    return(version_parts[length(version_parts)])  # Use last match
  }
  # Fallback to basename
  basename(path)
}

get_transient_sims <- function(path) {
 dirs <- list.dirs(path, full.names = FALSE, recursive = FALSE)
 sort(dirs[grepl("^transient", dirs)])
}

is_simulation_dir <- function(path) {
  dir.exists(path) && length(list.files(path, pattern = "\\.bin$|\\.nc$|\\.clm$")) > 0
}

# =============================================================================
# Detect mode and find simulations
# =============================================================================

baseline_version <- extract_version(baseline_sim_path)
under_test_versions <- sapply(under_test_sim_paths, extract_version)
author <- Sys.info()["user"]

baseline_is_direct <- is_simulation_dir(baseline_sim_path)
under_test_is_direct <- sapply(under_test_sim_paths, is_simulation_dir)

if (baseline_is_direct) {
  cat("Using DIRECT mode\n\n")
  sim_name <- if (all(under_test_is_direct)) {
    basename(under_test_sim_paths[1])
  } else {
    non_direct <- under_test_sim_paths[!under_test_is_direct]
    sims_list <- lapply(non_direct, get_transient_sims)
    Reduce(intersect, sims_list)[1]
  }
  if (is.na(sim_name)) stop("Cannot determine simulation name")
  transient_sims <- sim_name

} else {
  cat("Using AUTO-DETECT mode\n\n")
  baseline_sims <- get_transient_sims(baseline_sim_path)
  if (length(baseline_sims) == 0) {
    stop("No transient_* directories found in baseline path")
  }

  under_test_sims_list <- lapply(under_test_sim_paths, get_transient_sims)
  transient_sims <- Reduce(intersect, c(list(baseline_sims), under_test_sims_list))

  if (length(transient_sims) == 0) {
    stop("No common transient simulations found across all paths")
  }

  # Warn about missing simulations
  all_sims <- unique(c(baseline_sims, unlist(under_test_sims_list)))
  missing <- setdiff(all_sims, transient_sims)
  if (length(missing) > 0) {
    cat("WARNING: Some simulations missing in some paths\n")
    cat("Benchmarking only:", paste(transient_sims, collapse = ", "), "\n\n")
  }
}

# Apply runs_subset filter
if (!is.null(runs_subset)) {
  if (baseline_is_direct) {
    warning("runs_subset ignored in direct mode")
  } else {
    subset_values <- trimws(strsplit(runs_subset, ",")[[1]])
    invalid <- subset_values[!subset_values %in% transient_sims]
    if (length(invalid) > 0) {
      stop(paste("Invalid runs_subset:", paste(invalid, collapse = ", ")))
    }
    transient_sims <- transient_sims[transient_sims %in% subset_values]
    cat("Filtered to:", paste(transient_sims, collapse = ", "), "\n\n")
  }
}

# =============================================================================
# Print summary
# =============================================================================

cat("Running benchmarks:\n")
cat("  Baseline:    ", baseline_version, "\n")
cat("  Under test:  ", paste(under_test_versions, collapse = ", "), "\n")
cat("  Simulations: ", paste(transient_sims, collapse = ", "), "\n")
cat("  Output dir:  ", output_dir, "\n\n")

# =============================================================================
# Run benchmarks
# =============================================================================

benchmark_results <- list()

for (sim_name in transient_sims) {
  cat("===================================================\n")
  cat("Benchmarking:", sim_name, "\n")
  cat("===================================================\n")

  baseline_dir <- if (baseline_is_direct) baseline_sim_path
                  else file.path(baseline_sim_path, sim_name)

  under_test_dirs <- sapply(seq_along(under_test_sim_paths), function(i) {
    if (under_test_is_direct[i]) under_test_sim_paths[i]
    else file.path(under_test_sim_paths[i], sim_name)
  })

  description <- paste("Benchmark:", baseline_version, "vs",
                       paste(under_test_versions, collapse = ", "), "-", sim_name)
  # Replace dots with underscores in version strings for PDF filename
  baseline_ver_safe <- gsub("\\.", "_", baseline_version)
  under_test_ver_safe <- gsub("\\.", "_", under_test_versions)
  output_file <- file.path(output_dir, paste0(
    "benchmark_", sim_name, "_", baseline_ver_safe, "_vs_",
    if (length(under_test_ver_safe) == 1) under_test_ver_safe[1] else "multiple",
    ".pdf"
  ))

  run_type <- get_run_type(sim_name)
  bm_settings <- get_bm_settings(default_settings, run_type,
                                  time_avg_map = time_avg_map)

  cat("  Run type:", run_type, "\n")
  cat("  Output:  ", output_file, "\n\n")

  # Create named lists for custom names in benchmark report
  baseline_list <- setNames(list(baseline_dir), baseline_version)
  under_test_list <- setNames(as.list(under_test_dirs), under_test_versions)

  # Set metric options for TimeAvgMapWithAbs if enabled
  metric_opts <- if (time_avg_map) {
    list(TimeAvgMapWithAbs = list(n_breaks = 5))
  } else {
    NULL
  }

  tryCatch({
    benchmark_results[[sim_name]] <- benchmark(
      baseline_list,
      under_test_list,
      author = author,
      description = description,
      output_file = output_file,
      pdf_report = TRUE,
      settings = bm_settings,
      metric_options = metric_opts
    )
    cat("SUCCESS:", sim_name, "\n\n")
  }, error = function(e) {
    cat("ERROR:", sim_name, "-", conditionMessage(e), "\n\n")
    benchmark_results[[sim_name]] <<- NULL
  })
}

# =============================================================================
# Summary
# =============================================================================

cat("===================================================\n")
cat("SUMMARY\n")
cat("===================================================\n")
for (sim in transient_sims) {
  status <- if (!is.null(benchmark_results[[sim]])) "[SUCCESS]" else "[FAILED] "
  cat(status, sim, "\n")
}
cat("\nBenchmarks completed. Reports saved to:", output_dir, "\n")
