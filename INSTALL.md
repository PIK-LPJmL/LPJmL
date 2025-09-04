# Installation of LPJmL C Version 5.9.16

## Overview

LPJmL is a dynamic global vegetation model. This document describes how to install LPJmL on your system. The code has been tested on the following platforms:
- **AIX**
- **Linux**
- **Mac OS X**
- **Windows** (using Cygwin or Microsoft C compiler)

---

## Directory Structure

After extracting the source files, the directory structure will look like this:

```plaintext
--lpjml
  |
  +-config        : OS- and compiler-specific Makefiles
  +-bin           : Directory for executables and shell scripts
  +-man           : Manual pages root directory
  | +-man1        : Manual pages for programs and scripts
  | +-man3        : Manual pages for functions
  | +-man5        : Manual pages for file formats
  +-par           : Parameter files for PFTs and soils
  +-output        : Output data (created by `make test`)
  +-restart       : Restart data (created by `make test`)
  +-lib           : Library files (created by `make`)
  +-include       : Include files
  +-src           : Source tree
     +-numeric    : Source for numerical routines
     +-tools      : Source for tools
     +-pnet       : Library for distributed networks
     +-climate    : Climate code
     +-lpj        : LPJ functions
     +-soil       : Soil code
     +-grass      : Grass PFT code
     +-tree       : Tree PFT code
     +-crop       : Crop PFT code
     +-landuse    : Land use code
     +-spitfire   : Spitfire fire code
     +-reservoir  : Reservoir code
     +-socket     : Socket communication library
     +-coupler    : Coupler code
     +-image      : Coupler to IMAGE model
     +-netcdf     : NetCDF input/output functions
     +-utils      : Utility programs
     +-test       : Unit tests for source code using Ceedling
```

---

## Installation Steps

### 1. Extract the Source Files

```bash
mkdir lpjml
cd lpjml
gzip -cd lpjml-5.9.16.tar.gz | tar -xf -
```

---

### 2. Configure for Your Operating System

#### On the New PIK Cluster

Set the following modules:

```bash
module use /p/system/modulefiles/compiler \
               /p/system/modulefiles/gpu \
               /p/system/modulefiles/libraries \
               /p/system/modulefiles/parallel \
               /p/system/modulefiles/tools

module load intel/oneAPI/2024.0.0
module load udunits/2.2.28
module load json-c/0.17
module load openssl/3.0.12
module load netcdf-c
module load curl/8.4.0
module load expat/2.5.0
```

#### On a Linux System

Install the required libraries:

```bash
sudo apt-get install libnetcdf-dev
sudo apt-get install libudunits2-dev
sudo apt-get install libjson-c-dev
sudo apt-get install mpich  # Optional: for parallel library
```

---

### 3. Run the Configuration Script

```bash
./configure.sh
```

- If the script exits with the message `Unsupported operating system`, a `Makefile.$osname` is created from `Makefile.gcc`. You may need to modify it for your operating system/compiler.
- If an MPI environment is detected, a parallel version of LPJmL will be built.

---

### 4. Compilation

Compile the code using:

```bash
make
```

This creates the following executables in the `bin` directory:
- `lpjml`: LPJmL simulation code
- `lpjcheck`: Utility to check JSON configuration files

To compile all utility programs:

```bash
make utils
```

---

### 5. Installation

To install the binaries and necessary files to a specific directory, set the `LPJROOT` environment variable:

```bash
export LPJROOT=<installation directory>
make install
```

---

## Compilation Flags

Compilation of LPJmL can be customized using the following flags in the `LPJFLAGS` section of `Makefile.inc`:

| Flag                | Description                                      |
|---------------------|--------------------------------------------------|
| `CHECK_BOUNDARY`    | Check array boundaries of output                 |
| `COUPLING_WITH_FMS` | Enable coupling to FMS                           |
| `DAILY_ESTABLISHMENT` | Enable daily establishment                     |
| `DEBUG`             | Generate diagnostic output for debugging         |
| `USE_MPI`           | Compile parallel version of LPJmL               |
| `USE_NETCDF`        | Enable NetCDF input/output                       |
| `USE_UDUNITS`       | Enable unit conversion in NetCDF files           |
| `SAFE`              | Compile with additional checks                  |

---

## Running the Code

### Sequential Version

Run the sequential version with:

```bash
./bin/lpjml lpjml_config.cjson
```

If running outside the root directory, set the `LPJROOT` environment variable:

```bash
export LPJROOT=$HOME/lpjml
export PATH=$PATH:$LPJROOT/bin
```

### Parallel Version

Submit a job using SLURM or LoadLeveler:

```bash
bin/lpjsubmit ntasks LPJmL_args...
```

Or run interactively:

```bash
mpirun -np 32 ./bin/lpjml lpjml_config.cjson
```

---

## Error Codes

If LPJmL fails, an error message is displayed in the format:

```plaintext
ERRORxxx: message
```

| Error Code | Description                                   | Type       |
|------------|-----------------------------------------------|------------|
| 1          | Error reading configuration                   | External   |
| 4          | Invalid carbon balance                        | Internal   |
| 9          | Error allocating memory                       | External   |
| 19         | Floating point error                          | Internal   |
| 38         | Invalid climate data                          | External   |

- **External Errors**: Caused by invalid or missing input files.
- **Internal Errors**: Caused by issues in the LPJmL code and may require debugging.

---

## Environment Variables

The following environment variables are used by LPJmL:

| Variable         | Description                                         |
|------------------|-----------------------------------------------------|
| `LPJROOT`        | Root directory for LPJmL                           |
| `LPJINPATH`      | Path appended to input filenames                   |
| `LPJOUTPATH`     | Path appended to output filenames                  |
| `LPJRESTARTPATH` | Path appended to restart filenames                 |

---

## Man Pages

Manual pages are located in the `$LPJROOT/man` directory. Add them to your `MANPATH`:

```bash
export MANPATH=$MANPATH:$LPJROOT/man
```

Use the `apropos` command to list related man pages:

```bash
apropos lpj
```

---

## Windows Installation (Without Cygwin)

For Windows, the code has been tested with the Microsoft C++ compiler and `nmake`. Install the Microsoft SDK and follow these steps:

1. Unzip the source and data files.
2. Configure the makefile:

   ```cmd
   configure.bat
   ```

3. Build the executable:

   ```cmd
   nmake
   ```

---

This document is now fully formatted in GitLab-flavored Markdown. You can copy and paste it directly into your `INSTALL.md` file.