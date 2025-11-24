# Makefile : Builds the application.
#
# Usage:     NMAKE options     (builds the application)
#    or:     NMAKE CLEAN       (erases the compiled files)
#
# Options:   DEBUG=[0|1]       (default is 0)
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

PCH=oclient.h
OBJS=oclient.obj mainwnd.obj fileio.obj itemwnd.obj


all: oclient.exe

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AM /W3 /GA /Zp /Yu$(PCH)
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Odr /f /Zi
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd mlibcew libw shell olecli commdlg
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=mafxcw mlibcew libw shell olecli commdlg
!endif

oclient.exe: $(OBJS) oclient.def oclient.res
	link $(LINKFLAGS) @<<
$(OBJS),
oclient,
NUL,
$(LIBS), 
oclient.def
<<
	rc -30 -k -t oclient.res

!else

!include ..\ntsample.mak

oclient.exe: $(OBJS) oclient.def oclient.res
    $(LINK) $(GUIFLAGS) -out:oclient.exe $(OBJS) oclient.res $(MFCLIB) $(GUILIBS)
!endif


oclient.obj oclient.pch: oclient.cpp oclient.h resource.h mainwnd.h
	$(CPP) $(CPPFLAGS) /Yc$(PCH) /c oclient.cpp

oclient.res: oclient.rc resource.h

fileio.obj  : oclient.h mainwnd.h itemwnd.h
itemwnd.obj : oclient.h mainwnd.h itemwnd.h
mainwnd.obj : oclient.h mainwnd.h itemwnd.h

clean:
	-erase oclient.exe 
	-erase oclient.res 
	-erase oclient.pch
	-erase *.obj 
