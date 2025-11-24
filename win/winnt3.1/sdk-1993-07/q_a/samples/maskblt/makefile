# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: maskblt.exe

# Update the resource if necessary

maskblt.rbj: maskblt.rc maskblt.h
    rc -r -fo maskblt.res maskblt.rc
    cvtres -$(CPU) maskblt.res -o maskblt.rbj

# Update the object file if necessary

maskblt.obj: maskblt.c maskblt.h track.h bitmap.h
    $(cc) $(cdebug) $(cflags) $(cvars) maskblt.c

track.obj: track.c track.h
    $(cc) $(cdebug) $(cflags) $(cvars) track.c

bitmap.obj: bitmap.c bitmap.h
    $(cc) $(cdebug) $(cflags) $(cvars) bitmap.c

# Update the executable file if necessary, and if so, add the resource back in.

maskblt.exe: maskblt.obj track.obj bitmap.obj maskblt.rbj maskblt.def
    $(link) $(linkdebug) $(guiflags) -out:maskblt.exe maskblt.obj track.obj bitmap.obj maskblt.rbj $(guilibs)
