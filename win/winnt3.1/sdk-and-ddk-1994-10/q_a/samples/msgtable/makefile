!include <ntwin32.mak>

all: msgtest.exe messages.dll

msgtest.exe: $*.c messages.h
  $(cc) $(cflags) $(cdebug) $(cvars) $*.c
  $(link) $(ldebug) $(conlflags) -out:$*.exe $*.obj $(conlibs) \
      user32.lib

messages.dll: $*.obj $*.rbj
  $(link) $(ldebug) -subsystem:console -base:0x60000000 -dll \
      -out:$*.dll $*.obj $*.rbj

messages.obj: $*.c
  $(cc) $(cflags) $(cdebug) $(cvarsdll) $*.c

messages.h: messages.rc

messages.rc: messages.mc
  mc -c $*.mc

.rc.rbj:
    $(rc) $(rcvars) -r -fo $*.res $*.rc
    $(cvtres) -$(CPU) $*.res -o $*.rbj
