TARGETOS=WINNT

!include <ntwin32.mak>

all: rnrmsg.h rnrsvc.exe rnrsvc1.exe rnrsvc2.exe rnrsvc3.exe rnrsetup.exe rnrsrv.exe rnrclnt.exe

rnrsvc.res: rnrsvc.rc rnrmsg.rc
    $(rc) -dWIN32 -r -fo rnrsvc.res rnrsvc.rc

# Update the resources if necessary

rnrsvc.exe: rnrutil.obj  \
            client.obj	\
            connect.obj	\
            globals.obj	\
            log.obj	\
            main.obj	\
            rnrsvc.res
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrsvc1.exe: rnrutil.obj  \
            client1.obj	\
            connect.obj	\
            globals.obj	\
            log.obj	\
            main.obj	\
            rnrsvc.res
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrsvc2.exe: rnrutil.obj  \
            client2.obj	\
            connect.obj	\
            globals.obj	\
            log.obj	\
            main.obj	\
            rnrsvc.res
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrsvc3.exe: rnrutil.obj  \
            client2.obj	\
            connect.obj	\
            globals.obj	\
            log.obj	\
            main.obj	\
            rnrsvc.res
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrsetup.exe: rnrsetup.obj
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrclnt.exe: rnrclnt.obj
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrsrv.exe: rnrsrv.obj
  $(link) $(linkdebug) $(conlflags) -out:$*.exe $** $(conlibs) wsock32.lib

rnrmsg.h rnrmsg.rc msg00001.bin: rnrmsg.mc
    mc -v rnrmsg.mc

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $(scall) $*.c

clean:
    del *.obj
    del *.bin
    del *.exe
    del *.res
    del rnrmsg.rc
    del rnrmsg.h
