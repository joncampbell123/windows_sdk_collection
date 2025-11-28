# Makefile : Builds the Microsoft Foundation Class OLE common dialogs.
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
#   "LIBNAME"          (defaults to OLE2UI)
#           This macro defines the name of the resulting library.  The
#           MFC versions of this DLL are named MFCUIA32 and MFCUIW32.
#           If you customize the library, you must use a different name,
#           preferably one that contains the name of your application
#           or company.
#
#   "DEBUG"             (defaults to 1)
#           If this item is 1, debugging support is compiled into
#           the library.  If this item is 0, then debugging support
#           is disabled.  Debug support does not include CodeView information.
#
#   "UNICODE"           (defaults to 0)
#           If this item is set to 1, the UNICODE variant is built.
#           Otherwise, the ANSI version is built.  ANSI and UNICODE
#           variants should be given different names.
#
#   "ANSILIB"           (defaults to OLE2ANSI)
#           Specifies the name of the ANSI wrapper DLL to use.  This option
#           is only used for the ANSI build variant.
#
#   "LANG"              (defaults to USA)
#           Specifies the subdirectory from which resources should be
#           built from.  Resources for FRN and GER are provided as
#           samples.
#
#  The redistributable MFC versions were built with the following command:
#
#       nmake DEBUG=0 UNICODE=0 LIBNAME=MFCUIA32 ANSILIB=MFCANS32
#       nmake DEBUG=0 UNICODE=1 LIBNAME=MFCUIW32
#
#  To build the debug versions of the standard MFC DLLs:
#
#       nmake DEBUG=1 UNICODE=0 LIBNAME=MFCUIA32 ANSILIB=MFCANS32
#       nmake DEBUG=1 UNICODE=1 LIBNAME=MFCUIW32
#
#############################################################################

# NOTE: YOU MUST SET "LIBNAME" MACRO TO A UNIQUE NAME FOR YOUR APPLICATION'S
#       PRIVATE VERSION OF THE OLE2UI DLL. YOU MAY NOT SHIP A DLL CALLED
#       "OUTLUI.DLL"
#
# MFC applications use the standard name MFCUIx32.DLL

LIBNAME=OLE2UI32
ANSILIB=OLE2ANSI

# default build is DEBUG
DEBUG=1

RESOURCE=RES
CC=cl
RC=rc
LINK=link

!ifndef LANG
LANG=USA
!endif

!if ("$(DEBUG)"=="1")
#
#  Build DEBUG UILibrary
#
OBJ=debug
CFLAGS=/c /Od /MD /W3 /Zi /Fd$(TARGET).pdb \
	/D_DEBUG /D_WINDLL /DWIN32 /DWIN32_LEAN_AND_MEAN
RFLAGS=/D_DEBUG /DWIN32 /DWIN32_LEAN_AND_MEAN
LFLAGS=/debug:full /debugtype:cv
UILIBS=/nod kernel32.lib user32.lib gdi32.lib shell32.lib \
	advapi32.lib comdlg32.lib ole32.lib uuid.lib
CVPACK=rem cvpack
!else
#
#  Build RETAIL UILibrary
#
OBJ=retail
CFLAGS=/c /O1 /MD /W3 /Zi /Fd$(TARGET).pdb \
	/D_WINDLL /DWIN32 /DWIN32_LEAN_AND_MEAN
RFLAGS=/DWIN32 /DWIN32_LEAN_AND_MEAN
LFLAGS=/debug:full /debugtype:cv /incremental:no /opt:ref
UILIBS=/nod kernel32.lib user32.lib gdi32.lib shell32.lib \
	advapi32.lib comdlg32.lib ole32.lib uuid.lib
CVPACK=cvpack
!endif

!if "$(PROCESSOR_ARCHITECTURE)"=="x86"
CFLAGS=$(CFLAGS) /Gz
!endif

!ifndef NTSDK
!if "$(PROCESSOR_ARCHITECTURE)"=="x86"
NTSDK=0
!endif
!if "$(PROCESSOR_ARCHITECTURE)"=="MIPS"
NTSDK=0
!endif
!ifndef NTSDK
NTSDK=1
!endif
!endif

!if "$(NTSDK)"=="1"
UILIBS=$(UILIBS) crtdll.lib
!else
UILIBS=$(UILIBS) msvcrt.lib
!endif

!if "$(UNICODE)"=="1"
CFLAGS=$(CFLAGS) /D_UNICODE
OBJ=U$(OBJ)
LFLAGS=$(LFLAGS) /base:0x5F300000
!else
UILIBS=$(ANSILIB).lib $(UILIBS)
LFLAGS=$(LFLAGS) /base:0x5F200000
!endif
LFLAGS=$(LFLAGS) /subsystem:windows,4.0

DLLOBJS = $(UI_DLLOBJS)
LIBOBJS = $(UI_COBJS)
TARGET = $(OBJ)\$(LIBNAME)

goal: create.dir $(TARGET).DLL

# ----------------------------------------------------------------------------
#                      O B J E C T   F I L E   L I S T
# ----------------------------------------------------------------------------

UI_COBJS = \
	$(OBJ)\precomp.obj \
	$(OBJ)\busy.obj \
	$(OBJ)\common.obj \
	$(OBJ)\convert.obj \
	$(OBJ)\drawicon.obj \
	$(OBJ)\icon.obj \
	$(OBJ)\iconbox.obj \
	$(OBJ)\insobj.obj \
	$(OBJ)\links.obj \
	$(OBJ)\ole2ui.obj \
	$(OBJ)\olestd.obj \
	$(OBJ)\pastespl.obj \
	$(OBJ)\targtdev.obj \
	$(OBJ)\oleutl.obj \
	$(OBJ)\regdb.obj \
	$(OBJ)\resimage.obj \
	$(OBJ)\utility.obj \
	$(OBJ)\geticon.obj \

