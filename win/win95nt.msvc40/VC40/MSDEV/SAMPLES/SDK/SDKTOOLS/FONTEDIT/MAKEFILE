!include <ntwin32.mak>

all: fontedit.exe

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.c

fontedit.res: fontedit.rc fontedit.h
    $(rc) -r fontedit.rc

fontedit.exe: fontedit.res \
              FONTCVT.obj FONTDLG.OBJ FONTEDIT.OBJ \
              FONTHEAD.OBJ TYPECVT.OBJ FONTLOAD.OBJ FONTCHAR.OBJ
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(guilibs)
