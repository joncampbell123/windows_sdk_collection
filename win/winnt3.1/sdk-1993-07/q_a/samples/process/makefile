# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: process.exe

# Update the resource if necessary

process.rbj: process.rc process.h
    rc -r -fo process.res process.rc
    cvtres -$(CPU) process.res -o process.rbj

# Update the object file if necessary

process.obj: process.c process.h
    $(cc) $(cdebug) $(cflags) $(cvars) process.c

# Update the executable file if necessary, and if so, add the resource back in.

process.exe: process.obj process.rbj process.def
    $(link) $(linkdebug) $(guiflags) -out:process.exe process.obj process.rbj $(guilibs)
