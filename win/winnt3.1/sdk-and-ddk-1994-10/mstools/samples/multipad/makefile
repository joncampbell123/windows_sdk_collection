# Some nmake macros for building Win32 applications

!include <ntwin32.mak>

all: multipad.exe

OBJS = multipad.obj mpinit.obj mpfile.obj mpfind.obj mpprint.obj mpopen.obj

multipad.obj: multipad.c multipad.h
    $(cc) $(cflags) $(cvars) $*.c

mpinit.obj: mpinit.c multipad.h
    $(cc) $(cflags) $(cvars) $*.c

mpfind.obj: mpfind.c multipad.h
    $(cc) $(cflags) $(cvars) $*.c

mpfile.obj: mpfile.c multipad.h
    $(cc) $(cflags) $(cvars) $*.c

mpprint.obj: mpprint.c multipad.h
    $(cc) $(cflags) $(cvars) $*.c

mpopen.obj: mpopen.c multipad.h
    $(cc) $(cflags) $(cvars) $*.c

multipad.res: multipad.rc multipad.h
    $(rc) $(rcvars) -r multipad.rc

multipad.rbj: multipad.res
    cvtres -$(CPU) multipad.res -o multipad.rbj

multipad.exe: $(OBJS) multipad.def multipad.rbj
    $(link) $(guiflags) -out:multipad.exe multipad.obj mpinit.obj \
    mpfile.obj mpfind.obj mpprint.obj mpopen.obj multipad.rbj $(guilibs)
