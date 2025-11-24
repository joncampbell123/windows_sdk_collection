!include <ntwin32.mak>

all: log.exe messages.dll

log.exe: $*.c messages.h
  $(cc) $(cflags) $(cdebug) $(cvars) $*.c
  $(link) $(ldebug) $(conlflags) -out:$*.exe $*.obj $(conlibs) \
      advapi32.lib user32.lib

messages.dll: $*.obj $*.rbj
  $(link) $(ldebug) -subsystem:console -base:0x1C000000 -dll \
      -out:$*.dll $*.obj $*.rbj

messages.obj: $*.c
  $(cc) $(cflags) $(cdebug) $(cvarsdll) $*.c

messages.h: $*.rc

messages.rc: $*.mc
  mc $*.mc

.rc.rbj:
    $(rc) $(rcvars) -r -fo $*.res $*.rc
    $(cvtres) -$(CPU) $*.res -o $*.rbj
