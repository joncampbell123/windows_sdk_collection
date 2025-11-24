MDGCHGO v1.00.01
----------------
Madge Networks Windows 95 (M6) NDIS 3.1 Driver Release Notes (8 July 1994)
-----------------------------------------------------------------------

This disk contains the files required to install MDGCHGO, the Madge
Networks Windows 95 (M7, Beta-2) NDIS 3.1 driver for Madge Smart token 
ring adapters.

IMPORTANT NOTICE
----------------

The driver supplied on this disk is for demonstration purposes only, and 
is supplied 'as is'. Madge cannot guarantee correct functionality of this
driver or provide support for any failings. 

This driver is only for use with Windows 95 release M7 (Beta-2) dated 
October 1994.

INSTALLATION
------------

To install the driver place this disk in a floppy drive, log onto that
drive and type the command line:

install <windows-path>

where <windows-path> is the path to the location where Windows 95 is
installed. For example:

install c:\windows

if Windows 95 is installed in c:\windows. Once this installation process has
completed, Windows 95 can be set up for a Madge Smart token ring adapter
by using the normal Windows 95 network set up utilities. That is, open the
Control Panel and then open Networks. Choose Adapter and then Add. From
the list of manufacturers select Madge. Finally choose the type of 
Madge Smart token ring adapter in your machine.

When Windows 95 is completing the driver setup it will ask for the location
of the MDGCHGO.VXD driver file. You should enter the drive identifier 
of the drive containing this disk. For example if this disk is in floppy
drive A: then enter A:\

Should your driver fail to load, please make sure the following two files
are in your c:\windows\system directory: mdgchgo.vxd, mdgchgo.bin.  If they
are not, copy them from the directory this readme.txt file is in.

REMOVING THE DRIVER
-------------------

Please note that there is a problem with removing this evaluation version
of MDGCHGO if the adapter is operating. Therefore before trying to Delete
a Madge adapter from the Network setup utility please shutdown Windows 95,
remove the lobe cable from your Madge adapter and then re-start Windows 95.
This process with ensure that the Madge adapter does not operate and will
allow the driver to be removed properly.


Copyright (c) Madge Networks Ltd 1994



