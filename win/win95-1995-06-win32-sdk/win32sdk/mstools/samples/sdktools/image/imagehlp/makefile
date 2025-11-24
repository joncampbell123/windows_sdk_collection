# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: ..\lib\imagehlp.exp ..\bin\imagehlp.dll

cflags = $(cflags) -DWIN32 -DTARGET32 -DHOST32 -DADDR_MIXED -DNT_BUILD -D_PORTABLE_32BIT_CONTEXT

.c.obj:
    $(cc) $(cflags) $(cvarsdll) -I..\include -I. $(scall) $(cdebug) $**

OBJS = checksum.obj \
       debug.obj    \
       imagedir.obj \
       bindi.obj    \
       map.obj	    \
       pdb.obj      \
       rebasei.obj  \
       symbols.obj  \
       symbolsp.obj \
       walk.obj	    \
       walkalp.obj  \
       walkx86.obj  \
       walkmip.obj  \
       walkppc.obj  \
       undecsym.obj \
       chksum.obj

..\lib\imagehlp.exp: $(OBJS) imagehlp.def
    @if not exist ..\lib md ..\lib > nul
    $(implib) @<<
-machine:$(CPU)
-def:imagehlp.def	
$(OBJS: =
)
-out:..\lib\imagehlp.lib
<<NOKEEP

..\bin\imagehlp.dll: ..\lib\imagehlp.exp $(OBJS)
    @if not exist ..\bin md ..\bin > nul
    $(link) @<<
$(dlllflags)
-base:0x1B000000
$(linkdebug)
-out:$@
$(**: =
)
$(conlibsdll: =
)
<<NOKEEP
