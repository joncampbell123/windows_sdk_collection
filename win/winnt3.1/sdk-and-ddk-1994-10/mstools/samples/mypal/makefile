# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: mypal.exe

# Update the resource if necessary

mypal.res: mypal.rc mypal.h
    rc -r mypal.rc

mypal.rbj: mypal.res
    cvtres -$(CPU) mypal.res -o mypal.rbj

# Update the object file if necessary

mypal.obj: mypal.c mypal.h
    $(cc) $(cflags) $(cvars) $(cdebug) mypal.c

# Update the executable file if necessary, and if so, add the resource back in.

mypal.exe: mypal.obj mypal.rbj mypal.def
    $(link) $(linkdebug) $(guiflags) -out:mypal.exe mypal.obj mypal.rbj $(guilibs)
