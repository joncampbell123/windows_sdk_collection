# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: ..\bin\imagehlp.dll

.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $**

checksum.obj: checksum.c
    $(cc) $(cflags) $(cvars) -I..\include $(cdebug) $**

debug.obj: debug.c
    $(cc) $(cflags) $(cvars) -I..\include $(cdebug) $**

imagedir.obj: imagedir.c
    $(cc) $(cflags) $(cvars) -I..\include $(cdebug) $**

..\lib\imagehlp.lib: checksum.obj debug.obj undecsym.obj imagedir.obj imagehlp.def
    @if not exist ..\lib md ..\lib > null
    $(implib) -machine:$(CPU)     \
    -def:imagehlp.def	    \
    checksum.obj debug.obj undecsym.obj imagedir.obj    \
    -out:..\lib\imagehlp.lib

..\bin\imagehlp.dll: ..\lib\imagehlp.lib
    @if not exist ..\bin md ..\bin > null
    $(link) -dll -base:0x1B000000 -entry:_CRT_INIT$(DLLENTRY) -subsystem:console $(ldebug) -out:$@ $** ..\lib\imagehlp.exp $(conlibsdll)
