# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>


all: Platform.exe

# Update the resource if necessary
Platform.res: Platform.rc Platform.h
    rc -r -fo Platform.tmp Platform.rc
    cvtres -$(CPU) Platform.tmp -o Platform.res
    del Platform.tmp

# Update the object file if necessary

Platform.obj: Platform.c Platform.h
    $(cc) $(cdebug) $(cflags) $(cvars) Platform.c

# Update the executable file if necessary, and if so, add the resource back in.

Platform.exe: Platform.obj Platform.res Platform.def
    $(link) $(linkdebug) $(guiflags) -out:Platform.exe Platform.obj Platform.res $(guilibs)
