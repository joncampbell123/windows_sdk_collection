!include <ntwin32.mak>

all: monotest.exe

monotest.obj: monotest.c
    $(cc) $(cflags) $(cvars) $(cdebug) monotest.c

monotest.exe: monotest.obj
    $(link) $(linkdebug) $(conflags) -out:monotest.exe monotest.obj $(conlibs)
