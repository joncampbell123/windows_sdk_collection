# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: getsys.exe

# Update the resource if necessary

getsys.rbj: getsys.rc getsys.h
    rc -r -fo getsys.res getsys.rc
    cvtres -$(CPU) getsys.res -o getsys.rbj

# Update the object file if necessary

getsys.obj: getsys.c getsys.h
    $(cc) $(cdebug) $(cflags) $(cvars) getsys.c

# Update the executable file if necessary, and if so, add the resource back in.

getsys.exe: getsys.obj getsys.rbj getsys.def
    $(link) $(linkdebug) $(guiflags) -out:getsys.exe getsys.obj getsys.rbj $(guilibs)
