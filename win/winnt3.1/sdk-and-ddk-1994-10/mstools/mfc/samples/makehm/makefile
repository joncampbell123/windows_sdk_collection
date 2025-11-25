# Makefile : Builds the makehm application
#
# Usage:     NMAKE options (build makehm)
#    or:     NMAKE clean   (erase all compiled files)
#
# options:   DEBUG=[0|1]   (DEBUG not defined is equivalent to DEBUG=0)
#            PLATFORM=INTEL     (defaults to INTEL)
#            This option chooses the appropriate tools and sources for the
#            different platforms support by Windows/NT.  Currently INTEL and
#            MIPS are supported; more will be aded as they become available.
#
# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.
#

!if "$(PLATFORM)"==""
PLATFORM=ALPHA
!endif

!if "$(PLATFORM)"=="INTEL"
CPP = cl
CPPFLAGS=/W3 /D_X86_ /D_CONSOLE
LINK=link
LIBS=
!else
!if "$(PLATFORM)"=="MIPS"
CPP = mcl
CPPFLAGS=/W3 /D_MIPS_ /D_CONSOLE
LINK=link32
LIBS=kernel32.lib libc.lib
!else
!if "$(PLATFORM)"=="ALPHA"
CPP = claxp
CPPFLAGS=/W3 /D_CONSOLE
LINK=link32
LIBS=kernel32.lib libc.lib
!endif
!endif
!endif
LINKFLAGS=-subsystem:console -entry:mainCRTStartup

!if "$(DEBUG)" == "1"
CPPFLAGS=$(CPPFLAGS) /D_DEBUG /Od /Zi
LINKFLAGS=$(LINKFLAGS) -debug:full -debugtype:cv
LIBS=nafxcrd.lib $(LIBS)
!else
CPPFLAGS=$(CPPFLAGS) /Ox
LIBS=nafxcr.lib $(LIBS)
!endif

.SUFFIXES : .cpp
.cpp.obj :
	$(CPP) /c $(CPPFLAGS) $*.cpp

makehm.exe:   makehm.obj
	$(LINK) $(LINKFLAGS) -out:$*.exe makehm.obj $(LIBS)

clean:
	-erase makehm.exe
	-erase makehm.obj
	-erase msvc.pdb
	-erase makehm.map
