# Makefile : Builds the helloapp application
#
# Usage:     NMAKE helloapp

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

helloapp.exe: helloapp.obj helloapp.def
	link /NOD /ONERROR:NOEXE helloapp,,,safxcw libw slibcew,helloapp.def;
	rc -30 /t helloapp.exe

helloapp.obj: helloapp.cpp
	cl /DWINVER=0x0300 /Zp /Oxs /c helloapp.cpp
!else
!include ..\ntsample.mak

OBJS=helloapp.obj

helloapp.exe: $(OBJS) helloapp.def
    $(LINK) $(GUIFLAGS) -out:helloapp.exe helloapp.obj $(MFCLIB) $(GUILIBS)

!endif

clean:
	-del helloapp.exe
	-del helloapp.obj
