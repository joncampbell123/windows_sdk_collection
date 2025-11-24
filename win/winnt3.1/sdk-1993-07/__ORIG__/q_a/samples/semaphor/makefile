# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: sema.exe


# Update the object file if necessary

sema.obj: sema.c sema.h
    $(cc) $(cdebug) $(cflags) $(cvars) sema.c

# Update the executable file if necessary, and if so, add the resource back in.

sema.exe: sema.obj
    $(link) $(linkdebug) $(guiflags) -out:sema.exe sema.obj $(guilibs)
