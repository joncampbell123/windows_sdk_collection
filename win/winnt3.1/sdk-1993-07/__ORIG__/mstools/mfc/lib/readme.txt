==========================================================================
                 MICROSOFT FOUNDATION CLASSES : LIBRARIES
==========================================================================

This directory contains the following MFC library variants:

NAFXCWD.LIB - 32 bit Windows NT, GUI, debug build
NAFXCW.LIB  - 32 bit Windows NT, GUI, retail build
NAFXCRD.LIB - 32 bit Windows NT, CONSOLE, debug build
NAFXCR.LIB  - 32 bit Windows NT, CONSOLE, retail build

These library variants are the minimum set needed to link all
MFC sample applications.

See the file ..\SRC\README.TXT for specific instructions on how
to build MFC library variants.

When you build an MFC library in the ..\SRC directory, the
resulting static library (.LIB) file is placed in this directory.
The sample application makefiles will look for the MFC library in
this directory.

LIBCXX.LIB contains memory allocation helper routines needed by all
C++ applications.  This library will be folded into the regular
C runtime library in a future release.
