# Makefile : Builds the minmdi application
#
# Usage:     NMAKE option (build minmdi)
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

CPPFLAGS=  /DWINVER=0x0300 /AS /W3 /Zp /GA /GEs
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


minmdi.exe:     minmdi.obj minmdi.def minmdi.res
		link $(LINKFLAGS) minmdi, minmdi, NUL, $(LIBS),minmdi.def;
		rc -30 /t minmdi.res
!else

!include ..\ntsample.mak

minmdi.exe:     minmdi.obj minmdi.def minmdi.res
    $(LINK) $(GUIFLAGS) -out:minmdi.exe minmdi.obj minmdi.res $(MFCLIB) $(GUILIBS)
!endif

minmdi.obj:    minmdi.h resource.h
minmdi.res:    resource.h frame.ico child.ico about.dlg

clean:
		-erase minmdi.exe
		-erase minmdi.res
		-erase minmdi.obj
