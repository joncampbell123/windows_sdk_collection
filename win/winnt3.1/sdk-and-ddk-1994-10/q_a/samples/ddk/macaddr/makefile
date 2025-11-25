# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

INCLUDE=$(INCLUDE);$(BASEDIR)\src\network\inc

.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $*.c

.obj.exe:
    $(link) $(linkdebug) $(conflags) -out:$*.exe $*.obj $(conlibs) advapi32.lib

all: macaddr.exe

macaddr.exe:  macaddr.obj
