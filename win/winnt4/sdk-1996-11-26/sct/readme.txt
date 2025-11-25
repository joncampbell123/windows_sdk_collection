
*****************
NOTE: This directory contains the Software Compatibility Tests for
16-bit applications on Windows NT.  For the Windows 95 System 
Compatibility tests, please see the Windows 95 DDK.
*****************

Three batch files are included to help you manage the 16-bit debugging
utilities:

INSTALL.BAT  - Installs the SCT files to your hard drive and sets up system
parameters to allow 16-bit debugging.  The system must be rebooted after this
script has completed for the system parameters to take effect. If no command
line parameters are used, the US SCT is installed. If "install Japan [pc98]"
is used the Japanese SCT is installed. There is not a Chinese or Korean 
specific SCT is on this CD.

NONDEBUG.BAT - Restores the system to RETAIL binaries and associated symbols for
the 16-bit subsystem.  If you have run setdebug.bat, you should run this before
doing any performance measurements.

SETDEBUG.BAT - Installs the DEBUG binaries and symbols for the 16-bit subsystem.

See the Software Compatibility Test documentation for more details.
