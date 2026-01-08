# Standardized LPJmL benchmark comparison using lpjmlstats
#
# This script automatically detects and runs benchmarks for all transient
# simulations in the baseline directory (directories starting with
# "transient"). It compares baseline and under-test versions.
#
# Usage example:
# Rscript benchmark_default.R baseline_sim_path="/path/to/baseline" \
#   under_test_sim_paths="/path/to/test1"
# Rscript benchmark_default.R baseline_sim_path="/path/to/baseline" \
#   under_test_sim_paths="/path/to/test1,/path/to/test2"
#
# Arguments (use name=value format):
#   baseline_sim_path    : Path to baseline simulation results (required)
#   under_test_sim_paths : Comma-separated paths to test simulation results
#                          (required)

library(lpjmlkit)
library(lpjmlstats)

# Parse command line arguments
args <- commandArgs(trailingOnly = TRUE)

# Function to parse named arguments
parse_args <- function(args) {
  parsed <- list(
    baseline_sim_path = NULL,
    under_test_sim_paths = NULL
  )

  for (arg in args) {
    if (grepl("=", arg)) {
      parts <- strsplit(arg, "=", fixed = TRUE)[[1]]
      key <- parts[1]
      value <- parts[2]

      if (key %in% names(parsed)) {
        parsed[[key]] <- value
      } else {
        warning(paste("Unknown argument:", key))
      }
    }
  }

  parsed
}

# Parse arguments
parsed_args <- parse_args(args)

# Check if required arguments are provided
if (is.null(parsed_args$baseline_sim_path) ||
      is.null(parsed_args$under_test_sim_paths)) {
  stop(paste(
    "Usage: Rscript benchmark_default.R baseline_sim_path=<path>",
    "under_test_sim_paths=<path1[,path2,...]>\n",
    "  baseline_sim_path    : Path to baseline simulation directory",
    "(required)\n",
    "  under_test_sim_paths : Comma-separated paths to test simulation",
    "directories (required)\n"
  ))
}

baseline_sim_path <- parsed_args$baseline_sim_path
under_test_sim_paths <- strsplit(parsed_args$under_test_sim_paths, ",",
                                 fixed = TRUE)[[1]]

# Function to extract version from path
extract_version <- function(path) {
  # Remove trailing slash if present
  path <- sub("/$", "", path)
  # Get the last directory name
  basename(path)
}

# Extract versions
baseline_version <- extract_version(baseline_sim_path)
under_test_versions <- sapply(under_test_sim_paths, extract_version)

# Get current user
author <- Sys.info()["user"]

# Function to get transient simulation directories from a path
get_transient_sims <- function(path) {
  all_dirs <- list.dirs(path, full.names = FALSE, recursive = FALSE)
  transient <- all_dirs[grepl("^transient", all_dirs)]
  sort(transient)
}

# Detect transient simulations from baseline directory structure
baseline_transient_sims <- get_transient_sims(baseline_sim_path)

# Check if any transient simulations were found in baseline
if (length(baseline_transient_sims) == 0) {
  stop(paste(
    "No transient simulation directories found in baseline path:",
    baseline_sim_path, "\n",
    "Expected directories starting with 'transient_'\n"
  ))
}

# Detect transient simulations from all under-test directories
under_test_transient_sims_list <- lapply(under_test_sim_paths,
                                         get_transient_sims)

# Find common simulations across all paths
transient_sims <- baseline_transient_sims
for (i in seq_along(under_test_transient_sims_list)) {
  transient_sims <- intersect(transient_sims,
                              under_test_transient_sims_list[[i]])
}

# Check if there are any common simulations
if (length(transient_sims) == 0) {
  stop(paste(
    "No common transient simulation directories found across all paths.\n",
    "Baseline contains:",
    paste(baseline_transient_sims, collapse = ", "), "\n",
    "Under-test paths contain different sets of simulations.\n"
  ))
}

# Check for differences and warn if directories don't match
all_sims <- unique(c(baseline_transient_sims,
                     unlist(under_test_transient_sims_list)))
