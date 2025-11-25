# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: sharemem.exe

# Update the resource if necessary

sharemem.res: sharemem.rc sharemem.h
    rc -r -fo sharemem.tmp sharemem.rc
    cvtres -$(CPU) sharemem.tmp -o sharemem.res
    del sharemem.tmp

# Update the object file if necessary

sharemem.obj: sharemem.c sharemem.h
    $(cc) $(cdebug) $(cflags) $(cvars) sharemem.c

# Update the executable file if necessary, and if so, add the resource back in.

sharemem.exe: sharemem.obj sharemem.res
    $(link) $(linkdebug) $(guiflags) -out:sharemem.exe sharemem.obj sharemem.res $(guilibs)
