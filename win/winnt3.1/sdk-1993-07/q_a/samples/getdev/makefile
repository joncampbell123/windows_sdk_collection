# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: getdev.exe

# Update the resource if necessary

getdev.rbj: getdev.rc getdev.h
    rc -r -fo getdev.res getdev.rc
    cvtres -$(CPU) getdev.res -o getdev.rbj

# Update the object file if necessary

getdev.obj: getdev.c getdev.h
    $(cc) $(cdebug) $(cflags) $(cvars) getdev.c

# Update the executable file if necessary, and if so, add the resource back in.

getdev.exe: getdev.obj getdev.rbj getdev.def
    $(link) $(linkdebug) $(guiflags) -out:getdev.exe getdev.obj getdev.rbj $(guilibs)
