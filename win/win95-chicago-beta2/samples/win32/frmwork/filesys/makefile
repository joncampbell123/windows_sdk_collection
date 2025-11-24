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


# **TODO** Change PROJ = to the name of your application

PROJ = FILESYS
all: $(PROJ).exe $(PROJ).bsc


#--------------------------------------------------------------------------
# Files in project

# **TODO** Add any project-specific object files to PROJ_OBJS
# **TODO** Add any global dependencies to GLOBAL_DEP
# **TODO** Add any resource dependencies to RC_DEP

PROJ_OBJS  = filesys.obj demo.obj
BASE_OBJS  = winmain.obj init.obj misc.obj about.obj dispatch.obj
GLOBAL_DEP = globals.h
RC_DEP     = globals.h demo.h demo.dlg 

# **TODO** Add any app build libraries to LIBS
# **TODO** Add any 32-bit build libraries to LIBS32
# **TODO** Add any browser objects (.sbr) to SBRS

LIBS   =
LIBS32 = user32.lib gdi32.lib kernel32.lib version.lib libc.lib 
SBRS   = winmain.sbr init.sbr dispatch.sbr misc.sbr about.sbr \
         demo.sbr filesys.sbr 

   


#--------------------------------------------------------------------------
# Dependencies

# **TODO** Add file dependences to build any new files listed above

winmain.obj:  winmain.c  $(GLOBAL_DEP)
init.obj:     init.c     $(GLOBAL_DEP)
misc.obj:     misc.c     $(GLOBAL_DEP)
about.obj:    about.c    $(GLOBAL_DEP)
dispatch.obj: dispatch.c $(GLOBAL_DEP)
demo.obj:     demo.c     $(GLOBAL_DEP) demo.h
filesys.obj:  filesys.c  $(GLOBAL_DEP) demo.h 


#--------------------------------------------------------------------------
# Note:  You should not need to change anthing below this line unless you
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
LFLAGS    = /NOLOGO /SUBSYSTEM:windows
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

$(PROJ).EXE: $(BASE_OBJS) $(PROJ_OBJS) $(PROJ).res
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

# End of file -------------------------------------------------------------
