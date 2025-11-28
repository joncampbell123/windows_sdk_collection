README.TXT for Windows 95 Release 1.51 for CS4232/36
====================================================

AUDIO DRIVERS
--------------------------

This CD contains advanced CrystalWare audio drivers from Crystal Semiconductor 
that support CS4232, CS4236, CS4236B, CS4237B, and CS4238B based systems. New 
releases of the drivers will support all of Crystal's innovative features, 
including 3D sound, symmetrical mixing, ISA Plug-and-Play, Advanced Power 
Management (APM), and Hardware Master Volume Control. 

The drivers included on this CD are release 1.51 and will be upgraded 
periodically. To obtain the current release of Crystal's drivers, as well as 
technical documentation and new product information, please visit our web site 
at 

http://www.crystal.com/

While drivers can be obtained directly from Crystal via the web site, technical 
support for your PC is provided by the manufacturer.

Crystal Semiconductor Corporation, headquartered in Austin, Texas, is a 
wholly-owned subsidiary of Cirrus Logic, Inc. Crystal designs, develops, and 
markets industry-leading mixed-signal solutions for applications including 
multimedia audio, consumer/professional audio, telecommunications, data 
acquisition, and networking.


To Enable Real DOS Mode With CS4232/36 Support:
-------------------------------------------------

1.  Select a DOS prompt icon.
2.  Select Properties.
3.  Go to the Program tab.
4.  Select the Advanced... button.
5.  Click MS-DOS Mode.
6.  Click "Specify a new MS-DOS configuration."
7.  The following line will be automatically placed into
    the new MS-DOS configuration (with right path):
        DEVICE=C:\WIN95\CS4232C.EXE /O /R

To enable a CS4232/36 audio card in a system that 
contains multiple CS4232/36 audio cards:
1.  Add the following option to the CS4232C.EXE 
    entry in the CONFIG.SYS.
    Device=C:\WINDOWS\CS4232C.EXE /Sxx
    where xx is the OEM ID number for the CS4232
    audio card to be enabled.
2.  The OEM ID number can be obtained from the 
    manufacturer or by adding a "/V" to the 
    CS4232C.EXE entry in the CONFIG.SYS:
    Device=C:\WINDOWS\CS4232C.EXE /V
