# Makefile : Builds a Foundation class library variant.
#
# Usage: NMAKE CLEAN        (removes all intermediary files)
#    or: NMAKE options      (builds one library variant (see below))
# Note that an NMAKE CLEAN should be performed before building a new variant.
#
# 'Options' are one of each of:
#   "MODEL=M"            (defaults to M)
#           Any of the following models are accepted: S (small), M (medium),
#           C (compact), L (large) or N (Windows NT).
#   "TARGET=W"           (defaults to W)
#           Any of the following platforms are accepted: R (real-mode DOS),
#           or W (windows).
#   "DLL"              (defaults to 0)
#           If this item is 1, a DLL version of the library is generated.
#           If this item is 0, then a normal library is generated.
#           Only Large model versions of DLLs are supported.
#   "DEBUG"             (defaults to 1)
#           If this item is 1, debugging support is compiled into
#           the library.  If this item is 0, then debugging support
#           is disabled.  Debug support does not include CodeView information.
#   "CODEVIEW=0"           (defaults to 2, always)
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
#
#   The default is to build MODEL=M TARGET=W DEBUG=1
#
#
#############################################################################
# Standard tools

CPP=cl
CC=cl
CV=/Zi

!if "$(MODEL)"=="n" || "$(MODEL)"=="N"
_NTWIN=1
!else
!if "$(OS)"=="Windows_NT"
!error MODEL=N required for Windows NT
!endif
!endif

#############################################################################
# Parse these options:

!ifndef DEBUG
DEBUG=1
!endif

!ifndef DLL
DLL=0
!else
!ifndef _NTWIN
# DLL must be large model, except under Windows/NT.
MODEL=l
!endif
!endif

!ifndef TARGET
TARGET=W
!endif

!ifndef MODEL
MODEL=M
!endif

!ifndef CODEVIEW
CODEVIEW=2
!endif

!if "$(DEBUG)" != "0"
DEBUGSUF=D
DEBDEFS=/D_DEBUG
!ifndef _NTWIN
DEBOPTS=/Odr /f
!else
DEBOPTS=/Od
!endif
!else
DEBUGSUF=
DEBDEFS=
DEBOPTS=/Oxt
!endif

!if "$(CODEVIEW)" == "1"
DEBOPTS=$(DEBOPTS) $(CV)
!endif

# CVEXTRA used for select CodeView information (main files only)
!if "$(CODEVIEW)" == "2"
CVEXTRA=$(CV)
!endif

!if "$(MODEL)"=="s" || "$(MODEL)"=="S"
CL_MODEL=/AS
!else
!if "$(MODEL)"=="m" || "$(MODEL)"=="M"
CL_MODEL=/AM
!else
!if "$(MODEL)"=="c" || "$(MODEL)"=="C"
CL_MODEL=/AC
!else
!if "$(MODEL)"=="l" || "$(MODEL)"=="L"
CL_MODEL=/AL
!else
!if "$(MODEL)"=="n" || "$(MODEL)"=="N"
# Windows NT
CL_MODEL=/D_NTWIN
!else
!error MODEL must be one of S, M, L, or N

!endif
!endif
!endif
!endif
!endif

!if "$(TARGET)"=="r" || "$(TARGET)"=="R"
!ifndef _NTWIN
TARGDEFS=/D_DOS
TARGOPTS=
EXPFLAG=
!else
!if "$(CPU)" == "MIPS"
TARGDEFS=/D_MIPS_=1
!else
TARGDEFS=/D_X86_=1
!endif
TARGOPTS=
EXPFLAG=
!endif
!else

!if "$(TARGET)"=="w" || "$(TARGET)"=="W"
MKWIN=1
!ifndef _NTWIN
TARGDEFS=/D_WINDOWS
TARGOPTS=/GA /GEs /G2
EXPFLAG=/GEe
!else
!if "$(CPU)" == "MIPS"
TARGDEFS=/D_WINDOWS /D_MIPS_=1
!else
TARGDEFS=/D_WINDOWS /D_X86_=1
!endif
TARGOPTS=/Gd
EXPFLAG=
!endif

!else

!error TARGET must be one of W, R

!endif
!endif


# REVIEW_NT: workaround for NMAKE bug
!if "$(OBJ)" == ""
!if "$(DLL)" != "0"
D=$$$(MODEL)$(TARGET)$(DEBUGSUF).dll
!else
D=$$$(MODEL)$(TARGET)$(DEBUGSUF)
!endif
!else
D=$(OBJ)
!endif

DEFS=$(DEBDEFS) $(TARGDEFS)
CL_OPT=/W3 $(DEBOPTS) $(TARGOPTS) $(OPT)

!if "$(DLL)" == "0"
# Normal library
GOAL=$(MODEL)afxc$(TARGET)$(DEBUGSUF)
!else
# DLL library (SS!=DS) - only Large model supported (compact model is possible)
GOAL=$(MODEL)afxd$(TARGET)$(DEBUGSUF)
!ifndef _NTWIN
CL_MODEL=$(CL_MODEL)w
TARGOPTS=/GD /G2
!else
TARGOPTS=/D_WINDLL
!endif
# /GD will define _WINDLL
!endif


