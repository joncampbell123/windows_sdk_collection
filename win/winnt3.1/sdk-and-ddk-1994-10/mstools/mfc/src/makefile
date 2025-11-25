# Makefile : Builds a Microsoft Foundation Class library variant.
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
#   "TARGET=W"           (defaults to W)
#           Any of the following targets are accepted: 
#				R (console mode),
#           	W (windows).
#   "DLL"              (defaults to 0)
#           If this item is 0, then a normal library is generated.
#           If this item is 1, a DLL version of the library is generated.
#   "DEBUG"             (defaults to 1)
#           If this item is 1, debugging support is compiled into
#           the library.  If this item is 0, then debugging support
#           is disabled.  Debug support does not include CodeView information.
#   "CODEVIEW=0"           (defaults to 1, always)
#           If this item is 1 CodeView information is compiled into
#           the library.  You must use the /CODEVIEW link option
#           in addition, when linking your executable.  If this item
#           is 2, then only selected modules will be compiled with
#           CodeView information.  You must use the link option /CODEVIEW.
#           A value of 0 indicates that no CodeView information is to be
#           generated.
#   "OBJ=.\obj"          (defaults to '$$(MODEL)$(TARGET)$(DEBUG)'
#           This optional specification specifies where temporary OBJ files
#           are stored during the build process.  The directory is created or
#           removed as necessary.
#   "OPT="               (no default value)
#           This allows additional compiler options to be added to the build.
#           If more than one switch is desired, put double-quotes around the
#           whole OPT= argument, e.g., "OPT=/J /W3".
#   "NO_PCH=1" 
#           Set this item to override the default use of precompiled headers
#   "PLATFORM=INTEL"     (defaults to INTEL)
#           This option chooses the appropriate tools and sources for the
#           different platforms support by Windows/NT.  Currently INTEL and
#           MIPS are supported; more will be aded as they become available.
#
#   The default is to build PLATFORM=INTEL TARGET=W DEBUG=1
#
#
#############################################################################
# Define defaults if not defined

# Only NT 'MODEL' is supported by MFC 2.1
MODEL=N

# Default to ALPHA platform
!ifndef PLATFORM
PLATFORM=ALPHA
!endif

# Default to DEBUG mode
!ifndef DEBUG
DEBUG=1
!endif

# Default to NOT DLL
!ifndef DLL
DLL=0
!endif

# Default Target to Windows
!ifndef TARGET
TARGET=w
!endif

# Default Codeview Info
!ifndef CODEVIEW
!if "$(DEBUG)"=="1"
CODEVIEW=1
!else
CODEVIEW=0
!endif
!endif


#############################################################################
# normalize cases of parameters, or error check

!if "$(TARGET)"=="W"
!undef TARGET
TARGET=w
!else
!if "$(TARGET)"=="R"
!undef TARGET
TARGET=r
!endif
!endif

!if "$(CPU)"=="MIPS"
!if "$(PLATFORM)"!="MIPS"
!error Must set PLATFORM=MIPS for MIPS builds
!endif
!endif

!if "$(CPU)"=="ALPHA"
!if "$(PLATFORM)"!="ALPHA"
!error Must set PLATFORM=ALPHA for ALPHA builds
!endif
!endif


#############################################################################
# Parse these options:

#
# DEBUG OPTIONS
#
!if "$(DEBUG)" != "0"
DEBUGSUF=D
DEBDEFS=/D_DEBUG
DEBOPTS=/Od
!else

#
# NON-DEBUG OPTIONS
#
!if "$(PLATFORM)"=="INTEL"
DEBUGSUF=
DEBDEFS=
DEBOPTS=/O1	/Gy
!else
!if "$(PLATFORM)"=="MIPS"
DEBUGSUF=
DEBDEFS=
DEBOPTS=/Osg /Gs /Gy
!else
!if "$(PLATFORM)"=="ALPHA"
DEBUGSUF=
DEBDEFS=
DEBOPTS=/Ox
!endif
!endif
!endif
!endif

#
# CODEVIEW options
#

