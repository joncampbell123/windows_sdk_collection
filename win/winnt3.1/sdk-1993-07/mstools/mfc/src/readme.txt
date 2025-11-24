=======================================================================
			MICROSOFT FOUNDATION CLASSES - SOURCE CODE
=======================================================================

This directory contains the source code to the MFC library.

These files have been included for reference purposes, in
conjunction with the Class Library reference manual and tutorial.

These files are compiled to form the Microsoft Foundation Class library.
The library may be built in a number of configurations, depending upon
operating system, memory usage or model, and whether or not debugging
and diagnostic aids are to be included in applications which link with
the library.

================================
 1.  WINDOWS/NT BUILD NOTES
================================
Please read this section carefully before reading the rest of this
file.  This section describes important differences between the
Microsoft C/C++ 7.0 MFC 1.0 release and the Windows/NT BETA release.

There are four MFC library variants under Windows/NT; these variants
are built using one of the following nmake invocations:

nmake MODEL=N TARGET=W DEBUG=1     -- debug, GUI subsystem
nmake MODEL=N TARGET=R DEBUG=1     -- debug, console subsystem
nmake MODEL=N TARGET=W DEBUG=0     -- retail, GUI subsystem
nmake MODEL=N TARGET=R DEBUG=0     -- retail, console subsystem


Model should always be set to N for NT versions of MFC.

================================
 2.  BUILDING A LIBRARY VARIANT
================================

To build a library in a particular configuration, use the NMAKE tool
and the Makefile which is in this directory.  The following arguments
can be given to NMAKE to successfully build a specific library variant.

  NMAKE {MODEL=[S|M|C|L|N]} {TARGET=[W|R]} {DEBUG=[0|1]} {DLL=[0|1]} \
		{CODEVIEW=[0|1|2]} {OBJ=path} {OPT=<CL command line switches>}

MODEL=[S|M|C|L|N]
  The "MODEL" argument specifies the ambient memory model, which can be
  one of S, M, C, or L (for Small, Medium, Compact or Large, NT
  respectively).  N is for 32 bit Windows NT hosting and targeting.

TARGET=[W|R]
  The "TARGET" argument specifies the operating-system on which your
  compiled programs will run.  This may be one of W or R (for
  Windows or Real-mode DOS, respectively).  R may be used with TARGET=N
  for character mode applications on Windows NT.

DLL=[0|1]
  The "DLL" argument specifies whether or not to compile the library
  so that it may subsequently be used for developing a dynamic link
  library (DLL).  The default is DLL=0 (do not include DLL support).
  If DLL=1, MODEL must be L (large).

  NOTE: The MFC library is not itself a DLL; it is always a statically
  linked library.  This option does not build MFC into a DLL; it builds
  a static library that can be used to build your DLLs; it is similar
  in spirit to the C runtime LDLLCEW.LIB library.


DEBUG=[0|1]
  The "DEBUG" argument specifies whether or not to include diagnostic
  support code for the library.  This may be 0 (for no diagnostics) 
  or 1 (for full diagnostics).

CODEVIEW=[0|1|2]
  The "CODEVIEW" argument specifies whether to compile the library with
  CodeView information or not.  You need to compile the library with
  CodeView information if you want to trace into MFC code using CodeView.
  You should also compile your application files with the /Zi option,
  and link your executable with the /CODEVIEW option.

  Setting CODEVIEW does not affect the DEBUG argument, although the
  value of the DEBUG argument does affect the default value of CODEVIEW
  (discussed below).  A value of 0 indicates that no CodeView
  information is to be compiled into the library.  A value of 1 says
  to compile in full CodeView information for all modules of the library.
  A value of 2 compiles CodeView information only into a select set of
  the most commonly referenced modules (WinMain, diagnostic memory
  allocator, message map, main message loop, and the application class.)

  The default value depends on the setting of the DEBUG argument.
  If DEBUG=1, CODEVIEW defaults to 2.  If DEBUG=0, CODEVIEW also defaults
  to 0.

OBJ=[path]
  We recommend storing .OBJ files in a separate directory so that you
  may compile different versions of the MFC library concurrently.
  The "OBJ" argument allows you to specify where these files are stored
  during the build process.  The directory specified is created and
  removed automatically as required.  This defaults to a combination
  of the target, model, and debug status, preceded by a '$' (i.e. $MWD).

OPT=[switches]
  If your library needs to be built with custom compiler switches, then
  these may be included in the "OPT" argument.  Note that switches need
  to be separated by spaces, so when including more than one extra
  compiler switch, enclose the whole OPT= argument in double-quotes.
  This is an advanced feature; read the Makefile and the details on each
  of the switches concerned in the Microsoft C/C++ 7.0 Compiler User Manual
  before using this option.

Defaults
  The default is:
	  nmake MODEL=M TARGET=W DEBUG=1 CODEVIEW=2 OBJ=$MWD

Note.  If you wish to use only the non-Windows classes then build
an 'R' (real-mode) library variant. There are three MFC sample applications
that do not require Microsoft Windows, and use the real-mode libraries.
See the file MFC\SAMPLES\README.TXT for details about each sample and
which variant of the library each requires.


===============================
 3. AFTER BUILDING THE LIBRARY
===============================

Once the library has been built successfully, you may want to delete
object files with:

  NMAKE CLEAN OBJ=[path]

Note that if you used the "OBJ" argument while building the library,
specify the same subdirectory in the cleanup command.

This will remove all of the temporary .OBJ files created by building the
library, and remove the directory where they were stored.

Always perform a cleanup before building a new variant of the library,
or use different object paths for each variant.  Note that the OBJ files
are only necessary during the building process.


======================================
 4. SOURCE CODE FORMATTING CONVENTION
======================================

All MFC source code has been formatted such that leading whitespace
on a line is made up of physical tabs, while embedded whitespace is
physical spaces.  MFC source code assumes that your editor is set to
display a physical tab as four blanks.

For example:

int FormatExample()
{
/*
Statements below should start in column 5 if tabs are set correctly
Comment should start in column 20
12345678901234567890
*/
	int i;
	i = 5;         // whitespace between statement and comment is spaces

	return i;

}
