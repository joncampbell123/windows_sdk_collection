# Makefile : Builds the application.
#
# Usage:     NMAKE options     (builds the application)
#    or:     NMAKE CLEAN       (erases the compiled files)
#
# Options:   DEBUG=[0|1]       (DEBUG not defined is equivalent to DEBUG=0)


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

PCH=fileview.h
OBJS=fileview.obj linefile.obj

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AM /W3 /Zp /GA /GEs /G2 /Yu$(PCH) 
LINKFLAGS=/NOD /ONERROR:NOEXE


!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /f /Zi
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd libw mlibcew commdlg
!else
CPPFLAGS=$(CPPFLAGS) /Gs /Oselg
LINKFLAGS=$(LINKFLAGS)  
LIBS=mafxcw libw mlibcew commdlg
!endif

fileview.exe: fileview.obj linefile.obj \
	fileview.res
	link $(LINKFLAGS) linefile fileview,fileview,nul, \
	$(LIBS),fileview.def
	rc -30 /k /t fileview.res

!else

!include ..\ntsample.mak

fileview.exe: $(OBJS) fileview.res fileview.def
    $(LINK) $(GUIFLAGS) -out:fileview.exe $(OBJS) fileview.res $(MFCLIB) $(GUILIBS)
!endif

fileview.obj:      fileview.cpp $(PCH) resource.h
	$(CPP) $(CPPFLAGS) /Yc$(PCH) /c fileview.cpp

fileview.res:      fileview.rc resource.h fileview.ico
linefile.obj:      linefile.cpp


clean:
	-erase fileview.exe
	-erase fileview.pch
	-erase fileview.res
	-erase fileview.obj
	-erase linefile.obj
