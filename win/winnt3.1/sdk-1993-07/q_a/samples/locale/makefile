# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: locale.exe

# Update the resource if necessary

locale.rbj: locale.rc locale.h
    rc -r -fo locale.res locale.rc
    cvtres -$(CPU) locale.res -o locale.rbj

# Update the object file if necessary

locale.obj: locale.c locale.h
    $(cc) $(cdebug) $(cflags) $(cvars) locale.c

# Update the executable file if necessary, and if so, add the resource back in.

locale.exe: locale.obj locale.rbj locale.def
    $(link) $(linkdebug) $(guiflags) -out:locale.exe locale.obj locale.rbj $(guilibs)
