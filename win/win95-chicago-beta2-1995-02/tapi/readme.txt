Windows Telephony Software Development Kit Release Notes
Windows Windows 95 Beta 2 Release
Aug 3, 1994

1. Introduction
2. Contents of this release
3. Telephony SDK support
4. Known limitations and problems
5. Compiling the sample code
6. Documentation Errata
7. Programming Tips

================================================================

1. Introduction

This release includes the following Windows Telephony components:

TAPI.DLL: the WOSA interface layer which connects Telephony
applications to Telephony service providers.

TAPI32.DLL: the "thunk layer" that allows Win32 applications to
access Windows Telephony services.

TAPIEXE.EXE: a small program which is automatically launched
(and terminated) by TAPI.DLL when needed to provide a context for
processing of Telephony events.

DIALER.EXE: a Windows Telephony application which can dial
voice calls, maintain a call log by monitoring other telephony
activity, and process Simple Telephony requests from other
applications. Source code is included for reference.

Note: The contents of this release of the Windows Telephony SDK
are for development and test and evaluation, and are not to be
included in shipping products.


2. Contents of This Release

This release contains documentation, C language headers,
libraries, and samples to help you develop telephony applications
and service providers.  Most of the contents are organized into
subdirectories of the directory containing this README file, as
follows:

Item                 Location                Format
==================   =====================   ==========================
API and SPI help     HELP\*.HLP              Windows Help
API user's guide     APIDOC\USERGDE\*.DOC    Word for Windows 2.0
SPI user's guide     SPIDOC\USERGDE\*.DOC    Word for Windows 2.0
API reference        APIDOC\REF\*.DOC        Word for Windows 2.0
SPI reference        SPIDOC\REF\*.DOC        Word for Windows 2.0
Technical notes      TECHNOTE\*.DOC          Word for Windows 2.0
Sample Application   SAMPLES\WIN16\DIALER    ASCII
Sample Svc Provider  SAMPLES\WIN16\ATSP      ASCII


Header files, an import library, and utility applications are
located in subdirectories of the Windows 95 SDK root directory, as
follows:

\SDK\INC16 - TAPI.H and TSPI.H interface definition header files
\SDK\LIB16 - TAPI.LIB import library
\SDK\INC32 - TAPI.H interface definition header file
\SDK\LIB32 - TAPI32.LIB import library
\SDK\BIN   - EXTIDGEN.EXE - Unique extension ID generator application
           - TELEPHON.CPL - Service provider installation utility
               for compatibility with older service providers.
           - TELEPHON.HLP - Help file for the control panel applet

Note that TAPI supports only 16-bit service providers; thus there is
no TSPI.H file in the \SDK\INC32 directory.


3. Telephony SDK Support

Please see the general Windows 95 SDK information for guidance on
where to post Windows Telephony questions on the Windows 95 SDK
support forum on CompuServe.

If you will be developing Windows Telephony applications and/or
service providers, please let us know so that we can keep you
up-to-date on Windows Telephony information: send email to the
Windows Telephony Coordinator ("telephon@microsoft.com", or
">INTERNET: telephon@microsoft.com" on CompuServe), and include
your name, company, general areas of Telephony interest, and your
postal, phone, email, fax, and other addresses.



4. Known Limitations and Problems

See the document TAPICHIC.DOC in the TECHNOTE directory for
descriptions of new Windows Telephony features in Windows 95. It
also includes an indication of the status of implementation of
each of these features (some are not yet implemented for this
beta release).

The Windows Telephony components of Windows 95 continue to be a
"work in progress". The Beta 2 version will see them made
localizable, context-sensitive help enabled, parameters moved
into the registry from the TELEPHON.INI, integration with
Plug'n'Play and the System and Add Device control panels,
and other improvements in addition to those mentioned in the
TAPICHIC.DOC file.

You may discover that TAPI.DLL does not unload from memory in
the Beta 1 release of Windows 95 even though there appear to be
no Telephony applications running. This is an artifact of the
operation of the TAPI32.DLL "thunk layer". This will be resolved
in the second beta release so that TAPI.DLL and TAPI32.DLL are
properly released and do not consume memory when no Telephony
applications are running.


5. Compiling the Sample Code

The ATSP and DIALER samples have been developed using Visual
C++ 1.0;  however, the source code is completely compatible with
Microsoft C/C++ 7.0 with Windows SDK.  The makefiles supplied
with the samples are external makefiles; these makefiles can be
used either with NMAKE at the command line, or with the Visual
C++ Visual Workbench or the Programmer's Workbench supplied with
Microsoft C/C++ 7.0 by treating them as external makefiles.

Before compiling the sample source code, set your environment variables
for 16-bit SDK development using the SDKENV batch file located in the
root SDK directory.

Both makefiles use the following syntax:

nmake DEBUG=1 - creates a version of the sample with Codeview
information and debugging messages included.
nmake DEBUG=0 - creates a 'nodebug' version of the sample, with
no Codeview information or debugging messages.

Warning: nmake may fail on the final step (mapsym) because the version of
mapsym located by the PATH set up by SDKENV.BAT is suitable for 32 bit
map files, not 16 bit map files.  If this happens, simply remove the MAPSYM
step from your makefile if you don't plan to use SYM files, or rename
\SDK\BIN\MAPSYM.EXE to something else.  Note that renaming the 32-bit
version of MAPSYM.EXE may cause 32-bit makefiles to break.

6. Documentation Errata

The Windows Telephony documentation is being updated for the
second Windows 95 beta release. In the meantime, there are a few
points of potential confusion that developers should be aware
of now.


6.1 Call Handle Validity for lineMakeCall Function

