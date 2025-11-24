# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: input.exe

# Update the resource if necessary

input.res: input.rc input.h
    rc -r input.rc

input.rbj: input.res
    cvtres -$(CPU) input.res -o input.rbj

# Update the object file if necessary

input.obj: input.c input.h
    $(cc) $(cflags) $(cvars) $(cdebug) input.c

# Update the executable file if necessary, and if so, add the resource back in.

input.exe: input.obj input.rbj input.def
    $(link) $(linkdebug) $(guiflags) -out:input.exe input.obj input.rbj $(guilibs)
