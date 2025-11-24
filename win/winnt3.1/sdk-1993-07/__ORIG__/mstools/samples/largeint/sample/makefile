# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: bigmult.exe

bigmult.obj: bigmult.c
    $(cc) $(cflags) $(cvars) bigmult.c

bigmult.exe: bigmult.obj
    $(link) $(conflags) -out:bigmult.exe bigmult.obj $(conlibs) ..\largeint.lib

