# -----------------------------------------------------------------
# File name:  MAKEFILE
#
# This file contains the build information necessary to create both
# the application (APPSKEL) and the DLL it calls (DLL16).
#
# This is an NMAKE FILE.
#
# Versions:  Microsoft C 6.00a, Microsoft C/C++ 7.0
#
# Development Team:  Dan Ruder
#
# Written by Microsoft Product Support Services, Windows Developer Support.
#
# Copyright (c) 1991, 1992 Microsoft Corporation.  All rights reserved.
# -----------------------------------------------------------------

all: DLL16.dll 

!message
!message +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!message + To make the file dll32.dll, you will need to have the       +
!message + Microsoft Thunk compiler and the Microsoft Macro Assembler  +
!message + (ML) on the path.                                           +
!message +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!message

# Defines

#
# Build flags
#
DLL_CFLAGS = -c -W3 -AMw -G2sw -Zi -Od /nologo
DLLMFLAGS   = /c /DIS_16 /W3 /Zi /nologo 
DLL_LINKFLAGS = /NOD/NOE/MAP/COD
DLL_OBJS = DLL16.obj 16thk.obj

#
# Rules for building DLL16.DLL, the DLL.
# Get the thunking asm file from the directory up one level.
# This assumes that we've maintained the ..\thunk\dll16 directory
# structure.
#
..\32to16.asm: ..\32to16.thk
    thunk -t thk -o ..\32to16.asm ..\32to16.thk

16thk.obj: ..\32to16.asm
    ml $(DLLMFLAGS) /Fo 16thk.obj ..\32to16.asm

DLL16.obj: DLL16.c DLL16.h
    cl $(DLL_CFLAGS) DLL16.c

DLL16.dll: $(DLL_OBJS) DLL16.def
    link @<<
    $(DLL_LINKFLAGS) $(DLL_OBJS) 
    DLL16.dll
    DLL16.map /map/li
    libw mdllcew
    DLL16.def
<<
    rc -40 DLL16.dll
    mapsym DLL16
    implib DLL16.lib DLL16.def
    copy DLL16.DLL ..
    del DLL16.DLL

#
# Rules of convenience to remove the extra files in the directory
#
clean:
    del *.bak
    del *.obj
    del *.res
    del *.map
    del *.sym
    del *.lib
    del ..\32to16.asm
cleaner:
    del ..\dll16.dll
    del *.exe
    nmake clean

