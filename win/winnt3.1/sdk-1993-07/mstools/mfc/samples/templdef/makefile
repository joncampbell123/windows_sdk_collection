# Makefile : Builds the templdef application
#
# Usage:     NMAKE options (build templdef)
#    or:     NMAKE clean   (erase all compiled files)
#
# options:   DEBUG=[0|1]   (DEBUG not defined is equivalent to DEBUG=0)
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

CPPFLAGS= /AM /W3 /Zp /D_DOS
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcrd mlibce $(LIBS)
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=mafxcr mlibce $(LIBS)
!endif

templdef.exe:   templdef.obj
	link $(LINKFLAGS) templdef, templdef, NUL, $(LIBS);
!else

CONSOLE=1
!include ..\ntsample.mak

templdef.exe:   templdef.obj
    $(LINK) $(CONFLAGS) -out:templdef.exe templdef.obj $(MFCLIB) $(CONLIBS)

!endif

clean:
	-erase templdef.exe
	-erase templdef.obj
