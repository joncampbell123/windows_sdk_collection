
                Microsoft Win32 SDK for Microsoft Windows
                              August, 1996


---------
Contents:
---------

     1. Introduction
     2. New in This Release
     3. Header File Conventions
     4. OpenGL
     5. Windows Sockets 2
     6. Windows Telephony (TAPI) 
     7. RPC and MIDL
     8. OLE
     9. Known Issues


---------------
1. Introduction
---------------

Welcome to the Microsoft Win32 SDK for Microsoft Windows.  This release of the 
Win32 SDK provides the tools and information necessary to build retail 
applications for Windows 95 and Windows NT version 4.0.  It is possible to 
install the Win32 SDK on Windows NT version 3.51, and it is possible to build 
applications that will run on Windows NT version 3.51.  However, some of the 
Windows NT-specific tools in the SDK will not run correctly on Windows NT 
version 3.51.

To install the Win32 SDK on a development machine, run SETUP.EXE from the root 
of the SDK.  The setup program will detect whether the target machine is running 
Windows 95 or Windows NT, the processor type (if Windows NT), and the default 
language.  The setup program will copy the appropriate pieces of the Win32 SDK 
to the target hard drive depending on these three values.

Most of the utilities, headers, libraries, documentation, and sample code in the 
Win32 SDK are useful when building applications for both Windows 95 and Windows 
NT.  Platform-specific files are located in appropriately named subdirectories: 
WIN95 and WINNT.  Language-specific files are usually located in a subdirectory 
on the Win32 SDK compact disc (for example, \MSTOOLS\DEBUG\JAPAN); however, they 
are installed in the standard SDK directories (for example, \MSTOOLS\DEBUG).

There is no compiler, linker, lib utility, or make utility provided with this 
SDK.  This version of the Win32 SDK has been thoroughly tested with Microsoft 
Visual C++ version 4.x.

For a general introduction to the Win32 SDK, including many of the tools, see 
the Getting Started help file (\MSTOOLS\HELP\SDKSTART.HLP).

Windows 95, OEM Service Release 2 is the most recent OEM (Original Equipment 
Manufacturer) release of the Windows 95 operating system.  Beginning in the 
second half of 1996, this is the version of Windows 95 that most computer 
manufacturers will pre-install on new machines.  Windows 95, OEM Service 
Release 2 will be made available to MSDN customers in a release later this 
year.  Since that release will not include an update of the Win32 SDK, all 
of the API information about OEM Service Release 2 is included in this Win32 
SDK.  See the online documentation for additional information.


----------------------
2. New in This Release
----------------------

Windows 95 debug binaries are provided for the following localized versions: 
USA, China, Japan, Korea, NEC, Taiwan, Arabic and Hebrew.  For all eight 
locales, the debug binaries are provided for build 950 of Windows 95.  Since 
August of 1995, Microsoft has released OEM Service releases of Windows 95 with 
later build numbers.  If the build number of the Windows 95 system that the SDK 
is being installed to does not equal 950, then the debug binaries are not copied 
to the system.

DirectX is now implemented in Windows NT 4.0, with the exception of Direct3D. 
See \MSTOOLS\SAMPLES\DIRECTX\README.TXT for more information on DirectX. See 
\MSTOOLS\D3DFORNT\README.TXT for more information on the beta release of 
Direct3D for Windows NT that is provided in the SDK.

Microsoft Internet Server Application Programming Interface (ISAPI) is included 
as an integrated part of the SDK.  The sample code is located in 
\MSTOOLS\SAMPLES\WIN32\WINNT\ISAPI, the two headers (HTTPEXT.H and HTTPFILT.H) 
are installed to the \MSTOOLS\INCLUDE directory, and the documentation is 
located under the "Internet" table of contents entry in the general SDK 
documentation.

Windows 95 introduced a new INF file format used for installing drivers and 
utilities.  This INF file format is now supported on Windows NT 4.0 and is now
described in the online SDK documentation.  

Windows NT 4.0 has a new SETUPAPI DLL that works with the new INF file format 
and will be useful for writing install programs.  This new setup API is 
described in the online SDK documentation.  There is a version of SETUPAPI.DLL 
and CFGMRG32.DLL provided in the SDK that may be used for Windows 95.  The 
CFGMRG32.DLL provided for Windows 95 is a pure stub DLL needed only so that 
SETUPAPI.DLL will load.  See the \MSTOOLS\SAMPLES\WIN32\WINNT\INFINST sample
for more information.

The interface to the system performance counters has been abstracted in the
new "performance data helper" DLL.  This new DLL is fully described in the 
online SDK documentation.  See sample code in
\MSTOOLS\SAMPLES\WIN32\WINNT\PERFTOOL for more information.

Windows NT 4.0 introduces a new access-control-list management API that is 
fully described in the online SDK documentation.  See sample code in 
\MSTOOLS\SAMPLES\WIN32\WINNT\SECURITY\ACLAPI for more information.

Previous releases of the Win32 SDK included a statically linked library that 
provided support for the Simple Network Management Protocol (SNMP).  This 
library has been removed.  The functionality has been moved into a dynamic-link 
library named SNMPAPI.DLL.  This new DLL is distributed with Windows NT 4.0.  
ISVs that want to use the DLL on Windows NT 3.51 or on Windows 95 should see the 
\MSTOOLS\SNMPAPI directory on the Win32 SDK compact disc.

The MSTOOLS\RPC_RT16 directory contains the RPC run-time components for MS-DOS 
and 16-bit Windows.  The MSTOOLS\RPC_SDK directory contains the SDK components 
for building RPC applications for MS-DOS, 16-bit Windows, and the Macintosh.  
The two directories named RPC_DOS and RPC_MAC on previous releases of the Win32 
SDK have been superseded by the new RPC_RT16 and RPC_SDK directories.

The compact disc containing the checked build of Windows NT Workstation 4.0 
contains a new tool in the PAGEHEAP directory that will be useful to many 
application developers.  The Pageheap tool provides an alternate system heap 
manager that aligns each allocation at the end of a separate virtual page, and 
marks the next virtual page with NO_ACCESS.  In this way, read or write 
operations beyond the end of the heap allocation will cause an immediate access 
violation, allowing you to more quickly identify errors in your applications. 


--------------------------
3. Header File Conventions
--------------------------

In the header files, information guarded by 
    #if _WIN32_WINNT >= 0x0400
is implemented only in Windows NT version 4.0 and later.  It is not implemented 
in Windows 95.  This precompiler guard allows you to do compile-time checking 
for platform differences.  

The value of _WIN32_WINNT is set in WIN32.MAK, depending on the platform you 
choose to target.  By default, WIN32.MAK now sets the TARGETOS to WINNT and the 
APPVER to 4.0.  As a result, by default, _WIN32_WINNT is now defined as 0x0400.  

If you are building an application to run on Windows 95 and you want compile-
time notification of compatibility issues, set TARGETOS=BOTH in your makefile. 
When TARGETOS is defined as BOTH, _WIN32_WINNT is not defined for the 
precompiler, and the only information parsed at compile time will be applicable 
to both Windows 95 and Windows NT. 

