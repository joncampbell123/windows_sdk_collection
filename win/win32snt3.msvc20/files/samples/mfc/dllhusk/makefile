# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# QuickHelp and/or WinHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.

AFXDLL=1
EXTRA_LIBS=testdll1.lib testdll2.lib

goal: $(EXTRA_LIBS) testdll1.dll testdll2.dll dllhusk.exe

PROJ=DLLHUSK
OBJS=dllhusk.obj mainfrm.obj

!include ..\sample_.mak

clean::
	if exist *.dll erase *.dll

#############################################################################
# Common rules and values for building MFC Extension DLLs

DLL_CPPFLAGS=/MD /D_WINDLL /D_AFXEXT /W3
DLL_LINKFLAGS=/dll
!if "$(DEBUG)" == "0"
DLL_CPPFLAGS=$(DLL_CPPFLAGS) /O1
DLL_LINKFLAGS=$(DLL_LINKFLAGS) /debug:none
!else
DLL_CPPFLAGS=$(DLL_CPPFLAGS) /D_DEBUG /Od /Z7
DLL_LINKFLAGS=$(DLL_LINKFLAGS) /debug:full /debugtype:cv
!endif
!if "$(UNICODE)" != "1"
DLL_CPPFLAGS=$(DLL_CPPFLAGS) /D_MBCS
!else
DLL_CPPFLAGS=$(DLL_CPPFLAGS) /D_UNICODE
!endif

RFLAGS=$(RFLAGS) /D_AFXDLL

#############################################################################
# Special rules for building TESTDLL1

testdll1.obj: testdll1.cpp testdll1.h
	cl $(DLL_CPPFLAGS) /c testdll1.cpp

testdll1.dll testdll1.lib: testdll1.obj testdll1.res
	link $(DLL_LINKFLAGS) /out:testdll1.dll /map:testdll1.map \
	/implib:testdll1.lib testdll1.obj testdll1.res

#############################################################################
# Special rules for building TESTDLL2

testdll2.obj: testdll2.cpp testdll2.h
	cl $(DLL_CPPFLAGS) /c testdll2.cpp

testdll2.dll testdll2.lib: testdll2.obj testdll2.res
	link $(DLL_LINKFLAGS) /out:testdll2.dll /map:testdll2.map \
	/implib:testdll2.lib testdll2.obj testdll2.res

#############################################################################
