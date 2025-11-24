-------------------------------
Win32s Version 1.3 beta release
-------------------------------

Win32s is a technology that allows Win32 applications to run with binary
compatibility on Windows 3.1.  This is accomplished by shipping a set of DLLs
and a VxD, which implement a Windows 3.1-compatible subset of the Win32 API.
For more information, see the Win32s Programmers Reference (win32s.hlp).

This beta release of Win32s version 1.3 is being made available so that
application developers interested in Win32s can begin working with the new
Windows 95 common controls.  Most of the software required to support these
controls is in this beta release.  One known exception is that
ImageList_ReplaceIcon and ImageList_LoadBitmap are currently not functional.

Version 1.3 is scheduled to contain additional support for the new Windows 95
common dialogs and the new Windows 95 help engine.  These features were not
ready as of this beta release.  Applications running on Win32s which try to
use them will fail.

Note: You may not redistribute the Win32s 1.3 Pre-Release files.



Win32s Programmers Reference
----------------------------

The Win32s Programmers Reference for version 1.20 is included with this beta
release.  While using this help file, you may find that some of the hot-links
result in a "Help is not available for this request" message.  The search
topics list is rather limited (use ctrl+'>' to navigate where necessary).
The complete, retail Win32s SDK version 1.20 is available in the Windows NT
Win32 SDK distributed via a Microsoft Developer Network level 2 subscription.



Win32s Programmers Reference version 1.20 errata
------------------------------------------------

Win32s System Requirements: 2.2MB of hard disk space is required for Win32s, 
and another 2MB of hard drive space is required for OLE (for a total of 
4.2MB).

Win32s Dynamic Link Libraries: With respect to _declspec(thread), only the 
DLLs that contain the instance data should be loaded at load time. It is 
still possible to use LoadLibrary() to load libraries that don't have 
static instance data (static data that was declared using _declspec(thread)).

Localized Win32s Files:  The file W32S.386, not WIN32S.386, is localized.  
Nine languages are provided with this release of the Win32 SDK.

Figure 7.a, which depicts Debugger Options, incorrectly shows the process 
to choose options.  Consult the text of the Debugging and Testing section 
for correct information.

The Win32s kernel traps all exceptions that occur during execution of a 32-bit 
process. Some exceptions, however, are also handled by Windows 3.1 kernel; 
for example, calling the 16-bit function IsBadPointer or IsWindow with an 
invalid parameter causes a GP fault, which is trapped by the Windows 3.1 
kernel and handled gracefully. In Win32s Version 1.1 such an exception would 
be trapped by the Win32s SEH mechanism instead of the Windows 3.1 kernel. 
This was a problem for 16-bit code that was called through the Universal Thunk 
(UT) and makes calls to the mentioned APIs. This problem is now fixed in
Win32s.  Exceptions in 16-bit that are handled by the Windows 3.1 kernel are
left to the 16-bit fault handler.

MS-DOS does not support UniversalTimeConvention (UTC) translation. Win32s 
always uses local time instead of UTC. The functions FileTimeToLocalFileTime, 
LocalFileTimeToFileTime, FindFirstFile, and FindNextFile all return local time.
 
Under Win32s, the base address of the flat code and data selectors is not zero.
Applications can use the GetThreadSelectorEntry function to query the base 
address at run time.

Memory for data that is passed to NetBIOS must be allocated by using 
GlobalAlloc. An attempt to pass a pointer to memory allocated by using 
LocalAlloc, VirtualAlloc, or to data on the stack or in a global variable 
will fail. The return error is 0x22 - too many commands outstanding.

A 16-bit DLL can be loaded by Win32s so that GetProcAddress can be called for 
printer driver APIs. Behavior has changed as follows. LoadLibrary now sets 
the last error code to ERROR_BAD_EXE_FORMAT (193) if a 16-bit DLL is 
successfully loaded. This error code can be used to determine whether a loaded 
DLL is 16-bit or 32-bit.



16-bit OLE Support
------------------

The 16-bit OLE libraries with Win32s are designed to replace existing 16-bit
OLE libraries.  These 16-bit OLE libraries have a higher version number than
previous versions of the OLE libraries that are redistributed with 16-bit OLE
applications. The new 16-bit OLE libraries have version numbers beginning with
2.02.

OLE Custom Interfaces and Custom Marshalling are not supported in Win32s.

Memory that is passed between processes and marshalled must be allocated with 
globalAlloc(GMEM_MOVEABLE). Memory allocated as GMEM_FIXED will not be 
marshalled correctly for 32-bit OLE applications running under Win32s. 

Although OLE is exposed as Unicode APIs and methods, formats put on the
clipboard are always ANSI (for compatibility with 16 bit apps). Therefore, apps
should not put Unicode CF_OBJECTDESCRIPTOR or CF_LINKSOURCEDESCRIPTOR directly 
on the Clipboard (by using the Win32 api SetClipboardData), rather they should 
use the OLE API OleSetClipboard.

The dlls OLEPRX32.DLL and OLECNV32.DLL do not exist on Win32s. The 
functionality in these libraries is implemented by OLE32.DLL. Apps should not 
try to load these dlls explicitly. However, Windows NT registry files that use
oleprx32.dll are valid on Win32s.

When using the default OLE memory allocator (calling OleInitialize(NULL)),
::DidAlloc will always return -1.

::CreateInstance is always called with IID_IUnknown and not with the iid
supplied by the calling application.
