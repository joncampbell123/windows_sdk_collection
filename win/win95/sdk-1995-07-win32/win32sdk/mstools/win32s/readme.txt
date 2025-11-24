June '95 Win32s Readme

In \MsTools\Win32s\1_25 you will find Win32s 1.25A.  This is the latest 
released version.  Win32s 1.25A fixes many bugs and adds important new
features.  The single largest improvement is the removal of the 128K 
stack size limitation.  Win32s now supports a growable stack.  Win32s
1.25A also enhances support for international locales, and for Novell
NetWare and Stac Stacker software.  There are also now 3 versions of 
Win32s. These are the Japanese (Win32s-J) and Far East (Win32s-FE) 
versions.  Win32s-J and Win32s-FE have not changed since Win32s 1.25,
and they are available on MSDN Level II (On the April CD Win32s-J is 
on Disc 1 in Win32sJ directory.  As we went to press the final location
of Win32s-FE and Win32s-J for the July release was not available.)
Win32s-J is a DBCS enabled version of Win32s with Japanese messages and
support for the NT-J IME APIs.  Win32s-FE is a DBCS enabled version but
the messages have not been localized.  It also does not have IME API
support.

In \MsTools\Win32s\1_30Beta you will find the most recent Beta of
Win32s 1.30.  This version has many improvements over the previous
Betas of Win32s 1.30, and it now includes a version of RichEd32.Dll.

KB articles Q130138 and Q130139 (Win32s 1.25A BugList and FixList)
are reproduced here for your convenience.  Several of the bugs in Win32s
1.25A have already been fixed in Win32s 1.30 Beta.

BUG: Win32s 1.25a Bug List 

ARTICLE ID: Q130138

------------------------------------------------------------------------
The information in this article applies to:

 - Microsoft Win32s, version 1.25a

------------------------------------------------------------------------

The following is a list of the known bugs in Win32s version 1.25 at the
time of its release.

 - Incorrect context at EXIT_PROCESS_DEBUG_EVENT.

 - Progman gets restored when debugger app exits.

 - Using StartDoc() does not produce document from printer.

 - EM_GETWORDBREAKPROC return code is incorrect.

 - Int 3 cannot be trapped via Structured Exception Handling (SEH) on
   Win32s.

 - Win32s does not open all files in RAW mode, as Windows NT does.

 - Cannot do ReadProcessMemory (RPM) on memory that has a hardware
   breakpoint set on it.

 - C run-time functions getdcwd()/getcwd() do not work.

 - GetFullPathName() returns the root directory for any drive that is not
   the current drive.

 - PlayMetaFileRecord()/EnumMetaFile() contains incorrect lpHTable.

 - Size of memory mapped files is rounded to a whole number of pages,
   meaning that the size is a multiple of 4096 bytes.

 - Functions chdrive() and SetCurrentDirectory() fail on PCNFS network
   drives.

 - GetExitCodeProcess() does not return exit codes for 16-bit Windows-based
   applications.

 - Memory passed to Netbios() must be allocated with GlobalAlloc().

 - biSizeImage field of BITMAPINFOHEADER is zero.

 - CreateFile() on certain invalid long filesnames closes Windows.

 - Only the first CBT hook gets messages.

 - Most registry functions return the Windows 3.1 return codes, not the
   Windows NT return codes.

 - GlobalReAlloc(x,y,GMEM_MOVEABLE) returns wrong handle type.

 - GetVolumeInformation() fails for Universal Naming Convention (UNC)
   root path.

 - ResumeThread while debugging writes to debuggee stack.

 - GetShortPathName() doesn't fail with a bad path, as it does on
   Windows NT.

 - CreateDirectory()/RemoveDirectory() handle errors differently than on
   Windows NT.

 - SetCurrentDirctory() returns different error codes than on Windows NT.

 - FindText() leaks memory.

 - Win32s doesn't support language files other than default (l_intl.nls).

 - spawnl does not pass parameters to an MS-DOS-based application.

 - Win32s does not support forwarded exports.

 - GetDlgItemInt() only translates numbers <= 32767 (a 16-bit integer).

 - Changing system locale in Win32s will not have an effect until Win32s
   is loaded again, unlike on Windows NT.

 - Module Management APIs missing ANSI to OEM translation.

 - When WS_TABSTOP is passed to CreateWindow(), this forces a
   WS_MAXIMIZEBOX.

 - Stubbed API FindFirstFileW() does not return -1 to indicate failure.

 - SearchPath() and OpenFile() don't work properly with OEM chars in the
   filename.

 - GetSystemInfo() doesn't set correct ProcessorType for the Pentium.

 - FormatMessage() doesn't set last error.

 - FormatMessage() fails with LANG_NEUTRAL | SUBLANG_DEFAULT, but works
   with LANG_ENGLISH | SUBLANG_ENGLISH_US.

 - After calling CreateFile() on a write-protected floppy GetLastError()
   returns 2, instead of 19, as it should.

 - VirtualProtect() with anything other than PAGE_NOACCESS, PAGE_READ, OR
   PAGE_READWRITE yields unpredictable page protections.

 - COMPAREITEMSTRUCT, DELETEITEMSTRUCT, DRAWITEMSTRUCT, AND
   MEASUREITEMSTRUCT incorrectly sign-extend fields.

 - GetWindowTextLength() & GetWindowText() incorrectly sign-extend the
   return value.

 - MoveFile() fails on Windows for Workgroups when the source is remote and
   the destination is local.

