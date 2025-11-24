# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: threads.exe

# Update the resource if necessary

threads.res: threads.rc threads.h
    rc -r -fo threads.tmp threads.rc
    cvtres -$(CPU) threads.tmp -o threads.res
    del threads.tmp

# Update the object file if necessary

threads.obj: threads.c threads.h
    $(cc) $(cdebug) $(cflags) $(cvars) threads.c

# Update the executable file if necessary, and if so, add the resource back in.

threads.exe: threads.obj threads.res
    $(link) $(linkdebug) $(guiflags) -out:threads.exe threads.obj threads.res $(guilibs)
