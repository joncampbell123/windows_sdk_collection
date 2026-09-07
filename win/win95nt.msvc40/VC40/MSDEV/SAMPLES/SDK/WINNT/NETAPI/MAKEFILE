# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT
APPVER=3.51

!include <ntwin32.mak>

all: netsmple.exe

netsmple.obj: netsmple.c
    $(cc) $(cflags) $(cvars) -Zi -Od netsmple.c

netsmple.exe: netsmple.obj
    $(link) $(linkdebug) $(conflags) -out:netsmple.exe netsmple.obj $(conlibs) netapi32.lib
