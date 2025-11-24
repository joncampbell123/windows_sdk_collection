Windows Telephony Software Development Kit Release Notes
Windows 95 Final Release
==========================================================================

CONTENTS OF THIS RELEASE

The final release of Windows 95 includes the following Windows Telephony 
components:

TAPI.DLL: the WOSA interface layer which connects Telephony applications 
to Telephony service providers.

TAPI32.DLL: the "thunk layer" that allows Win32 applications to access 
Windows Telephony services.

TAPIEXE.EXE: a small program which is automatically launched (and 
terminated) by TAPI.DLL when needed to provide a context for processing of 
Telephony events.

TAPIADDR.DLL: a subcomponent of TAPI involved in address translation 
functions; it is not directly accessed from applications or service 
providers.

TAPIINI.EXE: a small utility that is run once during Windows 95 setup, to 
convert any existing TELEPHON.INI file from Windows 3.1 into the format 
required by Windows 95.

TELEPHON.CPL (and TELEPHON.HLP): the "Telephony" control panel.  This is 
installed by default if the user upgrades a system that already has 
TELEPHON.CPL installed (for example, if they had TAPI on their Windows 3.1 
system). If the user didn't previously have TELEPHON.CPL, then it is 
copied onto their system as TELEPHON.CP$; they won't see it in the control 
panel (since most users don't need it). Setup programs for telephony 
service providers that need the Telephony control panel can rename 
TELEPHON.CP$ to TELEPHON.CPL.

DIALER.EXE (and DIALER.HLP): a Windows Telephony application which can 
dial voice calls, maintain a call log by monitoring other telephony 
activity, and process Simple Telephony requests from other applications. 
It appears in the Accessories section of the Start Menu as "Phone Dialer". 
Users can choose whether or not to install Dialer during setup, and can 
add or remove it later using the "Add/Remove Programs" control panel.

The Windows Telephony components for Windows 3.1 were not included in the 
base system, and therefore were redistributable by ISVs when shipping 
applications or service providers that depended on them. The Windows 
Telephony components are, however, always installed on Windows 95 systems, 
and are therefore NOT REDISTRIBUTABLE BY ISVs.  Also, because of 
dependencies on the Winodws 95 Kernel, these components cannot be used on 
any Windows 3.x or Windows NT release.

The SDK also contains documentation, C language headers, libraries, 
samples, and testing tools to help you develop telephony applications and 
service providers.  All are in subdirectories of the "\win32sdk\mstools" 
directory (indicated as ".." below). Here is a list of the files that
are provided, to help you locate them.

Item                               Location
=================================  ====================================

TAPI header file                   ..\include\win95\tapi.h
                     
TSPI header file                   ..\include\win95\tspi.h

TAPI Library                       ..\lib\win95\tapi32.lib

TAPI Reference                     ..\help\tapi.hlp

TSPI Reference                     ..\help\tspi.hlp
 
ATSP Sample Service Provider       ..\samples\win16\tapi\atsp\*.*

TAPICOMM Sample Application        ..\samples\win32\win95\tapicomm\*.*

ESP Test Service Provider          ..\bin\win95\esp.tsp
                                   ..\bin\win95\espexe.exe

TAPI Browser Test Application      ..\bin\win95\tb14.exe

Extension ID Generator Utility     ..\bin\win95\extidgen.exe

Debug Build of TAPI.DLL            ..\debug\usa\tapi.dll
                                   ..\debug\usa\tapi.sym



TELEPHONY SDK SUPPORT

Support for developers of Windows Telephony applications and
service providers is available through Microsoft Product Support 
Services through several types of support contracts. In addition, 
peer support and general annoucements are available through 
section 3 of the Windows Extensions forum on CompuServe.

If you will be developing Windows Telephony applications and/or
service providers, please let us know so that we can keep you
up-to-date on Windows Telephony information: send email to the
Windows Telephony Coordinator ("tapi@microsoft.com"), and include
your name, company, general areas of Telephony interest, and your
postal, phone, email, fax, and other addresses. Microsoft
maintains a list of commercial applications and service providers
in a file that is distributed through various electronic channels,
including the forum on CompuServe; send email to tapi@microsoft.com
if you would like your product to be included in this list.


