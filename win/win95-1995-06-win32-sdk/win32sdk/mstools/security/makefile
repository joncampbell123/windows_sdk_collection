# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

INCLUDE=.;$(INCLUDE)

all: lsasamp.exe

lsasamp.obj: lsasamp.c
    $(cc) $(cflags) $(cvars) -Zi -Od lsasamp.c

lsasamp.exe: lsasamp.obj
    $(link) $(linkdebug) $(conflags) -out:lsasamp.exe lsasamp.obj $(conlibs)