!if "$(CODEVIEW)" == "1"
CVOPTS=/Z7
!endif


# CVEXTRA used for select CodeView information (main files only)
!if "$(CODEVIEW)" == "2"
CVEXTRA=/Z7
!endif

#
# PLATFORM options
#
!if "$(PLATFORM)"=="INTEL"
CPP=cl
CL_MODEL=/D_X86_
LIB32=lib
!else
!if "$(PLATFORM)"=="MIPS"
CPP=mcl
CL_MODEL=/D_MIPS_
LIB32=lib32
!else
!if "$(PLATFORM)"=="ALPHA"
CPP=claxp
CL_MODEL=
LIB32=lib32
!else
!error PLATFORM must be one of INTEL or MIPS or ALPHA
!endif
!endif
!endif

#
# TARGET options
!if "$(TARGET)"=="r"
TARGDEFS=/D_CONSOLE
TARGOPTS=
!else
!if "$(TARGET)"=="w"
MKWIN=1
TARGDEFS=/D_WINDOWS
TARGOPTS=
!else
!error TARGET must be one of W or R.
!endif
!endif

# TYPE = Library Type Designator
#       c = normal C library
#       d = DLL library
TYPE=c

#
# DLL Variants
#
!if "$(DLL)" == "1"
# _USRDLL library (SS!=DS)
TYPE=d
DEXT=.dll
TARGOPTS=/D_USRDLL /D_WINDLL
!else
DEXT=
!endif

#
# Object File Directory
#
!if "$(OBJ)" == ""
D=$$$(MODEL)$(TARGET)$(DEBUGSUF)$(DEXT)  # subdirectory specific to variant
!else
D=$(OBJ)                                 # User specified directory
!endif

#
# COMPILER OPTIONS
#
!if "$(PLATFORM)"=="ALPHA"
CL_OPT=/W3 /Gf $(DEBOPTS) $(CVOPTS) $(TARGOPTS)
!else
!if "$(PLATFORM)"=="MIPS"
CL_OPT=/W3 /Gf $(DEBOPTS) $(CVOPTS) $(TARGOPTS)
!else
# INTEL
CL_OPT=/W3 /WX /Gf /Zl $(DEBOPTS) $(CVOPTS) $(TARGOPTS)
!endif
!endif

#############################################################################
# Options always built from above conditionals

DEFS=$(DEBDEFS) $(TARGDEFS)
GOAL=$(MODEL)afx$(TYPE)$(TARGET)$(DEBUGSUF)

#############################################################################
# Library Components

OBJECT=$D\objcore.obj $D\except.obj $D\afxver.obj \
    $D\validadd.obj $D\dumpcont.obj $D\dumpflt.obj \
    $D\arccore.obj $D\arcobj.obj $D\arcex.obj

# non-shared diagnostics
OBJDIAG=$D\dumpinit.obj $D\dumpout.obj \
    $D\afxasert.obj $D\afxmem.obj $D\afxabort.obj

FILES=$D\filecore.obj $D\filetxt.obj $D\filemem.obj $D\filex.obj $D\filest.obj

COLLECTIONS1=$D\array_b.obj $D\array_d.obj $D\array_p.obj $D\array_o.obj \
    $D\array_s.obj $D\array_u.obj $D\array_w.obj \
    $D\list_o.obj $D\list_p.obj $D\list_s.obj

COLLECTIONS2=$D\map_pp.obj $D\map_pw.obj $D\map_so.obj \
    $D\map_sp.obj $D\map_ss.obj $D\map_wo.obj $D\map_wp.obj $D\plex.obj

MISC=$D\strcore.obj $D\strex.obj $D\timecore.obj

WINDOWS=\
    $D\wincore.obj $D\winfrm.obj $D\winmdi.obj $D\winhand.obj $D\winmain.obj \
    $D\barcore.obj $D\bartool.obj $D\bardlg.obj \
    $D\dcprev.obj $D\dcmeta.obj