If you do not include WIN32.MAK in your makefile, you need to explicitly define 
_WIN32_WINNT as 0x0400 to get some of the new Windows NT 4.0-specific material 
from the header files.

There are several API sets present in Windows 95, OEM Service Release 2 that are 
still guarded by (_WIN32_WINNT >= 0x0400), e.g. the Cryptography API.  If you 
are writing an application specifically for Windows 95, OEM Service Release 2, 
and you want the header files to provide compile time access to these APIs, it 
is necessary to define _WIN32_WINNT as 0x0400.  Notice that an application that
uses these technologies will not run correctly on the retail release of Windows 
95.  The vast majority of application programs that are expected to run on
Windows 95 should be built without defining _WIN32_WINNT.

The value of MB_SERVICE_NOTIFICATION has changed between releases of Windows NT 
3.51 and Windows NT 4.0.  See WINUSER.H for the old and new values.  Windows NT 
4.0 provides backward compatibility for pre-existing services by mapping the old 
value to the new value in the implementation of the MessageBox and MessageBoxEx 
functions.  This mapping is performed only for executables that have a version 
number, as set by the linker, that is less than 4.0.

If you want to build a service that uses MB_SERVICE_NOTIFICATION, and that will 
run on both Windows NT 3.x and Windows NT 4.0, you have two choices:
      1.  At linktime, specify a version number less than 4.0; or
      2.  At linktime, specify a version number equal to 4.0, detect
          the version of the operating system at run time, and dynamically
          choose the correct value.  The sample code module
          \MSTOOLS\SAMPLES\MAPI\COMMON\MAPIDBG.C demonstrates this 
          technique.

In version 4.1 of the Microsoft Visual C++ compiler, "bool" has become a 
reserved token.  This conflicts with the OAIDL.H and OBJIDL.H header files where 
"bool" is used as a field name, and it will cause the following warning: "C4237: 
nonstandard extension used : 'bool' keyword is reserved for future use."  It is 
possible to turn off this warning message by using #pragma warning( disable : 
4237).

Previous releases of the Win32 SDK included definitions in WIN32.MAK to map 
structured exception handling keywords to their proper underscored names:
    try -> __try 
    except -> __except 
    finally -> __finally 
    leave -> __leave

This caused problems for developers who chose to use C++ structure exception 
handling where "try" is supposed to be "try" and not "__try".  For this reason, 
by default, the mapping has been removed from WIN32.MAK. This  may cause build-
time errors for your applications.  To get the old behavior add 
    SEHMAP = TRUE 
to your makefile before including WIN32.MAK.


---------
4. OpenGL
---------

The Windows NT 4.0 and Windows 95 releases of OpenGL includes new functionality 
and performance enhancements.  These include:

1) A complete implementation of OpenGL 1.1.  OpenGL 1.1 contains several 
functions, including vertex array, polygon offset, logic ops, and several new 
functions for handling textures.  The vertex array and texture calls are 
particularly significant, as they may enable order of magnitude performance 
improvements in some applications.

2) Overlay planes extensions.  These Microsoft OpenGL extensions permit 
applications to manage and render into overlay planes, where supported in the 
graphics hardware.  This permits applications to display dialog boxes and other 
UI features without overwriting 3-D renderings.

3) Extended metafile support.  Applications may encapsulate OpenGL calls and 
data in GDI extended metafiles.  This, together with Windows NT 4.0 print 
spooler enhancements for remote metafile rendering, makes it possible to print 
OpenGL graphics at high resolution on the print server.  This feature is limited 
to the Windows NT release.

4) Microsoft extensions.  The Microsoft OpenGL implementation also supports 
these performance extensions: GL_WIN_swap_hint, GL_EXT_bgra, and 
GL_EXT_paletted_texture.  They improve the performance of some applications 
significantly.

5) Performance.  The software renderer has been tuned for this release.  
Performance tuning has been carried out for the front end of the OpenGL pipeline 
as well as for rendering particular primitives, especially anti-aliased lines 
and texturing.  Software rendering is generally two to four times faster.

6) OpenGL hardware acceleration.  This release of OpenGL supports a simpler 
mini-client driver (MCD) model to accelerate 3-D graphics operations.  In 
particular, Windows NT 4.0 includes a Matrox Millennium mini-client driver that 
accelerates OpenGL functions.  A corresponding driver for Windows 95 is expected 
to be available later in 1996.

OpenGL is provided for Windows 95, both as an ISV redistributable, and a built-
in component of Windows 95 OEM Service Release 2.

For more information on OpenGL, see \MSTOOLS\SAMPLES\OPENGL\README.TXT.
 

--------------------
5. Windows Sockets 2
--------------------

Windows NT 4.0 includes the new Windows Sockets 2 programming interface.  The 
complete Windows Sockets 2.2 specification is available on the Win32 SDK compact 
disc in the \DOC\SPEC\WINSOCK2 directory.  The Win32 SDK online documentation 
(WIN32SDK.MVB and SPK.MVB) includes much of the material that is in the 
specification.  

The new header files (WINSOCK2.H and WS2SPI.H) are in \MSTOOLS\INCLUDE, the 
library (WS2_32.LIB) is installed in \MSTOOLS\LIB, and there is new sample code.  
See MSTOOLS\SAMPLES\WIN32\WINSOCK2\README.TXT.  The SPORDER.EXE tool is 
installed in \MSTOOLS\BIN and will allow you to order the service providers that 
you have installed on your system.  A SPORDER.DLL makes the same functionality 
available programmatically.  In this way your installation program can set the 
service provider order without requiring user input.  See \LICENSE\REDIST.TXT 
for the terms under which SPORDER.DLL may be redistributed.

To rebuild your application with WinSock2 rather than WinSock1:  If your 
makefile includes WIN32.MAK (as is recommended), and your source files include 
windows.h (as is recommended), you will get the correct winsock.h header and the 
correct ws2_32.lib library by default.  If your code includes winsock.h 
directly, and you simply change this to include winsock2.h, you will get 
undefined externals in mswsock.h at compile time.  Don't include winsock2.h 
directly; include windows.h instead.  Notice that sample code in previous 
releases of the Win32 SDK occasionally included winsock.h rather than windows.h.  
It has now been fixed.

The Windows NT DDK includes sample code for a "Windows Sockets helper DLL."  If
your network service provider is written to TDI as the top level interface, you
will find that providing a helper DLL is the quickest way to produce a Windows
Sockets 2 service provider for Windows NT 4.0.

Run-Time Components
-------------------

When you install Windows NT 4.0, Windows Sockets 2 is installed automatically 
with the operating system.  The Microsoft network transport stacks in Windows NT 
4.0 support quality of service only as "best effort."  In other words, the 
XP1_QOS bit in the service flags is not set.  The Microsoft network transport 
stacks in Windows NT 4.0 do not support "callee data" in the condition function 
of WSAAccept.

A beta release of the Windows Sockets 2 run-time components for Windows 95 is 
available on the internet.  For the most recent updates, see the Microsoft and 
Intel FTP sites:  ftp://ftp.microsoft.com/bussys/winsock/winsock2/ and 
ftp://ftp.intel.com/pub/winsock2/.


