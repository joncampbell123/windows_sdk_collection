# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: output.exe

# Update the resource if necessary

output.res: output.rc output.h
    rc -r output.rc

output.rbj: output.res
    cvtres -$(CPU) output.res -o output.rbj

# Update the object file if necessary

output.obj: output.c output.h
    $(cc) $(cflags) $(cvars) $(cdebug) output.c

# Update the executable file if necessary, and if so, add the resource back in.

output.exe: output.obj output.rbj output.def
    $(link) $(linkdebug) $(guiflags) -out:output.exe output.obj output.rbj $(guilibs)
