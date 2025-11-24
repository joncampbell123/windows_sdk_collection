# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: setinfo.exe

# Update the resource if necessary

setinfo.res: setinfo.rc setinfo.h
    rc -r -fo setinfo.tmp setinfo.rc
    cvtres -$(CPU) setinfo.tmp -o setinfo.res
    del setinfo.tmp

# Update the object file if necessary

setinfo.obj: setinfo.c setinfo.h
    $(cc) $(cdebug) $(cflags) $(cvars) setinfo.c

# Update the executable file if necessary, and if so, add the resource back in.

setinfo.exe: setinfo.obj setinfo.res
    $(link) $(linkdebug) $(guiflags) -out:setinfo.exe setinfo.obj setinfo.res $(guilibs)
