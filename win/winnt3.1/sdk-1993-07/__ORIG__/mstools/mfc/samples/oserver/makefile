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

PCH=bibref.h
BIBREF_OBJS = bibref.obj bibsvr.obj bibdoc.obj bibitem.obj mainwnd.obj

all: bibref.exe

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AM /W3 /Zp /GA /G2 /Yu$(PCH)
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Odr /f /Zi
LIBS=mafxcwd libw mlibcew olesvr shell
LINKFLAGS=$(LINKFLAGS) /COD
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=mafxcw libw mlibcew olesvr shell
!endif

bibref.exe: $(BIBREF_OBJS) bibref.def bibref.res
	link $(LINKFLAGS) @<<
$(BIBREF_OBJS),
bibref,
NUL,
$(LIBS),
bibref.def;
<<
	rc -30 /k /t bibref.res

!else

!include ..\ntsample.mak

bibref.exe: $(BIBREF_OBJS) bibref.def bibref.res
    $(LINK) $(GUIFLAGS) -out:bibref.exe $(BIBREF_OBJS) bibref.res $(MFCLIB) $(GUILIBS)

!endif


bibref.pch bibref.obj: bibref.cpp bibref.h resource.h mainwnd.h bibsvr.h
	$(CPP) $(CPPFLAGS) /Yc$(PCH) /c bibref.cpp


bibsvr.obj  : bibref.h bibsvr.h bibdoc.h mainwnd.h
bibdoc.obj  : bibref.h bibdoc.h bibitem.h
bibitem.obj : bibref.h bibitem.h bibdoc.h
mainwnd.obj : bibref.h mainwnd.h bibdoc.h

bibref.res: resource.h bibref.ico about.dlg

clean:
	-erase bibref.exe
	-erase bibref.res
	-erase bibref.pch
	-erase *.obj
