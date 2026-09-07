           Microsoft Platform Software Development Kit (SDK)
                        April 2000 Edition
                             Readme.Txt

=========
Contents
=========

1. General Release Notes and Known Issues
2. Tested Compilers and New Linker
3. Header File Conventions
4. RPC/MIDL Release Notes
5. COM+ (Component Services) Developer Release Notes
6. Windows Management Instrumentation (WMI) Release Notes
7. Windows Debuggers and Visual C++ 6.0 Support
8. Platform SDK Content Overview
9. Other Known Issues
10. AD/ADSI Release Notes
11. Windows Media(TM) Services
12. Building Visual Basic DirectX Samples
13. Microsoft® SDK for Java 4.0
14. Future Microsoft Support of the Alpha Processor
15. Incorrect Disk Space calculation
16. Upgrading previous Platform SDK installations
17. Remote Access Service -- Invoking the Custom Scripting DLL
18. Providing Feedback on the Platform SDK


============================================
1.0  General Release Notes and Known Issues
============================================

To get the latest released version of the Platform SDK, subscribe 
to the Microsoft Developer Network (MSDN) or visit our Web site at
http://msdn.microsoft.com/developer/sdk/. An MSDN Professional 
subscription is the best way to receive current Microsoft operating
system beta versions, releases, and the Platform SDK.

The Platform SDK includes support for Windows(R) 2000, Windows NT(R) 4.0, 
Windows 98, and BackOffice(R), and limited support for Win64(TM) development.

1.1 Setup
---------

The setup program for the Platform SDK is the Microsoft Windows Installer. 
If you have feedback on the SDK setup program, please 
send it to sdkfdbk@microsoft.com. The SDK team will use this feedback to 
improve future versions. You can use the installer tools included in the 
SDK to examine the SDK setup program's use of the installer. 

1.1.1 Uninstalling or Installing Over Previous Versions 
-------------------------------------------------------
We recommend that you either remove any previous installations 
of the Platform SDK or install this release into a new, separate 
directory. 

To remove an old Platform SDK installation, use the Windows 
Control Panel Add/Remove Programs utility.

If Add/Remove Programs does not remove your old Windows Installer-based 
installation (the Platform SDK for Windows 2000 Beta 3 or later), run the 
following command from the Platform SDK CD: 

  <CD>:\Setup\SDKZap.bat

This tool is only available on the CD.  It will remove all Windows Installer 
information about any Platform SDK installation, as well as the InProgress key 
for all applications.  In general, this should not be a problem.  
You must still manually remove the installed Platform SDK image from your hard
drive.  Your environment variables may also need to be edited.  

1.1.2 New Visual C++ IDE Directory Settings
-------------------------------------------
The Microsoft Visual C++(R) Integrated Development Environment (IDE) 
does not use the environment variable settings. During setup, 
you will have the option to update your Visual C++ 5.0 or 6.0 
directory settings to match those in the Platform SDK build 
environment. 

Choosing this option places the Platform SDK library, executable 
directories, and include directories at the beginning of the list of 
directories searched by Visual C++ when building programs in the IDE.
This creates a consistent build environment both on the command line 
and in the IDE. 

Note: This is a global change and will affect all programs built in 
the IDE. If you have customized your IDE directory settings to search 
custom library directories or include directories before searching 
Visual C++ directories, you may want to verify the changes to ensure 
that you maintain correct behavior in your other IDE-based programming
projects. See your Microsoft Visual C++ documentation for instructions
on how to check and modify the IDE search paths.

1.2 Environment Variables
-------------------------
To set the Platform SDK path, run SetEnv.bat from your Platform SDK 
installation directory. The SDK path must appear before your
compiler path.

If you are using Visual C++ as your compiler, run VCVars32.bat before 
running SetEnv.bat from the Platform SDK directory.

Usage:   SetEnv MSSDK
Where:   MSSDK specifies where the Platform SDK was installed.

Example: SetEnv C:\MSSDK sets the environment relative to C:\MSSDK.

1.3 InfoViewer Integration
--------------------------
The Platform SDK documentation is no longer integrated with the
Visual Studio(R) 97 documentation. If you previously installed
the Platform SDK and deleted the %MSSDK%\Help directory, Visual
Studio Help may encounter an error indicating that it cannot find
the Platform SDK documentation files. This occurs because the
registry settings are pointing the InfoViewer to files that no
longer exist.

If this occurs, uninstall your version of the Microsoft Developer Network Library 
and reinstall it.

To verify that InfoViewer is looking for the correct files, 
check whether the following registry keys point to valid file 
locations:

    Key: HKLM\SOFTWARE\Microsoft\Infoviewer\5.0\Titles\<BASENAME>\Content
    Should point to: <HelpFileCDPath><BASENAME>.ivt

    Key: HKLM\SOFTWARE\Microsoft\Infoviewer\5.0\Titles\<BASENAME>\Index
    Should point to: <HelpFileLocalPath><BASENAME>.ivi

    Where: <HelpFileCDPath> is the full path to the MSDN CD-ROM 
	directory containing the .ivt file, <HelpFileLocalPath> is the full path to 
	the MSDN installed directory containing the .ivi file, and <BASENAME> is in 
	the following list of Platform SDK InfoViewer files: 

        pdintro
        pdapp
        pdmsg
        pdgrmm
        pdinet
        pdnds
        pdobj
        pdsms
        pdui
        pdwbase
        pdref

