                           README.TXT
                           ==========

This document covers the following three topics:

* A guide to sources of information that complement the
  Windows 3.0 Software Development Kit (SDK) manuals.

* A solution for a bug in CodeView for Windows (CVW).

* Instructions for setting your PATH, INCLUDE, and LIB
  environment variables, and other instructions for
  configuring your Windows development environment. (The SDK
  Install program appends this information to this
  README.TXT file.)


============================
OTHER SOURCES OF INFORMATION
============================

This section describes information that supplements the
Windows 3.0 SDK manuals. You can find this information in
the SDK online documents and in sources other than the SDK.

---------------------------------
A Guide to Other SDK Online Files
---------------------------------

The SDK Install program copies to your Windows development
directory (default is \WINDEV) the following online files.
You can view or print the .WRI files by using the Microsoft
Write editor included with the Windows retail product.

The following is a list of the online documents that
accompany the SDK:

Online File      Description
-----------      -----------
README.WRI       Microsoft Windows 3.0 SDK Update Notes.

                 This file contains information about the
                 SDK that was not available when the SDK
                 manuals were printed.

PWBINFO.WRI      Using Programmer's WorkBench (PWB) for
                 Windows Program Development.

                 This file provides important information
                 for Windows developers using PWB. PWB is an
                 integrated software development tool
                 included with Microsoft C version 6.0.

TIPS.TXT         Tips for Windows Developers from Microsoft
                 Product Support Services.

                 This file provides answers to common
                 questions submitted to Microsoft Product
                 Support Services by developers of Windows
                 applications.

NETAPI.TXT       Developing Network Applications for Windows
                 3.0.

                 This file describes programming
                 considerations for applications which use
                 certain network protocols.


---------------------------------
Additional Sources of Information
---------------------------------

Additional information is available on the following topics:

* Windows clipboard formats

* Windows developers' notes

* DOS Protected Mode Interface (DPMI) Specification

These sources are described in this section.


Windows Clipboard Formats
-------------------------
Specifications for various clipboard formats supported by
Microsoft and other vendors' applications are available from
sources described below.

* Rich Text Format (RTF) -- similar to the standard text
  format, except that formatting attributes such as font,
  style, and size are embedded.  The RTF specification is
  included in the "Microsoft Word Technical Reference for
  Windows and OS/2 Presentation Manager,", which may be
  ordered through bookstores or directly from Microsoft
  Press at (800)888-3303.

* Binary Interchange File Format (BIFF) -- the native
  Microsoft Excel format. The BIFF specification is
  available from Microsoft Excel Telephone Sales at
  (800)227-6444; ask for department "ET".

* Tag Image File Format (TIFF) -- a format that assists in
  incorporating line art, photographs, and other raster
  images into documents via desktop publishing application
  programs. The TIFF specification is available from Aldus
  Corporation at (206)628-2320; select the Production
  Information phone extension.  It is also available in the
  CompuServe "Aldus" Forum, Library 10.

* Data Interchange Format (DIF), WKS, and WK1 -- formats
  maintained by Lotus Development Corporation,
  (617)253-9150.


Windows Developer's Notes
-------------------------

The Windows Developer's Notes are available from the
Microsoft Information Center at (800)428-9400 for a nominal
charge. This document is a collection of notes on various
topics, including the following:

* Windows Setup Sources -- code excerpts from the retail
  Windows Setup program, which you may borrow from to
  develop your own Windows application setup program.

* Preparing Windows Applications for International Use --
  guidelines for developing Windows applications that can be
  easily ported to other languages.

* Windows Executable File Header Format -- information
  useful only for developers who need their applications
  to read .EXE files directly.

* Windows Font File Format -- information useful only for
  developers who want their applications to read and modify
  .FNT files in the same manner as does the Windows SDK Font
  Editor.

* Windows Support for DPMI -- describes which DOS interrupts
  and BIOS calls are supported and which are not supported
  by Windows' implementation of the DOS Protected Mode
  Interface (DPMI) specification. This information is useful
  for applications that make direct DOS interrupt or BIOS
  calls. The availability of the DPMI specification itself
  is described below in this file.

* Windows Write and Calendar File Formats -- for use by
  developers who want their applications to directly read
  or write Write or Calendar files.


DOS Protected Mode Interface (DPMI) Specification
-------------------------------------------------
This specification defines how DOS programs can access the
extended memory of PC architecture computers while
maintaining system protection.  Windows provides
applications access to extended memory through Windows'
implementation of DPMI. Most Windows applications do not
need to directly use DPMI services, since these services are
provided by the Windows application programming interface
(API). Special Windows applications and device drivers may
need to access DMPI services directly.

The DPMI specification is available from Intel Corporation
at (800)548-4725. (Request part number JP-26.) This DPMI
specification should be read in conjunction with the
"Windows Support for DPMI" note included in the Windows
Developer's Notes, described above.


==============================
CodeView for Windows (CVW) Bug
==============================

If you use a VGA primary monitor and it is corrupted after
the initial start-up of Code View for Windows (CVW), you
might be able to solve this problem by replacing your
VGA driver (VGA.DRV) with an updated version included on the
SDK disks.

The bug appears as follows: At start-up, the debugging
monitor displays the CVW screen and prompts for the name of
the application you are debugging. Once the focus is
returned to Windows, the VGA screen is corrupted; colors and
pixels are randomly changed, or the VGA monitor displays
static (white noise).

This problem arises only if the primary monitor is VGA, and
only for machines with certain VGA ROM BIOS. In particular,
the problem arises due to a conflict between CVW's use of
INT 10h instructions and the VGA BIOS. This problem should
not arise for Windows applications, since they do not
directly use INT 10h instructions.

Before you install the updated VGA.DRV, you should save the
old VGA.DRV file in your Windows system directory. This will
allow you to restore the old file in case the new driver
doesn't solve the problem.  Then you should copy the VGA.DRV
file from the root directory of your 5-1/4 inch "Development
Files" disk or 3-1/2 inch "Debug Files 2" disk to your
Windows system directory (typically \WINDOWS\SYSTEM).


=========================================================
Setting Your PATH, INCLUDE, and LIB Environment Variables
=========================================================

Based on the directories that you specified during the SDK
installation procedure, you should add the following
directories in your environment variables:
