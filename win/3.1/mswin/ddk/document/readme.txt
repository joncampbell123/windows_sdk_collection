********README.TXT for the Windows 3.1 Device Development Kit************

This file contains two sections:

Section A: Specific Information/workarounds to help build the
	   drivers provided with the Windows 3.1 DDK.
Section B: A Description of the Device Driver Sources Provided.


****SECTION A: Specific Information/Workarounds To Build Drivers*********

If WINDEV Directory Already Exists
**********************************
If you already have a WINDEV directory installed on your disk prior to
installing the DDK, we strongly recommend you use a different directory
to install the TOOLS, LIB and INCLUDE files provided with the DDK.

Running Out Of Memory
*********************
Because it can be very easy to run out of conventional memory when building
device drivers and virtual devices when using the NMAKE utility, we are
also providing the XMSMAKE utility which will additionally use extended
memory.  If you are encountering strange errors, you may be running out
of memory and we recommend trying XMSMAKE which is located in the \TOOLS
directory of the Device Drivers Samples & Tools Disks.

Building 386 Grabbers
*********************
You MUST use the LINK.EXE in the \TOOLS directory of the Device Driver Samples
and Tools Disk Set when building the 386 Grabbers.

Binary Compare of the PSCRIPT.DRV, SNDBLST2.DRV, and MCIPIONR.DRV Files
***********************************************************************
If you build either the PSCRIPT.DRV, SNDBLST2.DRV or MCIPIONR.DRV drivers, your
built driver may not exactly binary compare with the corresponding drivers
provided in the Windows 3.1 retail product.  The few differences noted are due
to different build environments used, machine and memory configurations, etc.
The driver you build will function identically to that provided in the retail
product.

Building  VDDVGA.386,VDD8514.386 Virtual Devices
************************************************
When executing the Makefiles MAKEVGA and MAKE8514, please note that the
created object files for both share the same object filename.  So if you
first create the VDDVGA.386 virtual device and then proceed to create the
VDD8514.386 device you must be sure to first delete all filenames with
the *.obj file extension.

Nmake Version
*************
If you want to build a driver that binary compares to the driver in the retail
version, you need to be sure to use the NMAKE.EXE tool provided in the \TOOLS
directory on the Device Driver Samples & Tools disk set.  If you use the one
provided with the C 6 Compiler, the driver may not binary compare but should
function identically.

****SECTION B: Description of Device Driver Sources**********************

Below are descriptions of device driver sources contained in the DDK.  The
information includes where you can find the code on the diskettes, the
procedure for building the driver and the features the code demonstrates.

NOTE: We recommend that you print out the text file "LAYOUTS.TXT" located
in the root directory of the Device Driver Samples and Tools Disk#1 for a
listing of the source directories and disk locations for the Samples and
Tools Disk Sets.

The sources are setup in sub-directories on the enclosed diskettes.  You
should use the DOS COPY or XCOPY command to copy all files and related
sub-directories to your hard disk.  It is important that you maintain the
structure as provided on the diskettes for the accompanying MAKE and batch
files to work properly.  Also be sure to expand the compressed files to
their normal size after copying them to your hard disk.

Display Driver Sources
**********************
We are providing 1, 4 and 8 plane display driver sources in the DDK.  You
will find all the 1 plane driver sources on the Device Driver Samples and
Tools Disk Sets  under \DISPLAY\1PLANE.  The 4 plane sources are  under
\DISPLAY\4PLANE.  The 8 plane sources are under \DISPLAY\8PLANE.

1 Plane Drivers - CGA, EGA mono, EGA high res B&W, Hercules, Plasma, and MCGA
*****************************************************************************
All of these drivers are 1 plane drivers.  These drivers run Bi-Modal (in Real
and Protected memory modes), support Device Independent Bitmaps (DIB), and
greater than 64K fonts. They are located on the Device Driver Samples and
Tools Disk Sets under \DISPLAY\1PLANE.

These drivers share common code in many areas.	Thus, the directory structures
should be copied in their entirety to your hard disk.  We suggest you use the
MS-DOS XCOPY command to do this.

