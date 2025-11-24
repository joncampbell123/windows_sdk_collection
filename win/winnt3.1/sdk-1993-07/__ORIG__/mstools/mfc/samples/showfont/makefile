# Makefile : Builds the Showfont application
#
# Usage:     NMAKE option (build Showfont)
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

PCH=showfont.h
OBJS= showfont.obj mainwnd.obj drawing.obj dialogs.obj cfont.obj 

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AM /W3 /Zp /GA /GEs /G2 /Yu$(PCH)
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd libw mlibcew
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=mafxcw libw mlibcew
!endif


showfont.exe:   showfont.obj mainwnd.obj drawing.obj dialogs.obj cfont.obj \
	showfont.def showfont.res
	link $(LINKFLAGS) @<<
showfont mainwnd drawing dialogs cfont, 
showfont,
NUL, 
$(LIBS), 
showfont.def;
<<
	rc -30 /k /t showfont.res

!else

!include ..\ntsample.mak

showfont.exe:   showfont.obj mainwnd.obj drawing.obj dialogs.obj cfont.obj \
	showfont.def showfont.res
    $(LINK) $(GUIFLAGS) -out:showfont.exe $(OBJS) showfont.res $(MFCLIB) $(GUILIBS)

!endif

showfont.obj:   showfont.h mainwnd.h
	$(CPP) $(CPPFLAGS) /Yc$(PCH) /c showfont.cpp

mainwnd.obj:    showfont.h mainwnd.h
drawing.obj:    showfont.h mainwnd.h
dialogs.obj:    showfont.h mainwnd.h
cfont.obj:  showfont.h

showfont.h: resource.h

showfont.res:    resource.h about.dlg cfont.dlg showfont.ico

clean:
	-erase showfont.exe
	-erase showfont.pch
	-erase showfont.res
	-erase *.obj
