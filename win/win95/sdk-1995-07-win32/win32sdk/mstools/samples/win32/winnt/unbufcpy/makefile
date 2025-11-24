# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT

!include <ntwin32.mak>


ALLEXES = unbufcp1.exe unbufcp2.exe

all: $(ALLEXES)

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

$(ALLEXES): $*.obj
  $(link) $(ldebug) $(conflags) -out:$*.exe $*.obj user32.lib $(conlibs) $(int64lib)

