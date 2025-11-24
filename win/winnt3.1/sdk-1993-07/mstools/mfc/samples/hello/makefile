# Makefile : Builds the hello application
#
# Usage:     NMAKE option (build hello)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)


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

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AS /W3 /Zp /GA /GEs /G2
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=safxcwd libw slibcew
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=safxcw libw slibcew
!endif

hello.exe:     hello.obj hello.def hello.res
	link $(LINKFLAGS) hello, hello, NUL, $(LIBS),hello.def;
	rc -30 /t hello.res
!else

!include ..\ntsample.mak

hello.exe:     hello.obj hello.def hello.res
    $(LINK) $(GUIFLAGS) -out:hello.exe hello.res hello.obj $(MFCLIB) $(GUILIBS)

!endif

hello.obj:  hello.h resource.h

hello.res:  resource.h hello.ico hello.dlg


clean:
	-erase hello.exe
	-erase hello.res
	-erase hello.obj
