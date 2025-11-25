!include <ntwin32.mak>

all: messages.dll log.exe

log.obj: $*.c messages.h
  $(cc) $(cflags) $(cdebug) $(cvarsdll) $*.c

log.exe: $*.obj
  $(link) $(ldebug) $(conlflags) -out:$*.exe $*.obj $(conlibsdll) \
    user32.lib $(optlibs)

messages.dll: $*.res
  $(link) $(lflags) -base:0x60000000 -machine:$(CPU) -dll -noentry \
    -out:$*.dll $*.res
    
messages.h: $*.rc

#WARNING: using the -c option can cause problems with replaceable parameter
#strings. Do not compile message files that contain replaceable parameter
#strings with -c.

messages.rc: $*.mc
  mc -w $*.mc

.rc.res:
    rc -r -fo $*.res $*.rc
