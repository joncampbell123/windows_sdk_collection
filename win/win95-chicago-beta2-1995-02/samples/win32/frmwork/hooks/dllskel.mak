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
#     32-bit, debug             nmake -f dllskel.mak
#     32-bit, non-debug         nmake "_NODEBUG=1" -f dllskel.mak
#
#     16-bit builds are not allowed as this is a 32-bit only sample.
#


PROJ = dllskel

all: $(PROJ).dll $(PROJ).bsc


#--------------------------------------------------------------------------
# Files in project

PROJ_OBJS  =
BASE_OBJS  = dllmain.obj exports.obj
GLOBAL_DEP = dllglob.h
RC_DEP     = dllglob.h

LIBS   =
LIBS32 = user32.lib gdi32.lib kernel32.lib version.lib libcmt.lib
SBRS   = exports.sbr dllmain.sbr



#--------------------------------------------------------------------------
# Dependencies

dllmain.obj: dllmain.c $(GLOBAL_DEP)
exports.obj: exports.c $(GLOBAL_DEP)


#--------------------------------------------------------------------------
# Note:  You should not need to change anything below this line unless you
#        want to modify the compiler options, etc.  For normal project
#        maintenance, all changes are done above.

# Only 32-bit builds are allowed.
!ifdef MAK16
!ERROR This sample is 32-bit only. Please build it without defining MAK16.
!endif


#--------------------------------------------------------------------------
# Tools Options

!ifdef _NODEBUG
# Non-debugging options

!MESSAGE 32-bit Non-debugging build

CFLAGS    = /c /W3 /LD /MT /Ox /FR /nologo
CDEFINES  = /D"_X86_" /D"NDEBUG" /D"_WINDOWS" /D"WIN32" /D"_MT"
LFLAGS    = /DLL /NOLOGO /SUBSYSTEM:windows
RCFLAGS   = /r
RCDEFINES = /dNDEBUG /d, /dWIN32
DEFFILE   = /DEF:$(PROJ).def

!else
# Debugging options

!MESSAGE 32-bit Debugging build

CFLAGS    = /c /W3 /LD /MT /Od /Zi /FR /Fd$(PROJ).pdb /nologo
CDEFINES  = /D"_X86_" /D"_DEBUG" /D"_WINDOWS" /D"WIN32" /D"_MT"
LFLAGS    = /DLL /NOLOGO /DEBUG /DEF:$(PROJ).def /DEBUGTYPE:cv \
            /SUBSYSTEM:windows
RCFLAGS   = /r
RCDEFINES = /d_DEBUG /d, /dWIN32
MAPFILE   = /map:$(PROJ).map
!endif


#--------------------------------------------------------------------------
# Build Rules

$(PROJ).dll: $(BASE_OBJS) $(PROJ_OBJS) $(PROJ).res 
    link @<<
    $(LFLAGS)
    $(BASE_OBJS) $(PROJ_OBJS)
    /out:$(PROJ).dll
    /implib:$(PROJ).lib
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
# Inference Rules

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
    del *.dll
    del *.lib
