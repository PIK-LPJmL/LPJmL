#!/bin/bash
################################################################################
# Wrapper script to run simulate_default.R with proper module environment
#
# Usage:
#   ./run_simulate_default.sh sim_path="/path/to/results" [other args...]
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

# Add piam module path if not present
if [[ ":$MODULEPATH:" != *":/p/system/lenovo/ctt/modulefiles/defaults:"* ]]; then
    export MODULEPATH="$MODULEPATH:/p/system/lenovo/ctt/modulefiles/defaults"
fi

# Load required modules
module load piam/1.27
module load intel/oneAPI/2024.2.1

# Run the R script with all forwarded arguments
Rscript "$(dirname "$0")/simulate_default.R" "$@"
