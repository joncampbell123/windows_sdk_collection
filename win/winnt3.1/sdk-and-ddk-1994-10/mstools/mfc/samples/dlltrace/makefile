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

# Build hello1 application and Tracer DLL

# Options to NMAKE:
#     "OPT" => use to set custom compile options
#     "PLATFORM=INTEL"     (defaults to INTEL)
#           This option chooses the appropriate tools and sources for the
#           different platforms support by Windows/NT.  Currently INTEL and
#           MIPS are supported; more will be aded as they become available.


!if "$(PLATFORM)"==""
PLATFORM=ALPHA
!endif

DEBUGFLAGS = /Z7 /Od /D_DEBUG
MFCLIB=..\..\lib\nafxcwd.lib
MFCLIBDLL=..\..\lib\nafxdwd.lib

!if "$(PLATFORM)"=="INTEL"
CPUFLAGS = /D_X86_
CPP = cl
LINK = link
LIB32 = lib
MFCDLLENTRY = 
!else
!if "$(PLATFORM)"=="MIPS"
CPUFLAGS = /D_MIPS_
CPP = mcl
LINK = link32
LIB32 = lib32
MFCDLLENTRY = -entry:DllMain
!else
!if "$(PLATFORM)"=="ALPHA"
CPUFLAGS =
CPP = claxp
LINK = link32
LIB32 = lib32
MFCDLLENTRY = -entry:DllMain
!endif
!endif
!endif

MFCFLAGS=/D_WINDOWS
CPPFLAGS = /c /W3 $(DEBUGFLAGS) $(MFCFLAGS) $(CPUFLAGS) $(OPT)
GUIFLAGS = -debug:full -debugtype:both -subsystem:windows -entry:WinMainCRTStartup

GUILIBS = user32.lib gdi32.lib   \
	winspool.lib comdlg32.lib advapi32.lib \
	olecli32.lib olesvr32.lib shell32.lib \
	kernel32.lib libc.lib

GUILIBSDLL = $(GUILIBS)

goal: hello1.exe tracer.dll


#############################################################################

hello1.obj: hello1.cpp resource.h traceapi.h
	$(CPP) $(CPPFLAGS) hello1.cpp

hello1.exe: hello1.obj hello1.def hello1.res tracer.lib
	$(LINK) $(GUIFLAGS) -out:hello1.exe hello1.obj tracer.lib hello1.res $(MFCLIB) $(GUILIBS)

hello1.res: hello1.rc resource.h hello1.ico
!if "$(PLATFORM)"=="INTEL"
	rc -r hello1.rc
!else
!if "$(PLATFORM)"=="MIPS"
	rc -r -fo hello1.rct hello1.rc
	cvtres -MIPS -o hello1.res hello1.rct
	-erase hello1.rct
!else
!if "$(PLATFORM)"=="ALPHA"
	rc -r -fo hello1.rct hello1.rc
	cvtres -ALPHA -o hello1.res hello1.rct
	-erase hello1.rct
!endif
!endif
!endif

#############################################################################


tracer.obj: tracer.cpp traceapi.h
	$(CPP) $(CPPFLAGS) /D_WINDLL /D_USRDLL tracer.cpp

tracer.dll: tracer.obj tracer.res tracer.lib
	$(LINK) -debug:full -debugtype:cv -dll -out:tracer.dll \
	-subsystem:windows \
	-map:tracer.map \
	$(MFCDLLENTRY) \
	-base:0x1C000000 \
	tracer.obj tracer.res tracer.exp \
	$(MFCLIBDLL) $(GUILIBSDLL)

tracer.res: tracer.rc traceres.h
!if "$(PLATFORM)"=="INTEL"
	rc -r tracer.rc
!else
!if "$(PLATFORM)"=="MIPS"
	rc -r -fo tracer.rct tracer.rc
	cvtres -MIPS -o tracer.res tracer.rct
	-erase tracer.rct
!else
!if "$(PLATFORM)"=="ALPHA"
	rc -r -fo tracer.rct tracer.rc
	cvtres -ALPHA -o tracer.res tracer.rct
	-erase tracer.rct
!endif
!endif
!endif

# import library
tracer.lib: tracer.obj
!if "$(PLATFORM)"=="INTEL"
	$(LIB32) -machine:i386 -def:tracer.def tracer.obj -out:tracer.lib
!else
!if "$(PLATFORM)"=="MIPS"
	$(LIB32) -machine:mips -def:tracer.def tracer.obj -out:tracer.lib
!else
!if "$(PLATFORM)"=="ALPHA"
	$(LIB32) -machine:alpha -def:tracer.def tracer.obj -out:tracer.lib
!endif
!endif
!endif

#############################################################################

clean:
	-del *.exe
	-del *.dll
	-del *.res
	-del *.obj
	-del *.lib

#############################################################################
