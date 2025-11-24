The Visual C++ 2.0 environment is no longer included in DDKENV.BAT.
To set the proper environment, the VC20 environment must first be 
set, followed by execution of DDKENV.BAT which will include any paths
(INCLUDE, LIB, PATH) which may have been previously set.

The BASE\VXDRES directory contains references to a tool (ADRC2VXD.EXE) 
for adding version resources to a virtual device. A couple of samples 
demonstrate the usage of this tool (COMM\SAMPLES\VCD and VPD). 

LINK.EXE and RC.EXE are tools that have both a 32-bit and a 16-bit
version. The LINK.EXE that is provided w/ MSVC20 (or later versions)
is a 32-bit tool. When building VxDs (32-bit) care must be
taken to not have an executable named LINK.EXE on the path ahead of
the MSVC20 LINK.EXE. Similarly, when adding version resources to a
VxD one must ensure that the 16-bit RC.EXE (from BINW16) is directly
referenced, as VxDs are built in a 32-bit environment (which typically 
contains a 32-bit RC.EXE).

Include and lib paths should be perused to ensure that they do not
contain invalid duplicate copies of include files and libraries that 
the application/driver may be including.