missing_somewhere <- setdiff(all_sims, transient_sims)

if (length(missing_somewhere) > 0) {
  cat("WARNING: Not all simulation directories match across all paths!\n")
  cat("=========================================================\n")

  # Check which are missing from baseline
  missing_in_baseline <- setdiff(unlist(under_test_transient_sims_list),
                                 baseline_transient_sims)
  if (length(missing_in_baseline) > 0) {
    cat("Missing in BASELINE (", baseline_version, "):\n  ",
        paste(missing_in_baseline, collapse = ", "), "\n", sep = "")
  }

  # Check which are missing from each under-test path
  for (i in seq_along(under_test_sim_paths)) {
    missing_in_test <- setdiff(baseline_transient_sims,
                               under_test_transient_sims_list[[i]])
    if (length(missing_in_test) > 0) {
      cat("Missing in UNDER-TEST (", under_test_versions[i], "):\n  ",
          paste(missing_in_test, collapse = ", "), "\n", sep = "")
    }
  }

  cat("\nOnly benchmarking COMMON simulations:\n  ",
      paste(transient_sims, collapse = ", "), "\n", sep = "")
  cat("=========================================================\n\n")
}

# Print summary
cat("Running benchmarks for:\n")
cat("  Baseline version:    ", baseline_version, "\n")
cat("  Under test versions: ", paste(under_test_versions,
                                     collapse = ", "), "\n")
cat("  Author:              ", author, "\n")
cat("  Common sims:         ", paste(transient_sims, collapse = ", "),
    "\n")
cat("  Number of sims:      ", length(transient_sims), "\n\n")

# Run benchmark for each transient simulation
benchmark_results <- list()

for (sim_name in transient_sims) {
  cat("===================================================\n")
  cat("Running benchmark for:", sim_name, "\n")
  cat("===================================================\n")

  # Construct baseline directory
  baseline_dir <- file.path(baseline_sim_path, sim_name)

  # Construct under-test directories
  under_test_dirs <- file.path(under_test_sim_paths, sim_name)

  # Generate description
  if (length(under_test_versions) == 1) {
    description <- paste("Benchmark comparing", baseline_version, "with",
                         under_test_versions[1], "for", sim_name)
  } else {
    description <- paste("Benchmark comparing", baseline_version, "with",
                         paste(under_test_versions, collapse = ", "),
                         "for", sim_name)
  }

  # Generate output file name
  if (length(under_test_versions) == 1) {
    output_file <- paste0("benchmark_", sim_name, "_", baseline_version,
                          "_vs_", under_test_versions[1], ".pdf")
  } else {
    output_file <- paste0("benchmark_", sim_name, "_", baseline_version,
                          "_vs_multiple.pdf")
  }

  cat("  Baseline:    ", baseline_dir, "\n")
  cat("  Under test:  ", paste(under_test_dirs, collapse = ", "), "\n")
  cat("  Description: ", description, "\n")
  cat("  Output file: ", output_file, "\n\n")

  # Run benchmark
  tryCatch({
    bm_result <- benchmark(
      baseline_dir = baseline_dir,
      under_test_dirs = under_test_dirs,
      author = author,
      description = description,
      output_file = output_file,
      pdf_report = TRUE
    )
    benchmark_results[[sim_name]] <- bm_result
    cat("Successfully completed benchmark for", sim_name, "\n\n")
  }, error = function(e) {
    cat("ERROR in benchmark for", sim_name, ":", conditionMessage(e),
        "\n\n")
    benchmark_results[[sim_name]] <- NULL
  })
}

# Print final summary
cat("===================================================\n")
cat("BENCHMARK SUMMARY\n")
cat("===================================================\n")
for (sim_name in transient_sims) {
  if (!is.null(benchmark_results[[sim_name]])) {
    cat("[SUCCESS]", sim_name, "\n")
  } else {
    cat("[FAILED] ", sim_name, "\n")
  }
}
cat("\nAll benchmarks completed.\n")
