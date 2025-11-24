# Makefile : Builds the Multipad application
#
# Usage:     NMAKE option (build Multipad)
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


OBJS=mpmain.obj mpinit.obj mpfile.obj mpfind.obj mpprint.obj bar.obj 
HDRS=multipad.h bar.h resource.h menu.h
PCH=multipad.h

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AM /W3 /Zp /GA /GEs /G2 /Yu$(PCH) 
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd libw mlibcew commdlg
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)
LIBS=mafxcw libw mlibcew commdlg
!endif

multipad.exe: multipad.res multipad.def $(OBJS)
	link $(LINKFLAGS) @<<
$(OBJS),
multipad,
NUL,
$(LIBS),
multipad.def;
<<
	rc -30 /k /t multipad.res
!else

!include ..\ntsample.mak

multipad.exe: multipad.res multipad.def $(OBJS)
    $(LINK) $(GUIFLAGS) -out:multipad.exe $(OBJS) multipad.res $(MFCLIB) $(GUILIBS)

!endif

mpmain.obj : mpmain.cpp $(HDRS)
	$(CPP) $(CPPFLAGS) /Yc$(PCH) /c mpmain.cpp

mpmain.obj: $(PCH)
mpinit.obj: $(PCH)
mpfile.obj: $(PCH)
mpfind.obj: $(PCH)
mpprint.obj: $(PCH)
bar.obj: $(PCH)
multipad.res:   multipad.dlg multipad.ico notepad.ico resource.h menu.h

clean:
	-erase multipad.exe
	-erase multipad.res
	-erase multipad.pch
	-erase mpmain.obj
	-erase mpinit.obj
	-erase mpfile.obj
	-erase mpfind.obj
	-erase mpprint.obj
	-erase bar.obj
