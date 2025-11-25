============================================================================
The Microsoft Windows NT Device Driver Kit (DDK) is provided on CD-ROM only.
Printed documentation is available as a stand-alone package. The DDK
CD-ROM includes the Postscript source files for all printed documents, as
well as on line help.

NOTE: The WinHelp documentation should always be considered the most
      accurate information other than the README.TXT found on the disk.
      Use those files as the main source of development information wherever
      possible.

To print these files, you will need a 100% PostScript(R) compatible printer.
The printer should have at least 4MB of RAM or some of the longer and more
complicated chapters may not print. This is especially true of chapters
containing detailed diagrams such as Chapter 3 of the Kernel-mode Driver
Design Guide.

These PostScript files were created using Microsoft Word for Windows,
version 2.0c, using the Apple LaserWriter driver. The Kernel-mode Driver
Design Guide was created using Microsoft Word 5.0 and associated PostScript
driver.

The following fonts are used:

    - Times
    - Helvetica Narrow
    - Letter Gothic MS

Letter Gothic MS is a soft font. To license this font, contact Linotype
at 1-800-633-1900. Letter Gothic MS is a monospace font, used for code
samples. If you don't load it before printing the PostScript files, your
PostScript printer will default to Courier, which will yield acceptable
results.

To print the PostScript files, copy each one to the printer, using commands
in the following form: copy <filename> <printer port>.

Before printing the Kernel-mode Driver Design Guide, it is necessary to copy
the postscript header file 'psheader.ps' to the printer first, before
copying any of the remaining chapters.

Some serially connected printers and other special-case printers require
PostScript files to contain leading and trailing CTRL-D characters, which
cause a reset of a printer's PostScript interpreter. Some other printers
require the *absence* of a leading CTRL-D.

The PostScript files on this CD-ROM should have no leading CTRL-D.
The files do have a trailing CTRL-D.  If your printer requires leading
CTRL-D characters, insert them manually.

Below is a list of the contents of each documentation file included on the
DDK CD-ROM, along with its location. Most of these files are compressed on 
the CD-ROM. In order to view and/or print them, you must first run expand -r 
filename. For the Postscript files, you can then just copy them to a standard 
Postscript Printer.
============================================================================

