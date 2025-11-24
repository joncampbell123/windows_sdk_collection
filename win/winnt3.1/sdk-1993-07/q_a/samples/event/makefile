# Some nmake macros for NT Win32 building

!include <ntwin32.mak>

all: asyn_io.exe

asyn_io.obj: asyn_io.c
    $(cc) $(cdebug) $(dbflags) $(cflags) $(cvars) asyn_io.c


asyn_io.exe: asyn_io.obj
    $(link) $(linkdebug) $(conflags) -out:asyn_io.exe asyn_io.obj $(conlibs)
