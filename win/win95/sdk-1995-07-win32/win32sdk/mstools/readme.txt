
                Microsoft Win32 SDK for Microsoft Windows
                     Windows 95 Release Candidate 
                            June, 1995


---------------
1. Introduction
---------------

Welcome to the Microsoft Win32 SDK for Microsoft Windows.  This release of the 
SDK is targetted for the Release Candidate of Windows 95.  You can not produce
retail applications using this version of the SDK.

The Windows NT components of this SDK are synched to the retail release of
Windows NT 3.51. (Build 1057)


--------
2. Tools
--------

Most of the tools, headers, libraries, documentation, and sample code in this 
SDK are useful when building applications for both Windows NT 3.51 and 
Windows 95.  Platform specific files are located in appropriately named 
subdirectories, i.e. WIN95 and WINNT.  When the SDK is installed, it detects 
the host operating system and installs any binaries specific to that OS.  All 
of the headers and libraries are installed, so that either operating system 
can host development targeting the other.  If the SDK is installed on a RISC 
platform, samples that will run only on Windows 95 are not installed.


-----------
3. Compiler
-----------

There is no compiler, linker, lib utility, or make utility provided for x86, 
MIPS, or Alpha platforms. You must have Microsoft Visual C++ version 2.0, 
Microsoft Visual C++ version 2.1, or a compatible 32-bit compiler.

The SDK does include a compiler, linker, lib utility, and make utility for the 
PowerPC platform.  The compiler will display a banner each time it is run.  
The banner contains a phone number to be used for registering the compiler.  
Registration is free and relatively easy.  Once the compiler is registered, 
it will not expire.  However, a compiler that is not registered will cease
working 30 days after it is first used.  To register the compiler, call the 
phone number listed in the banner, provide the people on the phone the "code" 
in the banner and receive an "access code" in return.  Run register.exe with 
the access code as the single command line parameter, e.g. 
register <access_code>.

Applications built on PowerPC with the beta version of the SDK, and linked 
with LIBC.LIB or LIBCMT.LIB should be re-linked with the version of these 
libraries provided on this SDK.  The newest libraries include bug fixes in code 
that handles floating point computations.  Certain floating point exceptions
that were masked by the operating system in the beta version are now enabled,
and will show up for the first time if the application is not re-linked.

Visual C++ provides structured exception handling though the use of catch and 
throw.  Although one of the SDK samples uses this technique (oleapt), the 
PowerPC compiler will not support it.  There will be no catch/throw support 
until Microsoft Visual C++ provides a compiler for PowerPC. 


----------------
4. Documentation
----------------

This release of the Win32 SDK contains new technology for viewing the on-line
documentation.  The familiar MSDN browser is now installed with the SDK, and 
all of the core documentation is provided in this format.  The new browser 
offers a unified window containing both the table of contents and the topic 
page.  It also offers the ability to print multiple pages at one time.  Like 
earlier versions of the SDK documentation, most reference pages include a 
QuickInfo popup with platform specifics as well as Group and Overview links 
to help move between related pages.

The "Contents, What's New" menu provides no utility in this release of the 
documentation.


--------------
5. API Changes
--------------

The SYSTEM_INFO structure has been reorganized, although the new design is
backwards compatible.  See the reference page in the on-line documentation
for more information.  The following three flags have been removed from 
winnt.h since they were never used: PROCESSOR_INTEL_860, PROCESSOR_MIPS_R2000, 
and PROCESSOR_MIPS_R3000.  

The TransmitFile function, introduced in the 3.51 beta, has one additional 
parameter which is reserved and should be set to zero.  Failure to add the
parameter will cause a compile time error in modules using this API.

In the OpenGL header files (gl.h) the definition of GLint and GLuint have 
changed from long and unsigned long to int and unsigned int.  This change in
the header may cause new warning messages in preexisting OpenGL code.


-----------------
6. Setup Toolkits
-----------------

This version of the Win32 SDK provides 2 setup toolkits.

The InstallSHIELD kit, provided by Stirling Technologies will run on Windows 95
as well as x86 and alpha versions of Windows NT.  This kit has been developed 
closely with Microsoft, and adheres to the Setup Guidelines that Microsoft has 
developed.  For more information, see the \MSTOOLS\ISHIELD directory.

The Microsoft Setup Toolkit is provided as an alternative for those developers
who require a solution on all RISC platforms as well.  It now includes support 
for silent install.  For information on using this feature, see 
\MSTOOLS\MSSETUP\README.WRI


-------------------
7. More Information
-------------------

Windows 95 and Windows NT both have fully compatible 32-bit implementations of
OLE.  For specific OLE information, see \MSTOOLS\SAMPLES\OLE\OLEREL.WRI.

Windows 95 and Windows NT both have fully compatible 32-bit implementations of
RPC.  For specific RPC information, see \MSTOOLS\SAMPLES\RPC\RPCREAD.ME.

The x86 version of MAPI is supported on both Windows NT and Windows 95.  For
the first time, the 32-bit MAPI headers, libraries, documentation, tools, and 
sample code is integrated into the Win32 SDK.  See \MSTOOLS\MAPI\README.WRI for 
more information.

Win32s version 1.25A is available in the \MSTOOLS\WIN32S\1_25 directory.  This 
contains bug fixes not found in earlier versions.  Win32s version 1.30 Beta is 
also available in the \MSTOOLS\WIN32S\1_30BETA directory.  This release 
contains the new Windows 95 common controls.  As this is a beta release, Win32s 
1.30 Beta is not redistributable.  See \MSTOOLS\WIN32S\README.TXT for more 
information.
