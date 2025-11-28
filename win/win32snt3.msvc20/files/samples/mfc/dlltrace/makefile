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
#
# Options to NMAKE:
#     "OPT" => use to set custom compile options

CPPFLAGS=/c /W3 /GX /MT $(DEBUGFLAGS) $(OPT)
LINKFLAGS=/subsystem:windows
!if "$(DEBUG)" == "0"
CPPFLAGS=$(CPPFLAGS) /O1
!else
CPPFLAGS=$(CPPFLAGS) /Od /D_DEBUG
!endif
!if "$(UNICODE)" != "1"
CPPFLAGS=$(CPPFLAGS) /D_MBCS
!else
CPPFLAGS=$(CPPFLAGS) /D_UNICODE
LINKFLAGS=$(LINKFLAGS) /entry:wWinMainCRTStartup
!endif

!if "$(CODEVIEW)" != "0"
CPPFLAGS=$(CPPFLAGS) /Z7
LINKFLAGS=$(LINKFLAGS) /debug:full /debugtype:cv
!endif

!if "$(NO_PDB)" == "1"
LINKFLAGS=$(LINKFLAGS) /pdb:none
PDB_NONE=/pdb:none
!endif

goal: hello1.exe tracer.dll

#############################################################################

hello1.obj: hello1.cpp resource.h traceapi.h
	cl $(CPPFLAGS) hello1.cpp

hello1.exe: hello1.obj hello1.res tracer.lib
	link $(LINKFLAGS) /out:hello1.exe hello1.obj tracer.lib hello1.res

hello1.res: hello1.rc resource.h hello1.ico
	rc /r hello1.rc

#############################################################################

tracer.obj: tracer.cpp traceapi.h
	cl $(CPPFLAGS) /D_WINDLL /D_USRDLL tracer.cpp

tracer.dll tracer.lib: tracer.obj tracer.res
	link /debug:full /debugtype:cv /dll $(PDB_NONE) \
	/out:tracer.dll /def:tracer.def /implib:tracer.lib /map:tracer.map \
	tracer.obj tracer.res

tracer.res: tracer.rc traceres.h
	rc /r tracer.rc

#############################################################################

clean::
	if exist *.exe erase *.exe
	if exist *.dll erase *.dll
	if exist *.aps erase *.aps
	if exist *.pch erase *.pch
	if exist *.map erase *.map
	if exist *.obj erase *.obj
	if exist *.pdb erase *.pdb
	if exist *.map erase *.map
	if exist *.lib erase *.lib
	if exist *.res erase *.res

#############################################################################
