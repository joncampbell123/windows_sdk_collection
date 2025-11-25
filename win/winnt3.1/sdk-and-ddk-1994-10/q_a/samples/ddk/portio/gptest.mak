# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $*.c

.obj.exe:
    $(link) $(linkdebug) $(conflags) -out:$*.exe $*.obj $(conlibs)

all: gpdread.exe gpdwrite.exe

gpdread.exe:  gpdread.obj

gpdwrite.exe: gpdwrite.obj
