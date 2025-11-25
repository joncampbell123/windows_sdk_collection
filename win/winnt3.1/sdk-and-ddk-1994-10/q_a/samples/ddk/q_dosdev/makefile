!include <ntwin32.mak>

all: q_dosdev.exe

q_dosdev.obj: q_dosdev.c
    $(cc) $(cflags) $(cvars) $(cdebug) q_dosdev.c

q_dosdev.exe: q_dosdev.obj
    $(link) $(linkdebug) $(conflags) -out:q_dosdev.exe q_dosdev.obj $(conlibs)
