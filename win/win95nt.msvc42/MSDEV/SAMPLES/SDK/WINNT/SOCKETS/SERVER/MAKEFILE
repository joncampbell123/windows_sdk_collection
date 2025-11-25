# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT

!include <ntwin32.mak>

all: socksrv.exe

socksrv.obj: socksrv.c socksrv.h ..\inc\commdef.h
    $(cc) /I..\inc $(cflags) $(cvars) socksrv.c

socksrv.exe: socksrv.obj
    $(link) $(conflags) -out:socksrv.exe socksrv.obj wsock32.lib $(conlibs)
