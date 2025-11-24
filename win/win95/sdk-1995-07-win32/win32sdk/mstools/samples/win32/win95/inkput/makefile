# Nmake macros for building Windows 32-Bit apps

APPVER=4.0

!include <win32.mak>

all: ip.exe

# Update the resource if necessary

ip.res: ip.rc ip.h
    $(rc) $(rcflags) $(rcvars) ip.rc

# Update the object file if necessary

ip.obj: ip.c ip.h
    $(cc) $(cflags) $(cvars) $(cdebug) ip.c

# Update the executable file if necessary, and if so, add the resource back in.

ip.exe: ip.obj ip.res
    $(link) $(linkdebug) $(guilflags) -out:ip.exe ip.obj ip.res $(guilibs) penwin32.lib
