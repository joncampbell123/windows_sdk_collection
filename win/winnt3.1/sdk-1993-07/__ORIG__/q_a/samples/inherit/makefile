!include <ntwin32.mak>

all: inherit.exe child.exe

.c.exe:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c
  $(link) $(ldebug) $(conlflags) -out:$*.exe $*.obj $(conlibs) \
      user32.lib
