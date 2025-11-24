# Makefile : Builds the tracer application
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

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AS /W3 /Gsw /Zp
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Odr /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=safxcwd libw slibcew
!else
CPPFLAGS=$(CPPFLAGS) /Oxs
LINKFLAGS=$(LINKFLAGS)  
LIBS=safxcw libw slibcew
!endif 

tracer.exe: tracer.obj tracer.def tracer.res
	link $(LINKFLAGS) @<<
tracer,
tracer,
NUL,
$(LIBS),
tracer.def;
<<
	rc -30 /t tracer.res
!else

!include ..\ntsample.mak

tracer.exe: tracer.obj tracer.def tracer.res
    $(LINK) $(GUIFLAGS) -out:tracer.exe tracer.obj tracer.res $(MFCLIB) $(GUILIBS)
!endif


tracer.obj : tracer.cpp

clean:
	-erase tracer.exe
	-erase tracer.obj
	-erase tracer.res