---------------------------
6. Windows Telephony (TAPI) 
---------------------------

The TAPI 2.0 components included with Windows NT 4.0 are:

*  Core TAPI Components (TAPISRV.EXE, TAPI.DLL, TAPI32.DLL). TAPISRV.EXE
   is now the core module of TAPI. TAPI.DLL (16-bit) and TAPI32.DLL (32-bit) 
   load into the application process and communicate with TAPISRV.EXE. See
   the TAPI section of the Win32 SDK Documentation for further information.

*  Telephony Service performance monitoring add-in (TAPIPERF.DLL). Allows
   the Windows NT Performance Monitor to track items such as active lines, 
   calls (incoming and outgoing), applications, and so on.

*  Telephony Control Panel (TELEPHON.CPL) and its associated help file.

*  Kernel-Mode Device Driver Service Provider (KMDDSP.TSP, NDISTAPI.SYS). 
   Supports TAPI drivers that execute entirely in kernel mode, such as NDIS 
   WAN miniport drivers (which support ISDN, Switched 56, and so on).

*  Universal Modem Driver Service Provider (UNIMDM.TSP, MODEM.SYS and 
   several other associated components). This TSP has functionality equivalent 
   to that of the Unimodem that shipped with Windows 95 (except that it does
   not support VoiceView). It also does not include the voice or DTMF handling
   features of Unimodem/V. It has been updated to the TAPI 2.0 32-bit 
   architecture and enhanced to simplify the configuration of large
   numbers of modems.

*  Phone Dialer accessory (DIALER.EXE) and its associated help file.

In addition, the Dial-up Networking and HyperTerminal components included with 
Windows NT 4.0 use TAPI when placing or answering calls on modems and ISDN 
adapters.


The TAPI 2.0 components included with the Win32 SDK are:

*  TAPICOMM sample. This sample demonstrates how to write a Win32-based 
   TAPI application that makes datamodem calls.

*  ACD (Automatic Call Distribution) samples ACDSMPL and ACDCLNT. These 
   samples demonstrate basic functionality for an ACD Proxy application 
   and an ACD Agent application.

*  DIALER sample. The source code of the 32-bit Phone Dialer accessory is 
   included with the samples.

*  ATSP32 sample. a sample Telephony Service Provider that shows how to use 
   a modem for speed dialing voice calls.

*  TAPI Browser (TB20.EXE). This is the primary tool for TAPI testing.  It is 
   used for learning the response and return values to all TAPI function 
   calls.  Before writing any TAPI code, learn to use TAPI Browser.

*  A Test Service Provider (ESP32.TSP, ESPUI.DLL, ESPEXE.EXE).  This component 
   can be used to generate most TAPI messages and structure fields for testing 
   TAPI applications. It is a simple PBX simulator, in that if you create 
   multiple line devices in ESP, you can place calls between them, and the 
   expected call states and messages will be automatically generated.

*  The Repeater Service Provider (REPEATER.TSP, REPAPP.EXE, REPSETUP.EXE, and 
   associated help file). Repeater is installed between TAPISRV and any other 
   TSP, and stores in a log file all function calls and messages across the 
   TSPI. This utility can be used for debugging, and, eventually, with a TSP 
   emulator from Microsoft to enable the testing of applications in the absence 
   of hardware that would be needed to test the "real" TSP.

*  Libraries and headers (TAPI32.LIB, TAPI.H, TSPI.H).

*  Documentation for TAPI and TSPI is included in the SDK help viewer 
   databases (WIN32SDK.MVB and SPK.MVB).


Getting Started
---------------

Application developers will find that TAPI 2.0 is backward-compatible with 
earlier versions of TAPI. Applications written for Windows 3.x and Windows 95 
will run without modification.

Service provider developers have a bit more work ahead. Service providers for 
Windows 3.x or Windows 95 have a 16-bit DLL as their top-most component, and 
these are not supported under TAPI 2.0 on Windows NT. If you are porting an 
existing service provider to Windows NT, your primary task is to redesign it as 
a Win32 DLL, multithread and multiprocessor safe, that compiles and runs on all 
RISC platforms as well as x86 platforms. You will also find that it is necessary 
to provide new UI functions in a DLL that can be loaded into the application 
process, while the main service provider DLL runs in TAPISRV's process (see the 
TAPI documentation for details). If your service provider currently launches a 
companion EXE for handling asynchronous events, you will likely find that this 
is no longer necessary; instead, you can create threads in TAPISRV's context.  
On Windows NT, all strings passed through the service provider interface are now 
Unicode.  Refer to the documentation for more information.

If your service provider communicates with hardware using a serial interface on 
Windows 3.x or Windows 95, conversion to Win32 should be relatively 
straightforward using the built-in Windows NT serial drivers. If you directly 
access hardware on the local system, however, you will need to provide a Windows 
NT kernel-mode device driver. Porting a Windows 3.x or Windows 95 VxD to Windows 
NT is not a trivial task, but help is available through Microsoft Developer 
Support.

If you are developing a server service provider to control a PBX CTI link or 
other multiple-line device, you need to write the code to handle the protocol 
stack on the CTI link. This code needs to manage states for many devices and 
calls, and the communications over the link. This is true whether that is a LAN 
transport or something else.


Registry Information
--------------------

TAPI configuration information can be found in the registry under the following 
key:

     \HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Telephony

During initial testing, you can use REGEDT32.EXE to edit the \Providers section 
under this key to add or remove providers. Or you can use the Telephony Control 
Panel when you have implemented TSPI_providerInstall.

Warning: No production (shipping) code should assume anything about what is in 
the registry, or that the location of any specific information will not change. 
Microsoft reserves the right to alter or remove any registry keys or values 
without notice.  The information provided regarding registry keys and values is 
only intended to help during development and debugging.


Debugging
---------

You can enable TAPI debug output by creating and changing DWORD values found 
under the registry key mentioned earlier from 0 (no debug output) to 4 (maximum 
debug output). Value names follow the convention 
"<ModuleName>DebugLevel", e.g., "Tapi32DebugLevel", "TapisrvDebugLevel", 
"esp32DebugLevel", and so on.

To debug TAPISRV.EXE (the process context in which service providers run), you 
can do one of the following:

 (1) Start REGEDT32.EXE and change the ImagePath value under

          \HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tapisrv

     from "<path>\tapisrv.exe" to "ntsd -g -G -x <path>\tapisrv.exe" (where 
     <path> is your Windows NT SYSTEM32 directory path). 

     Then, open the Services control panel, select "Telephony Service," 
     click the Startup button, and check the "Allow Service to Interact 
     with Desktop" check box. (Note that this step is also necessary 
     in order to see the "User-specified request results" dialog box in 
     ESP, if you use it, because ESP displays that dialog box in TAPISRV's 
     context.)

 (2) Run PVIEW or TLIST to get a process identifier (PID) for TAPISRV.EXE, 
     and then attach NTSD to TAPISRV as follows:

          start ntsd -g -G -x -p <pid>

     Note that NTSD requires <pid> to be in decimal format. You can start 
     TAPISRV from the command line by typing "NET START TAPISRV".

     The "-g" and "-G" options prevent NTSD from breaking on startup and exit, 
     respectively;  the "-x" option disables first-chance access violation 
     exception handling/breaking in NTSD, allowing a module's exception handler 
     to deal with the exception (if a handler is not found, Windows NT will 
     break).