If you have MSDN installed you can verify the paths by looking at a different 
Title\<BASENAME> key.


1.4 Removal of Native Structured Storage 
(NSS)
-----------------------------------------------------------------
The Native Structured Storage (NSS) Implementation within COM 
Structured Storage has been removed from Windows 2000. All NSS files have 
been converted to .doc files.

1.5 Known Issues
----------------
- New linker format (see "2.0 Tested Compilers and New Linker,"
  following).

- Building all of the samples requires more than 2 GB of disk space.

- Building assembler files.

  A few of the samples build some components out of .asm files. If you
  do not have a program equivalent to ML.exe, the Microsoft Assembler
  product, MASM, can be purchased.  No assembler is provided with the
  Platform SDK. If you are an MSDN Universal subscriber, MASM is
  included in your Office Test Platform CD-ROMs.

- If Microsoft Visual C++ is installed from the administrator 
  account, the Platform SDK \lib and \include directories are added 
  at the bottom of the IDE search paths. If you encounter errors 
  when compiling the Platform SDK samples from the IDE, you may need 
  to move these directories to the top. 

  See your Microsoft Visual C++ documentation for instructions on 
  how to check and modify the IDE search paths.


====================================
2.0 Tested Compilers and New Linker
====================================

The Platform SDK has been tested with Visual C++ version 6.0 Service 
Pack 3. Some samples may not build without applying Visual Studio Service Pack 3. 
See http://msdn.microsoft.com/vstudio/ for more information.
Most samples can be built with other compilers, but other compilers 
were not completely tested. The Active Template Library (ATL) requires 
updates to work with the current headers. These updates can be found 
in the %mssdk%\Include\ATL30 directory. The files in this subdirectory 
must replace the ones shipped with Visual C++ 6.0.

If you are using Visual C++ 5.0, you must use the linker supplied in 
this version of the Platform SDK (\Bin\Link\Link.exe).

Some samples require that you install the Unicode MFC libraries. You can
select them during a custom Visual C++ installation. If you do not install these 
libraries, building samples that need them generates an error similar to the 
following:

    LINK : fatal error LNK1104: cannot open file "MFC42ud.lib"

2.1  New Import Library Format
------------------------------
The import libraries included with this release of the Platform SDK
have a new format that reduces their size and allows for faster link
times. If you cannot use the linker that comes with the Platform SDK,
or if you do not have Visual C++ 6.0, the supplied libraries must be
converted to the old format by using the linker supplied with this
Platform SDK. Create a new directory to contain the old format import
libraries. For each import library, use the command: 

    link /lib /convert /out:.\{oldformat}\{library}.lib {library}.lib

where {oldformat} is the directory that will contain the converted 
library and {library} is the import library to be converted. 
Visual C++ 5.0 users can replace the linker supplied with Visual C++ 
with the one in this version of the Platform SDK.

Visual C++ 5.0 users who have trouble debugging binaries should 
delete or rename the following files in MSSDK\BIN:

  Link.exe
  CvPack.exe
  CvtRes.exe

2.2  Common Sources of Warnings
-------------------------------
- MakeProcInstance: Do not use this function. Pass 
  DLGPROC directly (the first parameter to MakeProcInstance).
  Also, be sure to delete the FreeProcInstance function
  that is paired with MakeProcInstance.

- Explicit casts are now necessary in many situations 
  in which intrinsic casts worked before. An example:
  MSSDK\Samples\Dbmsg\Sql\Dblib\C\Sqltestn\SqltestN.c(186).
  The (DLGPROC) cast was not necessary before. A warning
  is now generated if a cast is omitted.

- CODE and DATA statements are not supported for use in 
  makefiles.


============================
3.0 Header File Conventions
============================

While this version of the Platform SDK can be used to target 
applications for Windows 95, Windows NT 4.0, and Windows 98 using 
the following header file conventions, it is primarily intended for 
Windows 2000 preliminary development and testing.

The following table indicates the macros you must define to target 
each system using the SDK headers.

Target platform                    Value to set
---------------                    ------------

  Microsoft Windows 95             WINVER=0x0400
  and Windows NT 4.0

  Microsoft Windows 98             _WIN32_WINDOWS=0x0410 and
  and Windows NT 4.0               WINVER=0x0400

  Windows NT 4.0                   _WIN32_WINNT=0x0400 and
                                   WINVER=0x0400

  Windows 98 and Windows 2000      WINVER=0x0500

  Windows 2000                     _WIN32_WINNT=0x0500 and
                                   WINVER=0x0500

  Internet Explorer 3.0            _WIN32_IE=0x0300
  (and later)

  Internet Explorer 4.0            _WIN32_IE=0x0400
  (and later)

  Internet Explorer 5            _WIN32_IE=0x0500
  (and later)

Note that setting WINVER to 0x0500 implies _WIN32_IE=0x0500.

