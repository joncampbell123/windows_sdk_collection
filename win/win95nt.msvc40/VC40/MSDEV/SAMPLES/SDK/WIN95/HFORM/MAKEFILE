# Nmake macros for building Windows 32-Bit apps

APPVER=4.0

!include <win32.mak>

all: hform.exe

# Update the resource if necessary

hform.res: hform.rc hfres.h
    $(rc) $(rcflags) $(rcvars) hform.rc

# Update the object file if necessary

hform.obj: hform.c hfres.h
    $(cc) $(cflags) $(cvars) $(cdebug) hform.c

# Update the executable file if necessary, and if so, add the resource back in.

hform.exe: hform.obj hform.res
    $(link) $(linkdebug) $(guilflags) -out:hform.exe hform.obj hform.res $(guilibs) penwin32.lib
