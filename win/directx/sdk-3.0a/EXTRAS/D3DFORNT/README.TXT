This directory contains a beta release of Direct3D for NT.  Please note that
this is a beta release of Direct3D is being provided on the Win32 SDK for
development purposes only.  It may not be redistributed by ISVs under any
circumstance.

This implementation of D3D may only be installed on a machine running Windows
NT version 4.  The D3D for NT binaries will only work on final (non-beta) 
Windows NT version 4. In order to install it, copy the 14 DLLs into the 
%Systemroot%\system32 directory, and run "REGEDIT D3D.REG" from the
Start.Run dialog.

The PPC version of Direct3D is not included because we found a bug in it
which is specific to the PPC.  It will of course be fixed before Direct3D
is released in an NT service pack in the near future.

