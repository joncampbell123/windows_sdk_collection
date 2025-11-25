# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: monkey.exe

# Update the resource if necessary

monkey.rbj: monkey.rc monkey.h
    rc -r -fo monkey.res monkey.rc
    cvtres -$(CPU) monkey.res -o monkey.rbj

# Update the object file if necessary

monkey.obj: monkey.c monkey.h
    $(cc) $(cdebug) $(cflags) $(cvars) monkey.c

# Update the executable file if necessary, and if so, add the resource back in.

monkey.exe: monkey.obj monkey.rbj
    $(link) $(linkdebug) $(guiflags) -out:monkey.exe monkey.obj monkey.rbj $(guilibs) advapi32.lib
