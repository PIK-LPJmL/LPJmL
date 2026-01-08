# Standardized LPJmL benchmark comparison using lpjmlstats
#
# Usage example:
# Rscript benchmark_default.R baseline_dir="/path/to/baseline" \
#   under_test_dirs="/path/to/test1,/path/to/test2"
# Rscript benchmark_default.R baseline_dir="/path/to/baseline" \
#   under_test_dirs="/path/to/test1" output_file="my_benchmark.pdf"
#
# Arguments (use name=value format):
#   baseline_dir     : Path to baseline simulation results (required)
#   under_test_dirs  : Comma-separated paths to test simulation results
#                      (required)
#   output_file      : Name of output PDF file (optional, auto-generated
#                      if not provided)

library(lpjmlkit)
library(lpjmlstats)

# Parse command line arguments
args <- commandArgs(trailingOnly = TRUE)

# Function to parse named arguments
parse_args <- function(args) {
  parsed <- list(
    baseline_dir = NULL,
    under_test_dirs = NULL,
    output_file = NULL
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
if (is.null(parsed_args$baseline_dir) ||
      is.null(parsed_args$under_test_dirs)) {
  stop(paste(
    "Usage: Rscript benchmark_default.R baseline_dir=<path>",
    "under_test_dirs=<path1[,path2,...]> [output_file=<name>]\n",
    "  baseline_dir    : Path to baseline simulation directory",
    "(required)\n",
    "  under_test_dirs : Comma-separated paths to test simulation",
    "directories (required)\n",
    "  output_file     : Name of output PDF file (optional,",
    "auto-generated if not provided)\n"
  ))
}

baseline_dir <- parsed_args$baseline_dir
under_test_dirs <- strsplit(parsed_args$under_test_dirs, ",", fixed = TRUE)[[1]]

# Function to extract version from path
extract_version <- function(path) {
  # Remove trailing slash if present
  path <- sub("/$", "", path)
  # Get the last directory name
  basename(path)
}

# Extract versions
baseline_version <- extract_version(baseline_dir)
under_test_versions <- sapply(under_test_dirs, extract_version)

# Get current user
author <- Sys.info()["user"]

# Generate description
if (length(under_test_versions) == 1) {
  description <- paste("Benchmark comparing", baseline_version, "with",
                       under_test_versions[1])
} else {
  description <- paste("Benchmark comparing", baseline_version, "with",
                       paste(under_test_versions, collapse = ", "))
}

# Generate output file name if not provided
if (is.null(parsed_args$output_file)) {
  if (length(under_test_versions) == 1) {
    output_file <- paste0("benchmark_", baseline_version, "_vs_",
                          under_test_versions[1], ".pdf")
  } else {
    output_file <- paste0("benchmark_", baseline_version,
                          "_vs_multiple.pdf")
  }
} else {
  output_file <- parsed_args$output_file
}

# Print summary
cat("Running benchmark with:\n")
cat("  Baseline:    ", baseline_dir, "(", baseline_version, ")\n")
cat("  Under test:  ", paste(under_test_dirs, collapse = ", "), "\n")
cat("               ", paste(paste0("(", under_test_versions, ")"),
                             collapse = ", "), "\n")
cat("  Author:      ", author, "\n")
cat("  Description: ", description, "\n")
cat("  Output file: ", output_file, "\n\n")

# Run benchmark
bm_lu <- benchmark(
  baseline_dir = baseline_dir,
  under_test_dirs = under_test_dirs,
  author = author,
  description = description,
  output_file = output_file,
  pdf_report = TRUE
)
