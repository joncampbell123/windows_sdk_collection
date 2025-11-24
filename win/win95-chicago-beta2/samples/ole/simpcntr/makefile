!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

all: simpcntr.exe

simpcntr.exe:  app.obj         \
            doc.obj         \
            ias.obj         \
            iocs.obj        \
            ioipf.obj        \
            ioips.obj        \
            simpcntr.obj     \
            simpcntr.res     \
            site.obj
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(olelibs) ole2ui.lib

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.cpp

simpcntr.res: simpcntr.rc simpcntr.h
    $(rc) -r -I..\ole2ui -I..\ole2ui\res\usa -I..\ole2ui\res\static simpcntr.rc