You need to change directory to the appropriate sub-directory for the driver
you wish to build.  Each driver directory has a Makefile to build the driver.
You will find the Makefile for each driver under:

	\DISPLAY\1PLANE\BW\CGAHERC\CGA\		for CGA
	\DISPLAY\1PLANE\BW\CGAHERC\PLASMA\ 	for Compaq Plasma
	\DISPLAY\1PLANE\BW\CGAHERC\HERCULES\ 	for Hercules monochrome
	\DISPLAY\1PLANE\BW\EGA\EGAHIBW\		for EGA hi res black/white
	\DISPLAY\1PLANE\BW\EGA\EGAMONO\		for EGA monochrome
	\DISPLAY\1PLANE\BW\EGA\VGAMONO\		for MCGA

To build the driver, use the NMAKE utility provided with Microsoft C 6.
If your display device is based on the 1 plane CGA, EGA, or Hercules standard,
you should base your driver code on one of these drivers.  Otherwise, these
sources should provide you with the sample code necessary for you to design
and plan for a new driver.


4 Plane Drivers - EGA, VGA, and Super VGA
******************************************
The EGA, VGA, and Super VGA driver sources utilize all 4 planes to support 16
colors.  The Super VGA display driver is new for Windows 3.1.  It is nearly
identical to the VGA sources, but determines an appropriate extended resolution
mode number from the WIN.INI file.

These drivers run Bi-Modal (in Real and Protected memory modes), support Device 
Independent Bitmaps (DIB), and greater than 64K fonts.	 The  EGA, VGA, and
Super VGA driver sources utilize all 4 planes to support 16 colors.  All three
drivers demonstrate a new feature of Windows 3.1, "Mouse Blur" that leaves
"mouse trails" on the screen to improve cursor visibility on LCD screens.

These drivers share common code in many areas.  Thus, all the files in the 
\DISPLAY\4PLANE directory structure found on the Device Driver Samples and
Tools should be copied in their entirety to your hard disk.  We suggest you
use the MS-DOS XCOPY command to do this.  Be sure to expand the compressed
files to their original size before using them.

You need to change directory to the appropriate sub-directory for either
the EGA, VGA, or Super VGA driver and execute the MakeFile using the NMAKE
utility.  You will find a makefile file for each driver under:

	\DISPLAY\4PLANE\EGA\		for EGA hi res color
	\DISPLAY\4PLANE\SVGA\		for Super VGA
	\DISPLAY\4PLANE\VGA\		for  VGA

If your display device is based on the 4 plane EGA or VGA standard, you
should base your driver code on either of these drivers.  Otherwise, these
sources should provide you with the sample code necessary for you to design
and plan for a new driver.

4 Plane Drivers - Pen-Capable VGA
*********************************
To build Pen-Capable VGA display drivers, set the MS-DOS environment
variable PENWIN=1, delete all object files, and rebuild.  Note that this
procedure is documented in the VGA driver makefile.

8 Plane Driver - Video Seven VRAM, VGA, 8514/a
**********************************************
We have provided you sources to the Video Seven VRAM/FastWrite VGA and 8514/a  
drivers.  These drivers use all 8 planes available on the card for 256 color
support.  The code provides a good example of the use of the Palette Manager
and Device Independent Bitmaps (DIB).  This driver is adapted to run Bi-Modal
and supports >64K fonts.  This driver source can also be used as an example of
how a packed pixel driver is implemented under Windows.

The 8514/s driver provides a sample of how to support the TrueType font
technology with a display adapter that does its own font caching.  In addition,
this driver provides an example of Windows 3.1's multiple-resolution driver
feature, which allows a single driver executable to support more than one
screen resolution, or supply a variety of resolution-dependent resources
(e.g.. small or large fonts).

To demonstrate the use of DIBs and the Palette Manager in these  drivers, you
can run the sample application, SHOWDIB, provided in the SDK.

You will find these sources on the Device Driver Samples and Tools Disk Set
under the directory \8PLANE.  You need to copy the entire directory structure
to your hard disk.  Use the MS-DOS XCOPY command to do this.   Be sure to
expand the compressed files to their original size before using them.

Note, the 8PLANE directory contains an empty sub-directory called DEBUG.
You must have this directory available when you build the driver, otherwise
the MAKE files will fail.