The SDK headers use guard statements to determine the system on which 
each element is supported. The following table describes these 
statements.

Guard statement                 Implemented in
------------------------        ---------------

 #if _WIN32_WINNT >= 0x0400     Windows NT 4.0 and later. 
                                It is not implemented in
                                Windows 95.

 #if _WIN32_WINDOWS >= 0x0410   Windows 98. The image
                                may not run on Windows 95. 

 #if _WIN32_WINNT >= 0x0500     Windows 2000. The image
                                may not run on Windows 95/98 or
                                Windows NT.

 #if WINVER >= 0x0410           Windows 98.

 #if WINVER >= 0x0500           Windows 2000 and Windows 98.

 #if _WIN32_IE >= 0x0300        Internet Explorer 3.0 
                                and later.

 #if _WIN32_IE >= 0x0400        Internet Explorer 4.0 
                                and later.

 #if _WIN32_IE >= 0x0500        Internet Explorer 5 
                                and later.

The value of _WIN32_WINNT is set in Win32.mak, depending on the
platform you choose to target. By default, Win32.mak sets the
TARGETOS to WINNT and the APPVER to 4.0. As a result, by default, 
_WIN32_WINNT is now defined as 0x0400.  

By default, Win32.mak sets _WIN32_IE to 0x0400 if it is not
already defined. To specifically target Internet Explorer 5, 
set _WIN32_IE to 0x0500.

If you are building an application to run on Windows 95 and you want 
compile-time notification of compatibility issues, set TARGETOS=BOTH 
in your makefile. When TARGETOS is defined as BOTH, _WIN32_WINNT is 
not defined for the precompiler, and the only information parsed at 
compile time is applicable to both Windows 95 and Windows NT.

If you do not include Win32.mak in your makefile, you must
explicitly define _WIN32_WINNT as 0x0500 to use the 
Windows 2000-specific material from the header files.

CryptoAPI is one of several API sets present in Windows 95
OEM Service Release 2 (OSR2) that are still guarded by
(_WIN32_WINNT >= 0x0400). If you are writing an application
specifically for OSR2 and you want the header files to provide
compile-time access to these functions, it is necessary to define
_WIN32_WINNT as 0x0400. Notice that an application that uses these
technologies does not run correctly on the retail release of
Windows 95. Most applications that are expected to run on unmodified
Windows 95 should be built without defining _WIN32_WINNT.

Previous releases of the Platform SDK included
the following definitions in Win32.mak to map structured exception
handling keywords to their proper underscored names:

    try -> __try 
    except -> __except 
    finally -> __finally 
    leave -> __leave

This caused problems for developers who chose to use C++ 
structured exception handling in which "try" is supposed to
be "try" rather than "__try". For this reason, by default, 
the mapping has been removed from Win32.mak. This may
cause compile-time errors for your applications. To get 
the old behavior, add the following to your makefile before including Win32.mak: 

    SEHMAP = TRUE 


===========================
4.0 RPC/MIDL Release Notes
===========================

The following items are new for this release. 

4.0.1 Asynchronous Remote Procedure Call
The Remote Procedure Call (RPC) run-time environment now supports 
asynchronous remote procedure calls to allow your programs to handle 
multiple outstanding calls from a single-threaded client. This 
prevents data transmission bottlenecks that can arise from slow or 
delayed clients or servers. With asynchronous pipe parameters, 
client/server applications can transfer large amounts of data 
incrementally, without blocking the client or server threads from 
performing other tasks. For more information, see the "Asynchronous 
RPC" section in the Platform SDK documentation.

4.0.2 New HTTP Protocol Sequence
The ncacn_http protocol allows client and server applications to 
communicate across the Internet by using the Microsoft Internet 
Information Server (IIS) as a proxy. Because calls are tunneled 
through an established HTTP port, they can cross most firewalls. 

4.0.3 Name Service
The RPC Name Service (Locator) uses Windows 2000 Active Directory 
as its database. This means that exported entries can be made 
persistent even when a server is rebooted. For more information, 
see the RPC reference pages for RpcNsBindingExport,
RpcNsBindingImportNext, RpcNsBindingLookupBegin, and
RpcNsBindingUnexport in the Platform SDK documentation.

4.0.4 New Type Library Attributes
Microsoft Interface Definition Language (MIDL) supports double-byte 
character set (DBCS) international locales. Type Library (TLB) file 
generation has been significantly improved and all the type library 
attributes are supported. For more information, see "Type Library 
Attributes" in the Platform SDK documentation.

4.1 Using New and Updated SDK Headers
-------------------------------------
Headers that were compiled with the new MIDL compiler (including most
MIDL-generated headers in the Platform SDK) require the new SDK
headers to work. This is intentional. As long as the SDK remains
in the include path ahead of the compiler, everything should work.


======================================================
5.0 COM+ (Component Services) Developer Release Notes
======================================================

5.1 COM+ Application Export
---------------------------
A COM+ application export can fail with the following message:

     Error occurred writing to the application file.

