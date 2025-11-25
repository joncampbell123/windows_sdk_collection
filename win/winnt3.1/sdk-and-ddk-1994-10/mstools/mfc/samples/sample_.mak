# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992,93 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# QuickHelp and/or WinHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.

# Common include for building MFC Sample programs
#
#  typical usage
#       PROJ = foo
#       OBJS = foo.obj bar.obj ...
#       !INCLUDE ..\SAMPLE_.MAK
#
# NOTE: do not include 'stdafx.obj' in the OBJS list - the correctly
#    built version will be included for you
#
# Options to NMAKE:
#     "DEBUG=0" => use retail (default debug)
#     "OPT" => use to set custom compile options
#     "BROWSE=1" => build Visual C++ compatible browse file (.BSC file)
#     "NO_PCH=1" => do not use precompiled headers (defaults to use pch)
#     "PLATFORM=INTEL"     (defaults to INTEL)
#           This option chooses the appropriate tools and sources for the
#           different platforms support by Windows/NT.  Currently INTEL,
#           MIPS, and Alpha_AXP (ALPHA) are supported; more will be added 
#           as they become available.

!ifndef PROJ
	!ERROR You forgot to define 'PROJ' !!
!endif
!ifndef OBJS
	!ERROR You forgot to define 'OBJS' !!
!endif 

!ifndef LVL
LVL = ..
!endif

!if "$(PLATFORM)"==""
PLATFORM=ALPHA
!endif

STDAFX=stdafx

DLLENTRY = @12

!ifdef CONSOLE
MFCFLAGS=/D_CONSOLE
!else
MFCFLAGS=/D_WINDOWS
!endif

!ifdef CONSOLE
!if "$(DEBUG)" == "1"
MFCLIB = nafxcrd.lib
DEBUGFLAGS = /Z7 /Od /D_DEBUG
!else
MFCLIB = nafxcr.lib
!if "$(PLATFORM)"=="ALPHA"
DEBUGFLAGS = /Ox /Gs 
!else
DEBUGFLAGS = /Otx /Gs 
!endif
!endif
!else
!if "$(DEBUG)" == "1"
MFCLIB = nafxcwd.lib
DEBUGFLAGS = /Z7 /Od /D_DEBUG
!else
MFCLIB = nafxcw.lib
!if "$(PLATFORM)"=="INTEL"
DEBUGFLAGS = /O1
!else
!if "$(PLATFORM)"=="MIPS"
DEBUGFLAGS = /Osg /Gs
!else
!if "$(PLATFORM)"=="ALPHA"
DEBUGFLAGS = /Ox /Gs
!endif
!endif
!endif
!endif
!endif

!if "$(PLATFORM)"=="INTEL"
CC = cl
CFLAGS  = /c /Gd /W3 /D_X86_ $(DEBUGFLAGS) $(MFCFLAGS)

CPP = cl
CPPMAIN_FLAGS = /c /Gd /W3 /D_X86_ $(DEBUGFLAGS) $(MFCFLAGS)
!else
!if "$(PLATFORM)"=="MIPS"
CC = mcl
CFLAGS  = /c /Gd /W3 /D_MIPS_ $(DEBUGFLAGS) $(MFCFLAGS)

CPP = mcl
CPPMAIN_FLAGS = /c /Gd /W3 /D_MIPS_ $(DEBUGFLAGS) $(MFCFLAGS)
!else
!if "$(PLATFORM)"=="ALPHA"
CC = claxp
CFLAGS  = /c /W3 $(DEBUGFLAGS) $(MFCFLAGS)

CPP = claxp
CPPMAIN_FLAGS = /c /W3 $(DEBUGFLAGS) $(MFCFLAGS)

!endif
!endif
!endif

!if "$(NO_PCH)"=="1"
CPPFLAGS=$(CPPMAIN_FLAGS)
!else
CPPFLAGS=$(CPPMAIN_FLAGS) /Yustdafx.h /Fp$(LVL)\$(STDAFX).pch
!endif

!if "$(DEBUG)" == "1"
LINKDEBUG = -debug:full -debugtype:both
!else
LINKDEBUG = -debug:none
!endif
!if "$(PLATFORM)"=="INTEL"
LINK = link $(LINKDEBUG)
!else
!if "$(PLATFORM)"=="MIPS"
LINK = link32 $(LINKDEBUG)
!else
!if "$(PLATFORM)"=="ALPHA"
LINK = link32 $(LINKDEBUG)
!endif
!endif
!endif

# link flags - must be specified after $(link)
#
# conflags : creating a character based console application
# guiflags : creating a GUI based "Windows" application

CONFLAGS =  -subsystem:console -entry:mainCRTStartup
GUIFLAGS =  -subsystem:windows

# Link libraries - system import and C runtime libraries
#
# conlibs  : libraries to link with for a console application
# guilibs  : libraries to link with for a "Windows" application
# baselibs : libraries to link with for all application
#
# note : $(LIB) is set in environment variables

CONLIBS = netapi32.lib

GUILIBS = user32.lib gdi32.lib   \
	winspool.lib comdlg32.lib advapi32.lib \
	olecli32.lib olesvr32.lib shell32.lib

!if "$(PLATFORM)"=="INTEL"
BASELIBS = 
!else
!if "$(PLATFORM)"=="MIPS"
BASELIBS = libc.lib kernel32.lib
!else
!if "$(PLATFORM)"=="ALPHA"
BASELIBS = libc.lib kernel32.lib
!endif
!endif
!endif

.SUFFIXES : .cpp
.cpp.obj :
	$(CPP) $(CPPFLAGS) $*.cpp

.c.obj :
	$(CC) $(CFLAGS) $(CVARS) $*.c

.rc.res :
!if "$(PLATFORM)"=="INTEL"
	rc -r $<
!else
!if "$(PLATFORM)"=="MIPS"
	rc -r -fo $*.rct $<
	cvtres -MIPS -o $*.res $*.rct
	-erase $*.rct
!else
!if "$(PLATFORM)"=="ALPHA"
	rc -r -fo $*.rct $<
	cvtres -ALPHA -o $*.res $*.rct
	-erase $*.rct
!endif
!endif
!endif

#############################################################################

!if "$(DEBUG)" == "1"
STDAFX=stdafxnd
!else
STDAFX=stdafxn
!endif

$(PROJ).exe: $(LVL)\$(STDAFX).obj $(PROJ).def $(PROJ).res $(OBJS)
	$(LINK) $(GUIFLAGS) -map:$(PROJ).map -out:$(PROJ).exe $(LVL)\$(STDAFX).obj \
			$(OBJS) $(PROJ).res $(MFCLIB) $(GUILIBS) $(EXTRA_LIBS) $(BASELIBS)

$(LVL)\$(STDAFX).obj $(LVL)\$(STDAFX).pch: stdafx.h stdafx.cpp
	echo "BUILDING SHARED PCH and PCT files"
	$(CPP) $(CPPMAIN_FLAGS) /Ycstdafx.h /Fp$(LVL)\$(STDAFX).pch /Fo$(LVL)\$(STDAFX).obj /c $(LVL)\stdafx.cpp

$(OBJS): $(LVL)\$(STDAFX).pch
$(PROJ).res:  resource.h

clean::
	-erase $(PROJ).exe
	-erase $(PROJ).res
	-erase *.aps
	-erase *.pch
	-erase *.map
	-erase *.obj

#############################################################################
