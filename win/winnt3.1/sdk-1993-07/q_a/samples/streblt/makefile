# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: streblt.exe

# Update the resource if necessary

streblt.rbj: streblt.rc streblt.h
    rc -r -fo streblt.res streblt.rc
    cvtres -$(CPU) streblt.res -o streblt.rbj

# Update the object file if necessary

streblt.obj: streblt.c streblt.h track.h
    $(cc) $(cdebug) $(cflags) $(cvars) streblt.c

track.obj: track.c track.h
    $(cc) $(cdebug) $(cflags) $(cvars) track.c

bitmap.obj: bitmap.c
    $(cc) $(cdebug) $(cflags) $(cvars) bitmap.c

# Update the executable file if necessary, and if so, add the resource back in.

streblt.exe: streblt.obj track.obj bitmap.obj streblt.rbj streblt.def
    $(link) $(linkdebug) $(guiflags) -out:streblt.exe streblt.obj track.obj bitmap.obj streblt.rbj $(guilibs)
