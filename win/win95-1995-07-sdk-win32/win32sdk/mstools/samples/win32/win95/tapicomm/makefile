TARGETOS=WIN95
APPVER=4.0

!include <win32.mak>

all: tapicomm.exe tapicomm.hlp

tapicomm.res: tapicomm.rc
    rc -r -fo tapicomm.res tapicomm.rc

tapicomm.hlp: tapicomm.hpj tapicomm.rtf
    @if exist tapicomm.PH del tapicomm.PH
    $(hc) tapicomm.hpj

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

tapicomm.exe:   tapicomm.res \
                    tapicomm.obj \
                ABOUT.obj    \
                COMMCODE.obj \
                DISPATCH.obj \
                EDITCTLS.obj \
                INIT.obj     \
                MISC.obj     \
                STATBAR.obj  \
                TAPICODE.obj \
                TAPIINFO.obj \
                TOOLBAR.obj  \
                WINMAIN.obj
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(guilibs) advapi32.lib comctl32.lib tapi32.lib version.lib
