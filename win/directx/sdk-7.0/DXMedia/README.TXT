Microsoft DirectX Media 6.0 SDK Readme.txt

(c)1999 Microsoft Corporation. All rights reserved.
-----------------------------------------------------------------

Welcome to the DirectX Media SDK.

The DirectX Media 6.0 SDK and runtime shipped on the DirectX 7.0
CD is unchanged from the DirectX Media 6.0 release. New on this CD
is a patch that addresses issues in the DirectX Media 6.0 runtime. 
See redist\dxminstall.txt for more information. This will be the final
release of the DirectX Media SDK and runtime. The next planned release
of DirectShow will be as part of DirectX 8.0. Windows Media
Player is available as a stand-alone release from the Microsoft
web site http://www.microsoft.com. For updated information on
DirectAnimation, refer to the microsoft website
http://www.microsoft.com.

This document gives an overview of the product and describes the 
layout of the SDK, installation, and hardware requirements. Be 
sure to review the licensing agreement in License.doc, and its
addendum, Redist.txt.

Please see the following sections:

- What's New
- Overview
- How to Use the Documentation
- SDK Layout
- Installation
- Hardware and Software Requirements
- Known Issues
- Contact Information 

-----------------------------------------------------------------
What's New
==========

- DirectX Media 6.0 Runtime patch

New on this CD is a patch that addresses issues in the DirectX 
Media 6.0 runtime. See redist\dxminstall.txt for more information. 

- Directory Structure

The directory structure for the samples has changed in this 
release to enable future integration with the Microsoft 
Platform SDK. You'll now find the sample source code installed 
in the samples\multimedia\<technology> directory off of your 
DirectX media installation directory. <technology> is 
DShow (DirectShow), DAnim (DirectAnimation), or DTrans 
(DirectX Transform). 

- DirectDraw Exclusive Mode Support

This release adds support for video playback from within 
applications that are using DirectDraw Exclusive Mode. See 
the DirectShow documentation for the IDDrawExclModeVideo 
interface and the DDRAWXCL sample in the 
Samples\Multimedia\DShow\src\DDrawXCL subdirectory for more 
information. 

- Expanded MPEG-2 Support

DirectShow has two new filters and two new interfaces to 
support MPEG-2 applications. See the "What's New in the 
DirectShow SDK?" topic in the Getting Started section of the 
DirectShow documentation for more information.

- DirectX Transform

DirectX Transform is new with this version of DirectX Media.

- Documentation

See the What's New topics in the Getting Started sections of 
the SDK documentation for specific detailed information about 
these and other new features. 

-----------------------------------------------------------------
Overview
========

This release of Microsoft DirectX Media contains the 
following DirectX media run time and SDK components:

- DirectShow 
- DirectAnimation
- DirectX Transform

For detailed last-minute notes pertaining to these technologies 
see their respective readme files in the DXMedia\Help directory:

   ReadmeDAnim.htm - DirectAnimation readme
   ReadmeDShow.htm - DirectShow readme
   ReadmeDTrans.htm - DirectX Transform readme

DirectShow

DirectShow enables capture of multimedia streams from devices 
and playback of multimedia streams from local files or Internet 
servers. The streams can contain video and audio data compressed 
in various formats, including MPEG, Apple QuickTime, audio-video 
interleaved (AVI), and WAV files. Capture can be based on either 
Video for Windows or Windows Driver Model (WDM).

DirectAnimation

DirectAnimation provides unified, comprehensive support for 
animation, streaming, and integration of diverse media types, 
such as two-dimensional (2-D) vector graphics, three-dimensional 
(3-D) graphics, sprites, audio, and video. Because it is a 
Component Object Model (COM) API with an underlying engine/run 
time, DirectAnimation functionality can be accessed - in 
different ways - by a wide variety of programmers and authors: 
HTML and scripting authors, and programmers using Java, Visual 
Basic, and Microsoft Visual C++.

DirectX Transform

DirectX Transform is new with this release. It provides 2-D 
and 3-D image transformations and effects.

-----------------------------------------------------------------
How to Use the Documentation
============================

You must have a Web browser to view the SDK documentation. 
Microsoft Internet Explorer 4.01, is recommended for viewing 
HTML files and is required for the new HTMLHelp standard. 

The DirectX Media SDK documentation is supplied in two 
formats: HTML and compressed HTML (HTMLHelp). 

The HTMLHelp files DXMedia.chm and DXMedia.chi are in the 
subdirectory DXMedia\help within the SDK and contain the 
DirectShow, DirectAnimation, DirectX Transform, and Direct3D 
Retained Mode documentation. The HTMLHelp files require that 
Internet Explorer version 4.01 be installed on the system. 
HTMLHelp files offer features such as full-text search and 
printing of entire chapters that are not available in plain 
HTML.