This can occur if the same type library is registered with two 
different paths under the HKEY_CLASSES_ROOT\TypeLib key and the 
HKEY_CLASSES_ROOT\CLSID\{<clsid>}\TypeLib key. An example of this is 
short file name versus long file name or different capitalization in 
the paths. This should not affect components generated by Visual Basic. 
The workaround for Windows 2000 is to either write the same path to 
both the type library and the class registration key by changing the 
component's self-registration code or to manually modify one of the 
registry keys using Regedit.exe before exporting the application.

5.2 COM+ Application Proxies
----------------------------
The CLSIDFromProgID for COM+ application proxies is case-sensitive. 
COM+ application proxies do not appear in the registry 
(HKEY_CLASSES_ROOT). All class information is stored in the COM+ 
Registration Database. For Windows 2000, progID lookups in the 
Registration Database are case-sensitive.

5.3 Administrative Interfaces
-----------------------------

5.3.1 COM Administrative Interfaces
ICOMAdmin::GetCollectionByQuery to Change
The signature or format of the query string for 
ICOMAdmin::GetCollectionByQuery is likely to change in the final release 
of Windows 2000.

5.3.2 MTS 2.0-Style Administrative Interfaces
COM+ offers MTS 2.0-style administrative interfaces (MTSAdmin class) 
that enable existing applications targeting MTS 2.0 to continue to run 
without changes on Windows 2000 and allow new applications to target 
both MTS 2.0 and Windows 2000. However, note that these interfaces are 
not entirely compatible due to changes in the underlying functionality. 
Refer to the Platform SDK for a detailed listing of all compatibility 
breaks.

The ApplicationInstallPath and Replication Share properties have been 
removed from Windows 2000.

5.4 Bring Your Own Transaction
------------------------------
When using Bring Your Own Transaction (BYOT), you must not begin the 
transaction inside a COM+ configured component. Should you do so, 
deadlocks are likely to occur.

5.5 Enable Oracle XA Transaction Support
----------------------------------------
If you are using Oracle 7.3, follow these steps:

1. Ensure that V$XATRANS$ exists. This view should have been created 
when the XA library was installed. If this view does not exist, your 
Oracle system administrator must create it by running the Oracle-supplied 
script named "XAVIEW.SQL". This file can be found in 
C:\ORANT\RDBMS73\ADMIN. This SQL script must be executed by the Oracle 
user "SYS".

2. The Oracle system administrator must grant SELECT access to the 
public on the $XATRANS$ view. 
To grant SELECT access, use the following command:

	Grant Select on V$XATRANS$ to public

If you are using Oracle 8, follow these steps:

1. Oracle8 should have created both the V$XATRANS$ and the 
DBA_PENDING_TRANSACTIONS views. You should not need to create either of 
these views.

2. The Oracle system administrator must grant SELECT access to the public 
for the DBA_PENDING_TRANSACTIONS view. To grant SELECT access, use the 
following command:

	Grant Select on DBA_PENDING_TRANSACTIONS to public

5.6 Queued Components (QC)
--------------------------
QC interfaces can only be marshaled via typelibs. Marshaling by a 
proxystub DLL is not supported.

5.6.1 Cluster Failover Support
COM+ Queued Components does not yet support failover in a cluster 
environment.

5.6.2 IIS
The issues with activating and calling into a queued component from an 
.asp file are beyond the scope of this readme. Please refer to the 
documentation for complete details.

5.6.3 Interface Passing
Interface pointer parameters for methods on queueable interfaces need to 
be [in] only. Furthermore, the only legal interface pointer that can be 
passed into a queued component is an interface pointer to another queued 
component. Any other type of interface pointer will generate a runtime 
error. Please see QCSamp2 in the Queued Components SDK Sample for an 
illustration of this technique.
In Windows 2000, passing a recorder into another recorder's 
IDispatch::Invoke method is not supported. Therefore, you must make sure 
that you use vtable binding in Visual Basic when you activate a queued 
component.

5.6.4 Getting Started
The best way to get a feel for Queued Components is to go through the 
step-by-step instructions for QCSamp1, QCSamp2 and QCSamp3 in the Queued 
Components SDK Sample.

5.7 Autoeverything
------------------
5.7.1 Development
Although unmodified MTS packages are completely supported, for new COM+ 
applications, please link to Uuid.lib and #include "Comsvcs.h", this 
supersedes all other headers and libraries that were shipped with MTS.

5.7.2 JIT
Just-in-Time (JIT) Activation is no longer optional when components are 
transacted. You can only disable JIT when the component is marked 
"Transaction Ignored" or "Transaction Not Supported".

5.7.3 CoGetObjectContext
Instead of calling GetObjectContext, new applications should use the new 
CoGetObjectContext API to get the object context. This API is described 
as follows:

    HRESULT CoGetObjectContext(REFIID iid, void **ppInterface);
    
    iid - Desired interface for returned object context
    **ppInterface - Address to place the interface pointer of the desired 
    interface for the returned object context

IObjectContext::CreateInstance (C++) or CreateInstance (Visual Basic) is 
no longer mandatory.

Any of the standard activation APIs such as CoCreateInstance or 
CreateInstance correctly propagates context to the newly created object.

