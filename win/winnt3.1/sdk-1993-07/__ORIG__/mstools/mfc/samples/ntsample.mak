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

!if "$(CPU)" == "MIPS"
DLLDECORATE = 
PLATFORM = _MIPS_
!else
DLLDECORATE = @12
PLATFORM = _X86_
!endif

MFCDLLENTRY = AfxLibMain$(DLLDECORATE)

!ifdef CONSOLE
MFCFLAGS=/D_NTWIN /D$(PLATFORM)=1
!else
MFCFLAGS=/D_NTWIN /D$(PLATFORM)=1 /D_WINDOWS /DWINVER=0x0300
!endif

!ifdef CONSOLE
!if "$(DEBUG)" == "1"
MFCLIB = nafxcrd.lib
DEBUGFLAGS = /Zi /Od /D_DEBUG
!else
MFCLIB = nafxcr.lib
DEBUGFLAGS = /Oxt /Gs
!endif
!else
!if "$(DEBUG)" == "1"
MFCLIB = nafxcwd.lib
DEBUGFLAGS = /Zi /Od /D_DEBUG
!else
MFCLIB = nafxcw.lib
DEBUGFLAGS = /Oxt /Gs
!endif
!endif

!if "$(DEBUG)" == "1"
MFCLIBDLL = nafxdwd.lib
!else
MFCLIBDLL = nafxdw.lib
!endif

CC = cl 
CFLAGS	= /c /Gd /W3 $(DEBUGFLAGS) $(MFCFLAGS)

CPP = cl
CPPFLAGS = /c /Gd /W3 $(DEBUGFLAGS) $(MFCFLAGS)

!if "$(PCH)" != ""
CPPFLAGS=$(CPPFLAGS) /Yu$(PCH)
!endif

!if "$(DEBUG)" == "1"
LINKDEBUG = -debug:full -debugtype:cv
!else
LINKDEBUG = -debug:none
!endif
LINK = link32 $(LINKDEBUG)
LIB32 = lib32

# link flags - must be specified after $(link)
#
# conflags : creating a character based console application
# guiflags : creating a GUI based "Windows" application
#

CONFLAGS =  -subsystem:console -entry:mainCRTStartup
GUIFLAGS =	-subsystem:windows -entry:WinMainCRTStartup

# Link libraries - system import and C runtime libraries
#
# conlibs : libraries to link with for a console application
# guilibs : libraries to link with for a "Windows" application
# guilibsdll : libraries to link for an MFC DLL
#

CONLIBS = libc.lib kernel32.lib netapi32.lib

GUILIBS = libc.lib kernel32.lib user32.lib gdi32.lib	\
	winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib \
	shell32.lib

GUILIBSDLL = $(GUILIBS)

.SUFFIXES: .cpp

.cpp.obj :
	$(CPP) $(CPPFLAGS) $*.cpp

.c.obj :
	$(CC) $(CFLAGS) $(CVARS) $*.c

.rc.res :
	rc /r /fo $*.tmp $<
    cvtres -$(CPU) $*.tmp -o $*.res
    del $*.tmp

