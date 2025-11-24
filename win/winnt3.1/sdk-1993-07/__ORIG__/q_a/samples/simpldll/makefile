!include <ntwin32.mak>


all: linktest.exe loadtest.exe the_dll.dll


linktest.obj: linktest.c
    $(cc) $(cdebug) $(cflags) $(cvarsdll) linktest.c

loadtest.obj: loadtest.c
    $(cc) $(cdebug) $(cflags) $(cvarsdll) loadtest.c

the_dll.obj: the_dll.c
    $(cc) $(cdebug) $(cflags) $(cvarsdll) the_dll.c


linktest.rbj: linktest.rc linktest.h
    rc -r -fo linktest.res linktest.rc
    cvtres -$(CPU) linktest.res -o linktest.rbj

loadtest.rbj: loadtest.rc loadtest.h
    rc -r -fo loadtest.res loadtest.rc
    cvtres -$(CPU) loadtest.res -o loadtest.rbj

the_dll.rbj: the_dll.rc
    rc -r -fo the_dll.res the_dll.rc
    cvtres -$(CPU) the_dll.res -o the_dll.rbj


the_dll.lib: the_dll.obj the_dll.def
    $(implib) -machine:$(CPU)  \
    -def:the_dll.def     \
    the_dll.obj           \
    -out:the_dll.lib     \

the_dll.dll: the_dll.obj the_dll.def the_dll.rbj
    $(link) $(linkdebug)              \
    -base:0x1C000000     \
    -dll                 \
    -entry:_DllMainCRTStartup$(DLLENTRY) \
    -out:the_dll.dll     \
    the_dll.rbj the_dll.exp the_dll.obj $(guilibsdll)


linktest.exe: linktest.obj the_dll.lib linktest.rbj linktest.def
    $(link) $(linkdebug) $(guiflags) -out:linktest.exe linktest.obj the_dll.lib linktest.rbj $(guilibsdll)

loadtest.exe: loadtest.obj loadtest.rbj loadtest.def
    $(link) $(linkdebug) $(guiflags) -out:loadtest.exe loadtest.obj loadtest.rbj $(guilibsdll)