To build the drivers, you need to run the batch file:
	
	\DISPLAY\8PLANE\V7VGA\		for Video 7 VRAM VGA
(NOTE:  This builds the driver file V731VGA.DRV.  This driver is renamed to V7VGA.DRV in Windows 3.1)
	\DISPLAY\8PLANE\8514\SOURCE	for 8514/a

NOTE: Be sure to use the /E parameter when XCOPYING  the 8514 sources to your disk.
The makefile is expecting to have the empty subdirectory \DISPLAY\8PLANE\8514\SOURCE\OBJ
and will generate an error if this subdirectory is not present.

Windows Real and Standard Mode Grabbers
***************************************
The Windows real and standard mode grabbers are on the Device Driver Samples
and Tools under \GRABBERS.  Since much of the grabber code for different displays
are common, you should copy the GRABBERS sub-directory in its entirety to your
hard disk.  Be sure to expand the compressed files to their original size before
using them.  The individual grabbers are in the following directories:

	\DISPLAY\GRABBERS\CGAHERC\CGA
	\DISPLAY\GRABBERS\CGAHERC\HERCULES
	\DISPLAY\GRABBERS\EGA\EGACOLOR
	\DISPLAY\GRABBERS\EGA\EGAMONO
	\DISPLAY\GRABBERS\VGA\VGACOLOR
	\DISPLAY\GRABBERS\VGA\VGAMONO

You should run the NMAKE utility with the Makefiles provided in each of these
directories to build the appropriate grabber.

System Font Sources
*******************
We have provided the Windows 3.1 proportional system font and the Windows 2.0
fixed pitch system font.  If your display driver supports resolutions other
than the display resolutions provided in Windows, you may need to modify the
system fonts for your display.

You will need to copy the \FONTS directory to your hard disk in its entirety.
Use the MS-DOS XCOPY command to do this.  You will find this directory on the
Device Driver Samples and Tools.

The 3.x proportional system fonts are hand tuned for the screen and match the
Adobe font widths for their Helvetica font.  If you need to adapt the system
font for a display with a different horizontal resolution than the ones
supported by these fonts, you need to adjust the font widths accordingly.
This will ensure the character will best "copy fit" the printed character
with those on the screen.

You can modify these files with the Font Editor tool provided with the SDK.

Before you will be able to build any fonts, you must make the FONTS.OBJ file.
You do this by running:

	MASM FONTS;

from the \FONTS directory.  You only need to do this once.

To build the proportional system font files, execute the Makefile by using NMAKE
from the \FONTS\SYSTEM directory:

The font file names for the new proportional system fonts are:

	CGASYS.FON			for (640 x 200) CGA resolution
	EGASYS.FON			for (640 x 350) EGA high resolution
	VGASYS.FON			for (640 x 480) VGA resolution
	8514SYS.FON			for (1024 x 768) 8514/a resolution

In addition to the 3.x system font, the Windows 2.0 fixed pitch system font
is necessary for older Windows applications running under Windows 3.0 and 3.1.
All Windows applications which are not "marked" to run with the new proportional
font, or which were not developed as a Windows 3.x application, will be given
the fixed pitch font for displaying text in dialog and message boxes, and the
client area. Windows 3.x supports both of these files, so it may be necessary
for you to also modify the appropriate fixed pitch font.

You will find these fonts in the \FONTS\FIXED directory.  The Makefile is
executed from this directory by running NMAKE.

The font file names for the 2.0 fixed pitch system fonts are:
	CGAFIX.FON		for (640 x 200) CGA resolution
	EGAFIX.FON		for (640 x 350) EGA high resolution
	VGAFIX.FON		for (640 x 480) VGA resolution (square pixels)
	8514FIX.FON		for (1024 x 768) 8514/a resolution (square pixels)
	8514OEM.FON		Uses the OEM Character Set
	CGAOEM.FON		Uses the OEM Character Set
	EGAOEM.FON		Uses the OEM Character Set
	VGAOEM.FON		Uses the OEM Character Set
	VGA850.FON		Uses Code Page 850
	VGA860.FON		Uses Code Page 860
	VGA861.FON		Uses Code Page 861
	VGA863.FON		Uses Code Page 863
	VGA865.FON		Uses Code Page 865

