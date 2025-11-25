# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: termproc.exe

termproc.obj: termproc.c
    $(cc) $(cdebug) $(cflags) $(cvars) termproc.c

termproc.exe: termproc.obj
    $(link) $(linkdebug) $(conflags) -out:termproc.exe termproc.obj $(conlibs)
