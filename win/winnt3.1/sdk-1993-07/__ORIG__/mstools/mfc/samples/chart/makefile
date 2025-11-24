# Makefile : Builds the Chart application in medium model.
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
# Usage:     NMAKE        (build chart application)
#    or:     NMAKE clean  (erase all compiled files)
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)


PCH=chart.h
OBJS= chart.obj chartwnd.obj chartdlg.obj chfile.obj dobject.obj chart.res

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AM /W3 /Zp /GA /GEs /G2 /Yu$(PCH) 
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=mafxcwd libw mlibcew commdlg
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=mafxcw libw mlibcew commdlg
!endif

chart.exe: chart.obj chartwnd.obj chartdlg.obj chfile.obj dobject.obj \
		   chart.res
	   link $(LINKFLAGS) @<<
chart chartwnd chartdlg chfile dobject,
chart,
NUL,
$(LIBS),
chart.def
<<
	   rc -30 /k /t chart.res
!else

!include ..\ntsample.mak

chart.exe:  $(OBJS)
    $(LINK) $(GUIFLAGS) -out:chart.exe $(OBJS) $(MFCLIB) $(GUILIBS)
	
!endif

chart.obj:      chart.cpp resource.h $(PCH) dobject.h chartwnd.h
	$(CPP) $(CPPFLAGS) /Yc$(PCH) /c chart.cpp

chartwnd.cpp:   resource.h chartwnd.h chartdlg.h dobject.h
chartdlg.cpp:   resource.h chartwnd.h chartdlg.h dobject.h
chfile.cpp:     chartwnd.h dobject.h
dobject.cpp:    resource.h dobject.h
chart.res:      resource.h entry.dlg about.dlg chart.ico

clean:
	-erase chart.exe
	-erase chart.pch
	-erase *.obj
	-erase chart.res
