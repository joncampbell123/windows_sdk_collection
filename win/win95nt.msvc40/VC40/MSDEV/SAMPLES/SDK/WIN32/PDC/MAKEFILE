# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: pdc.exe

pdc.obj: pdc.c pdc.h
    $(cc) $(cflags) $(cvars) $(cdebug) pdc.c

pdc.exe: pdc.obj
    $(link) $(linkdebug) $(conflags) -out:pdc.exe pdc.obj $(conlibs)
