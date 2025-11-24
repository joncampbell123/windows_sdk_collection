# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: client32.exe

# Update the resource if necessary

client32.rbj: client32.rc client32.h
    rc -r -fo client32.res client32.rc
    cvtres -$(CPU) client32.res -o client32.rbj

# Update the object file if necessary

client32.obj: client32.c client32.h
    $(cc) $(cdebug) $(cflags) $(cvars) client32.c

# Update the executable file if necessary, and if so, add the resource back in.

client32.exe: client32.obj client32.rbj
    $(link) $(linkdebug) $(guiflags) -out:client32.exe client32.obj client32.rbj $(guilibs)