The description of the lineMakeCall function implies that
the call handle returned to lphCall is available and valid
immediately upon return from the function call. This is not
true. The handle is only valid after the a LINE_REPLY message
is received by the application indicating that the lineMakeCall
successfully completed.


6.2 Insufficient Room to Return Non-Directed Park Address

The linePark function, when used in the non-directed mode,
returns the park address in a VARSTRING structure pointed to by
lpNonDirAddress. Applications should be sure to allocated
ample room for return of this address, since there is no way for
the application to repeat the function to obtain the full
variable portion of the VARSTRING structure if insufficient room
is allocated on the first call. Service providers may return the
LINEERR_STRUCTURETOOSMALL error if the dwTotalSize field of the
VARSTRING structure is insufficient to contain the entire park
address, even though this error is normally not used in cases
where there is insufficient space for the variable portion of
a structure (normally, dwNeededSize is simply set to the indicate
the amount of memory actually needed).


6.3 Function Prototype for lineCallbackFunc

The function prototype for the lineCallbackFunc as given in
the documentation for the lineInitialize function indicates that
the first parameter, hDevice, is of type "HANDLE". This should
be DWORD. This is correct in the declaration of LINECALLBACK in
TAPI.H.


6.4 Representation of "Empty" Strings in Variable Structures

When a service provider returns a variable structure, "empty"
strings in the structure can be indicated in any of several ways:
both the Size and Offset fields for the structure could be set
to 0; the Offset could be non-zero but the Size set to 0; or, in
the case of ASCII strings, the Offset could be non-zero, the Size
set to 1, and the byte at that offset into the location could be
a 0 (a null C string). Applications should be designed to handle
all of these methods.


7. Programming Tips

Here are some Application and Service Provider programming tips
that will be included in future versions of the documentation:


7.1 Access to Application data by Service Providers during
    Asynchronous Function Execution

Service providers which execute functions asynchronously must be
be sure to copy any parameters from application memory, pointed
to by pointer parameters passed into the function, into memory
local to the service provider. Once the function has "returned"
synchronously to the application, the application might
deallocate memory used to temporarily contain information to be
passed to the service provider (particularly true of local stack
variables in the application). Also, the function is executing
in the context of the application during the initial function
call, so the service provider has access to the memory, but, now
that 32-bit applications are supported with separate, protected
memory address spaces, the service provider might not have access
later during asynchronous operation when it is running in
interrupt context or an execution context provided by a component
of the service provider (such as the ATSPEXE.EXE program in the
sample service provider).

Note that writing of values asynchronously back to 32-bit
applications is handled with assistance from TAPI.DLL, which
allocates, in globally shared memory in the 16-bit virtual
machine, a temporary buffer the same size as the buffer from
the application.  After the asynchronous function completes (the
service provider calls the ASYNC_COMPLETION callback in
TAPI.DLL), TAPI switches the process context back to the original
application and then copies the temporary buffer back to the
application's memory at the pointer location specified in the
original function call.


7.2 Using Win32 Comm Functions with TAPI

Applications that use TAPI to place data calls using the
LINEMEDIAMODE_DATAMODEM can obtain a handle to the open
comm port by using the lineGetID function. The application
should pass in the string "comm/datamodem" for the
lpszDeviceClass parameter. The function will return a
VARSTRING structure with dwStringFormat set to
STRINGFORMAT_BINARY; the content of the "string" pointed
to by dwStringOffset will be a structure as follows:

        {
        HANDLE  hComm;
        CHAR    szDeviceName[1];
        }

where hComm is the handle to an open Win32 file (the comm
device), and szDeviceName is a null-terminated ASCII string
giving the name of the port device associated with the hCall
specified in lineGetID.

hComm will be NULL (0) if the dwSelect parameter of lineGetID is
not equal to LINECALLSELECT_CALL, or if the service provider does
not have a comm port actually open. It will be set to a Win32
file handle, which the application can use to send and receive
data on the call, if there is a call active and if the service
provider has the comm device open (such as in the situation where
the only way for the service provider to communicate with and
control the comm hardware is for it to have the port open through
the Win32 comm API). When the service provider gives the
application access to the device in this manner, the handle
received by the application is a _duplicate_ of the service
provider's call handle; the service provide must CLOSE this
handle when it is through using the device (e.g., immediately
before calling lineDrop or upon receiving a LINECALLSTATE_IDLE
indication).

szDeviceName is always set to the name of the comm device
associated with the line, address, or call, even if hComm is
NULL. The application can use this to do a CreateFile to open the
device itself, in the event that a call is connected and the
service provider is of a type that is able to control the comm
device without having itself opened the port through the Win32
comm API.

Applications should only obtain comm device handles using
lineGetID in this manner if they are the owner of the call in
question.

Also note that data applications should use LINEBEARERMODE_VOICE
and LINEMEDIAMODE_DATAMODEM for datamodem calls.
LINEBEARERMODE_DATA is for _digital_ data calls only (such as
ISDN); LINEBEARERMODE_VOICE is used for all services carried
using modulated signals over voiceband channels, such as analog
modems, Group 3 facsimile, text telephone (TDD), etc.


7.3 Don't use the Telephony Control Panel Entry Points

TELEPHON.CPL, the Telephony Control Panel, contains entry
points for display the Locations and Calling Cards dialogs
so that users can update that information. The Telephony
Control Panel is being eliminated in Windows 95. Telephony
driver configuration will be handled using the System and
Add Device control panels, and address translation
parameter configuration will be handled directly through
Telephony applications using the "Dial Helper" common
dialog (exposed through the lineTranslateDialog function).
Applications should discontinue use of the control
panel entry points described in the CPLAPI.DOC technote.


7.4 Programming Telephony Applications Using Win32

See the file "TAPI32.DOC" in the TECHNOTE directory for
guidance on building 32-bit Telephony applications.