!if "$(DEBUG)" == "1"
UI_COBJS = $(UI_COBJS) \
	$(OBJ)\dbgutil.obj \
!endif

UI_DLLOBJS = \
	$(OBJ)\dllfuncs.obj \

# ----------------------------------------------------------------------------
#                       R E S O U R C E   L I S T
# ----------------------------------------------------------------------------
RES_DEP = \
	busy.h \
	common.h \
	convert.h \
	links.h \
	icon.h \
	iconbox.h \
	insobj.h \
	ole2ui.h \
	pastespl.h \
	resimage.h \
	$(RESOURCE)\STATIC\default.ico \
	$(RESOURCE)\STATIC\bang.ico \
	$(RESOURCE)\STATIC\egares.bmp \
	$(RESOURCE)\STATIC\hivgares.bmp \
	$(RESOURCE)\STATIC\vgares.bmp \
	$(RESOURCE)\$(LANG)\strings.rc \
	$(RESOURCE)\$(LANG)\busy.dlg \
	$(RESOURCE)\$(LANG)\convert.dlg \
	$(RESOURCE)\$(LANG)\fileopen.dlg \
	$(RESOURCE)\$(LANG)\icon.dlg \
	$(RESOURCE)\$(LANG)\insobj.dlg \
	$(RESOURCE)\$(LANG)\links.dlg \
	$(RESOURCE)\$(LANG)\pastespl.dlg \
	$(RESOURCE)\$(LANG)\prompt.dlg \
	$(RESOURCE)\ole2ui.rcv \
	$(RESOURCE)\$(LANG)\verlocal.h \

# Select language for version resource if localized version
!if ("$(LANG)"!="USA") && ("$(LANG)"!="usa")
RFLAGS=$(RFLAGS) -DVER_LOC
!endif

.SUFFIXES: .c .asm .obj .res .rc .def .bmp .ico .exe .dll .cod .str

# ----------------------------------------------------------------------------
#                     I N F E R E N C E   R U L E S
# ----------------------------------------------------------------------------

# compile C file into object directory
{}.c{$(OBJ)}.obj:
	$(CC) $(CFLAGS) -Yuprecomp.h -Fp$(OBJ)\precomp.pch -Fo$(OBJ)\ $(@B).c

# compile CPP file into object directory
{}.cpp{$(OBJ)}.obj:
	$(CC) $(CFLAGS) -Yuprecomp.h -Fp$(OBJ)\precomp.pch -Fo$(OBJ)\ $(@B).cpp

{}.rc{$(OBJ)}.res:
	@echo Creating <<uiclass.h
#define SZCLASSICONBOX  "$(LIBNAME)iconbox"
#define SZCLASSRESULTIMAGE "$(LIBNAME)resimage"
<<KEEP
	$(RC) /I$(RESOURCE)\$(LANG);$(RESOURCE)\static /Fo$(OBJ)\$(@B).res /r $(RFLAGS) $(@B).rc
	if exist $(TARGET).dll del $(TARGET).dll
	del uiclass.h

# ----------------------------------------------------------------------------
#                        G O A L   T A R G E T S
# ----------------------------------------------------------------------------
!include "depend"

clean:
	if exist $(OBJ)\*.* del /q $(OBJ)\*.*
	if exist $(TARGET).dll del $(TARGET).dll
	if exist $(TARGET).lib del $(TARGET).lib

create.dir:
	@-if not exist $(OBJ)\*.* mkdir $(OBJ)

$(OBJ)\precomp.obj: precomp.cpp
	$(CC) $(CFLAGS) -Fp$(OBJ)\precomp.pch -Ycprecomp.h -Fo$(OBJ)\$(@B) precomp.cpp

$(OBJ)\ole2ui.res: ole2ui.rc $(RES_DEP)

ole2ui.rc : $(RESOURCE)\$(LANG)\strings.rc

#
# Build .DLL dynamic-link library
#

$(TARGET).dll: $(PRECOMPOBJ) $(LIBOBJS) $(DLLOBJS) $(OBJ)\ole2ui.res ole2ui.def
   if exist $(LIBNAME).def del $(LIBNAME).def
   @echo Creating <<$(LIBNAME).def
LIBRARY         $(LIBNAME) INITINSTANCE
DESCRIPTION     'OLE 2.0 UI Support Library.'
<<KEEP
   type ole2ui.def >> $(LIBNAME).def
   $(LINK) @<<
$(LFLAGS)
$(PRECOMPOBJ: = ^
)
$(DLLOBJS: = ^
)
$(LIBOBJS: = ^
)
$(OBJ)\ole2ui.res
/out:$(TARGET).dll
/map:$(TARGET).map
/dll
$(UILIBS)
/def:$(LIBNAME).def
<<KEEP
	$(CVPACK) /strip $@

$(OBJ)\dllfuncs.obj:
	@echo Creating <<uiclass.h
#define SZCLASSICONBOX  "$(LIBNAME)iconbox"
#define SZCLASSRESULTIMAGE "$(LIBNAME)resimage"
#define LIBNAME "$(LIBNAME)"
<<KEEP
	$(CC) $(CFLAGS) -Yuprecomp.h -Fp$(OBJ)\precomp.pch -Fo$(OBJ)\ $(@B).cpp

# EOF ========================================================================
