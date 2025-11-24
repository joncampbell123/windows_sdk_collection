!include <ntwin32.mak>

# Turn off MIPS Debugging Support

!if "$(CPU)" == "MIPS"
cdebug = 
cvtdebug = 
linkdebug =
!endif

all: console.exe

console.exe: console.obj alocfree.obj create.obj fillatt.obj fillchar.obj \
    flush.obj cursor.obj conmode.obj coninfo.obj contitle.obj \
    getnumev.obj getlrgst.obj numbut.obj readout.obj readchar.obj \
    scroll.obj size.obj handler.obj writein.obj console.rbj
  $(link) $(linkdebug) $(conflags) -out:$*.exe $** $(conlibs) \
    user32.lib

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

console.rbj: console.rc console.ico
    rc -r -fo $*.res $*.rc
    cvtres -$(CPU) $*.res -o $*.rbj
