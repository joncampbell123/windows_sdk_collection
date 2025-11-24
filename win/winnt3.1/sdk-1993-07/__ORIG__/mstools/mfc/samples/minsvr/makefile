# Makefile : Builds the minsvr application
#
# Usage:     NMAKE option (build MinSvr)
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

OBJS = minsvr.obj mindoc.obj minitem.obj mainwnd.obj
PCH=minsvr.h

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

minsvr.exe: minsvr.res minsvr.def $(OBJS)
	link $(LINKFLAGS) @<<
$(OBJS),
minsvr,
NUL,
$(LIBS),
minsvr.def;
<<
	rc -30 /k /t minsvr.res

!else

!include ..\ntsample.mak

minsvr.exe: minsvr.res minsvr.def $(OBJS)
    $(LINK) $(GUIFLAGS) -out:minsvr.exe $(OBJS) minsvr.res $(MFCLIB) $(GUILIBS)

!endif

minsvr.obj : minsvr.cpp minsvr.h
	$(CC) $(CPPFLAGS) /Yc$(PCH) /c minsvr.cpp

*.obj:  $(PCH)
minsvr.res:  *.ico *.dlg minsvr.h

clean:
	-erase minsvr.exe
	-erase minsvr.res
	-erase minsvr.pch
	-erase *.obj