5.7.4 AutoComplete/AutoAbort
Methods that have this attribute set will automatically complete or abort 
the current transaction when the method returns, depending on the HRESULT. 
In other words, the COM+ examines the HRESULT on return and does the 
equivalent of calling SetComplete if the HRESULT is S_OK, and calling 
SetAbort otherwise.

5.7.5 Synchronization
In a component with synchronization, avoid calling out of 
IObjectControl::Deactivate. Deadlock may occur, as in the following 
example:

    Where Component A, B, and C are hosted in their own COM+ Applications 
    (implying they are hosted by their own processes) and reside in the 
    same synchronization domain, avoid application designs where 
    component A can call into Component C directly and indirectly through 
    Component B. The following situation might occur:
    - A calls B, which calls C
    - B dies
    - A's call to B returns (with failure)
    - A proceeds to call into C directly

The COM+ Services Runtime detects the second call into C as having the 
same logical thread ID as the first call and therefore allows the 
reentrancy, but nevertheless this creates a situation where there may 
actually be two physical threads of execution running inside C.

5.8 Windows Clustering
----------------------
5.8.1 How to Set Up COM+ and DTC to Run on a Windows 2000 Cluster
COM+ and DTC are now components of Windows 2000 Server editions. To 
set up COM+ and DTC to run on a Windows 2000 Advanced Server or 
Data Center Server, you must run the COM+ Cluster Wizard on your newly 
created Windows 2000 Advanced or Data Center Cluster. This Wizard will 
upgrade your standalone COM+ and DTC installations to run on the new 
Cluster. 

The COM+ Cluster Wizard does the following:

For DTC:
- Creates a DTC resource
- Moves the log file to a shared disk drive
- Marks the registry keys for replication
 
For Component Load Balancing Service (CLBS):
- Creates a component load balancing resource
- Sets up the router node for failover
- Marks the registry key for replication

Clustering is not supported for COM+ Events on Windows 2000. 

To run the new COM+ Cluster Wizard (COMclust.exe), make sure both nodes 
of the cluster are running and enter the following in the Command Prompt 
window:
  ComClust.exe

The COMClust.exe wizard is located in the System32 directory. 

5.8.2 Registrar Component Threading Model 
When using the registrar component, you may get errors when using the 
IComponentRegistrar::Attach method. In the error log, these errors will 
manifest themselves as an "Access Denied" error from the Attach method. 
This error occurs because the registry entry for CLSID_ReadTypeLib has 
been set to "Apartment" by default. To fix this error, you must change 
the value of CLSID_ReadTypeLib to "Both". The GUID for this entry is 
{E94836C5-E4D6-11CF-B938-00A0C9034817}.

5.9 Subscriptions and Event Firing
----------------------------------
The following scenario causes an infinite loop: 
If you subscribe to IComMethodEvents and you have events enabled for your 
subscription, your method will continually loop. By default, Events and 
Statistics are enabled for any component, unless declaratively specified. 
The workaround is to disable Events and Statistics for the subscriber.

5.10 Documentation
------------------
5.10.1 Obtaining Complete Documentation on COM+ Features
For complete documentation and reference material on implementing a 
COM+ feature, see the COM+ documentation located in the Microsoft 
Platform SDK.

The COM+ SDK can be installed as part of the MSDN Platform SDK. For 
additional last-minute changes, please take a look at the readme in the 
[MSSDK]\Samples\Com directory.

5.10.2 COM+ Software Development Kit
COM+ samples help you understand how to use COM+ features. We provide 
Visual Basic, Visual C++, and Visual J++ compilable projects that 
demonstrate the implementation of services such as queued components 
and events.

Compile the samples to see how the application runs. Then you can drill 
down into the code by stepping through the server components and client 
implementations. You can either use these applications as a learning 
tool or modify them to use in your applications.

Here is a list of the COM+ samples, each preceded by their directory 
location: 

- Administration -- Applications demonstrating how to use and automate 
  COM+ administration.
- Application_Samples -- Enterprise-level COM+ user applications useful 
  as a "best practice" model.
- Fundamentals -- Tutorial samples for basic COM programming and other 
  COM feature samples.
- Installation_Verification -- COM+ application to verify correct 
  installation of COM+.
- Performance -- Toolkit beta for tuning MTS, COM+, or IIS application 
  performance.
- Services -- Applications demonstrating COM+ services.
- Tools -- COM+ pack containing a COM+ wizard and a COM+ add-in.


===========================================================
6.0 Windows Management Instrumentation (WMI) Release Notes
===========================================================

6.1 WMI tools on Windows 95, Windows 98 and Windows NT 4.0
----------------------------------------------------------
The WMI tools are only installed on Windows 2000 in this SDK release. 

6.2 Registering the Event Viewer tools
--------------------------------------
To register the Event Viewer tools to receive WMI events, 
you must do the following:

- Set "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\WBEM\Application Directory" 
  to the Platform SDK\Bin\WMI\ directory.

- Set "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\WBEM\SDK Directory" to the 
  Platform SDK\Bin\WMI\ directory.

- Set "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\WBEM\SDK Build" to the 
  SDK build number.

