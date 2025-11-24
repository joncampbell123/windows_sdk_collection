# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>


# This line allows NMAKE to work as well

all: BOB.exe

# Update the resource if necessary
BOB.rbj: BOB.rc BOB.h
    rc -r -fo BOB.res BOB.rc
    cvtres -$(CPU) BOB.res -o BOB.rbj

# Update the object file if necessary

BOB.obj: BOB.c BOB.h
    $(cc) $(cflags) $(cvars) $(cdebug) BOB.c

# Update the executable file if necessary, and if so, add the resource back in.

BOB.exe: BOB.obj BOB.rbj BOB.def
    $(link) $(linkdebug) $(guiflags) -out:BOB.exe BOB.obj BOB.rbj $(guilibs) advapi32.lib


clean:
        del *.rbj
        del *.res
        del *.obj
        del *.bak