Printer Driver Sources
**********************
We are providing driver sources for our PCL5/HP LaserJet III and
Postscript printer drivers.  In additional, sample sources for
dot-matrix black/white and color raster printers are also included.

Building "Minidriver" Printer Drivers
*************************************
In order to build the printer drivers created by UNITOOL.EXE
from the Minidriver disk, make sure to install the include &
library files into the first directory on your LIB and INCLUDE MS-DOS
environment variables.

PCL5/HP LaserJet III Printer driver
***********************************
The PCL5/HP LaserJet III driver sources are being provided as an
example of how to support the dynamic downloading of TrueType fonts
as well as the new ResetDC API on PCL-type laser device.  It now
does printer memory tracking as well.

The PCL5/HP LaserJet III core driver sources are on the Device
Driver Samples and Tools Disk Set under \PRINTERS\HPPCL5A.  The
HPPCL5A directory contains subdirectories necessary for building
the driver.  You can build the driver by running the MAKEIT.BAT batch
file from the ..\HPPCL5A directory.  NOTE:  Be sure to maximize the
amount of conventional memory available when building this driver.
We recommend using MS-DOS Version 5.0 to provide as much conventional
memory as possible.

The PFM Editor (PFMEDIT.EXE) for PCL fonts is documented in the Printer
and Fonts Kit and is provided on the Device Driver Samples and Tools Disk
Set under \TOOLS.  The PFM Editor is for editing PCL fonts files that use
the 3.0 PFM font file format.  Please note that the PFM font files that
are used with the Universal driver for PCL devices uses a new updated 3.1
file format and they are modified via the UniTool tool.  If you want to use
these 3.1 format PFM files with the PFM editor, you must first run them
through a file converter we provide, CVTHPPFM.EXE which is provided under the
\TOOLS directory.  This is a simple DOS utility that takes as its first
parameter the 3.1 PFM Font file and creates a new 3.0 PFM font file with the
name specified in the second parameter.

The Printer Font Installer source is on the Device Driver Samples and Tools
Disk Set under \PRINTERS\FINSTALL.  You can build the font installer by
running the MAKEIT.BAT file from the ..\FINSTALL directory.  This module is
called by selecting the "Fonts..." button in the HPPCL5A driver dialog box.
The HPPCL5A driver and the Printer Font Installer are being provided to
serve as a base for your Windows 3.1 printer driver development.

PostScript Printer driver version 3.5
*************************************
The PostScript driver is also a good example of how to support the downloading
of TrueType fonts on a PostScript printer. It also provides the ability to map
TrueType fonts to Type 1 fonts in your PostScript printer.  In addition,  several
other new features have been added: the ability to specify the resolution, per
page downloading of fonts, setting the half-tone frequency, setting the halftone
angle, printing a negative image, printing a mirror image, generating output
that conforms to the Adobe Structured Document conventions and others.
The driver dialogs have also been updated to better conform to the suggested
design conventions for printer dialog boxes, according to the Microsoft Style
Guide.

The driver sources are on the Device Driver Samples and Tools Disk Sets under 
\PRINTERS\PS35.  To build the driver execute the Makefile by running NMAKE.  It is 
likely that you may run out of memory in building the driver.  To prevent this, run NMAKE 
in the RES and UTILS subdirectories prior to running NMAKE in the PS35 directory.  To 
build this driver, you will need to be sure to install the Alternate Math Small libraries 
during the SDK Install.  The name of the needed library is SDLLCAW.LIB   NOTE:  Be 
sure to maximize the amount of conventional memory available when building 
this driver.  We recommend using MS-DOS Version 5.0 to provide as much 
conventional memory as possible.

The PostScript driver is designed to make it easy to add new device support, without 
modifying the source files.  Provided in the DDK is a tool called MKPRN.EXE.  You use this 
tool to compile PostScript Printer Description (PPD) files into Windows Printer Description 
(WPD) files. 

