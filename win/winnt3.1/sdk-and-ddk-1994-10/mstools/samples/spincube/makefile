!include <ntwin32.mak>

all: spintest.exe spincube.dll

spintest.obj: spintest.c
    $(cc) $(cdebug) $(cflags) $(cvarsdll) spintest.c

spincube.obj: spincube.c spincube.h
    $(cc) $(cdebug) $(cflags) $(cvarsdll) spincube.c

paint.obj: paint.c paint.h spincube.h
    $(cc) $(cdebug) $(cflags) $(cvarsdll) paint.c

spintest.rbj: spintest.rc spintest.h spintest.dlg
    $(rc) $(rcvars) -r -fo spintest.res spintest.rc
    cvtres -$(CPU) spintest.res -o spintest.rbj

spincube.rbj: spincube.rc
    $(rc) $(rcvars) -r -fo spincube.res spincube.rc
    cvtres -$(CPU) spincube.res -o spincube.rbj

spincube.lib: spincube.obj paint.obj spincube.def spincube.rbj
     $(implib) -machine:$(CPU) \
     -def:spincube.def   \
!IF "$(CPU)" == "i386"
     spincube.obj paint.obj \
!ENDIF
     -out:spincube.lib

spincube.dll: spincube.obj paint.obj spincube.def spincube.rbj
    $(link) $(linkdebug)              \
    -base:0x1C000000     \
    -dll                 \
    -entry:_DllMainCRTStartup$(DLLENTRY) \
    -out:spincube.dll    \
    spincube.rbj spincube.exp spincube.obj paint.obj $(guilibsdll)

spintest.exe: spintest.obj spincube.lib spintest.rbj spintest.def
    $(link) $(linkdebug) $(guiflags) -out:spintest.exe spintest.obj spintest.rbj spincube.lib $(guilibsdll)
