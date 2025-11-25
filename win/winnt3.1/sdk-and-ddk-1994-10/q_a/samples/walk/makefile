# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: walk.exe

# Update the resource if necessary


# Update the object file if necessary

walk.obj: walk.c
    $(cc) $(cdebug) $(cflags) $(cvars) walk.c

# Update the executable file if necessary, and if so, add the resource back in.

walk.exe: walk.obj
    $(link) $(linkdebug) $(conflags) -out:walk.exe walk.obj $(conlibs)