Some useful NTSD commands are:

     ~*                Shows all the threads in a process 
     ~Nk               Dump the stack of thread N 
     u <addr>          Unassembles at address <addr> 
     dc <addr>         Dumps memory at address <addr> 
     bp <addr>         Sets a breakpoint at address <addr> 
     ?                 Shows other NTSD commands 


TAPI and Client-Server Telephony
--------------------------------

Client-server telephony allows an application to send its call-control functions 
over the LAN to a remote telephony server, which in turn controls a switching 
fabric directly on behalf of that and other clients.  Like all previous versions 
of TAPI, TAPI 2.0 also supports client-server telephony. Because TAPI is 
connection-model independent, you can develop Win32 telephony applications 
without regard to whether they will run against local hardware or over remote 
client-server connections. 

To enable client-server telephony with TAPI, you need a TAPI service provider 
for each client that communicates with a telephony server. Client service 
providers and telephony servers are available from several third parties. 
Microsoft intends to ship a generic client service provider, telephony server, 
and related administration tools, as an add-on for Windows NT 4.0 and Windows 
95, within a few months after the release of Windows NT 4.0 to manufacturing.
  

Known Issues with TAPI 2.0
--------------------------

*  The CompletionMsgText field in LINEADDRESSCAPS is not being converted from 
   Unicode to ANSI in lineGetAddressCapsA. The workaround is to either not
   use the CompletionMsgText, or to convert it to ANSI in the application, or
   to write the application as a Unicode application. This will be corrected in
   a future release.

*  The Telephony Control Panel does not support installation of service
   providers using an OEMSETUP.INF file; the "Have Disk" option in the "Add"
   dialog box does not work. Service providers will need to include either a 
   Setup program or other installation tool that copies all files to the
   correct locations on disk, including placing the TSP file in \winnt\system32.
   Preferably, the Setup program will call the TAPI lineAddProvider function
   itself to get the service provider added to the TAPI environment, but if
   this is not done, the Telephony Control Panel "Add" function *can* be used
   to add the service provider after the files are copied to the correct
   locations. Microsoft strongly encourages service providers to include a
   complete setup program that handles all installation; the most likely
   resolution of this issue will be the elimination of OEMSETUP.INF setup
   capability, rather than fixing it (since this is a holdover from 
   Windows 3.x and no longer a preferred installation method for Win32).


---------------
7. RPC and MIDL
---------------
7.1 RPC Run Time

New datagram features: The datagram transport protocols, ncadg_ipx and 
ncadg_ip_udp now support security (on Windows NT) and cancel operations (on both 
Windows NT and Windows 95).

Support for Banyan Vines SPP transport: The RPC run time includes support for 
the Banyan Vines SPP transport protocol on Windows NT 4.0 clients and servers, 
and on MS-DOS and Windows 3.x clients. Support on Windows NT requires the 
Windows NT edition of Banyan Enterprise Client version  5.56 [30] or newer. 
Please contact Banyan Systems, Inc., for questions relating to the Banyan 
software. For more information on using the transport protocol, see the online 
SDK topics "ncacn_vns_spp" in the MIDL Language Reference, and "String Bindings" 
in the RPC Reference.

New run time support for port restrictions and selective binding to NICs: A new 
RPC data type and a set of Extended RpcServerUseProtseq functions let you 
restrict port allocation for dynamic ports and allow multihomed computers to 
selectively bind to Network Interface Cards.  For more information, see the 
online SDK topic "RPC_POLICY" in the RPC Reference.

DCE pipes are supported on all transport protocols on 32-bit Windows platforms. 
For more information, see the "Pipes" overview in the RPC Reference.

7.2. MIDL Compiler

MIDL/ODL merge: The Microsoft Interface Definition Language (MIDL) now includes 
the Object Description Language (ODL) attributes, statements, and directives, 
allowing you to use the MIDL compiler to generate type library files for OLE 
applications. This replaces the type library generation tool MKTYPLIB.EXE. For 
more information, see the online SDK topic "Generating a Type Library With MIDL" 
in the MIDL documentation. 

New default mode for MIDL compiler: The /ms_ext and /c_ext modes for the MIDL 
compiler are now the default mode. It is not necessary to specify these two 
switches when you compile an IDL script that includes Microsoft extensions to 
the OSF-DCE IDL. A new compiler option, /osf, is available to force strict 
compatibility with OSF-DCE. For more information, see the online SDK topic 
"/osf" in the MIDL Command-Line Reference.

Compiler options /Oic and /Oif offer interpreted methods for marshaling stub 
code between client and server. For more information, see the online SDK topic 
"/Oi" in the MIDL Command-line Reference.

Pipe support: The new MIDL compiler and the RPC run time support DCE pipes to 
pass large or incrementally produced quantities of data in remote procedure 
calls. This release has some restrictions related to pipe types and non-pipe 
arguments in pipe calls. For more information see the online SDK topic "pipe" in 
the MIDL Language Reference and the "Pipes" overview in the Overviews node of 
the RPC documentation.

Performance attributes for data marshaling: Two new attributes, [wire_marshal] 
and [user_marshal], permit faster, more efficient marshaling of datatypes that 
are difficult to remote.  For more information see the online SDK topics "The 
wire_marshal Attribute" and "The user_marshal Attribute" in the RPC 
documentation, and the topics "wire_marshal" and "user_marshal" in the MIDL 
Language Reference.

Array attributes accept multiple parameters: You can use this feature to specify 
a sized array of pointers or a multidimensional section of a multidimensional 
array. For more information on this, and on how to use [out, size_is] to allow 
the server to return a dynamically sized array, see the online SDK topics 
"size_is" in the MIDL Language Reference, "Multiple Levels of Pointers" in the 
RPC documentation, and  the sample programs "dynout" and "strout" in the 
\mstools\samples\rpc directory of the Win32 SDK CD.

Support for international characters: Where appropriate, the MIDL compiler now 
accepts the entire ANSI character set in the input file.

Floating-point constants: The MIDL 3.0 compiler supports floating-point 
constants in IDL and ACF files.

32-bit int: The MIDL compiler recognizes the int data type as a 32-bit 
remoteable element on 32-bit platforms.

Object interface methods return type: Nonlocal object interface member functions 
must have a return type of HRESULT or SCODE. Earlier versions of MIDL allowed 
member functions to have a return type of void. For more information, see the 
online SDK topic "object" in the MIDL Language Reference.

Version control: To guard against using MIDL features that are not supported on 
older platforms, the MIDL compiler generates target macros that facilitate 
compatibility checking during C compilation. For more information, see 
"Targetting Stubs for Specific 32-Bit Platforms" in the MIDL Programmer's 
Reference.

7.3. Known Problems