\DOC
 \DDK
  |   
  +---GS Getting Started
  |   
  |       0.PS Table of Contents
  |       1.PS Chapter 1 - Introduction
  |       2.PS Chapter 2 - Installing the Windows NT DDK
  |       3.PS Chapter 3 - Building Windows NT Drivers
  |       4.PS Chapter 4 - Getting More Information
  |       A.PS Appendix A - Building Windows NT Drivers--the BUILD Utility
  |       B.PS Appendix B - Windows NT Installation (Checked Build)
  |       C.PS Appendix C - Driver Certification and Distribution
  |       
  +---PG Programmer's Guide
  |   
  |       000FRONT.PS Table of Contents
  |       100FRONT.PS Part I - Setup
  |       101INITL.PS Chapter 1 - System Initialization Overview
  |       102INSTL.PS Chapter 2 - Driver Installation
  |       103NCARD.PS Chapter 3 - Windows NT Netcard Detection: Architecture and Interfaces
  |       104BINDI.PS Chapter 4 - Network Component Bindings
  |       200FRONT.PS Part II - Network Component Bindings
  |       201DEBUG.PS Chapter 1 - Debugging Windows NT Drivers
  |       202STRM.PS  Chapter 2 - STREAMS Debugging
  |       203PERFM.PS Chapter 3 - Performance Counters in Windows NT Drivers
  |       300FRONT.PS Part III - Testing
  |       301OVERV.PS Chapter 1 - Overview
  |       302SYSHC.PS Chapter 2 - System Hardware Compatibility Tests
  |       303MULST.PS Chapter 3 - Serial Port and Multiport Serial Testing
  |       304MMTST.PS Chapter 4 - Multimedia Tests
  |       305NDDRT.PS Chapter 5 - NDIS Netcard Driver Tester
  |       306PRTST.PS Chapter 6 - Printer Tests
  |       307SCSTR.PS Chapter 7 - SCSI/Storage
  |       308VIDEO.PS Chapter 8 - Video Testing
  |       A_COMPAT.PS Appendix A - Microsoft Compatibility Labs
  |       B_GLOS.PS   Glossary
  |       
  +---KG Kernel-mode Driver Design Guide
  |   
  |       PSHEADER.PS  Document Header File 
  |       TOC.PS       Table of Contents
  |       INTRO.PS     Chapter 1 - NT Drivers
  |       IRPS.PS      Chapter 2 - Layered I/O, IRPs, and I/O Objects
  |       NTOBJ.PS     Chapter 3 - NT Objects and Support for Drivers
  |       DRVROVER.PS  Chapter 4 - Basic NT Driver Structure
  |       DENTRY.PS    Chapter 5 - DriverEntry and Reinitialize Routines
  |       DISPATCH.PS  Chapter 6 - Dispatch Routines
  |       STARTIO.PS   Chapter 7 - StartIo Routine or Queue-management Routines
  |       ISR.PS       Chapter 8 - Interrupt Service Routine
  |       DPC.PS       Chapter 9 - DpcForIsr Routine and CustomDpc Routines
  |       SYNCRIT.PS   Chapter 10 - SynchCritSection Routines
  |       DCONTROL.PS  Chapter 11 - AdapterControl and Controller Routines
  |       CANCEL.PS    Chapter 12 - Cancel Routines
  |       IOCOMPL.PS   Chapter 13 - I/OCompletion Routines
  |       TIMER.PS     Chapter 14 - IoTimer and CustomTimerDpc Routines
  |       UNLOAD.PS    Chapter 15 - Unload Routine
  |       ISSUES.PS    Chapter 16 - Common Driver Design Issues
  |       APPAVID.PS   Appendix A - Video Miniport Drivers
  |       APPBSCSI.PS  Appendix B - SCSI Drivers
  |       GLOS.PS      Glossary
  |       INDEX.PS     Index
  |       
  +---KR Kernel-mode Device Driver Reference
  |   
  |       K000.PS Part I - Kernel-mode Support Routines
  |       K100.PS Table of Contents
  |       K101.PS Chapter 1 - Summary of Kernel-mode Support Routines
  |       K102.PS Chapter 2 - Executive Support Routines
  |       K103.PS Chapter 3 - Hardware Abstraction Layer Routines
  |       K104.PS Chapter 4 - I/O Manager Routines
  |       K105.PS Chapter 5 - Kernel Routines
  |       K106.PS Chapter 6 - Memory Manager Routines
  |       K107.PS Chapter 7 - Object Manager Routines
  |       K108.PS Chapter 8 - Process Structure Routines
  |       K109.PS Chapter 9 - Runtime Library Routines
  |       K110.PS Chapter 10 - Security Reference Monitor Routines
  |       K111.PS Chapter 11 - ZwXxx Routines
  |       K112.PS Chapter 12 - System Structures
  |       K200.PS Part II - System Structures
  |       K201.PS Chapter 1 - IRP Function Codes and IOCTLs
  |       K202.PS Chapter 2 - I/O Requests for Keyboard and Mouse Drivers
  |       K203.PS Chapter 3 - I/O Requests for Beep Device Drivers
  |       K204.PS Chapter 4 - I/O Requests for Parallel and Serial Drivers
  |       K205.PS Chapter 5 - I/O Requests for Sound, MIDI, and Wave Drivers
  |       K206.PS Chapter 6 - I/O Requests for Disk Drivers
  |       K207.PS Chapter 7 - I/O Requests for CD-ROM Drivers
  |       K208.PS Chapter 8 - I/O Requests for Tape Drivers
  |       K209.PS Chapter 9 - I/O Requests for SCSI Drivers
  |       K210.PS Chapter 10 - I/O Requests for Video Drivers
  |       K300.PS Part III - SCSI Drivers
  |       K301.PS Chapter 1 - SCSIPort Functions
  |       K302.PS Chapter 2 - Miniport Driver Functions
  |       K303.PS Chapter 3 - Tape Subclass Entry Points
  |       K304.PS Chapter 4 - SCSI Structures
  |       K400.PS Part IV - SCSI Structures
  |       K401.PS Chapter 1 - Videoport Functions
  |       K402.PS Chapter 2 - Miniport Driver Functions
  |       K403.PS Chapter 3 - Video Structures      
  |       
  +---SG Win32 Subsystem Guide
  |   
  |       000FRONT.PS  Table of Contents
  |       100FRONT.PS  Part I - Win32 Graphic Device Driver Interface
  |       101GDIDR.PS  Chapter 1 - GDI Driver Overview
  |       102SUPDD.PS  Chapter 2 - Supporting the Graphics DDI
  |       199INDEX.PS  Index, Part I 
  |       200FRONT.PS  Part II - Win32 Display Drivers
  |       201DDREV.PS  Chapter 1 - Win32 Display Driver Environment
  |       202SUPDD.PS  Chapter 2 - DDI Support in Display Drivers
  |       299INDEX.PS  Index, Part II
  |       300FRONT.PS  Part III - Win32 Printer Drivers
  |       301PRENV.PS  Chapter 1 - Win32 Printing Environment
  |       302DDIPR.PS  Chapter 2 - Supporting DDI Printing and User Interface Functions
  |       303AMINI.PS  Chapter 3a - Writing a Miniprint Driver
  |       303BMINI.PS  Chapter 3b - Writing a Miniprint Driver (Continued)
  |       399INDEX.PS  Index, Part III
  |       400FRONT.PS  Part IV - Win32 Virtual Device Drivers
  |       401VDD.PS    Chapter 1 - Virtual Device Drivers for MS-DOS Applications 
  |                       or Special Hardware
  |       499INDEX.PS  Index, Part IV
  |       500FRONT.PS  Part V - Win32 Multimedia Drivers
  |       501MDI.PS    Chapter 1 - The Win32 Multimedia Driver Interface
  |       502MCID.PS   Chapter 2 - Win32 MCI Drivers
  |       503AUDDR.PS  Chapter 3 - Win32 Audio Drivers
  |       504JOYDR.PS  Chapter 4 - Win32 Joystick Drivers
  |       599INDEX.PS  Index, Part V
  |       601GLOS.PS   Glossary
  |       
  +---SR Win32 Subsystem Driver Reference
  |   
  |       000FRONT.PS  Table of Contents
  |       100FRONT.PS  Part I - Win32 Support for Graphics Drivers
  |       101GRDRF.PS  Chapter 1 - Graphic Driver Functions
  |       102GRDRS.PS  Chapter 2 - Graphic Driver Structures
  |       200FRONT.PS  Part II - Win32 Support for Printer Devices
  |       201PRIDF.PS  Chapter 1 - Printer Interface Driver Functions
  |       202PRMON.PS  Chapter 2 - Printer Monitor Functions
  |       203PRPRO.PS  Chapter 3 - Printer Processor Functions
  |       300FRONT.PS  Part III - Win32 Support for Multimedia Devices
  |       301MMFNC.PS  Chapter 1 - Multimedia Functions
  |       302MMSTR.PS  Chapter 2 - Multimedia Structures
  |       303MMMSS.PS  Chapter 3 - Multimedia Messages
  |       400FRONT.PS  Part IV - Win32 Support for Multimedia Devices
  |       401VDDFN.PS  Chapter 1 - VDD Functions
  |       402VDDST.PS  Chapter 2 - VDD Structures
  |
  +---NG Network Drivers
  |   
  |       000FRONT.PS  Table of Contents
  |       100FRONT.PS  Part I - Introduction to Windows NT Network Drivers
  |       101WHIND.PS  Chapter 1 - What is a Windows NT Network Driver
  |       102OPENV.PS  Chapter 2 - Windows NT Network Driver Operating Environment
  |       103NCARD.PS  Chapter 3 - NDIS 3.0 Netcard Driver
  |       104TDIDV.PS  Chapter 4 - TDI Driver
  |       105NDRCN.PS  Chapter 5 - Network Driver Programming Considerations
  |       199INDEX.PS  Index, Part I
  |       200FRONT.PS  Part II - NDIS 3.0 Network Driver Design
  |       201NDFDS.PS  Chapter 1 - Netcard Driver Function Descriptions
  |       202NDTHO.PS  Chapter 2 - Netcard Driver Theory of Operation
  |       203WRNTD.PS  Chapter 3 - Writing a Netcard Driver
  |       299INDEX.PS  Index, Part II
  |       300FRONT.PS  Part III - TDI Driver Design
  |       301TDFUD.PS  Chapter 1 - TDI Driver Function Descriptions
  |       302TDTOP.PS  Chapter 2 - TDI Driver Theory of Operation
  |       303WRTDR.PS  Chapter 3 - Writing a TDI Driver
  |       399INDEX.PS  Index, Part III
  |       A__APPFR.PS  Appendix Table of Contents
  |       A_CODES.PS   Appendix A - Codes and Messages
  |       B_OBATID.PS  Appendix B - Object Attribute Identifiers 
  |       C_NDFN1.PS   Appendix C - Network Driver Functions
  |       C_NDFN2.PS   Appendix C - Network Driver Functions (Continued)
  |       D_NDTYP.PS   Appendix D - Network Driver Structures and Enumerations
  |       E_TDFNC.PS   Appendix E - TDI Functions
  |       F_TDTYP.PS   Appendix F - TDI Structures
  |       G_TDIOCT.PS  Appendix G - TDI IOCTL Messages
  |       GLOSSARY.PS  Glossary
  |
  +---MISC Additional Documents

        KERNPROF.TXT  Windows NT Kernel Profiler
        MASM.TXT      Hints for using Microsoft MASM 6.1 for Windows NT
        MMTHUNK.DOC   Generic WOW32 Thunk Interface Extensions for Multimedia
        MMTHUNK.PS    Generic WOW32 Thunk Interface Extensions for Multimedia
        MORSETUP.DOC  Additional Setup Information
        MORSETUP.PS   Additional Setup Information
        MTF10.DOC     Microsoft Tape Format v1.0
        MTF10.PS      Microsoft Tape Format v1.0
        NCPAINF.DOC   Network Component INF Files
        NCPAINF.PS    Network Component INF Files
        PROFILER.TXT  Windows NT Sampling Profiler
        RASWAN.DOC    Remote Access Service Wide Area Networking
        RASWAN.PS     Remote Access Service Wide Area Networking
        STREAMS.DOC   STREAMS: An Environment for Network Transport Protocols on 
                         Windows NT       
        STREAMS.PS    STREAMS: An Environment for Network Transport Protocols on 
                         Windows NT
        WINSOCK.DOC   Windows Sockets Transport Independence for Windows NT
        WINSOCK.PS    Windows Sockets Transport Independence for Windows NT