The HTML documentation opening page, Default.htm, is in the 
DXMedia subdirectory within the SDK. You can also bypass this 
opening page and go directly to the default pages of documentation 
that interests you. These default pages can be viewed in any 
browser and are installed in the following directories 
(assuming the default installation directory): 
 
   DirectShow - DXMedia\help\ds\default.htm 
   DirectAnimation - DXMedia\help\da\default.htm 
   DirectX Transform - DXMedia\help\dxt\default.htm
   Direct3D Retained Mode - DXMedia\help\d3drm\default.htm. 

   Note: The DirectX Media SDK does not install the Direct3D 
         Retained Mode SDK component as that component is 
         installed by the DirectX Foundation SDK. The DirectX
         Foundation SDK is also supplied on this CD.

-----------------------------------------------------------------
SDK Layout
==========

The DirectX Media SDK contains the following top-level 
directories: 

   DXMedia - DirectX Media SDK
   Extras - Updated HTMLHelp control for non-Windows 98 systems;
            extra DV and DirectX Transform files
   Redist - Redistributable DirectX Media run tim and optional image 
            transforms

-----------------------------------------------------------------
Installation
============

Install the DirectX Media SDK by running 
   
   DXMedia\_setup_\<platform>\setup.exe 

where <platform> is x86 or alpha. 


-----------------------------------------------------------------
Hardware and Software Requirements
==================================

You should have a 486 processor or faster, at least 16 megabytes 
(MB) of RAM, and 200 MB of hard disk space before installing the
DirectX media SDK. If you want to hear the sound in the movies 
and samples, you need a sound card and speakers. If you want to 
do video or audio capture, you need capture equipment. 

Your system must include Windows 95 or higher or Windows NT 4.0 
SP3 or higher. 

Some features, such as 3-D DirectX Transforms, require the 
DirectX Foundation version 6.0 run-time. DirectAnimation requires 
at least DirectX Foundation version 3.0a for most funtionality. 
See the readme for each technology for details.

The documentation and portions of DirectAnimation and DirectX 
Transforms also require Microsoft Internet Explorer version 4.01 
or higher for complete functionality.

-----------------------------------------------------------------
Known Issues
============

- DirectX media SDK Setup

If you plan to install both the DirectX media SDK and the DirectX
foundation SDK from this CD, be sure to restart your system between 
SDK installations. 

Before installing the DirectX media SDK you should uninstall
previous versions of the SDK.

Setup on Alpha machines with a removable boot drive is not 
supported.

- International install

If you install the DirectX media SDK on an international version
of Windows and change the destination directory for the install to
include a 5c trail byte, then setup will appear to proceed 
normally but it will not copy any files. The problem does not 
occur with the default English directory name.

- DirectX media run-time setup

The system must normally be restarted after installing the DirectX 
media runtime and before playing any multimedia content. See the 
Redistribution Information section (help\ds\Oview\redist.htm) of 
the DirectX media SDK documentation for information about 
installing the DirectX media runtime from an application, 
detecting if a reboot is required, and turning on the run-time's
user interface.

Note: The following documented return values for dxmedia.exe 
installations on Windows NT are not currently implemented: 
- ERROR_MEMBER_NOT_IN_GROUP (administrator privileges are required) 
- ERROR_OLD_WIN_VERSION (Windows NT 4.0 service pack 3 is required)
Your installation program should check for these minimum
requirements (administrator privileges and at least Windows NT 4.0
SP3) before installing dxmedia.exe on Windows NT.

