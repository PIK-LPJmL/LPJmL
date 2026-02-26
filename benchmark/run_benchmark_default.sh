#!/bin/bash
################################################################################
# Wrapper script to run benchmark_default.R with proper module environment
#
# Usage:
#   ./run_benchmark_default.sh baseline_sim_path="/path/to/baseline" [other args...]
#
# All command line arguments are forwarded to the R script.
#
# (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file
# authors, and contributors see AUTHORS file
# This file is part of LPJmL and licensed under GNU AGPL Version 3
# or later. See LICENSE file or go to http://www.gnu.org/licenses/
# Contact: https://github.com/PIK-LPJmL/LPJmL
################################################################################

# Purge all modules to start from clean environment
module purge

# Load required modules
source /p/system/modulefiles/defaults/piam/module_load_piam
module load intel/oneAPI/2024.2.1

# Run the R script with all forwarded arguments
Rscript "$(dirname "$0")/benchmark_default.R" "$@"
