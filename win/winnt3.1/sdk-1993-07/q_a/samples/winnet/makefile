!include <ntwin32.mak>

all: winnet.exe

winnet.obj: winnet.c
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

winnet.exe: winnet.obj
  $(link) $(linkdebug) $(conflags) -out:$*.exe $*.obj $(conlibs) mpr.lib