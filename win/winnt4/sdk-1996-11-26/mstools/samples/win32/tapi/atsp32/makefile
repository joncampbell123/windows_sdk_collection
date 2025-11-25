!include <win32.mak>

all: atsp32.tsp

atsp.obj: atsp.c atsp.h
    $(cc) $(cdebug) $(cflags) $(cvarsdll) atsp.c

atsp.res: atsp.rc
    $(rc) $(rcvars) -r -fo atsp.res atsp.rc

atsp32.lib: atsp.obj atsp32.def atsp.res
     $(implib) -machine:$(CPU) \
     -def:atsp32.def   \
!IF "$(CPU)" == "i386"
     atsp.obj \
!ENDIF
     -out:atsp32.lib

atsp32.tsp: atsp.obj atsp32.def atsp.res atsp32.lib
    $(link) $(linkdebug) $(dlllflags) \
    -base:0x1C000000     \
    atsp.obj atsp32.exp atsp.res $(guilibsdll)
    copy atsp32.dll atsp32.tsp
