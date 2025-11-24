# Makefile : Builds the ctrltest application
#
# Usage:     NMAKE option (build Multipad)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)
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


OBJS=ctrltest.obj paredit.obj \
	dertest.obj wclstest.obj paredit2.obj subtest.obj \
	bbutton.obj custlist.obj custmenu.obj \
	spintest.obj spin.obj

!ifdef WIN16MFC
#!message compiling for WIN16

OBJS = $(OBJS) derpen.obj dlgpen.obj featpen.obj 
!endif

PCH=ctrltest.h

!ifdef WIN16MFC
#!message compiling for WIN16

# program uses Win31 #defines, but not any Win31 library entries (i.e. rc -30)
CPPFLAGS= /AM /W3 /Zp /GA /GEs /G2 /Yu$(PCH) 
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd libw commdlg mlibcew
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)
LIBS=mafxcw libw commdlg mlibcew
!endif

ctrltest.exe: ctrltest.res ctrltest.def $(OBJS)
	link $(LINKFLAGS) @<<
$(OBJS),
ctrltest,
NUL,
$(LIBS),
ctrltest.def;
<<
	rc -30 /k /t ctrltest.res

!else

!include ..\ntsample.mak

ctrltest.exe: ctrltest.def ctrltest.res $(OBJS)
    $(LINK) $(GUIFLAGS) -out:ctrltest.exe $(OBJS) ctrltest.res $(MFCLIB) $(GUILIBS)

!endif

ctrltest.obj : ctrltest.cpp ctrltest.h
	$(CC) $(CPPFLAGS) /Yc$(PCH) /c ctrltest.cpp

*.obj:  ctrltest.h $(PCH)
ctrltest.res:  *.ico *.dlg ctrltest.h

clean:
	-erase ctrltest.exe
	-erase ctrltest.res
	-erase ctrltest.pch
	-erase *.obj