In the Platform SDK\Bin\WMI\ directory, run the following:

  %SystemRoot%\System32\WBEM\MOFCOMP Eviewer.mof


=================================================
7.0 Windows Debuggers and Visual C++ 6.0 Support
=================================================

The debuggers in the Platform SDK (WinDbg, i386KD, AlphaKD, NTSD, and
CDB) have been updated to support the new PDB symbol format used by
Visual C++ 6.0, part of the Microsoft Visual Studio(R) suite. This 
SDK was tested primarily with Visual C++ 6.0.  Visual C++ 6.0 
is more stringent about warnings and errors in code; not all of the 
samples have been modified to compile cleanly under Visual C++ 6.0. 
For more information, see the Visual C++ 6.0 release notes and 
<http://msdn.microsoft.com/visualc/>.

For more information on WinDBG, the KD debuggers included in the 
Platform SDK, and debugging, please see Debuggers.txt in 
the directory where the debugger is installed (\Bin). For more information, 
see <http://www.microsoft.com/hwdev/driver/ntdebugging.htm>.


==================================
8.0 Platform SDK Content Overview
==================================

The Platform SDK is the successor to the Win32 and 
BackOffice SDKs. It is intended to support development 
for Microsoft's Distributed Network Architecture--the infrastructure 
provided by Microsoft for the "digital nervous system."

Many of the technology components of this infrastructure were once
shipped in separate SDKs and did not always work well together.
The Platform SDK integrates these components and tests the build 
environment as a whole, provides a unified set of documentation, and 
provides "one-stop shopping" for samples, headers, and so on.

The following is a partial list of SDKs that are now integrated into
the Platform SDK:

  ActiveX SDK
  BackOffice SDK
  COM+ Services SDK
  Data Access SDK
  DirectX SDK
  Internet Client SDK
  MAPI SDK
  MDAC SDK
  ODBC SDK
  OLEDB SDK
  Web Workshop
  Win32 SDK

Following is a brief description of the contents of the Platform SDK:

BIN       SDK tools, DLLs, and self-extracting .exe files
HELP      SDK documentation files use HTML Help
INCLUDE   Headers, IDL files, and global Platform SDK makefiles--
          Win32.mak and BkOffice.mak--used from most SDK command-line
          makefiles
LIB       Libraries
REDIST    Redistributables
SAMPLES   Platform SDK samples

This readme refers to the tools, the INCLUDE files, and the LIB
files collectively as the "build environment." 


=======================
9.0 Other Known Issues
=======================

9.1 MapDebugInformation deprecated; use SymGetModuleInfo instead
------------------------------------------------------------------
The MapDebugInformation function is obsolete for Windows 2000. 
Instead, use the SymGetModuleInfo function. Note that DbgHelp.Dll is a 
redistributable file, so this change affects all applications that install 
the Windows 2000 version of DbgHelp.Dll. 

MapDebugInformation does not work with PDB-style symbols. The existing 
SymGetModuleInfo provides all the necessary functionality, so a new 
MapDebugInformationEx function was not created.

The UnmapDebugInformation function and the IMAGE_DEBUG_INFORMATION structure 
are also obsolete. They have been replaced by the SymGetModuleInfo, 
SymLoadModule, SymUnloadModule functions and the IMAGEHLP_MODULE structure. 

ImageHlp.dll has been split into two parts: 
ImageHlp.dll and DbgHelp.dll (and the related .h and .lib files, included 
with the Platform SDK). For applications using symbol-handling and other 
debugging-related functionality, use DbgHelp instead of ImageHlp. In 
Windows 2000, the system-installed version of both of these DLLs are protected 
from being updated by Windows File Protection, and ImageHlp cannot be 
redistributed. Applications that need to ship an updated version of DbgHelp 
with their product can install an updated local copy of DbgHelp to the install 
directory. This also reduces an extra system reboot. This updated DbgHelp can 
also be used on Windows NT 4.0.

9.2 Visual Basic Command-Line Makefiles
---------------------------------------
Some of the Visual Basic samples have command-line makefiles. To 
build these samples from the command line, you must include Vb5.exe 
or Vb6.exe in your path.

9.3 Conflicts Between Edbbcli.h and Ntdsbcli.h
-----------------------------------------------
C/C++ modules or header files which include both Edbbcli.h (Exchange backup 
client definitions) and Ntdsbcli.h (NTDS backup client definitions) will 
encounter conflicts between the two files. The following symbols are multiply 
defined:

BFT_PATCH_FILE
tagEDB_RSTMAPA
tagEDB_RSTMAPW

A solution to work around this problem is to only include one or the other of 
these header files in the same compilation unit. Generally, each of these two 
header files relates to different backup functionality.  It should be possible 
to partition the application such that the functionality dealing with Exchange 
is in a separate module from the functionality dealing with NTDS.

9.4 Several Tools Not Documented in the SDK Docs
-------------------------------------------------
9.4.1 
The following tools are not documented in the Platform SDK Documentation:
  symedit
  tlist
  kill
  porttool
  Cacls.exe
  NetWatch.exe
  Remote.exe
  Switcher.exe
  Walker.exe
  WinAt.exe
