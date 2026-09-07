# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT

!include <ntwin32.mak>

all: sockcli.exe

sockcli.obj: sockcli.c sockcli.h ..\inc\commdef.h
    $(cc) /I..\inc $(cflags) $(cvars) sockcli.c

sockcli.exe: sockcli.obj
    $(link) $(conflags) -out:sockcli.exe sockcli.obj wsock32.lib $(conlibs)
