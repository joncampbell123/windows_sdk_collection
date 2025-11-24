!include <ntwin32.mak>

all: check_sd.exe

check_sd.obj: check_sd.c
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

check_sd.exe: check_sd.obj
  $(link) $(linkdebug) $(conflags) -out:$*.exe $*.obj $(conlibs) advapi32.lib user32.lib
