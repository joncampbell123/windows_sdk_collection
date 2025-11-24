!include <ntwin32.mak>

all: startp.exe

.c.exe:
  $(cc) $(cflags) $(cdebug) $(cvars) $*.c
  $(link) $(ldebug) $(conlflags) -out:$*.exe $*.obj $(conlibs) \
      user32.lib
