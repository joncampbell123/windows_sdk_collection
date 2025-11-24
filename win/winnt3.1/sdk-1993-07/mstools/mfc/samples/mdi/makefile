# Makefile : Builds the mdi application
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
# Usage:     NMAKE option (build mdi)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)

PCH=mdi.h
OBJS=mdi.obj hello.obj bounce.obj

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
mdi.exe:    $(OBJS) mdi.def mdi.res
	link $(LINKFLAGS) mdi hello bounce, mdi, NUL, $(LIBS), mdi;
	rc -30 /k /t mdi.res
!else
!include ..\ntsample.mak

mdi.exe:    $(OBJS) mdi.def mdi.res
    $(LINK) $(GUIFLAGS) -out:mdi.exe $(OBJS) mdi.res $(MFCLIB) $(GUILIBS)
!endif

mdi.obj:    resource.h mdi.h hello.h bounce.h common.h
    $(CPP) $(CPPFLAGS) /Yc$(PCH) /c mdi.cpp

hello.obj:  resource.h hello.h common.h
bounce.obj: resource.h bounce.h common.h

mdi.res:    resource.h about.dlg mdi.ico hello.ico

clean:
	-erase mdi.exe
	-erase mdi.pch
	-erase mdi.res
	-erase mdi.obj
	-erase hello.obj
	-erase bounce.obj