Static C run time libraries may cause problems with the RpcSm memory package: 
The new CRT library for Windows NT 4.0 may create problems for RPC applications 
that use the RpcSm/RpcSs memory management package instead of MIDL_user_allocate 
and MIDL_user_free, and link with the static version of the C run time 
libraries(LIBCMT.LIB or LIBC.LIB). Note that the dynamic library (MSVCRT.LIB and 
DLL) does not create this problem. 

The problem is this: When an RPC application links to the static versions of the 
C run time libraries, and it uses the RpcSm package, it will have a heap that is 
separate from the one used by RPC run time and stubs. When this happens, the RPC 
application cannot free memory blocks that were allocated by the stubs, and the 
stubs cannot free blocks that were allocated by the RPC application. Because the 
stubs and the application operate on different heaps, these cross references 
cause an access violation in the memory package that is executing the free 
operation on a pointer. Note that this is a problem only if your application 
causes cross-referenced free operations as described earlier.
 
There are two ways to handle this problem:

1. If you must link to the static versions of the C run time libraries, use 
MIDL_user_allocate and MIDL_user_free, and don't compile in /osf mode or use the 
[enable_allocate] attribute.  Implement these memory management routines so they 
operate on your application's heap.

2. If you must use the RpcSm memory management package, link to the dynamic 
version of the C run time library (MSVCRT.LIB and DLL).Linking to the dynamic 
version of the C run time library ensures that your application shares the same 
heap with the RPC run time.

In some situations non-pipe [out] arguments are corrupted when there is an [in] 
pipe but no [out] pipe. Use an [in,out] pipe or an empty [out] pipe as a work-
around.

The compiler doesn't generate correct mixed-mode (/Os) stubs for some cases with 
multidimensional array attributes. If this happens, use one of the interpreted 
modes (compiler option /Oi) instead.

When you define a coclass and a library in an IDL file, but you do not define an 
interface, MIDL fails to generate the .H and _I.C files. You need to use the /h 
compiler switch to generate these files.


---------------------------------------
8. OLE Release Notes for Windows NT 4.0
---------------------------------------

Applications that use OLE should note the following restrictions and features 
available in the Windows NT 4.0 Shell Update Release.

  * Availability of DCOM (Distributed COM)
      Changes from Windows NT 4.0 Beta 2/DCOM Preview
      MIDL/RPC/OLEAUT32.DLL provide marshaling support for VARIANTs
      Receiving OR_INVALID_OXID (0x80070776)
      Extensive Delays on Initial Remote Activation
      TreatAs on Remote Server
      Applications using CLSCTX_SERVER/CLSCTX_ALL do not run on 
      Windows 95 or previous versions of Windows NT if compiled with
      _WIN32_DCOM or _WIN32_WINNT>=0x0400.
  * MkParseDisplayName enables <progid>:<string> syntax
  * Overriding the binding behavior of monikers: Class Moniker,
    IClassActivator, BINDOPTS2
  * New APIs Help Avoid Race Conditions in Multithreaded COM Servers
  * New CoRegisterPSClsid API enables proxy/stub usage without
    registration
  * New OleCreateXXXEx APIs avoid multiple launch and shutdown of servers
  * Property Set Interfaces Available
  * Compound File Optimization APIs Available as Redistributable
    Component for Windows 95, Windows NT 3.51 and Windows NT 4.0
  * Asynchronous Storage interfaces and functions available
  * Component Categories Manager available as redistributable component
  * OLEPRO32.DLL Available in Windows NT 4.0
  * OLE Tutorial Code Samples in mstools\samples\ole\com
  * Marshaling code for OLE Control and ITypeLib2/ITypeInfo2-Interfaces


Availability of DCOM (Distributed COM)
--------------------------------------
Distributed COM (previously referred to as Network OLE) is now an integral part 
of Windows NT 4.0. Distributed COM allows you to use any 32-bit COM component 
across machine boundaries with rich features in concurrency and scalability, 
security, and robustness. For a complete discussion of the new interfaces and 
APIs, please refer to the online SDK documentation.

Changes from Windows NT 4.0 Beta 2/DCOM Preview
-----------------------------------------------
If you were using Windows NT 4.0 Beta 2, please note the following differences 
between Beta 2 and the final version.
  * The COSERVERINFO structure has been changed as follows:
    typedef struct _COSERVERINFO {
        DWORD           dwReserved1;
        LPWSTR          pwszName;
        COAUTHINFO *    pAuthInfo;
        DWORD           dwReserved2;
    } COSERVERINFO;
    The dwSize member has been removed and the pszName member has been
    renamed to pwszName. The additional parameters are described in
    detail in the online SDK documentation. To obtain compatibility
    with Beta 2, the additional members have to be initialized to 0 and
    NULL, respectively.

If you were on the DCOM preview program or attended the Professional Developers 
Conference in March where the DCOM preview for Windows NT 4.0 Beta 1 was 
distributed, please note the following differences between the Beta 1 preview 
and the final release.
  * CoInititializeEx can now be called with different COINIT flags from
    different threads, which supports the mixing of apartment-model and
    free-threaded objects.

  * CoInitializeSecurity now takes additional arguments which merge the
    functionality of CoRegisterAuthenticationServices, which has been
    removed. A call to CoInitializeSecurity(pSD, dwAuthnLevel,
    dwImpLevel, pReserved) can be replaced with a call to
    CoInitializeSecurity(pSD, -1, NULL, NULL, dwAuthnLevel, dwImpLevel,
    NULL, 0, pReserved).

  * Additional levels of impersonation and authentication are now
    supported by DCOM. The SECURE sample application
    (\mstools\samples\ole\dcom\secure) demonstrates the use of these
    security settings.

  * The OLECNFG command-line tool that supported configuring RunAs
    applications has been replaced with a graphical tool, DCOMCNFG,
    which displays information about and configures groups of classes
    packaged together (within the same executable [EXE] or library
    [DLL]) according to their APPID (see following). Run the DCOMCNFG tool
    directly from the command line or by clicking Run on the Start menu 
    and typing DCOMCNFG in the Run dialog box.

  * The LaunchPermissions, AccessPermissions, RunAs, ActivateAtStorage,
    RemoteServerName, LocalService keys/values that were stored under
    each class's CLSID key (HKEY_CLASSES_ROOT\CLSID\{...}) are now all
    named values of a new key associated with the CLSID key known as an
    APPID. Each CLSID has a reference to its APPID key as a named value
    ("AppID") whose value is the stringized GUID that is their APPID.
    The APPID keys are kept under HKEY_CLASSES_ROOT\APPID\{...}, and their
    named values are the attributes and security settings for the
    collection of classes (possibly only one) associated with the APPID.
    This assures that a process exposing multiple class factories
    obtains the same security settings regardless of which CLSID is initially 
    used to launch the process initially.

  * In the beta 1 preview release of DCOM, the security information
    associated with the LaunchPermissions and AccessPermissions keys was
    managed with the registry security APIs. These configuration entries
    are now named values that contain Win32 security descriptors. See
    the Win32 documentation for more information on manipulating security
    descriptors.

