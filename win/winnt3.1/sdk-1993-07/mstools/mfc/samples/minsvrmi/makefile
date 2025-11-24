# Makefile : Builds the minsvrMI application
#
# Usage:     NMAKE option (build minsvrMI)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)
#
# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# QuickHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.
#

OBJS = minsvrMI.obj mainwnd.obj
PCH=minsvrMI.h

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=/DWINVER=0x0300 /AM /Zp /GA /GEs /G2 /Yu$(PCH) 
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f /W4
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd libw mlibcew olesvr shell
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs /W3
LINKFLAGS=$(LINKFLAGS)
LIBS=mafxcw libw mlibcew olesvr shell
!endif

minsvrMI.exe: minsvrMI.res minsvrMI.def $(OBJS)
	link $(LINKFLAGS) @<<
$(OBJS),
minsvrMI,
NUL,
$(LIBS),
minsvrMI.def;
<<
	rc -30 /k /t minsvrMI.res

!else

!include ..\ntsample.mak

minsvrMI.exe: minsvrMI.res minsvrMI.def $(OBJS)
    $(LINK) $(GUIFLAGS) -out:minsvrMI.exe $(OBJS) minsvrMI.res $(MFCLIB) $(GUILIBS)
!endif

minsvrMI.obj : minsvrMI.cpp minsvrMI.h
	$(CC) $(CPPFLAGS) /Yc$(PCH) /c minsvrMI.cpp

*.obj:  $(PCH)
minsvrMI.res:  *.ico *.dlg minsvrMI.h

clean:
	-erase minsvrMI.exe
	-erase minsvrMI.res
	-erase minsvrMI.pch
	-erase *.obj
