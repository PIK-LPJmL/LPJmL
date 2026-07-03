# Installation of LPJmL C Version 6.1.3

## Overview

LPJmL is a dynamic global vegetation model. This document describes how to install LPJmL on your system. The code has been tested on the following platforms:
- **Linux**
- **Mac OS X**
- **Windows** (using Cygwin or Microsoft C compiler)

---

## Directory Structure

After extracting the source files, the directory structure will look like this:

```plaintext
--lpjml
  |
  +-config         : OS- and compiler-specific Makefiles
  +-bin            : Directory for executables and shell scripts
  +-man            : Manual pages root directory
  | +-man1         : Manual pages for programs and scripts
  | +-man3         : Manual pages for functions
  | +-man5         : Manual pages for file formats
  +-par            : Parameter files for PFTs and soils
  +-testcase_2cell : Mimimum 2-cell data set for test
  +-output         : Output data (created by `make test`)
  +-restart        : Restart data (created by `make test`)
  +-lib            : Library files (created by `make`)
  +-include        : Include files
  +-src            : Source tree
     +-numeric     : Source for numerical routines
     +-bstruct     : source for reading/writing restart files
     +-tools       : Source for tools
     +-pnet        : Library for distributed networks
     +-climate     : Climate code
     +-lpj         : LPJ functions
     +-soil        : Soil code
     +-grass       : Grass PFT code
     +-tree        : Tree PFT code
     +-crop        : Crop PFT code
     +-landuse     : Land use code
     +-spitfire    : Spitfire fire code
     +-reservoir   : Reservoir code
     +-socket      : Socket communication library
     +-coupler     : Coupler code
     +-image       : Coupler to IMAGE model
     +-netcdf      : NetCDF input/output functions
     +-utils       : Utility programs
     +-test        : Unit tests for source code using Ceedling
```

---

## Installation Steps

### 1. Extract the Source Files

```bash
mkdir lpjml
cd lpjml
gzip -cd lpjml-6.1.3.tar.gz | tar -xf -
```

---

### 2. Configure for Your Operating System

#### On PIK's HPC "Eunice Foote"

Set the following modules:

```bash
module load intel/oneAPI/2024.0.0
module load udunits/2.2.28
module load json-c/0.17
module load netcdf-c
```

#### On a Linux System

Install the required libraries:

```bash
sudo apt-get install libnetcdf-dev
sudo apt-get install libudunits2-dev
sudo apt-get install libjson-c-dev
```
and optionally the MPICH parallel library:
```bash
sudo apt-get install mpich
sudo apt-get install openmpi-bin openmpi-common libopenmpi-dev
```

---

### 3. Run the Configuration Script

```bash
./configure.sh
```

- If the script exits with the message `Unsupported operating system`, a `Makefile.$osname` is created from `Makefile.gcc`. You may need to modify it for your operating system/compiler.
- If a MPI environment is detected, a parallel version of LPJmL will be built.
- The configure script creates a copy of the following OS-specific makefiles from
directory config:

|File            | Description                              |
|----------------|------------------------------------------|
|Makefile.gcc    | GNU C-compiler settings                  |
|Makefile.icx    | New Intel C-compiler (icx) settings      |
|Makefile.icc    | Intel C-compiler (icc) settings          |
|Makefile.mpiicc | Intel C-compiler icc and Intel MPI       |
|Makefile.mpiicx | Intel C-compiler icx and Intel MPI       |
|Makefile.mpich  | GNU C-Compiler and MPI Chameleon settings|
|Makefile.win32  | Windows settings (used by configure.bat) |

---

### 4. Compilation

Compile the code using:

```bash
make
```
This creates the `lpjml` executable in the `bin` directory.

```bash
make lpjcheck
```
creates `lpjcheck` utility to check JSON configuration files.

To compile all utility programs:

```bash
make utils
```
The following programs are created in the bin directory:

| Program       | Description                                                         |
|---------------|---------------------------------------------------------------------|
|`adddrain`     | add river basin to coordinate file                                  |
|`addheader`    | add CLM header to binary file                                       |
|`arr2clm`      | convert 2-D float arrays into CLM file                              |
|`asc2clm`      | convert ARC/Info ASCII files to CLM data files for LPJmL            |
|`backtrace`    | print backtrace from core                                           |
|`bin2cdf`      | convert binary output files into NetCDF files                       |
|`binsum`       | aggregate daily and monthly binary output                           |
|`cat2bsq`      | concatenate output files from distributed LPJmL simulations         |
|`catclm`       | concatenate LPJmL climate data files                                |
|`cdf2bin`      | convert NetCDF files into raw binary data                           |
|`cdf2clm`      | convert NetCDF files into CLM files                                 |
|`cdf2coord`    | extract CLM grid file from NetCDF file                              |
|`cdf2grid`     | convert grid NetCDF file into grid CLM or raw file                  |
|`cdf2soil`     | convert NetCDF file into binary file                                |
|`cfts26lu2clm` | convert binary landuse data files to CLM data files for LPJmL       |
|`clm2cdf`      | convert CLM files into NetCDF files                                 |
|`cmpbin`       | compare two binary output files                                     |
|`copyheader`   | copy CLM file header                                                |
|`country2cdf`  | convert country/region code file into NetCDF file                   |
|`coupler_demo` | Demo program for coupling to a program witch socket communication   |
|`cru2clm`      | convert CRU data into file format suitable for LPJmL                |
|`cutclm`       | cut LPJmL climate data files                                        |
|`cvrtclm`      | convert version of CLM file to version 3 or 4                       |
|`cvrtsoil`     | convert text soil input data into binary data                       |
|`drainage2cdf` | convert CLM drainage file into NetCDF file                          |
|`getcellindex` | get cell index from latitude, longitude values                      |
|`getcountry`   | extract grid cells for specified countries                          |
|`grid2bsq`     | convert grid data file to CLM data files for LPJmL                  |
|`headersize`   | print header size of CLM files                                      |
|`json2restart` | convert JSON file into LPJmL restart file                           |
|`lpjcat`       | concatenate restart files from distributed LPJmL simulations.       |
|`lpjcheck`     | check syntax of LPJmL configuration files                           |
|`lpjfiles`     | print list of input/output files of LPJmL                           |
|`lpjprint`     | print contents of restart file of LPJmL model runs                  |
|`manage2js`    | convert management  `*.par` files to JSON files                     |
|`mathclm`      | perform mathematical operations with CLM files                      |
|`mergeclm`     | merge LPJmL climate data files                                      |
|`printclm`     | print contents of CLM files for LPJmL                               |
|`printdrain`   | print drainage map                                                  |
|`printglobal`  | print global values for raw binary output files                     |
|`printharvest` | print global harvested carbon for crops and managed grasslands      |
|`printreservoir` | print reservoir data                                              |
|`regridclm`    | regrid CLM file to new grid                                         |
|`regridlpj`    | regrid input data set for selected countries or specified grid file |
|`regridsoil`   | regrid binary soil file to different grid file                      |
|`reservoir2cdf`| convert CLM reservoir file into NetCDF file                         |
|`restart2yaml` | convert LPJmL restart file into YAML                                |
|`river_section_input_climate`     | regrid climate data                              |
|`river_section_input_countrycode` | reggrid country code data                        |
|`river_section_input_grid`        | re-orders grid cells according to river sections |
|`river_section_input_irrig`       | regrid irrigation data                           |
|`river_section_input_landuse`     | regrid land-use data                             |
|`river_section_input_soil`        | regrid soil data                                 |
|`setclm`       | set value in header of CLM files for LPJmL                          |
|`soil2cdf`     | convert binary files into NetCDF files                              |
|`splitclm`     | copy specific bands of clm data files                               |
|`statclm`      | print statistics of clm files                                       |
|`txt2clm`      | convert text files to CLM data files for LPJmL                      |
|`txt2grid`     | convert text files to CLM grid data files for LPJmL                 |
|`water_use_input` | create water use data from industrial, domestic, and livestock use |

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