You can then install .WPD files for the PostScript driver by creating a driver distribution 
disk just as for any other printer driver.  There is no longer an "Add Printer" option in the 
driver's Print Setup dialog box.  Refer to the file CONTROL.INF in the Windows SYSTEM 
subdirectory to see an example of how to create the  WPD entry for an OEMSETUP.INF file 
for installing unlisted PostScript models.

The MKPRN.EXE tool is on Device Driver Samples and Tools under \TOOLS.

Generic / Text Only Driver (TTY) Sample Sources
***********************************************
Sample sources for the Generic / Text Only driver are included in this kit. You can use the 
Generic / Text Only printer driver with any printer for printing text with no graphics. Since 
this driver uses the printer's internal character set (or sets), it is a convenient driver for 
printing quick drafts of large documents or spreadsheets. 

The files are on the Device Driver Samples and Tools Disk Set under \PRINTERS\TTY.  To 
build the TTY driver execute the Makefile under \PRINTERS\TTY using the NMAKE 
utility.

Keyboard Driver Sources
***********************
The keyboard sources are on the Device Driver Samples and Tools Disk Set under 
\KEYBOARD.  There is a batch file provided to build the IBM style keyboard driver and all 
the international tables for foreign keyboards.  

They keyboard driver structure has not been changed since Windows 3.0.  
To build the drivers, execute the Makefile by running NMAKE in the \KEYBOARD 
directory.  Please note that this makefile builds the driver file KBD.DRV.  This driver is 
renamed to KEYBOARD.DRV in Windows 3.1.

Mouse Driver Sources
********************
The Microsoft Mouse drive sources are on the Device Driver Samples and Tools Disk Set .  
Copy all the files from the \MOUSE directory to your hard disk with the MS-DOS XCOPY 
or COPY command.  Be sure to expand the compressed files to their original size before 
using them.   To build the driver, run: NMAKE in the \MOUSE directory.

The mouse driver has few changes from the version shipped with the Windows 3.0 DDK.   
No changes are expected, except bug fixes.  Note that this driver will not binary compare 
with the Mouse driver provided with Windows 3.1 since this driver contains proprietary 
source code.

COMM Driver Sources
*******************
The COMM driver sources are on the Device Driver Samples and Tools Disk Set.  Copy all 
the files from the \COMM directory to your hard disk with the MS-DOS XCOPY  command.  
Be sure to expand the compressed files to their original size before using them.   To build 
the driver, execute the Makefile by running the Nmake utility.

This driver requires the include file INT31.INC.  You will need to copy this file from the 
Virtual Devices and Tools Disk Set from the \INCLUDE directory.  Make sure this file is in 
your include path.

Sound Driver Sources
********************
The sound driver sources are on the Device Driver Samples and Tools Disk
Set.  Copy all the files from the \SOUND directory to your hard disk with
the MS-DOS XCOPY or COPY command.  Be sure to expand the compressed files
to their original size before using them. To build the driver, execute the
Makefile by running the Nmake utility.

The sound driver has few changes from the version shipped with the Windows 3.0
DDK. The driver was only modified for bug fixes.

Network Driver Sources
**********************
The MS-Net network driver sources are on the Device Driver Samples and Tools
Disk Set. Copy all the files from the \NET directory to your hard disk with
the MS-DOS XCOPY or COPY command.  Be sure to expand the compressed files to
their original size before using them.	 To build the driver, execute the
Makefile by running the Nmake utility.

Appendix B:  Description of Virtual Device Sources
**************************************************
Each source directory contains one or more .ASM source files, a .DEF file, and a
Makefile.  The Makefile is for use with Microsoft NMAKE or with other Unix-style
MAKE utilities.  Note, NMAKE is not provided in the DDK, but is included with
some of the Microsoft language products (for example,C 6.00A).

These MAKE files offer examples of the proper uses of the tools, options, and
general build procedures that should be followed in developing virtual devices.

As mentioned previously, these sources are provided for you to begin development
for Windows 3.1 386 enhanced mode virtual device support.

The Display Devices (CGA, EGA, VGA, 8514, HERC, V7VGA)
******************************************************
This device virtualizes the video display and is the most complex of the devices
supplied in the DDK.

