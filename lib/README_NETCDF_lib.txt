Visual Studio file LPJ.sln can build a 32- or a 64-bit target and expects
the corresponding netcdf binaries for Windows in this directory. If this 
directory is empty, building LPJ.exe will end with an LINK error:
LINK : fatal error LNK1181: cannot open input file 'netcdf.lib'

NetCDF binaries for Windows are not to be checked in on the LPJmL-repository 
of PIK. You can find them on the internet or, if you are working at PBL, on the 
PBL lpj_devenv repository:

https://pbl.sliksvn.com/lpj_devenv/lib/netcdf/netcdf_4.3.3.1_32bit
https://pbl.sliksvn.com/lpj_devenv/lib/netcdf/netcdf_4.4.1_64bit

After a fresh check-out, select directory netcdf_4.3.1.1_32bit and/or netcdf_4.4.1_64bit, 
right mouse button -> TortoiseSVN -> Export
and export the contents of 
https://pbl.sliksvn.com/lpj_devenv/lib/netcdf/netcdf_4.3.3.1_32bit. 
and/or
https://pbl.sliksvn.com/lpj_devenv/lib/netcdf/netcdf_4.4.1_64bit. 
to the directory selected.
