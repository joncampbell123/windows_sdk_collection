# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# Copyright (C) 1993-1995 Microsoft Corporation.  All Rights Reserved.
#
#
# Processor independant makefile

# Nmake macros for building Windows 32-bit applications

APPVER=4.0
!include <win32.mak>

PROJ=Stereo

all: $(PROJ).cpl

# Define project specific macros
PROJ_OBJS	= stereo.obj
EXTRA_LIBS	= 
GLOBAL_DEP	= stereo.h
RC_DEP		= amp.ico tape.ico tuner.ico

# Dependencies
stereo.obj	: stereo.c $(GLOBAL_DEP)

# Inference rule for updating the object files
.c.obj:
	$(cc) $(cdebug) $(cflags) $(cvarsdll) $*.c

# Build rule for the resource file
$(PROJ).res: $(PROJ).rc $(RC_DEP) $(GLOBAL_DEP)
	$(rc) $(rcflags) $(rcvars) /fo $(PROJ).res $(PROJ).rc

# Build rule for the CPL
$(PROJ).cpl: $(PROJ_OBJS) $(PROJ).res
	$(link) $(linkdebug) $(dlllflags) -out:$(PROJ).cpl -def:$(PROJ).def \
	$(guilibsdll) $(PROJ_OBJS) $(PROJ).res

# Rules for cleaning out the old files
clean:
	del *.obj *.lib *.exp *.res
