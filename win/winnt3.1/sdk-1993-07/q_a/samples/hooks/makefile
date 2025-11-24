# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: hooktest.exe

# Update the resource if necessary

hooktest.res: hooktest.rc hooktest.h
    $(rc) -r -fo hooktest.tmp hooktest.rc
    $(cvtres) -$(CPU) hooktest.tmp -o hooktest.res
    del hooktest.tmp

# Update the object file if necessary

hooktest.obj: hooktest.c hooktest.h
    $(cc) $(cdebug) $(cflags) $(cvars) hooktest.c

# Update the executable file if necessary, and if so, add the resource back in.

hooktest.exe: hooktest.obj hooktest.res
    $(link) $(linkdebug) $(guiflags) -out:hooktest.exe hooktest.obj hooktest.res $(guilibs)