The various VDD sources are on the Virtual Device Samples and Tools Disk Sets
under the directories \VDDCGA, \VDDEGA ,\VDDVGA, \VDDHERC, and \VDDV7VGA.

The \VDDCGA source tree contains files to only build the CGA VDD.  This VDD also 
provides support for the Compaq Plasma display (also called IDC).

The \VDDEGA source tree contains files to only build the VDDEGA.

The \VDDVGA source tree provided uses conditional assembly to build two separate virtual 
devices to handle standard VGA and 8514/a video adapters.

The \VDDHERC source tree contains files to only build the Hercules VDD.

The \VDDV7VGA source tree contains files to only build the Video 7 VDD.

You will find different Makefiles for each of the VDDs in the appropriate directories 
mentioned above.  Listed below are the Makefile names for each VDD:

*	Adapter			NMAKE                        *                 
	CGA			MAKEFILE	
	EGA			MAKEFILE		
	VGA			MAKEVGA		
	8514			MAKE8514		
	Hercules		MAKEFILE	
	Video 7			MAKEVGA

NMAKE by default will use the "Makefile" file to process.  You can also specify the
Makefile to use by using the /F parameter.   The VGA and 8514 Makefiles are located in
the same directory and so they have different names for the Makefile.

The Display/Windows Interface (GRABBER)
***************************************
The grabbers for 386 enhanced mode are different than the grabbers run in real or standard 
modes.  They are responsible for rendering a virtual machine's display context within a 
window; therefore, they are closely bound with the virtual display device (VDD).  Each VDD 
needs to be accompanied by its own grabber as a linked pair.

The sources are on the Virtual Device Samples and Tools Disk Sets under the \GRABBERS 
directory structure.

The DDK contains source for CGA, EGA, VGA, 8514/a, Compaq Plasma, Hercules, and 
Video 7 grabbers.  Since they are all built in the same directory, different Makefiles are 
provided for each of them.  Below are the Makefile names  for each grabber:

*	Adapter		NMAKE                          *

	CGA		MAKECGA.		
	EGA		MAKEFILE.		
	VGA		MAKEVGA.		
	8514		MAKEDIB.		
	Hercules	MAKEHERC.		
	AT&T/Compaq	MAKEPLSM.		
	Video 7		MKV7.

NMAKE by default will use the "Makefile" file to process.  You can also specify the Makefile 
to use by using the /F parameter.   These Makefiles require that the CMACROS.INC include 
file be located in the \GRABBERS directory.  This file is provided in the \OEMFONTS 
directory.

The font files used by the grabbers when running in a window are provided in this kit.  You 
will find them on disk 2 under \OEMFONTS.  Run the Nmake utility to execute the 
Makefile here.

The DMA Device (VDMAD)
**********************
This device handles direct memory access devices. Virtual devices can support hardware 
cards which use DMA by calling the DMA Services provided by the VDMAD.  These services 
are documented in the DDK documentation.

See the section on the Floppy Drive Device, for an example of a device which uses the DMA 
Services.

The VDMAD itself should not normally need to be customized; it would only need to be 
altered to support DMA on machines with non-standard architectures.

The sources are on the Virtual Device Samples and Tools Disk Set under the \VDMAD 
directory structure.

The EBIOS Device (EBIOS)
************************
This device detects the EBIOS page on machines (such as the PS/2) where it is used, 
identifies it, and ensures that it is reserved as global memory. It will probably not need to be 
modified, but is provided here as an example of a relatively simple device which passively 
accommodates something in the environment.
The sources are on the Virtual Device Samples and Tools Disk Set under the \EBIOS 
directory structure.

The Keyboard Device (VKD)
*************************
This is one of the more complicated devices, as it not only virtualizes the keyboard but also 
interacts with the Windows shell to handle hotkeys and other special functions.  It should be 
modified to support other, nonstandard keyboards.

The sources are on the Virtual Device Samples and Tools Disk Set under the \VKD 
directory structure.

The Mouse Device (VMD)
**********************
This device virtualizes the mouse and maps the INT 33H API between protect and virtual 
modes.

The sources are on the Virtual Device Samples and Tools Disk Set under the \VMD 
directory structure.

