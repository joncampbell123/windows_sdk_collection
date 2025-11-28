# Makefile : Builds the Microsoft Foundation Class OLE Ansi Wrappers
#
# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992,93 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# WinHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.
#
# Usage: NMAKE CLEAN        (removes all intermediary files)
#    or: NMAKE options      (builds one library variant (see below))
# Note that an NMAKE CLEAN should be performed before building a new variant.
#
# 'Options' are one of each of:
#   "LIBNAME"          (defaults to OLE2ANSI)
#           This macro defines the name of the resulting library.  The
#           MFC version of this DLL is named MFCANS32.
#           If you customize the library, you must use a different name,
#           preferably one that contains the name of your application
#           or company.
#
#   "DEBUG"             (defaults to 1)
#           If this item is 1, debugging support is compiled into
#           the library.  If this item is 0, then debugging support
#           is disabled.  Debug support does not include CodeView information.
#
#  The redistributable MFC version was built with the following command:
#
#       nmake DEBUG=0 LIBNAME=MFCANS32
#
#  To build the debug versions of the standard MFC DLL:
#
#       nmake DEBUG=1 LIBNAME=MFCANS32
#
#############################################################################

# NOTE: YOU MUST SET "LIBNAME" MACRO TO A UNIQUE NAME FOR YOUR APPLICATION'S
#       PRIVATE VERSION OF THE OLE2ANSI DLL. YOU MAY NOT SHIP A DLL CALLED
#       "OLE2ANSI.DLL"
#
# MFC applications use the standard name MFCANS32.DLL

LIBNAME=OLE2ANSI


!if "$(DEBUG)" != "0"
CFLAGS  = /Fo$(OBJDIR)\ /c /Od /W3 /MT /Zi /Fd$(OBJDIR)\$(TARGET).pdb
LINK    = /debug:full /debugtype:cv /base:0x5F100000
DEFS    = /D_DEBUG_HOOKS /DMAKEDLL /DSTRICT /D_DEBUG /DWIN32 /DWIN32_LEAN_AND_MEAN
OBJDIR  = debug
!else
CFLAGS  = /Fo$(OBJDIR)\ /c /O1 /W3 /MT /Zi /Fd$(OBJDIR)\$(TARGET).pdb
LINK    = /debug:full /debugtype:cv /incremental:no /opt:ref /base:0x5F100000
DEFS    = /D_DEBUG_HOOKS /DMAKEDLL /DSTRICT /DWIN32 /DWIN32_LEAN_AND_MEAN
OBJDIR  = retail
!endif

!if "$(PROCESSOR_ARCHITECTURE)" == "x86"
LINK=$(LINK) /entry:DllMain@12
CFLAGS=$(CFLAGS) /Gz
!else
LINK=$(LINK) /entry:DllMain
CFLAGS=$(CFLAGS)
!endif

.SUFFIXES: .h .obj .exe .cpp .rc .def

TARGET  = $(LIBNAME)

goal:   create.dir $(OBJDIR)\$(TARGET).dll

clean:
	if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
	if exist $(OBJDIR)\*.pch del $(OBJDIR)\*.pch
	if exist $(OBJDIR)\*.bak del $(OBJDIR)\*.bak
	if exist $(OBJDIR)\*.pdb del $(OBJDIR)\*.pdb
	if exist $(OBJDIR)\*.exp del $(OBJDIR)\*.exp
	if exist $(OBJDIR)\*.lib del $(OBJDIR)\*.lib

PCHFLAGS= /Yuole2ansi.h -Fp$(OBJDIR)\ole2ansi.pch

INCLS    = ole2ansi.h

OBJS1    = @ole2ansi.obj @AnsiComp.obj @WideComp.obj @AnsiStor.obj @WideStor.obj
OBJS2    = @AnsiDvOb.obj @WideDvOb.obj @AnsiMoni.obj @WideMoni.obj
OBJS3    = @AnsiOle1.obj @WideOle1.obj @AnsiOle2.obj @WideOle2.obj @AnsiOle3.obj
OBJS4    = @AnsiDisp.obj @WideDisp.obj @AnsiOA.obj @Globals.obj
OBJS5    = @Convert.obj @Convert2.obj @Wrapper.obj @Thunk.obj @Trace.obj
OBJS6    = @AnsiCtl.obj @WrapCtl.obj @ConvCtl.obj @WideCtl.obj
OBJS     = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) @Debug.obj

OLELIBS  = ole32.lib oleaut32.lib uuid.lib
LIBS     = /nod libcmt.lib kernel32.lib user32.lib advapi32.lib

!if "$(DEBUG)" != "0"
OBJS=$(OBJS:@=debug\)
CVPACK=rem cvpack
!else
OBJS=$(OBJS:@=retail\)
CVPACK=cvpack
!endif

##### Build Rules #####

.cpp{$(OBJDIR)}.obj:
	cl $(CFLAGS) $(DEFS) $(PCHFLAGS) $(@B).cpp

create.dir:
	@-if not exist $(OBJDIR)\*.* mkdir $(OBJDIR)

$(OBJDIR)\Ole2Ansi.obj : Ole2Ansi.h Ole2Ansi.cpp
	cl $(CFLAGS) $(DEFS) /YcOle2Ansi.h /Fp$(OBJDIR)\Ole2Ansi.pch $(@B).cpp

$(OBJDIR)\ole2ansi.res : ole2ansi.rc
	rc /r /Fo$(OBJDIR)\ole2ansi.res ole2ansi.rc

$(OBJDIR)\$(TARGET).dll : $(OBJS) ole2ansi.def $(OBJDIR)\ole2ansi.res
	link $(LINK) /dll /out:$(OBJDIR)\$(TARGET).dll /map \
		/def:ole2ansi.def /implib:$(OBJDIR)\$(TARGET).lib \
		$(OBJS) $(OBJDIR)\ole2ansi.res $(OLELIBS) $(LIBS)
	$(CVPACK) /strip $@

## END
