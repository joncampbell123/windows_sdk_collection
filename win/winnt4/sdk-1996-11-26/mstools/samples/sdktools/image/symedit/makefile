# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: ..\bin\symedit.exe

cflags = $(cflags) -DWIN32 -DTARGET32 -DHOST32 -DADDR_MIXED -DNT_BUILD -D_CROSS_PLATFORM_

.c.obj:
    $(cc) $(cflags) $(cvarsdll) -I..\include -I. $(scall) $(cdebug) $**

OBJS = symedit.obj

..\bin\symedit.exe: $(OBJS) ..\lib\symcvt.lib
    @if not exist ..\bin md ..\bin > nul
    $(link) @<<
$(conlflags)
$(linkdebug)
-out:$@
$(**: =
)
$(guilibsdll: =
)
imagehlp.lib
<<NOKEEP
