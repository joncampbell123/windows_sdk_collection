# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: angle.exe

# Update the resource if necessary

angle.rbj: angle.rc angle.h
    rc -r -fo angle.res angle.rc
    cvtres -$(CPU) angle.res -o angle.rbj

# Update the object file if necessary

angle.obj: angle.c angle.h
    $(cc) $(cdebug) $(cflags) $(cvars) angle.c

# Update the executable file if necessary, and if so, add the resource back in.

angle.exe: angle.obj angle.rbj angle.def
    $(link) $(linkdebug) $(guiflags) -out:angle.exe angle.obj angle.rbj $(guilibs)