Information may be found by searching msdn.microsoft.com.

9.4.2
Remote.Exe is a server/client utility that provides remote network access via 
named pipes to applications that use STDIN and STDOUT for input and output. 
This allows users at other computers on the network to connect to your Kernel 
debugging session and either view the debugging information or enter commands 
themselves. The syntax for starting the server end of the remote session is as 
follows:

remote /s "command" Unique_Id [/f foreground_color|/b background_color]

9.5 Additional IP Helper API Functions
----------------------------------------
The following IP Helper functions could not be included in time for this release 
of the Platform SDK documentation: EnableRouter / UnenableRouter.
The EnableRouter and UnenableRouter functions have been added to the IP Helper API. 
These functions make it possible to enable or disable IP forwarding on the local 
computer. These functions are available only on Windows 2000 Server.

For more information, see the supplemental documentation in 
%mssdk%\Help\NewIpHlp.doc.

9.6 Documentation Error - DeleteNtmsRequests
----------------------------------------------
DWORD WINAPI DeleteNtmsRequest(
  HANDLE hSession,
  LPNTMS_GUID lpRequestId,
  DWORD dwType,
  DWORD dwCount
);

 should be 

DWORD WINAPI DeleteNtmsRequests(
  HANDLE hSession,
  LPNTMS_GUID lpRequestId,
  DWORD dwType,
  DWORD dwCount
);

In other words, DeleteNtmsRequest should be DeleteNtmsRequests (note the 's'
at the end of DeleteNtmsRequests).

9.7 Building MMC Sample Snap-ins
----------------------------------
Information on building the MMC sample snap-ins can be found in 
%mssdk%\Samples\SysMgmt\MMC\Readme.rtf. This file also contains 
information about the MMC Snap-in Designer for Visual Basic that could not be 
included in this release of the Platform SDK documentation.

