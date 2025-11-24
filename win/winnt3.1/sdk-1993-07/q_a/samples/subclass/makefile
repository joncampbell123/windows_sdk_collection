# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: subclass.exe

# Update the resource if necessary

subclass.rbj: subclass.rc subclass.h
    rc -r -fo subclass.res subclass.rc
    cvtres -$(CPU) subclass.res -o subclass.rbj

# Update the object file if necessary

subclass.obj: subclass.c subclass.h
    $(cc) $(cdebug) $(cflags) $(cvars) subclass.c

# Update the executable file if necessary, and if so, add the resource back in.

subclass.exe: subclass.obj subclass.rbj subclass.def
    $(link) $(linkdebug) $(guiflags) -out:subclass.exe subclass.obj subclass.rbj $(guilibs)