| Flag                | Description                                                 |
|---------------------|-------------------------------------------------------------|
|`CHECK_BOUNDARY`     |check array boundaries of output                             |
|`CHECK_BALANCE`      |enable localy function balance checks                        |
|`COUPLING_WITH_FMS`  |enable coupling to FMS                                       |
|`DAILY_ESTABLISHMENT`|enable daily establishment                                   |
|`DEBUG`              |diagnostic output is generated for debugging purposes        |
|`DEBUG_BSTRUCT`      |diagnostic output is generated for reading/writing of restart|
|`DEBUG_COUPLER`      |diagnostic output is generated for the coupler               |
|`DEBUG_IMAGE`        |diagnostic output is generated for the IMAGE coupler         |
|`DEBUG_N`            |diagnostic output is generated for the nitrogen cycle        |
|`IMAGE`              |include coupler to IMAGE model                               |
|`LINEAR_DECAY`       |use linearized functions for litter decay                    |
|`MICRO_HEATING`      |enable microbial heating                                     |
|`NRECOVERY_COST`     |costs for N recovery enabled                                 |
|`PERMUTE`            |random permutation of PFT list                               |
|`SAFE`               |code is compiled with additional checks                      |
|`STRICT_JSON`        |strict JSON checking                                         |
|`USE_MPI`            |compile parallel version of LPJmL                            |
|`USE_NETCDF`         |enable NetCDF input/output                                   |
|`USE_RAND48`         |use drand48() random number generator                        |
|`USE_UDUNITS`        |enable unit conversion in NetCDF files                       |
|`USE_TIMING`         |enable timing for LPJmL functions for performace analysis    |
|`WITH_FIRE_MOISTURE` |enable moisture dependent fire emissions                     |
|`WITH_FPE`           |floating point exceptions are enabled for debugging purposes |

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

Peform a minimum 2-cell simulation:

make test

### Parallel Version

Submit a job using SLURM or LoadLeveler:

```bash
bin/lpjsubmit ntasks LPJmL_args...
```

Or run interactively:

```bash
mpirun -np 32 ./bin/lpjml lpjml_config.cjson
```
### Runtime Options of Lpjml

The following runtime options are defined:
| Option               | Description                                                                          |
|----------------------|--------------------------------------------------------------------------------------|
| `-Iincludepath`      | add include path for LPJmL configuration file                                        |
| `-Dmacro[=value]`    | define macro                                                                         |
| `-h`                 | print usage of lpjml                                                                 |
| `-l`                 | print license                                                                        |
| `-v`                 | print compiler used and LPJmL flags set                                              |
| `-vv`                | verbosely print the actual values during reading of the configuration files          |
| `-pedantic`          | stop on warnings                                                                     |
| `-print_noread`      | print variable names not read from restart file                                      |
| `-ofiles`            | list only all available output variables                                             |
| `-param`             | print LPJmL parameter for soils and PFTs                                             |
| `-nopp`              | disable preprocessing                                                                |
| `-pp cmd`            | set preprocessor program. Default is 'cpp'                                           |
| `-fpe`               | enable floating point exceptions                                                     |
| `-image host`        | set host where IMAGE model is running. Option is only available for the IMAGE version|
| `-couple host[:port]`| set host and port where the coupled model is running                                 |
| `-wait time`         | set time to wait for connection to IMAGE/coupled model                               |
| `-inpath dir`        | input directory path                                                                 |
| `-outpath dir`       | output directory path                                                                |
| `-restartpath dir`   | restart directory path                                                               |

---

## Error Codes

If LPJmL fails, an error message is displayed in the format:

```plaintext
ERRORxxx: message
```

