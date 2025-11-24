# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: regions.exe

# Update the object files if necessary

regions.obj: regions.c
    $(cc) $(cdebug) $(cflags) $(cvars) regions.c

# Update the resources if necessary

regions.rbj: regions.rc regions.h
    rc -r -fo regions.res regions.rc
    cvtres -$(CPU) regions.res -o regions.rbj

# Update the executable file if necessary.
# If so, add the resource back in.

regions.exe: regions.obj regions.rbj regions.def
    $(link) $(linkdebug) $(guiflags) -out:regions.exe regions.obj regions.rbj $(guilibs)
