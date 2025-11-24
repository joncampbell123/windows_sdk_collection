# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: plgblt.exe

# Update the resource if necessary

plgblt.rbj: plgblt.rc plgblt.h
    rc -r -fo plgblt.res plgblt.rc
    cvtres -$(CPU) plgblt.res -o plgblt.rbj

# Update the object file if necessary

plgblt.obj: plgblt.c plgblt.h track.h
    $(cc) $(cdebug) $(cflags) $(cvars) plgblt.c

track.obj: track.c track.h
    $(cc) $(cdebug) $(cflags) $(cvars) track.c

bitmap.obj: bitmap.c
    $(cc) $(cdebug) $(cflags) $(cvars) bitmap.c

# Update the executable file if necessary, and if so, add the resource back in.

plgblt.exe: plgblt.obj track.obj bitmap.obj plgblt.rbj plgblt.def
    $(link) $(linkdebug) $(guiflags) -out:plgblt.exe plgblt.obj track.obj bitmap.obj plgblt.rbj $(guilibs)
