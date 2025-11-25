# Some nmake macros for building Win32 applications

!include <ntwin32.mak>

all: mltithrd.exe

# Update the object file if necessary

mltithrd.obj: mltithrd.c mltithrd.h
    $(cc) $(cflags) $(cvarsmt) $(cdebug) mltithrd.c

# Update the resources if necessary

mltithrd.res: mltithrd.rc mltithrd.h
    rc -r mltithrd.rc

mltithrd.rbj: mltithrd.res
    cvtres -$(CPU) mltithrd.res -o mltithrd.rbj

# Update the executable file if necessary, and if so, add the resource back in.

mltithrd.exe: mltithrd.obj mltithrd.rbj
    $(link) $(linkdebug) $(guiflags) -out:mltithrd.exe mltithrd.obj mltithrd.rbj $(guilibsmt)