MIDL/RPC/OLEAUT32.DLL Provide Marshaling Support for VARIANTs
-------------------------------------------------------------
The marshaling code for IDispatch, dual, and oleautomation interfaces has been 
changed to use standard MIDL generated proxy/stubs. As a by-product of this 
change, VARIANTs, BSTRs, and so on can now be used in any non-Ole Automation COM 
interface, that uses MIDL generated proxy/stubs. However, this new marshaling 
support requires additional functionality in the RPC run time, so that MIDL 
generated proxy/stubs that use VARIANTs will not run on previous versions of 
COM/RPC, including Windows 95 and Windows NT 3.51. The only way to marshal 
VARIANT parameters on non-Ole Automation interfaces on Windows 95 and Windows NT 
3.51 remains to hand-code the proxy/stubs using MIDL's call-as mechanism.

It is important to realize that the wire representation used by proxy/stubs is 
part of the interface contract and cannot be changed without changing the 
interface identifier. Before Distributed COM, this has not been of practical 
concern, since updating the proxy on a machine forcibly updated the 
corresponding stub. Any proxy/stub that is installed on a DCOM-enabled system 
(Windows NT 4.0) effectively freezes the wire format for this interface. For 
designers of new interfaces, this leaves several options in dealing with 
VARIANTs:
  * Use hand-coded proxy/stubs on Windows NT 3.51 and Windows NT 4.0: this will 
    preclude any future use of MIDL/RPC's VARIANT marshaling.

  * Use MIDL generated proxy/stubs on Windows NT 4.0 and hand-coded proxy/stubs
    on Windows NT 3.51: installation programs have to guarantee that no Windows 
    NT 3.51 proxy is ever installed on a DCOM-enabled machine. When upgrading a
    system from Windows NT 3.51 for Windows NT 4.0, the hand-coded proxies must 
    fail, requiring a new installation of the application.

  * Use intelligent proxy/stubs that test for the presence of the new
    marshaling support and use either hand-coded marshaling on Windows NT 3.51


    or MIDL-based marshaling on Windows NT 4.0: this is the most flexible but
    most complicated option.

Receiving OR_INVALID_OXID (0x80070776)
--------------------------------------
When using COM creation APIs (such as CoCreateInstance, CoCreateInstanceEx, or 
CoGetClassObject) to create remote objects (objects on other machines), 
incorrectly configured network settings that otherwise appear to support file or 
print sharing properly may cause Distributed COM to fail with the distinguished 
error, OR_INVALID_OXID (0x80070776). This is typically not a problem with COM, 
but this network configuration problem that is not exposed by other networking 
operations that have fall-back behavior that differs from COM over improperly 
configured protocols.

This failure can occur when a machine has been configured with TCP/IP as well as 
another network transport, such as IPX/SPX, or NetBeui, but the TCP/IP 
configuration is improperly configured. For example, it may be configured to be 
dynamically assigned an IP address using DHCP, yet a DHCP server may not be 
available, so it has no IP address. The solution is to properly configure TCP/IP 
networking.

Extensive Delays on Initial Remote Activation
---------------------------------------------
When establishing an initial DCOM connection between two machines, a significant 
delay -- typically in the order of 30 seconds -- can be perceived, if the 
primary protocol (default: TCP/IP - UDP) is not configured correctly.

DCOM attempts remote activation using one protocol at a time. If a protocol 
indicates a failure, DCOM uses the next configured protocol. If a machine is 
configured to use TCP/IP and IPX/SPX for DCOM, with TCP/IP being the primary 
transport, a faulty TCP/IP configuration (missing DHCP server, for example) will 
not prevent the object from being activated, but the activation will succeed 
only after the TCP/IP transport's time-out of approximately 30 seconds. 

To solve this problem, you must either correct the configuration of the primary 
transport or remove the protocol by using the Network Control Panel.

TreatAs on Remote Server
------------------------
During a remote activation for a specific class, the server does not correctly 
use the TreatAs mappings. Although the correct server process gets launched, COM 
still expects the original CLSID instead of the mapped CLSID. If the server does 
not provide a class factory (through CoRegisterClassObject) for the original 
CLSID, the activation times out after 30 seconds.

Applications using CLSCTX_SERVER/CLSCTX_ALL do not run on Windows 95 or previous 
versions of Windows NT if compiled with _WIN32_DCOM or _WIN32_WINNT>=0x0400.
------------------------------------------------------------------------
The new definitions of CLSCTX_SERVER and CLSCTX_ALL include 
CLSCTX_REMOTE_SERVER. Applications using these flags will not run on Windows 95 
or previous versions of Windows NT when compiled with the header files included 
in this SDK. Previous versions of COM/OLE validate the flags passed to 
CoCreateInstance/CoGetClassObject and fail with E_INVALIDARG when 
CLSCTX_REMOTE_SERVER is specified. When targeting both Windows NT 4.0 and 
Windows 95 or previous versions of Windows NT, you need to make sure the 
_WIN32_WINNT preprocessor symbol is defined to be less than 0x0400 and the 
_WIN32_DCOM preprocessor symbol is not defined.

MkParseDisplayName enables <progid>:<string> syntax
---------------------------------------------------
MkParseDisplayName has been expanded to first check for a "<progid>:" prefix in 
the display name to be parsed. If such a prefix is found, MkParseDisplayName 
proceeds as for the existing "@progid!" syntax. This change simplifies the usage 
of display names: "@file:\\myserver\myshare\myfile.doc" can now be changed to 
"file: \\myserver\myshare\myfile.doc", effectively unifying the URL name space 
with OLE's moniker name space. The protocol prefix of a URL maps to a ProgID 
while the remainder of the URL is handled by the object corresponding to the 
protocol prefix. See the online SDK documentation for details.

Class Moniker, IClassActivator, BINDOPTS2
-----------------------------------------
The File Moniker now allows monikers to be composed to its left. Previous 
versions of File Moniker simply failed any bind operation when they detected a 
moniker to their left. In Windows NT 4.0, the file moniker's 
IMoniker::BindtoObject method delegates either both the detection of the CLSID 
and the instantiation of the object, or merely the instantiation of the object 
to the moniker to its left.

The File Moniker does this by calling IMoniker::BindtoObject on the moniker to 
its left. If this object exposes IClassActivator, the file moniker calls 
IClassActivator::GetClassObject to obtain the uninitialized instance. Otherwise, 
if the object exposes IClassFactory, the file moniker calls 
IClassFactory::CreateInstance. If the object exposes neither, the bind operation 
fails.

Custom monikers should be changed to enable this kind of extensibility, where 
applicable. Typically, this mechanism does not apply for monikers that already 
require or support a moniker to their left, like Item Monikers.

The new Class Moniker for Windows NT 4.0 uses this extensibility mechanism. It 
can be composed with a file moniker to override the CLSID. The Class Moniker can 
be created using the new CreateClassMonikerAPI, using CoCreateInstance (..., 
CLSID_ClassMoniker, ..., IID_IParseDisplayName) or through MkParseDisplayName 
with a display name of the form 
"clsid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxx".

