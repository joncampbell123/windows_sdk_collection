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


PROJ = HOOKS

all: $(PROJ).exe $(PROJ).bsc


#--------------------------------------------------------------------------
# Files in project

PROJ_OBJS  = browse.obj header.obj list.obj split.obj treeview.obj \
             toolbar.obj statbar.obj propsht.obj
BASE_OBJS  = winmain.obj dispatch.obj init.obj misc.obj about.obj $(PROJ).obj
GLOBAL_DEP = globals.h
RC_DEP     = globals.h about.dlg browse.dlg $(PROJ).ico treeview.h toolbar.h \
             statbar.h listbx.h $(PROJ).h

LIBS   = syshook.lib
LIBS32 = user32.lib gdi32.lib kernel32.lib version.lib libc.lib comctl32.lib \
         comdlg32.lib
SBRS   = winmain.sbr dispatch.sbr init.sbr about.sbr misc.sbr \
         browse.sbr list.sbr split.sbr treeview.sbr


#--------------------------------------------------------------------------
# Dependencies

winmain.obj:  winmain.c  $(GLOBAL_DEP)
dispatch.obj: dispatch.c $(GLOBAL_DEP)
init.obj:     init.c     $(GLOBAL_DEP)
$(PROJ).obj:     $(PROJ).c    $(GLOBAL_DEP) header.h toolbar.h \
                                       statbar.h $(PROJ).h \
                                       listbx.h treeview.h \
                                       propsht.h syshook.h
misc.obj:     misc.c     $(GLOBAL_DEP)
about.obj:    about.c    $(GLOBAL_DEP)
browse.obj:   browse.c   $(GLOBAL_DEP) $(PROJ).h
header.obj:   header.c   $(GLOBAL_DEP) header.h
list.obj:     list.c     $(GLOBAL_DEP) listbx.h
split.obj:    split.c    $(GLOBAL_DEP) split.h
treeview.obj: treeview.c $(GLOBAL_DEP) treeview.h syshook.h
toolbar.obj:  toolbar.c  $(GLOBAL_DEP) toolbar.h
statbar.obj:  statbar.c  $(GLOBAL_DEP) statbar.h
propsht.obj:  propsht.c  $(GLOBAL_DEP) propsht.h


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
LFLAGS    = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows
RCFLAGS   = /r
RCDEFINES = /d_DEBUG /d, /dWIN32
MAPFILE   = /map:$(PROJ).map

!endif


#--------------------------------------------------------------------------
# Build Rules

# Since the application needs the DLL's import library, let's make sure
# the DLL gets built.

BUILD_DLL:
    nmake /f dllskel.mak
    nmake /f syshook.mak

$(PROJ).EXE: $(BASE_OBJS) $(PROJ_OBJS) $(PROJ).res BUILD_DLL
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
    del *.dll
