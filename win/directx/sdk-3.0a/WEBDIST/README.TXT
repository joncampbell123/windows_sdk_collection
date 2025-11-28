Microsoft DirectX Web Redistribution
====================================

This directory contains files for web distribution.  Please read
license.txt for more information.


Files
=====
directx.exe - cabpack EXE containing DirectX subsystem.  (no drivers).
directx.cab - cabpack CAB containing DirectX subsystem.  (no drivers).
directx.zip - Contains DirectX Subsystem and Drivers redist directory
              compressed with PKZip
license.txt - License Agreement


directx.exe
===========
The command line arguments to install this are:

"directx.exe"

If you want it to install with out any prompting and minimal UI, the
command line arguments are:

"directx.exe /Q /R:N"


directx.cab
===========
This can be used with ActiveSetup.  It is very similar to the directx.exe,
but does not have wextract wrapper.  Currently, there is no way to install
this package.  Future releases of Microsoft Internet Explorer will be able
to install CAB files.


directx.zip
===========
To install this, first create a directory and then copy the directx.zip.
The command line arguments to install this are:

"pkunzip -d directx.zip"

NOTE: It is important to do the -d so that you uncompress the files
      and preserve the directory structure.
