# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: server32.exe

# Update the resource if necessary

server32.rbj: server32.rc server32.h
    rc -r -fo server32.res server32.rc
    cvtres -$(CPU) server32.res -o server32.rbj

# Update the object file if necessary

server32.obj: server32.c server32.h
    $(cc) $(cdebug) $(cflags) $(cvars) server32.c

# Update the executable file if necessary, and if so, add the resource back in.

server32.exe: server32.obj server32.rbj
    $(link) $(linkdebug) $(guiflags) -out:server32.exe server32.obj server32.rbj $(guilibs) advapi32.lib
