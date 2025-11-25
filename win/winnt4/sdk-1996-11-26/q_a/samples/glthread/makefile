# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
#
#
# Processor independent makefile

# Nmake macros for building Windows 32-Bit apps
!include <win32.mak>

PROJ = GLThread

all: $(PROJ).exe

# Define project specific macros
PROJ_OBJS  = glthread.obj threads.obj
EXTRA_LIBS = opengl32.lib glu32.lib
GLOBAL_DEP = glthread.h glos.h
RC_DEP     = resource.h	glthread.ico

# Dependencies
glthread.obj: glthread.c $(GLOBAL_DEP)
threads.obj:  threads.c  $(GLOBAL_DEP)

# Inference rule for updating the object files
.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c 

# Build rule for resource file
$(PROJ).res: $(PROJ).rc $(RC_DEP)
    $(rc) $(rcflags) $(rcvars) /fo $(PROJ).res $(PROJ).rc

# Build rule for EXE
$(PROJ).EXE: $(PROJ_OBJS) $(PROJ).res
    $(link) $(linkdebug) $(guilflags) \
    $(PROJ_OBJS) $(PROJ).res $(guilibs) $(EXTRA_LIBS) \
    -out:$(PROJ).exe $(MAPFILE)


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