| Error Code | Description                                   | Type       |
|------------|-----------------------------------------------|------------|
|        1   |Error reading configuration                    | External   |
|        2   |Error initializing input data                  | External   |
|        3   |Error initializing grid                        | External   |
|        4   |Invalid carbon balance                         | Internal   |
|        5   |Invalid water balance                          | Internal   |
|        6   |Negative discharge                             | Internal   |
|        7   |Negative fire probability                      | Internal   |
|        8   |Negative soil moisture                         | Internal   |
|        9   |Error allocating memory                        | External   |
|       10   |Negative stand fraction                        | Internal   |
|       11   |Stand fraction sum error                       | Internal   |
|       12   |List is empty in dellistitem                   | Internal   |
|       13   |Index out of range in dellistitem              | Internal   |
|       15   |Invalid year in getco2()                       | External   |
|       16   |Crop fraction >1                               | Internal   |
|       17   |No natural stand for deforest                  | Internal   |
|       18   |Wrong cultivation type                         | Internal   |
|       19   |Floating point error                           | Internal   |
|       21   |PFT list is not empty in setaside              | Internal   |
|       22   |Negative establishment rate                    | Internal   |
|       23   |Output channel is broken in socket connection  | External   |
|       24   |Sending data to IMAGE model failed             | External   |
|       25   |Opening connection to IMAGE model failed       | External   |
|       26   |Not enough setaside stand created              | Internal   |
|       27   |Forest left after deforestation                | Internal   |
|       28   |Outflow reservoir error                        | Internal   |
|       29   |Permafrost error                               | Internal   |
|       30   |Global waterbalance error                      | Internal   |
|       31   |Cannot store climate data                      | External   |
|       32   |NO FMS coupler                                 | External   |
|       33   |Cannot initialize soil temperature             | External   |
|       34   |Invalid radiation module                       | Internal   |
|       35   |Negative soil NO3                              | Internal   |
|       36   |Negative soil NH4                              | Internal   |
|       37   |Invalid nitrogen balance                       | Internal   |
|       38   |Invalid climate data                           | External   |
|       39   |Invalid FPC value                              | Internal   |
|       40   |Boundary check error                           | Internal   |
|       41   |Invalid time step in soiltemp()                | Internal   |
|       42   |Cannot read external flow                      | External   |
|       43   |Opening connection to coupled model failed     | External   |
|       44   |Invalid uniform temp sign value                | Internal   |
|       45   |Error initializing output data                 | External   |
|       46   |Invalid crop PHU option                        | Internal   |
|       47   |Invalid fire danger index                      | Internal   |
|       48   |Gamma function error                           | Internal   |
|       49   |Wetland error                                  | Internal   |
|       50   |Invalid `hag_beta` error                       | External   |
|       51   |Invalid inundation stress                      | Internal   |
|       52   |Error writing output                           | External   |
|       53   |Negative soil carbon error                     | Internal   |


- **External Errors**: Caused by invalid or missing input files.
- **Internal Errors**: Caused by issues in the LPJmL code and may require debugging.

---

## Environment Variables

The following environment variables are used by LPJmL:

| Variable         | Description                                                                                                 |
|------------------|-------------------------------------------------------------------------------------------------------------|
| `LPPJNOPP`       | disables preprocessor if set to "true", same as -nopp option                                                |
| `LPJPEDANTIC`    | enables pedantic mode if set to "true", same as -pedantic option                                            |
| `LPJPREP`        | defines preprocessor command for LPJmL configuration  file, default is "cpp"                                |
| `LPJROOT`        | defines the root directory for LPJmL. This directory is added to the include directories of the preprocessor|
| `LPJIMAGE`       | sets host where IMAGE is running                                                                            |
| `LPJCOUPLEDHOST` | sets host where the coupled model is running                                                                |
| `LPJWAIT`        | sets time to wait for connection to IMAGE/coupled model                                                     |
| `LPJINPATH`      | path appended to the input filenames. Only done for filenames without absolute path                         |
| `LPJRESTARTPATH` | path appended to the restart filenames. Only done for filenames without absolute path                       |
| `LPJOPTIONS`     | cpp runtime options for LPJmL                                                                               |
| `LPJOUTPATH`     | path appended to the output filenames. Only done for filenames without absolute path                        |

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
