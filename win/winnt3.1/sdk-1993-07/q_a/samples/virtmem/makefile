# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: virtmem.exe

# Update the resource if necessary

virtmem.res: virtmem.rc virtmem.h
    rc -r -fo virtmem.tmp virtmem.rc
    cvtres -$(CPU) virtmem.tmp -o virtmem.res
    del virtmem.tmp

# Update the object file if necessary

virtmem.obj: virtmem.c virtmem.h
    $(cc) $(cdebug) $(cflags) $(cvars) virtmem.c

# Update the executable file if necessary, and if so, add the resource back in.

virtmem.exe: virtmem.obj virtmem.res
    $(link) $(linkdebug) $(guiflags) -out:virtmem.exe virtmem.obj virtmem.res $(guilibs)
