!include <ntwin32.mak>

OBJS=printer.obj paint.obj getcaps.obj enumprt.obj getpdriv.obj

all: printer.exe

printer.rbj: printer.rc printer.h
    rc -r -fo printer.res printer.rc
    cvtres -$(CPU) printer.res -o printer.rbj

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.c

printer.exe: $(OBJS) printer.rbj printer.def
    $(link) $(linkdebug) $(guiflags) -out:printer.exe $(OBJS) printer.rbj $(guilibs)
