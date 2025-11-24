# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: timers.exe

# Update the object files if necessary

timers.obj: timers.c
    $(cc) $(cdebug) $(cflags) $(cvars) timers.c

# Update the executable file if necessary.
# If so, add the resource back in.

timers.exe: timers.obj timers.def
    $(link) $(linkdebug) $(guiflags) -out:timers.exe timers.obj $(guilibs)