9.8 Chat Server Samples
------------------------
The Chat Server samples are not included in this release of the Platform SDK.  
They can be found by installing Chat Server.  Please see KB article Q166370 
for details (http://support.microsoft.com/view/dev.asp, KB Search).  

9.9 Sample Compile Errors and Dependencies
-------------------------------------------

- The \Samples\Com\Services\BYOT sample will not work with a local SQL server. 

- The Microsoft Chat Samples in Samples\Web\MSChat require installation of the 
  ChatCntl redistributable.  To compile and use these samples, first run 
  Redist\MSChat\ChatCntl.exe.

- The \Samples\Web\SiteServer\ContentDeployment\VB\EventSink\FileSink samples 
  will not build until the following controls are installed:

    Microsoft Site Server\Bin\Crsapi.dll - Site Server 3.0 Content Deployment Object Library
    Microsoft Site Server\Bin\SSEVENT.dll - Site Server 3.0 Server Event Type Library

  These controls can be obtained from the Site Server 3.0 product. 

- The \Samples\DbMsg\CDO\Transport Event Registration sample requires that the 
  following control is installed:

    \System32\Inetsrv\Seo.dll - Server Extension Objects COM Library, which is 
    installed by Windows 2000 and the Windows NT 4.0 Server Option Pack.

- The following sample requires ATL 2.0:
    Mssdk\Samples\WinBase\Scard\Aggreg

  ATL 2.0 can be downloaded from the Microsoft Web site, 
  http://msdn.microsoft.com/visualc/downloads/atl/default.asp.

- The following sample requires ATL 2.1, which can be installed with 
  Visual C 5.0:
    Mssdk\Samples\NetDS\Tapi\Tapi3\Cpp\SampleMSP

- The DbMsg\Exchange\LibSrc\ACL sample can be built properly from the IDE,
  but not from the command line.


9.10 Samples\Com\Services\BYOT samples
-------------------------------------------
The BYOT samples do not function with a local SQL server. To run and test the 
BYOT samples, install and configure a remote SQL Server according to the 
sample documentation.

9.11 ADSI Sample Navigation
-------------------------------------------
Some of HTML pages for navigation the ADSI samples have broken links.  These
are known issues and will be addressed in a future release of the Platform SDK.


===========================
10.0 AD/ADSI Release Notes
===========================

10.1 Revised DsAddSidHistory API documentation
----------------------------------------------
The DsAddSidHistory API has changed significantly since this release of 
the Platform SDK documentation was completed. An updated version of the
DsAddSidHistory documentation can be found in 
<MSSDK>\Help\DsAddSidHistory.doc.

10.2 LDAP Documentation Correction
----------------------------------
The LDAP Reference, Session Options section of the Platform SDK Documentation 
includes LDAP_OPT_RESTART (0X09), which is no longer supported in Windows 2000.

10.3 Additional/Updated Requirements
------------------------------------
Information is missing or incorrect in the Requirements sections on the reference 
pages for the following functions and structures. The following lists show the 
correct information.

DS_DOMAIN_TRUSTS Declared in Dsgetdc.h.
EDB_RSTMAP[AD]	Declared in Ntdsbcli.h.

DsBrowseForContainer    Library: Use Dsuiext.lib.

Library: Use Ntdsapi.lib.
  DsAddSidHistory
  DsBind
  DsBindWithCred
  DsCrackNames
  DsCrackSpn
  DsFreeDomainControllerInfo
  DsFreeNameResult
  DsFreePasswordCredentials
  DsFreeSchemaGuidMap
  DsFreeSpnArray
  DsGetDomainControllerInfo
  DsGetSpn
  DsInheritSecurityIdentity
  DsListDomainsInSite
  DsListInfoForServer
  DsListRoles
  DsListServersForDomainInSite
  DsListServersInSite
  DsListSites
  DsMakePasswordCredentials
  DsMakeSpn
  DsMapSchemaGuids
  DsRemoveDsDomain
  DsRemoveDsServer
  DsReplicaAdd
  DsReplicaDel
  DsReplicaModify
  DsReplicaSync
  DsReplicaSyncAll
  DsReplicaUpdateRefs
  DsServerRegisterSpn
  DsUnBind
  DsWriteAccountSpn

Library: Use Ntdsbcli.lib.
  DsBackupClose
  DsBackupEnd
  DsBackupFree
  DsBackupGetBackupLogs
  DsBackupGetDatabaseNames
  DsBackupOpenFile
  DsBackupPrepare
  DsBackupRead
  DsBackupTruncateLogs
  DsIsNTDSOnline
  DsRestoreEnd
  DsRestoreGetDatabaseLocations
  DsRestorePrepare
  DsRestoreRegister
  DsRestoreRegisterComplete
  DsSetAuthIdentity
  DsSetCurrentBackupLog

Library: Use Netapi32.lib.
  DsAddressToSiteNames
  DsDeregisterDnsHostRecords 
  DsEnumerateDomainTrusts
  DsGetDcName 
  DsGetDcSiteCoverage 
  DsGetSiteName 
  DsRoleFreeMemory
  DsRoleGetPrimaryDomainInformation
  DsValidateSubnetName 

10.4 ADSI Viewer
----------------
In order to run the ADSI Viewer tool (\Bin\adsvw.exe) on platforms other than 
Windows 2000, you must first install the ADSI client software. The client 
software can be downloaded from the following location:
  http://www.microsoft.com/ntserver/nts/downloads/other/ADSI25/default.asp 


================================
11.0 Windows Media(TM) Services
================================

To download the complete suite of Windows Media Services, see the 
Windows Media(TM) Technologies page at the Microsoft Web site, 
http://www.microsoft.com/windows/windowsmedia/.


===========================================
12.0 Building Visual Basic DirectX Samples
===========================================

To build the DirectX Visual Basic samples, you may need to register one or
more controls located in the \Bin\DirectX\VBSupport directory after installing
the DirectX Foundation Samples. You can use RegSvr32.exe to register controls. 
These files will not be registered during setup.


=================================
13.0 Microsoft® SDK for Java 4.0 
=================================

Some samples may require Microsoft SDK for Java. Visit Microsoft's web site
for more information, http://www.microsoft.com/java/ .


=====================================================
14.0 Future Microsoft Support of the Alpha Processor
=====================================================

For more information about the changes to Microsoft's support of the 
Compaq Alpha processor, see 
http://www.microsoft.com/ntserver/nts/news/msnw/compaq.asp.


======================================
15.0 Incorrect Disk Space calculation
======================================
Occasionally the Windows Installer engine will miscalculate the amount
of temporary disk space needed to install the SDK.  This usually produces 
a "1307" Windows Installer error message.  The only work-around at this 
time is to select fewer components or free up additional disk space.

In most cases, having an extra 20 MB of disk space remaining after the SDK 
is installed will be sufficient to avoid this error.  Use the "Disk Space" button 
in setup to determine the amount of free disk space remaining as calculated by
the Windows Installer for a completed install. Be sure that the "Difference" is
greater than 20 MB.


===================================================
16.0 Upgrading previous Platform SDK installations
===================================================
The Platform SDK setup program will attempt to upgrade your existing SDK
installation.  There are several known problems with this, such as leaving behind 
old icons and not removing all of the files.  

The best installation procedure is to completely uninstall your previous releases. 
See the general install notes at the top of this readme.txt in section 
"1.1.1 Uninstalling or Installing Over Previous Versions " for more information 
about uninstalling and using SDKZap.Bat


================================================
17.0 Remote Access Service -- Invoking the Custom Scripting DLL
================================================
If the user activates a connectoid for a phone-book entry that has 
RASEO_CustomScript set, RAS will invoke the custom-scripting DLL. 

To invoke the custom-scripting DLL programmatically, establish the 
connection using the RasDialDlg function. The RasDial function 
will not invoke the custom-scripting DLL.


============================================
18.0 Providing Feedback on the Platform SDK
============================================

We are sincerely interested in your feedback on the Platform SDK. 
Please mail suggestions or bug reports for the Platform SDK
to SdkFdBk@Microsoft.com. This is not a support alias, but your
feedback helps us plan future changes for the Platform SDK and
will make the SDK more useful to you. 

For support information, go to http://msdn.microsoft.com/support/
or see the support section of the ReadMe.Htm on the 
Platform SDK CD.
