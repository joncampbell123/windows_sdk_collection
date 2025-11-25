# Makefile : Builds the helloapp application
#
# Usage:     NMAKE helloapp

# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992 Microsoft Corporation
# All rights reserved. *	
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# WinHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.
#

# Only retail build

LIBGUI=..\..\lib\nafxcw.lib libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
 shell32.lib                       

!if "$(CPU)"=="ALPHA"
PLATFORM=ALPHA
!endif

helloapp.exe: helloapp.cpp helloapp.def
!if "$(PLATFORM)"=="ALPHA"
	claxp /W3 /Zp /Ox /D_WINDOWS helloapp.cpp /link -subsystem:windows -entry:WinMainCRTStartup $(LIBGUI)
!else
!if "$(PLATFORM)"=="MIPS"
	mcl /Gd /W3 /Zp /Ox /D_WINDOWS /D_MIPS_ helloapp.cpp /link -subsystem:windows $(LIBGUI)
!else
# INTEL
	cl /G3d /W3 /Zp /O1 /D_WINDOWS /D_X86_ helloapp.cpp $(LIBGUI)
!endif
!endif
	
clean:
	-del helloapp.exe
	-del helloapp.obj