DIALOG=\
    $D\dlgcore.obj $D\dlgdata.obj $D\dlgfloat.obj \
    $D\winctrl.obj $D\winbtn.obj \
    $D\dlgfile.obj $D\dlgprnt.obj $D\dlgclr.obj $D\dlgfnt.obj $D\dlgfr.obj

WINMISC=\
    $D\wingdi.obj $D\winstr.obj $D\winmenu.obj \
    $D\auxdata.obj $D\afxtrace.obj

DOCVIEW=\
    $D\cmdtarg.obj $D\doccore.obj $D\doctempl.obj \
    $D\docsingl.obj $D\docmulti.obj \
    $D\viewcore.obj $D\viewprnt.obj $D\winsplit.obj $D\viewscrl.obj \
    $D\viewform.obj $D\viewedit.obj $D\viewprev.obj

APPLICATION=\
    $D\appcore.obj $D\appui.obj $D\appgray.obj $D\appdlg.obj $D\appprnt.obj \
    $D\apphelp.obj $D\apphelpx.obj

!if "$(DEBUG)" == "1"
!ifdef MKWIN
INLINES = $D\afxinl1.obj $D\afxinl2.obj $D\afxinl3.obj
!else
INLINES = $D\afxinl1.obj
!endif
!else
INLINES =
!endif

OLE= $D\olemisc.obj $D\olefile.obj $D\oledoc.obj $D\olecli.obj \
	$D\oleui.obj $D\oleui2.obj $D\olesvr.obj $D\oletsvr.obj

OBJS=$(OBJECT) $(OBJDIAG) $(INLINES) $(FILES) $(COLLECTIONS1) $(COLLECTIONS2) $(MISC)

!ifdef MKWIN
OBJS=$(OBJS) $(WINDOWS) $(DIALOG) $(WINMISC) $(DOCVIEW) $(APPLICATION) $(OLE)
!endif

#############################################################################
# Standard tools

#############################################################################
# Set CPPFLAGS for use with .cpp.obj and .c.obj rules
# Define rule for use with OBJ directory
# C++ uses a PCH file

CPPFLAGS=$(CPPFLAGS) $(CL_MODEL) $(CL_OPT) $(DEFS) $(OPT)

!ifndef NO_PCH
!ifndef PCH_FILE
PCH_FILE=$D\stdafx.pch
!endif

CPPFLAGS=$(CPPFLAGS) /Yustdafx.h /Fp$(PCH_FILE)
!else
PCH_FILE=
!endif

.SUFFIXES: .cpp

.cpp{$D}.obj:
    $(CPP) $(CPPFLAGS) /c /Fo$D\ $<

#############################################################################
# Goals to build

goal: create.dir ..\lib\$(GOAL).lib

create.dir:
    @-if not exist $D\*.* mkdir $D

clean:
    -if exist $D\*.obj erase $D\*.obj
    -if exist $D\*.pch erase $D\*.pch
    -rmdir $D

#############################################################################
# Precompiled header file

!ifndef NO_PCH
HDRS =..\include\*.h ..\include\*.inl

$D\objcore.obj $(PCH_FILE): objcore.cpp $(HDRS)
    $(CPP) /c /Ycstdafx.h /Fp$(PCH_FILE) $(CL_MODEL) $(CL_OPT) $(DEFS) $(CVEXTRA) $(OPT) /c /Fo$D\objcore.obj objcore.cpp

!endif

############################################################################
# CodeView for select files

!if "$(CODEVIEW)"=="2"

$D\afxmem.obj : afxmem.cpp
    $(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\afxmem.obj afxmem.cpp

!ifdef MKWIN
$D\winmain.obj : winmain.cpp
    $(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\winmain.obj winmain.cpp

$D\wincore.obj : wincore.cpp
    $(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\wincore.obj wincore.cpp

$D\appcore.obj : appcore.cpp
    $(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\appcore.obj appcore.cpp
!endif
!endif

#############################################################################
# Build the library from the up-to-date objs

..\lib\$(GOAL).lib: $(OBJS)
    @-if exist $@ erase $@
    @$(LIB32) -out:$@ @<<
$(OBJS)
<<

#############################################################################