Additional reference words: 1.25 1.25a
KBCategory: kbprg kbbuglist
KBSubcategory: W32s


FIX: Win32s 1.25a Fix List 

ARTICLE ID: Q130139


------------------------------------------------------------------------
The information in this article applies to:

 - Microsoft Win32s, version 1.25a

------------------------------------------------------------------------

The following is a list of the known bugs in Win32s version 1.2 that
were fixed in Win32s version 1.25.

 - GlobalAlloc(GMEM_FIXED) from 32-bit .EXE locks memory pages. It is
   more efficient to use GlobalAlloc(GMEM_MOVEABLE) and call GlobalFix()
   if necessary.

 - WINMM16.DLL has no version information.

 - CreateFileMapping() with SEC_NOCOMMIT returns ERROR_INVALID_PARAMETER.

 - PolyPolygon() does not close the polygons.

 - OpenFile() only searches the current directory when only a filename is
   given, not the application directory, the system directory, the windows
   directory, or the directories listed on the path.

 - GetFileInformationByHandle() doesn't return the correct file attribute.

 - GlobalUnlock() sets an error of ERROR_INVALID_PARAMETER.

 - lstrcmp()/lstrcmpi() do not use the collate table correctly.

 - FreeLibrary() in DLL_PROCESS_DETACH crashes the system.

 - FindResource(), LoadResource(), GetProcAddress(), GetModuleFileName(),
   EnumResourceNames(), and other APIs, fail with a NULL hInstance.

 - sndPlaySound() with SND_ASYNC | SND_MEMORY may cause a crash or may work
   poorly.

 - Thread Local Storage (TLS) data is not initialized to 0 in TlsAlloc().

 - The pointer received in the lParam of WM_INITDIALOG in the common dialog
   hook function becomes invalid in the following messages if the pointer
   is "remembered" in a static variable.

 - Stubbed API GetFileAttributesW() does not return -1 on error.

 - Code page CP_MACCP not supported.

 - Invalid LCIDs are not recognized.

 - CreateFile() fails to open an existing file in share mode.

 - GetLocaleInfo() for locale returns system defaults from WIN.INI.

 - GetVolumeInformation() fails with ERROR_INVALID_NAME for volumes
   without a label.

 - VirtualProtect() may miss the last page in an address range.

 - GetLocaleInfo() returns incorrect information for most non-US locales.

 - ANSI/OEM conversions always use code page 437.

 - GetProcAddress() for printer driver APIs is case sensitive.

 - The LanMan APIs are unsupported, but they return 0, which indicates
   that the API was successful. They should return NERR_InvalidAPI (2142).

 - CreateFileW() returns 0 instead of -1 (HFILE_ERROR).

 - lstrcpyn() copies n bytes from source to destination, then appends a
   NULL terminator, instead of copying n-1 bytes and appending the NULL
   terminator.

 - GetDriveType() doesn't report detecting a CD-ROM or a RAM DISK.

 - CRTDLL calls TlsFree() upon each process detach, not just the last.

 - If a DllEntryPoint calls FreeLibrary() when using universal thunks,
   the system can crash.

 - Not all 32-bit DLLs have correct version numbers.

 - GetCurrentDirectory() returns the wrong directory after calling
   GetOpenFileName(). The workaround is to call SetCurrentDirectory(".")
   right after returning from the call to GetOpenFileName().

 - RegEnumValue() and other Registry functions return ERROR_SUCCESS even
   though they are not implemented. Win32s implements only the registry
   functions supported by Windows.

 - AreFileApisANSI()/SetFileApisToANSI()/SetFileApisToOEM() are not
   exported. AreFileApisANSI() should always return TRUE, SetFileApiToOEM()
   should always fail, and SetFileApiToANSI() should always succeed.

 - SetLocaleInfoW()/SetLocateInfoA() are not implemented.

 - GetScrollPos() sets the last error if the scroll position is 0.

 - SetScrollPos() sets the last error if the last scroll position is 0.

 - LoadString() leaks memory if the string is a null string.

 - GetFileVersionInfoSize() fails if the resource section is small and
   close to the end of the file.

 - MoveFile() doesn't call SetLastError() on failure or sets a different
   error than on Windows NT.

 - SetCurrentDirectory() does not work on a CD-ROM drive.

 - GetFileVersionInfoSize() fails if the 2nd parameter is NULL.

 - WSOCK32.DLL is missing exported stubs for unimplemented APIs.

 - Win32s fails to load 64x64 monochrome (black and white) icons.

 - CreateFile() fails when called with a filename with an international
   character.

 - GetCurrentDirectory() returns an OEM string.

 - PrintDlg() causes GP fault if hDevMode!=NULL and another printer is
   selected that uses a larger DevMode buffer.

 - Unicode resources are not properly converted to 8-bit characters.

 - CreateDC() returns an incorrect DEVMODE. This can cause a variety of
   symptoms, like the inability to do a Landscape Print Preview from an
   MFC application or the displayed paper width and height not changing,
   even when you change the paper size.

 - OpenFile() fails on filenames with OEM characters in the name.

 - GetDriveType() fails on a Stacker 3.1 drive.

 - SetCurrentDirectory() fails on Novell client machines.

 - GetProp() returns 0 in the second instance of an app in certain cases.

 - fopen(fn, "w") fails on second call.

 - TLS indices allocated by a module are released when that module is
   freed.

 - CreateWindow() handles STARTUPINFO incorrectly if the application starts
   minimized.

 - CreateFile() creates files with incorrect attributes.

 - Resource sections are now read/write to emulate the behavior of Windows
   NT and Windows 95.

 - Removed the 128K stack limitation.

 - CompareStringW() sometimes uses incorrect locale, primarily Swedish and
   other Scandinavian locales.

 - Added dummy _iob to CRTDLL for applications that reference standard
   handles.

 - Added support for OPENCHANNEL, CLOSECHANNEL, SEXGDIXFORM, and
   DOWNLOADHEADER escapes.

Additional reference words: 1.25 1.25a
KBCategory: kbprg kbfixlist kbbuglist
KBSubcategory: W32s
