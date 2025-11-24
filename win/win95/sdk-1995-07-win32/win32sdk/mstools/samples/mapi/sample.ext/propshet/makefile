# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#
# Processor independent makefile

# Nmake macros for building Windows 32-Bit apps
APPVER=4.0
!include <win32.mak>

DLLNAME = EPRSHT

all: $(DLLNAME)32.dll

# Define project specific macros
DLL_OBJS   = extprsht.obj prshtdlg.obj extprsht.res
DLL_DEP    = extprsht.h resource.h
EXTRA_LIBS = mapi32.lib uuid.lib
RC_DEP     = resource.h extbtn.bmp


# Dependencies
extprsht.obj:     extprsht.cpp      $(DLL_DEP)
prshtdlg.obj:     prshtdlg.cpp      $(DLL_DEP)


# Inference rule for updating the object files
.cpp.obj:
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $*.cpp



# Build rule for the DLL
$(DLLNAME)32.dll: $(DLL_OBJS)
    $(link)                     \
    $(linkdebug) $(dlllflags)   \
	-out:$(DLLNAME)32.dll 	    \
	-def:$(DLLNAME)32.DEF       \
    $(DLL_OBJS) $(guilibsdll) $(EXTRA_LIBS)



# Build rule for resource file
$(DLLNAME).res: $(DLLNAME).rc $(RC_DEP)
    $(rc) $(rcflags) $(rcvars) /fo $(DLLNAME).res $(DLLNAME).rc



# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc
