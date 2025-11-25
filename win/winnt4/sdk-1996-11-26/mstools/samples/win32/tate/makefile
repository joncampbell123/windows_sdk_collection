# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

# This line allows NMAKE to work as well

all: tate.exe

# Update the resource if necessary

tate.res: tate.rc tate.h
    rc -r tate.rc

# Update the object file if necessary

tate.obj: tate.c tate.h
    $(cc) $(cflags) $(cvars) $(cdebug) tate.c

# Update the executable file if necessary, and if so, add the resource back in.

tate.exe: tate.obj tate.res tate.def
    $(link) $(linkdebug) $(guiflags) -out:tate.exe tate.obj tate.res $(guilibs)
