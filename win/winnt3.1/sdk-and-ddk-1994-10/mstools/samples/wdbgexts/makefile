# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

PROJ = wdbgexts

all: $(PROJ).dll

# Update the object files if necessary

$(PROJ).obj: $(PROJ).c
    $(cc) $(cflags) $(cvarsdll) $(cdebug) $(scall) $*.c

# Update the import library

$(PROJ).lib: $(PROJ).obj $(PROJ).def
    $(implib) -machine:$(CPU) \
    -def:$*.def \
    $*.obj \
    -out:$*.lib

# Update the dynamic link library

$(PROJ).dll: $(PROJ).obj $(PROJ).def $(PROJ).lib
    $(link) $(linkdebug) \
    -base:0x1C000000 \
    -dll \
    -entry:_DllMainCRTStartup$(DLLENTRY) \
    -out:$*.dll \
    $*.exp $*.obj $(conlibsdll)
