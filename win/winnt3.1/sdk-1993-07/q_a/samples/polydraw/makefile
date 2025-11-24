# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: polydraw.exe

# Update the resource if necessary

polydraw.rbj: polydraw.rc polydraw.h
    rc -r -fo polydraw.res polydraw.rc
    cvtres -$(CPU) polydraw.res -o polydraw.rbj

# Update the object file if necessary

polydraw.obj: polydraw.c polydraw.h
    $(cc) $(cdebug) $(cflags) $(cvars) polydraw.c

# Update the executable file if necessary, and if so, add the resource back in.

polydraw.exe: polydraw.obj polydraw.rbj polydraw.def
    $(link) $(linkdebug) $(guiflags) -out:polydraw.exe polydraw.obj polydraw.rbj $(guilibs)