You might receive an oleaut32.dll error on Windows 95 or OSR2 in 
the Intel Indeo 5 portion of setup. Upgrading to Internet Explorer 
4.0 or later or installing the latest Indeo 5 codecs 
(http://developer.intel.com/ial/indeo/video/driver.htm) and
then re-installing the DirectX media run-time should eliminate 
the error.

- Building samples

The DirectX media C/C++ samples build with Microsoft Developer 
Studio version 5.0 and higher. If you build from the command line
be sure your path environment variable lists the most recent
products first. For example, list Microsoft Developer Studio 
before an older edition of the Platform SDK.

- DirectShow DVD Sample build error

If you get a build error in Developer Studio involving 
SetWindowHookEx when building the DirectShow DVDSampl then 
define STRICT in Developer Studio's pre-processor directives 
(Project Settings, C/C++ tab, Preprocessor category, 
Preprocessor definitions).

- Debug version of DirectShow DDrawXcl Sample faults on startup

Before building the debug version of the DDrawXcl sample, define 
DEBUG in Developer Studio's pre-processor directives to avoid a 
fault when starting the sample. Note that defining "DEBUG" is 
in addition to defining "_DEBUG" that is already defined. To 
change the pre-processor directives in Developer Studio choose 
Project Settings, C/C++ tab, Preprocessor category, and add 
DEBUG to the Preprocessor definitions edit control.

- Unresolved External const bits565 link error 

Building the DirectShow Ball and MPGVideo samples with Developer 
Studio version 6.0 produces the following link error.

   error LNK2001: Unresolved external symbol 
      "unsigned long const * const bits565" (?bits565@@3QBKB)

Rebuilding the DirectShow base classes (provided in the
DirectX media classes\base directory) with Developer Studio
version 6.0 and linking with the resulting strmbase library 
(either debug or retail versions) should fix the problem. 

- Documentation

We strongly recommend that you have Internet Explorer 4.01 or 
later installed to view the documentation included with this SDK. 
Some features of the documentation such as inline samples might 
not work properly if you do not have Internet Explorer 4.01 or 
later installed. The compressed HTMLHelp files (.CHM) require 
Internet Explorer 4.01 or later and an updated HTMLHelp control. 
The DirectX media SDK setup can update this control for you 
automatically if you have already installed Internet Explorer 
4.01. Otherwise you can update the control by running HHUpd.exe 
from the Extras directory on the CD.

- Compatibility with previous DirectX foundation versions

Most features of DirectX media are compatible with DirectX
foundation version 3.0 and higher. By default the ddraw.h header
file from the DirectX foundation SDK is marked with DirectX
version 6.0 compatibility. If you want to build your application 
using the DirectX 6.0 foundation SDK and be compatible with an 
older DirectDraw run-time then define DIRECTDRAW_VERSION to be the 
earliest version of DirectDraw with which you want to be compatible. 
For example if you want your application to run on the DirectX 3
run-time then define DIRECTDRAW_VERSION to be 0x0300 in ddraw.h.

Note: The DirectX media SDK no longer ships a version of ddraw.h. 
It used to ship a version marked with DirectDraw 3 compatibility.

- DirectX Media and DEBUG DirectX Foundation

Due to incompatibilities between ddrawex.dll and the debug version
of ddraw.dll you should not install the debug version of DirectX 
Foundation 6.x if you intend to use any components (for example
DirectAnimation and DirectX Transform) of DirectX Media 6.0.

- Optional Transforms Uninstall

The Optional DirectX Transforms provide a separate uninstall
option in the Control Panel Add/Remove Programs applet. These
optional transforms are not removed by the DirectX media SDK
uninstall. 

- DirectX Transform and Windows NT

3-D DirectX Transforms require the DirectX foundation version 
6.0 run-time. These transforms will not work on Windows NT 4.0 
since DirectX foundation is not available on Windows NT 4.0.

Web pages that use DirectAnimation to display DirectX Transforms 
will not work on Windows NT 4.0 with this release of DirectX 
Media. The DirectX Transform Demo page 
(help\dxt\overview\demos_intro.htm) to which the DirectX Media 
SDK program group points is one such web page.

- Internal Protection fault with DirectAnimation and DirectX 5

If you encounter a fault in d3drg16f (the DirectX 5 rgb 
rasterizer) when using DirectAnimation, upgrade to DirectX 6. 
The crash is a DirectX 5 D3DIM bug that was fixed in DirectX 6.
 
-----------------------------------------------------------------
Contact Information 
===================

- Support

See http://www.microsoft.com/support/ for the product support
options available through Microsoft Technical Support.

- Suggestions

Email dxmwish@microsoft.com with suggestions for future versions
of the products which have been part of DirectX Media. We cannot
guarantee replies to all of your suggestions, but we will
carefully consider all ideas.  We appreciate your input.

- Redistribution

Be sure to read the license agreement for redistribution 
information concerning the DirectX Media Run-time.

We'd like to get to know you and your product better. Email 
dxmrdist@microsoft.com to let us know you're shipping our 
run-time. We can perform compatibility testing of your product 
with our future versions to help ensure backwards compatibility.

Include information such as your contact information (name, 
company name, phone/fax, shipping address), company 
focus (web development, games, content creation, etc.), 
product name, and a list of the DirectX Media technologies, 
versions, and tools you're using. Knowing who's using our 
technology helps us better meet your needs. 

DirectX Beta Program

For enrollment in future DirectX beta programs, contact
DirectX@Microsoft.com. The beta coordinator will send you
a non-disclosure agreement and ensure we have your contact 
information. 
