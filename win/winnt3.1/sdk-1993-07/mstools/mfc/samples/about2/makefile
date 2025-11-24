# Makefile : Builds the About2 application
#
# Usage:     NMAKE option (build About2)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)

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
# Based on the About2 application by Charles Petzold.
# The original application appeared in
# "Programming Windows", Second Edition (pp. 417-423),
# Copyright (C) 1990 Charles Petzold,
# published by Microsoft Press. Used with permission.


OBJS = about2.obj

!ifdef WIN16MFC
#!message compiling for WIN16

CPPFLAGS=  /DWINVER=0x0300 /AS /W3 /Zp /GA /GEs /G2
LINKFLAGS=/NOD /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /f /Zi
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=safxcwd libw slibcew
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)
LIBS=safxcw libw slibcew
!endif


about2.exe:    about2.obj about2.def about2.res
		link $(LINKFLAGS) about2, about2, NUL, $(LIBS), about2.def;
		rc -30 /t about2.res
!else
!include ..\ntsample.mak

about2.exe:    $(OBJS) about2.def about2.res
    $(LINK) $(GUIFLAGS) -out:about2.exe $(OBJS) about2.res $(MFCLIB) $(GUILIBS)
!endif

about2.res:     resource.h about2.dlg about2.ico

about2.obj: about2.h resource.h

clean:
		-erase about2.exe
		-erase about2.res
		-erase about2.obj