The Netbios Device (VNETBIOS)
*****************************
This device maps the Netbios API between protect and virtual modes, allowing Windows 
applications to access the network.  It also handles asynchronous network transactions by 
mapping the application's buffer into global memory, so the network software can access it 
when the asynchronous event occurs (even if another virtual machine is running at the 
time).

This should be modified by network vendors who extend the standard NetBIOS interface 
and also serve as a guide to writing virtual devices for other types of network software.

The sources are on the Virtual Device Samples and Tools Disk Set under the \VNETBIOS 
directory structure.

The Network Device (LDOSNET)
****************************
In general, this device manages network connections and assures network integrity across 
all virtual machines.  This device is not part of the Windows 3.1 retail package.  It is 
provided as a sample source to demonstrate some mechanisms used in supporting network 
functionality.

The device should be modified when your software does not use standard MS-DOS 
redirector calls for handling network connections.

You will find the sources on the Virtual Device Samples and Tools Disk Set under the 
\LDOSNET directory.

The BIOS Device (BIOSXLAT)
**************************
This device maps the ROM BIOS API between protected mode and virtual 8086 mode, 
allowing Windows applications and device drivers access to ROM BIOS services.

This should be modified when there are non-standard ROM BIOS calls that pass pointers to 
memory and which are used by Windows applications or device drives.

You will find the sources on the Virtual Device Samples and Tools Disk Set under the 
\BIOSXLAT directory,

The Paging Device (PAGESWAP)
****************************
This device is used by 386 enhanced mode for demand paging at either the INT 21H or INT 
13H level.
The sources are on the Virtual Device Samples and Tools Disk Set under the \PAGESWAP 
directory structure.

The Printer Device (VPD)
************************
The Printer Device virtualizes access to the parallel ports.  If a second virtual machine tries 
to access one of the ports while it is being used by another application, a contention dialog is 
presented to the user allowing them to resolve the dispute of ownership.  This virtual device 
was installed by default in Windows 3.0 but is no longer used with Windows 3.1.  It is a 
simple VxD and is used as an example in the Virtual Device Adaptation Guide.

The sources are on the Virtual Device Samples and Tools Disk Set under the \VPD 
directory structure.

The COMM Device (VCD)
*********************
This device virtualizes the standard serial ports on ISA architecture machines.  It supports 
COM1 through COM4.  It should be modified to add support for different chip sets or for 
additional COM ports.

You will find the sources on the Virtual Device Samples and Tools Disk Set under the \VCD 
directory.

The Virtual COMM Device (COMBUFF)
*********************************
This device works with the Vitual COMM Device (VCD) to buffer com input for MS-DOS 
Virtual Machines.  It supports COM1 through COM4.  It can be modified to provide other 
handshaking protocols, or just used as sample source for virtualizing COM ports by 
cooperating with VCD.

You will find the sources on the Virtual Device Samples and Tools Disk Set under the 
\COMBUFF directory.

The Floppy Drive Device (VFD)
*****************************
This device is responsible for two things:

--	It removes special timer port trappings to ensure copy protection schemes work 
properly.
--	It communicates with the VDMAD to synchronize DMA channel usage.

You will find the sources on the Virtual Device Samples and Tools Disk Set under the \VFD 
directory.

The 386 Enhanced Mode Block Device (WDCTRL)
*******************************************
This virtual device is used to talk directly to hard-drive controllers that are Western Digital 
1003 compatible.  This allows us to do disk i/o entirely in ring 0 protected mode, bypassing 
the real mode BIOS. 

You will find the sources on the Virtual Device Samples and Tools Disk Set under the 
\WDCTRL directory.

The PageFile Device (PAGEFILE)
******************************
This virtual device handles our virtual memory paging file, and will always call through 
BlockDev if any FastDisk devices are available.
You will find the sources on the Virtual Device Samples and Tools Disk Set under the 
\PAGEFILE directory.

The INT13 Virtual Device (INT13)
********************************
The Int13 device is used in conjunction with the 386 Enhanced Mode Block Device.  It traps 
and emulates Int 13h BIOS calls.   

You will find the sources on the Virtual Device Samples and Tools Disk Set under the 
\INT13 directory.

