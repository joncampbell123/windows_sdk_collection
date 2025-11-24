#************************************************************************
#
#                      Microsoft Developer Support
#             Copyright (c) 1992, 1993 Microsoft Corporation
#
#************************************************************************
#
# MAKEFILE  : Color
#
# COMMENTS  :
#
#************************************************************************

!include <ntwin32.mak>

all: Color.exe

Color.obj: Color.c Color.h
    $(cc) $(cdebug) $(cflags) $(cvars) Color.c

Color.exe: Color.obj
    $(link) $(ldebug) $(conlflags) -out:Color.exe Color.obj $(conlibs)
