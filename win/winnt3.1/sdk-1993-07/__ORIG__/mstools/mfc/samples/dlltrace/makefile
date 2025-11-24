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

# Build hello1 application and Tracer DLL

DEBUG=1

goal: hello1.exe tracer.dll

#############################################################################
# hello1 application is medium model DEBUG

!ifdef WIN16MFC

hello1.obj: hello1.cpp resource.h traceapi.h
	cl /DWINVER=0x0300 /AM /Zi /Zp /D_DEBUG /c hello1.cpp

hello1.exe: hello1.obj hello1.def hello1.res tracer.lib
	link /NOD /CO /ONERROR:NOEXE @<<
hello1,
hello1.exe,
nul,
mafxcwd libw mlibcew tracer,
hello1.def;
<<
	rc -30 /t hello1.res

hello1.res: hello1.rc resource.h hello1.ico

!else

!include ..\ntsample.mak

hello1.obj: hello1.cpp resource.h traceapi.h
	$(CPP) $(CPPFLAGS) hello1.cpp

hello1.exe: hello1.obj hello1.def hello1.res tracer.lib
	$(LINK) $(GUIFLAGS) -out:hello1.exe hello1.obj tracer.lib hello1.res $(MFCLIB) $(GUILIBS)

hello1.res: hello1.rc resource.h hello1.ico

!endif

#############################################################################
# Tracer is a large model DLL built with MFC large model debug DLL support
# library

# /GD option automatically defines /D_WINDLL

!ifdef WIN16MFC

tracer.obj: tracer.cpp traceapi.h
	cl /DWINVER=0x0300 /ALw /Zi /GD /Gs /W3 /Zp /D_DEBUG /c tracer.cpp

tracer.dll: tracer.obj tracer.def tracer.res
	link /NOD  /ONERROR:NOEXE /CO /PACKDATA @<<
tracer.obj,
tracer.dll,
nul,
lafxdwd libw ldllcew,
tracer.def;
<<
	rc -30 /t tracer.res tracer.dll

# import library
tracer.lib: tracer.dll
	implib tracer.lib tracer.dll

!else

MFCLIB1=nafxdwd.lib

tracer.obj: tracer.cpp traceapi.h
	$(CPP) $(CPPFLAGS) /D_WINDLL /c /Zi tracer.cpp

tracer.dll: tracer.obj tracer.res tracer.lib
	$(LINK) -debug:full -debugtype:cv -dll -out:tracer.dll \
	-subsystem:windows \
	-entry:$(MFCDLLENTRY) -base:0x1C000000 \
	-map:tracer.map \
	tracer.obj tracer.res tracer.exp \
	$(MFCLIBDLL) $(GUILIBSDLL)

# import library
tracer.lib: tracer.obj tracer1.def
	$(LIB32) -machine:$(CPU) -def:tracer1.def tracer.obj -out:tracer.lib

!endif

#############################################################################

clean:
	-del *.exe
	-del *.dll
	-del *.res
	-del *.obj
	-del *.lib

#############################################################################
