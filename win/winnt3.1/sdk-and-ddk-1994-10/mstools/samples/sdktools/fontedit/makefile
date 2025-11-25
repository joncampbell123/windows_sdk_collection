!include <ntwin32.mak>

all: fontedit.exe

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.c

fontedit.rbj: fontedit.rc fontedit.h
    $(rc) -r fontedit.rc
    $(cvtres) -$(CPU) fontedit.res -o fontedit.rbj

fontedit.exe: fontedit.rbj \
              FONTCVT.OBJ FONTDLG.OBJ FONTEDIT.OBJ \
              FONTHEAD.OBJ TYPECVT.OBJ FONTLOAD.OBJ FONTCHAR.OBJ
  $(link) $(linkdebug) $(guiflags) -out:$*.exe $** $(guilibs)
