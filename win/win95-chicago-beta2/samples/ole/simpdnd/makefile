!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

all: simpdnd.exe

simpdnd.exe:  app.obj         \
            doc.obj         \
            dxferobj.obj    \
            ias.obj         \
            ids.obj         \
            idt.obj         \
            iocs.obj        \
            simpdnd.obj     \
            simpdnd.res     \
            site.obj
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(olelibs) ole2ui.lib

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.cpp

simpdnd.res: simpdnd.rc simpdnd.h
    $(rc) -r -I..\ole2ui -I..\ole2ui\res\usa -I..\ole2ui\res\static simpdnd.rc

