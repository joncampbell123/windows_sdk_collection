
#include <ntwin32.mak>
!include <ntwin32.mak>

cf = -DUNICODE

.SUFFIXES: .cxx

.cxx.obj: ; $(cc) $(cflags) $(cvars) $(cdebug) $(cf) $*.cxx

all:	gridfont.exe

app.res:	app.rc app.h
    rc -r app.rc

app.rbj:	app.res
    cvtres -$(CPU) app.res -o app.rbj

app.obj:	app.cxx app.hxx app.h

view.obj:	view.cxx view.hxx grid.hxx app.h

font.obj:	font.cxx font.hxx app.h

grid.obj:	grid.cxx grid.hxx app.h

box.obj:	grid.hxx font.hxx app.h box.hxx

gridfont.exe:	app.rbj app.obj view.obj grid.obj font.obj  box.obj
	$(link) $(linkdebug) $(guiflags) -out:gridfont.exe \
	app.rbj app.obj view.obj grid.obj font.obj box.obj\
        $(guilibs)


