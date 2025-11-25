#  ************************************************************************
#
#                       Microsoft Developer Support
#                Copyright (c) 1993 Microsoft Corporation
#
#  ************************************************************************
#  MAKEFILE  : MinRec
#  PURPOSE   : A Small Win32 Recorder-like Sample Application
#  COMMENTS  :
#
#  ************************************************************************

# Nmake macros for building Win32 applications
!include <ntwin32.mak>

# target list
all: MinRec.Exe RecHook.Dll

# Update the resource if necessary
MinRec.Res: MinRec.Rc MinRec.Dlg MinRec.Ico MinRec.H
    $(rc) $(rcvars) -r -fo MinRec.Res MinRec.Rc
    $(cvtres) -$(CPU) MinRec.Res -o MinRec.Rbj

# Update the object file(s) if necessary
MinRec.Obj: MinRec.C MinRec.H
    $(cc) $(cdebug) $(cflags) $(cvars) MinRec.C

RecHook.Obj: RecHook.C RecHook.H MinRec.H
    $(cc) $(cdebug) $(cflags) $(cvars) RecHook.C

# Update the import library
RecHook.Lib: RecHook.Obj RecHook.Def
    $(implib)\
    -machine:$(CPU)\
    -def:RecHook.Def\
    -out:RecHook.Lib

# Update the dynamic link library
RecHook.Dll: RecHook.Obj RecHook.Lib
    $(link) $(ldebug) $(guilflags)\
    -base:0x1C000000\
    -dll\
    -entry:_DllMainCRTStartup$(DLLENTRY)\
    -out:RecHook.Dll\
    RecHook.Obj RecHook.Exp\
    $(guilibs)

# Update the Executable file if necessary.
MinRec.Exe: MinRec.Obj MinRec.Res MinRec.Def RecHook.Lib
    $(link) $(ldebug) $(guilflags)\
      -out:MinRec.Exe\
      MinRec.Obj RecHook.Lib MinRec.Rbj\
      $(guilibs)