#############################################################################
# Library Components

OBJECT=$D\object.obj $D\except.obj $D\dumpcont.obj $D\abort.obj \
	$D\assert.obj $D\archive.obj $D\archivex.obj $D\memory.obj \
	$D\validadd.obj $D\dumpinit.obj $D\version.obj

FILES=$D\file.obj $D\filetxt.obj $D\filemem.obj $D\filex.obj

COLLECTIONS=$D\array_b.obj $D\array_d.obj $D\array_o.obj $D\array_p.obj \
	$D\array_s.obj $D\array_w.obj $D\list_o.obj $D\list_p.obj \
	$D\list_s.obj $D\map_pp.obj $D\map_pw.obj $D\map_so.obj \
	$D\map_sp.obj $D\map_ss.obj $D\map_wo.obj $D\map_wp.obj $D\plex.obj

MISC=$D\string.obj $D\stringex.obj $D\time.obj

WINDOWS=$D\window.obj $D\wingdi.obj $D\winctrl.obj $D\winstr.obj \
	$D\winapp.obj $D\winmain.obj $D\winmenu.obj $D\winmdi.obj $D\trace.obj

!ifndef _NTWIN
WINEXTRAS=$D\penctrl.obj $D\winbtn.obj $D\windlgs.obj
!else
WINEXTRAS=$D\winbtn.obj $D\windlgs.obj
!endif

!if "$(DLL)" == "0"
OLE= $D\olemisc.obj $D\olefile.obj $D\olecli.obj $D\oleui.obj $D\oleui2.obj \
	$D\olesvr.obj
!else
OLE= # OLE not supported for DLLs
!endif

OBJS=$(OBJS) $(OBJECT) $(FILES) $(COLLECTIONS) $(MISC)

!ifdef MKWIN
OBJS=$(OBJS) $(WINDOWS) $(WINEXTRAS) $(OLE)
!endif


#############################################################################
# Set CPPFLAGS for use with .cpp.obj and .c.obj rules
# Define rule for use with OBJ directory
# C++ uses a PCH file

CPPFLAGS=$(CPPFLAGS) $(CL_STANDARD) $(CL_MODEL) $(CL_OPT) $(DEFS) 

!ifndef NO_PCH
PCH_FILE=$D\afxpch.pch
CPPFLAGS=$(CPPFLAGS) /Yu /Fp$(PCH_FILE)
!else
PCH_FILE=
!endif

CFLAGS=$(CFLAGS) $(CL_STANDARD) $(CL_MODEL) $(CL_OPT) $(DEFS) 

.SUFFIXES : .cpp

.cpp{$D}.obj:
	$(CPP) @<<
		$(CPPFLAGS) /c /Fo$@ $<
<<

.c{$D}.obj:
	$(CC) @<< 
		$(CFLAGS) /c /Fo$@ $<
<<

#############################################################################
# Goal to build

goal: $D ..\lib\$(GOAL).lib

$D:
	IF NOT EXIST $D mkdir $D

clean:
	-erase $D\*.obj
	-erase $D\*.pch
	-rmdir $D


#############################################################################
# Precompiled header file

!ifndef NO_PCH
INC_DIR=..\include
HDRS=$(INC_DIR)\afx.h $(INC_DIR)\afx.inl $(INC_DIR)\afxcoll.h \
		$(INC_DIR)\afxwin.h $(INC_DIR)\afxwin.inl $(INC_DIR)\afxmsg.h \
		$(INC_DIR)\afxres.h \
		$(INC_DIR)\afxole.h $(INC_DIR)\afxoleui.h

$D\object.obj $(PCH_FILE): object.cpp $(HDRS)
	$(CPP) /c /Yc /Fp$(PCH_FILE) $(CL_STANDARD) $(CL_MODEL) $(CL_OPT) $(DEFS) $(CVEXTRA) /Fo$D\object.obj object.cpp
!else
$D\object.obj: object.cpp
	$(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\object.obj object.cpp
!endif

############################################################################
# CodeView for select files
!if "$(CODEVIEW)"=="2"
$D\memory.obj : memory.cpp
	$(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\memory.obj memory.cpp

!ifdef MKWIN
$D\winmain.obj : winmain.cpp
	$(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\winmain.obj winmain.cpp

$D\window.obj : window.cpp
	$(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\window.obj window.cpp

$D\winapp.obj : winapp.cpp
	$(CPP) $(CPPFLAGS) $(CVEXTRA) /c /Fo$D\winapp.obj winapp.cpp
!endif
!endif


#############################################################################
# Windows 3.0 loader export/version number
$D\version.obj : version.cpp
	$(CPP) @<<
$(CPPFLAGS) $(EXPFLAG) /c /Fo$D\version.obj version.cpp
<<


#############################################################################
# Library results

..\lib\$(GOAL).lib: $D $(OBJS)
	-erase $@
!ifndef _NTWIN
	lib /PAGESIZE:128 @<<
..\lib\$(GOAL).lib
y
$(OBJS)
nul
;
<<
!else
	lib32 @<<
-out:..\lib\$(GOAL).lib $(OBJS)
<<
!endif

#############################################################################