The file moniker also accepts a new BINDOPTS2 structure, an extension of the 
current BINDOPT structure. Clients can use this new structure to 
IBindCtx::SetBindOptions to request additional behavior for a bind operation.
Note:
  Currently, only File Moniker uses the new bind options. COM's new Class
  Moniker does not currently support the pServerInfo flag.
  pServerInfo and the pointers included in pServerInfo are not
  copied when IBindCtx::SetBindOptions is invoked. Thus, these pointers need to 
  be kept valid until the last release to the bind context.

See the online SDK documentation for more details on any of these new 
mechanisms.

New APIs Help Avoid Race Conditions in Multithreaded COM Servers
----------------------------------------------------------------
Multithreaded local servers (apartment- or free-threaded) face a synchronization 
problem with regard to their class factory objects. While a class factory is 
being revoked, another thread can simultaneously be using this same class 
factory. Applications thus have to validate, that after revoking the class-
factory object(s) during normal shutdown (after the global reference count went 
to 0), no new references have been created.

To facilitate this, applications can use the new COM APIs CoAddRefServerProcess, 
CoReleaseServerProcess, CoSuspendClassObjects and CoResumeClassObjects. See the 
online SDK documentation for details.


New CoRegisterPSClsid API Enables Proxy/Stub Usage Without Registration
-----------------------------------------------------------------------
COM typically obtains the proxy/stub for a given interface from entries under 
the HKEY_CLASSES_ROOT\Interface registry key. When a COM object does not have 
write privileges to the registry (it may be untrusted code running in a 
restricted process), it can use the CoRegisterPSClsid API to temporarily 
register an IID to CLSID mapping for its process. See the online SDK 
documentation for details.

New OleCreateXXXEx APIs Avoid Multiple Launch/Shutdown of Servers
-----------------------------------------------------------------
Existing instantiation functions, (OleCreate, OleCreateFromFile, 
OleCreateFromData, OleCreateLink, OleCreateLinkToFile, and 
OleCreateLinkFromData) create only a single presentation or data format cache in 
the default cache location during instantiation. Most applications require 
caching at least two presentations (screen and printer) and may require caching 
data in a different format or location from the handler. Because of this, 
applications must typically launch and shut down the object server multiple 
times to prime their data caches during object creation (Insert Object, Insert 
Object from File, and Paste Object).

Extended versions of these creation functions solve this problem. See the online 
SDK documentation for details on OleCreateEx, OleCreateFromFileEx, 
OleCreateFromDataEx, OleCreateLinkEx, OleCreateLinkToFileEx, and 
OleCreateLinkFromDataEx.


Property Set Interfaces Available
---------------------------------
If your applications formerly created OLE property sets on their compound files 
by writing the binary property format according to the "OLE 2 Property Set 
Binary Format Specification," look in the SDK documentation for information 
about the IPropertyStorage and IPropertySetStorage interfaces new to Windows NT 
4.0. Use of these interfaces to create and manipulate property sets on compound 
files is preferred over manually reading and writing the binary format.

Compound File Optimization APIs Available as Redistributable Component
----------------------------------------------------------------------
Compound Files provide a file system within a file. The data within a compound 
file is allocated in blocks of 512 bytes, which can be fragmented 
(noncontiguous) just like files on any file system. The Compound File 
Optimization Tool (DFLAYOUT.EXE) provides a way of optimizing the order of the 
data sectors within a Compound File similar to a hard disk 
defragmentation/optimization tool. Unlike a hard disk optimization tool, 
DFLAYOUT.EXE arranges the data according to the access pattern of the 
application that loads the Compound File. It launches the application and 
monitors any read/write accesses to the file. It then places data that was used 
first by the application at the beginning of the file.

The tool's primary use is in conjunction with asynchronous storage (see 
following), where it allows applications to do optimal progressive rendering 
while a file is being downloaded asynchronously. An optimized file also provides 
performance advantages in standard network access and even local access 
scenarios as it minimizes seek operations and takes advantage of read-ahead 
caching. Access on compact discs or other storage media with high seek latency 
also benefit greatly from an optimized file 
layout. 

The tool provides a simple, dialog-based UI that allows the user to select 
multiple Compound Files that are to be optimized. When the user clicks the 
Optimize button, the tool will obtain the CLSID from each Compound File, launch 
the associated application (using CoCreateInstance) and initialize it using 
IPersistStorage. The tool monitors the activity of the application and writes an 
optimized Compound File based on this activity.

The tool uses the following Layout Optimization interfaces and APIs that are 
implemented in DFLAYOUT.DLL. 

  * StgOpenLayoutDocfile

  * ILayoutStorage

These APIs enable application-specific optimization tools and even integrated 
optimization support within applications. 

Please refer to the online SDK documentation for more information on the layout 
optimization interfaces and APIs.  The DFLAYOUT.DLL is NOT part of Windows NT 
4.0. Instead it is provided in the Win32 SDK as redistributable code in the form 
of a self-extracting executable DFLAYDLL.EXE. It is ONLY redistributable through 
the use of this executable (MSTOOLS\BIN\<platform>\DFLAYDLL.EXE for Windows 95, 
Windows NT 4.0, and Windows NT 3.51). See details on redistribution rights in 
REDIST.TXT in the \License directory. 


Asynchronous Storage Interfaces/APIs Available
----------------------------------------------
The following interfaces and APIs are available in OLE32.DLL:
  * StgOpenAsyncDocfileOnIFillLockBytes

  * StgGetIFillLockBytesOnILockBytes

  * StgGetIFillLockBytesOnFile

  * IFillLockBytes

  * IProgressNotify

Please refer to the online SDK documentation for information on
Asynchronous Storage.

Component Categories Manager Available as Redistributable Component
-------------------------------------------------------------------
The Component Categories Manager enables enumeration of COM components by their 
capabilities and by the capabilities they require from their containers. It 
implements the Component Category Standard (refer to the ActiveX SDK for more 
details), which extends the existing tagging mechanism ("Control", "Insertable" 
and so on) to a GUID-based categorization mechanism.

The Component Categories Manager is an in-process COM-object (COMCAT.DLL) that 
exposes the ICatInformation and ICatRegister interfaces. See the ActiveX SDK for 
a description of these interfaces and for header files (COMCAT.IDL/H).

The Component Categories Manager is provided as a self-extracting, self-
registering executable. It is ONLY redistributable through the use of this 
executable (MSTOOLS\BIN\<platform>\CCDIST.EXE for Windows 95 and Windows NT 4.0, 
MSTOOLS\BIN\<platform>\CCDIST35.EXE for Windows NT 3.51). For details on 
redistribution rights see the REDIST.TXT file in \License.

OLEPRO32.DLL Available in Windows NT 4.0
----------------------------------------
The OLEPRO32 library, which offers APIs to assist in the writing of OCX's and 
ActiveX controls, is now shipped with Windows NT 4.0. In Beta 1 of Windows NT 
4.0, this library was offered as a redistributable in the SDK. Before the Beta 1 
release of Windows NT 4.0, this library was offered as a redistributable part of 
the Microsoft Visual C++ Control Developers Kit for Windows 95 and Windows NT.

