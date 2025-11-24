# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: paths.exe

# Update the resource if necessary

paths.rbj: paths.rc paths.h
    rc -r -fo paths.res paths.rc
    cvtres -$(CPU) paths.res -o paths.rbj

# Update the object file if necessary

paths.obj: paths.c paths.h
    $(cc) $(cdebug) $(cflags) $(cvars) paths.c

# Update the executable file if necessary, and if so, add the resource back in.

paths.exe: paths.obj paths.rbj paths.def
    $(link) $(linkdebug) $(guiflags) -out:paths.exe paths.obj paths.rbj $(guilibs)
