# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: mdi.exe

# Update the object files if necessary

mdi.obj: mdi.c
    $(cc) $(cdebug) $(cflags) $(cvars) mdi.c

# Update the resources if necessary

mdi.rbj: mdi.rc mdi.h
    rc -r -fo mdi.res mdi.rc
    cvtres -$(CPU) mdi.res -o mdi.rbj


# Update the executable file if necessary.
# If so, add the resource back in.

mdi.exe: mdi.obj mdi.rbj
    $(link) $(linkdebug) $(guiflags) \
    -out:mdi.exe mdi.obj mdi.rbj $(guilibs) shell32.lib
