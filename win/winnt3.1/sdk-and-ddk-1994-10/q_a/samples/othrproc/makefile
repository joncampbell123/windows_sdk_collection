# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: othrproc.exe

# Update the resource if necessary

othrproc.res: othrproc.rc othrproc.h
    rc -r -fo othrproc.tmp othrproc.rc
    cvtres -$(CPU) othrproc.tmp -o othrproc.res
    del othrproc.tmp

# Update the object file if necessary

othrproc.obj: othrproc.c othrproc.h
    $(cc) $(cdebug) $(cflags) $(cvars) othrproc.c

# Update the executable file if necessary, and if so, add the resource back in.

othrproc.exe: othrproc.obj othrproc.res
    $(link) $(linkdebug) $(guiflags) -out:othrproc.exe othrproc.obj othrproc.res $(guilibs)
