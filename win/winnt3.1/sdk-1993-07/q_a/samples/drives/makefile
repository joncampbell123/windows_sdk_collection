# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: drives.exe

drives.obj: drives.c drives.h
    $(cc) $(cdebug) $(cflags) $(cvars) drives.c

drives.exe: drives.obj
    $(link) $(linkdebug) $(conflags) -out:drives.exe drives.obj $(conlibs)
