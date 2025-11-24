# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.


# There are two sets of tools options and build rules below: one for
# debug builds and one for non-debug builds.
#
# To execute a build, refer to the following table:
#
#     Build Type                Command line
#     -----------------         ----------------------------
#     32-bit, debug             nmake
#     32-bit, non-debug         nmake "_NODEBUG=1"
#
#     16-bit builds are not allowed as this is a 32-bit only sample.
#

PROJ = APP32

all: $(PROJ).exe $(PROJ).bsc

#--------------------------------------------------------------------------
# Files in project. Notice that the BASE_OBJS are dependent on the 16-bit
# dll. Technically, they are not but this is one way to force the making 
# of the 16-bit dll first so that we get the message about building the
# 16-bit side before building the 32-bit side.

PROJ_OBJS  = $(PROJ).obj thkdlg.obj
BASE_OBJS  = winmain.obj init.obj dispatch.obj misc.obj about.obj
GLOBAL_DEP = globals.h
RC_DEP     = globals.h about.dlg $(PROJ).ico thunk.dlg

LIBS   = DLL32.lib
LIBS32 = user32.lib gdi32.lib kernel32.lib version.lib libc.lib
SBRS   = $(PROJ).sbr winmain.sbr init.sbr dispatch.sbr misc.sbr about.sbr thkdlg.sbr


#--------------------------------------------------------------------------
# Dependencies

$(PROJ).obj:  $(PROJ).c  $(GLOBAL_DEP) DLL32.h
winmain.obj:  winmain.c  $(GLOBAL_DEP)
init.obj:     init.c     $(GLOBAL_DEP)
dispatch.obj: dispatch.c $(GLOBAL_DEP)
misc.obj:     misc.c     $(GLOBAL_DEP)
about.obj:    about.c    $(GLOBAL_DEP)
thkdlg.obj:   thkdlg.c   $(GLOBAL_DEP)


#--------------------------------------------------------------------------
# Note:  You should not need to change anything below this line unless you
#        want to modify the compiler options, etc.  For normal project
#        maintenance, all changes are done above.

!ifdef MAK16
!ERROR This sample is 32-bit only. Please build without defining MAK16.
!endif

#--------------------------------------------------------------------------
# Tools Options

!ifdef _NODEBUG
# Non-debugging options

!MESSAGE 32-bit Non-debugging build

CFLAGS    = /c /W3 /ML /Ox /FR /nologo
CDEFINES  = /D"_X86_" /D"NDEBUG" /D"_WINDOWS" /D"WIN32"
LFLAGS    = /NOLOGO /SUBSYSTEM:windows,4.0
RCFLAGS   = /r
RCDEFINES = /dNDEBUG /d, /dWIN32
MAPFILE   =

!else
# Debugging options

!MESSAGE 32-bit Debugging build

CFLAGS    = /c /W3 /ML /Od /Zi /FR /Fd$(PROJ).pdb /nologo
CDEFINES  = /D"_X86_" /D"_DEBUG" /D"_WINDOWS" /D"WIN32"
LFLAGS    = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows,4.0
RCFLAGS   = /r
RCDEFINES = /d_DEBUG /d, /dWIN32
MAPFILE   = /map:$(PROJ).map

!endif


#--------------------------------------------------------------------------
# Build Rules

# Since the application needs the DLL's import library, let's make sure
# the DLL gets built.

# -------------------------------------------------------------------------
# Check to see if the 16-bit dll is made first. If it's not, then we
# error out telling the user to go to the DLL16 directory and switch
# to 16-bit tools to make it.

DLL16.DLL:
    nmake /f dll16.mak

DLL32.DLL:
    nmake /f DLL32.mak

$(PROJ).EXE: $(BASE_OBJS) $(PROJ_OBJS) $(PROJ).res DLL32.DLL DLL16.DLL
    link @<<
    $(LFLAGS)
    $(BASE_OBJS) $(PROJ_OBJS)
    /out:$(PROJ).exe
    $(MAPFILE)
    $(PROJ).res
    $(LIBS32) $(LIBS)
<<

$(PROJ).bsc: $(SBRS)
    bscmake @<<
    /o$(PROJ).bsc $(SBRS)
<<

$(PROJ).res: $(PROJ).rc $(RC_DEP)
    rc $(RCFLAGS) $(RCDEFINES) /fo$(PROJ).res $(PROJ).rc


#--------------------------------------------------------------------------
# Inference rules

.c.obj:
    cl @<<
    $(CFLAGS) $(CDEFINES) $<
<<


#--------------------------------------------------------------------------
# Rules for cleaning out those old files

clean:
    del *.bak
    del *.pdb
    del *.obj
    del *.res
    del *.exp
    del *.map
    del *.sbr
    del *.bsc

cleaner: clean
    del *.exe
    del *.lib
    del dll32.dll
