!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

all: simpsvr.exe

simpsvr.exe:  app.obj         \
            doc.obj         \
            icf.obj         \
            ido.obj        \
            iec.obj        \
            ioipao.obj        \
            ioipo.obj        \
            ioo.obj        \
            ips.obj        \
            obj.obj         \
            simpsvr.obj     \
            simpsvr.res
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(olelibs) ole2ui.lib

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.cpp

simpsvr.res: simpsvr.rc simpsvr.h
    $(rc) -r -I..\ole2ui -I..\ole2ui\res\usa -I..\ole2ui\res\static simpsvr.rc