OLE Tutorial Code Samples in mstools\samples\ole\com
----------------------------------------------------
This release includes a new tutorial series of OLE code samples. Though 
individual samples can be extracted as a framework for further development, the 
series is a graduated sequence illustrating Win32 coding techniques using the 
COM Component Object Model and OLE services.  The series covers COM object 
construction using custom interfaces, component object construction in a server 
housing, in-process servers, licensing of component objects, standard marshaling 
of custom interfaces, out-of-process servers, multithreaded apartment model 
construction, Distributed COM (DCOM), multithreaded clients of free-threaded 
component objects, connectable component objects, and structured storage using 
the compound file services of OLE. Extensive comments are provided in the source 
code. Comprehensive tours of the internal behavior of the samples are also 
provided in separate .TXT files. These new tutorial samples are located in 
branch \MSTOOLS\SAMPLES\OLE\COM of the installed Win32 SDK. See TUTORIAL.TXT in 
the COM directory for more details.

Marshaling Code for OLE Control and ITypeLib2/ITypeInfo2 Interfaces
-------------------------------------------------------------------
Application writers need to know that in order to use any of the following new 
features in OLEAUT32.DLL, they must cause OLEAUT32.DLL to register itself.

This can be done in one of two ways:

1. regsvr32 oleaut32.dll (regsvr32 is provided in the Win32 SDK).

2. Call the DllRegisterServer() entrypoint in oleaut32.dll directly (this is 
what regsvr32 does):
   HMODULE hmodule;
   FARPROC pfn;
   hmodule = LoadLibraryA("oleaut32.dll");
   pfn = GetProcAddress(hmodule, "DllRegisterServer");
   pfn();      // call the DllRegisterServer routine in oleaut32.dll
   FreeLibrary(hmodule);

This enables the following new functionality in oleaut32.dll:
*  Enables remoting of the new ITypeLib2 and ITypeInfo2 interfaces
*  Enables the OLE Controls interfaces to be accessed in remote clients
*  Enables access to the Standard Font and Standard Picture objects,
   and to their remoting code.

OLE Automation for Windows NT 3.51
----------------------------------
An update to the OLE Automation components for Windows NT 3.51 and Windows 95 is 
available in the \MSTOOLS\OA351 directory.  This DLL and TLB are installed on 
Windows 95 by SDK setup.  See LICENSE\REDIST.TXT for information on 
redistribution rights. These two files are already built into Windows NT 4.0.


---------------
9. Known Issues
---------------

In order to put the Win32 SDK headers, libraries, and tools first on the search 
path, the INCLUDE, LIB, and PATH environment variables are modified.  Doing this 
on Windows 95 may overflow the environment space of COMMAND.COM.  To work around 
this, either add the following line to CONFIG.SYS:

    SHELL=c:\command.com c:\ /e:2048 /p

or open the Properties dialog box from the COMMAND.COM window, select the Memory 
tab, and set the "Initial environment" to the maximum value.

APIMON is a new tool in the Win32 SDK that will run only on Windows NT 4.0.  It 
is provided as a replacement for the old WAP and LOGGER32 tools.  APIMON 
provides an easy to use, graphical interface for gathering information on API 
calls and page faults.  It is documented in apimon.hlp.

On the MIPS processor, the values listed in the time column of the API Counters 
window are invalid.  Selecting the graph button when a graph is already displayed 
causes APIMON to fail.

The Z*.DLL files (e.g. zernel32, zser32, zdi32, ...) that were provided on 
previous releases of the SDK were to support the WAP and LOGGER32 tools.  These
DLL files are no longer present since the WAP and LOGGER32 tools have been 
replaced by APIMON.  The apfcvt and apf32cvt tools are still provided for use 
with FERNEL32.DLL and the FIOSAP tool (see the online SDK documentation for 
more information).  The apf32cvt tool will not work correctly on Windows 95.

The MIB compiler (MIBCC.EXE) requires MGMTAPI.DLL in order to run.  This DLL is 
distributed with the operating system, and will be installed when you install 
the SNMP service on your machine.  See the "Network" item in the Control Panel 
for more information.

The message compiler (mc.exe) does not compile Japanese-language strings 
correctly when run on the Japanese version of Windows 95.  The recommended work-
around is to use the Japanese version of Windows NT.

The \MSTOOLS\SAMPLES\MAPI\DOCFILE.MS sample does not compile correctly with 
Microsoft Visual C++ version 4.1 or version 4.0a for RISC.  This is a recognized 
limitation in that version of the VC++ compiler, and it is fixed in version 4.2 
and later.

Win32s has been removed from the Win32 SDK.  It is available on another 
Microsoft Developer Network compact disc.

The WINDBG debugger no longer supports Win32s.

The mssetup environment variables are no longer registered by default.  To build 
the mssetup sample and other code that uses the mssetup toolkit, run 
\mstools\mssetup\setenv.bat first.  Notice that there is now a 
\mstools\include\setupapi.h header file that is different than the older
\mstools\mssetup\include\setupapi.h header with the same name.  If you are
building code that uses mssetup, be sure that you use the older file.

Some of the sample code in the Win32 SDK requires LIBCMT.LIB in order to build.
This library is distributed with Microsoft Visual C++, but it is not installed
by the minimum installation.  If your code will not build because the linker is
not able to locate this library, either grab it from the VC++ distribution media
or reinstall VC++ making sure that this library is included.

The documentation indicates that the szKey member of the String and StringTable 
structures located in version resources can use the decimal value 1200 to 
indicate the Unicode character set.  This is not correct.  You should use the 
hexadecimal value 04b0 to indicate the Unicode character set.  See the BLOCK
string in the version resources of \mstools\samples\win32\deb\deb.rc for one 
example.

There online documentation includes references to "ActiveX."  This should not
be confused with the ActiveX SDK that is available on the internet and that 
will be provided to MSDN customers later this year.  

As in previous releases of the Win32 SDK, the DBG symbols for some of the 
Windows NT system components are provided on the SDK compact disc, in 
\SUPPORT\DEBUG\*.  The Win32 SDK setup logic has been changed slightly so that 
if you run setup on a machine with a build number that matches the build of the 
DBG symbols, you will not be prompted for the location of the symbols.  If you 
set up the SDK on a machine with a build number that does not match, you will be 
prompted, as has been the case in the past, for the location of the correct DBG 
symbols.

To fit all of the new material on a single compact disc, the debug symbols and 
the libraries have been compressed on the compact disc.  The regular SDK setup 
will expand these files automatically as part of setup.  If you copy the debug 
symbols or the libraries without using SDK setup, you will need to run 
EXPAND.EXE to decompress the files.  The EXPAND utility is built into Windows 
NT, and is provided in the SDK for Windows 95.

The install batch file for the Software Compatibility Test (SCT) has been 
significantly changed to make it easier to debug 16-bit applications on Windows 
NT.  See \SCT\SCT.HLP for additional information on using install, setdebug, and 
nondebug utilities.  Note that SCT installation now requires you to reboot at 
the end of the process.  

The client dynamic-link libraries for the licensing API (LSAPI32.DLL and 
MSLSP32.DLL) are still beta code.  They are provided here for development 
purposes only.  They are not redistributable.  The interfaces may change in the 
future, requiring applications to be recompiled.
