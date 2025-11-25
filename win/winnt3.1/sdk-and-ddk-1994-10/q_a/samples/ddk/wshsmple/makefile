# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

INCLUDE=$(INCLUDE);$(BASEDIR)\src\network\inc;$(BASEDIR)\src\network\streams\inc

all: wshsmple.dll

# Update the object files if necessary

wshsmple.obj: wshsmple.c
	 set INCLUDE=$(INCLUDE)
	 $(cc) $(cflags) $(cvarsdll) $(cdebug) wshsmple.c


# Update the import library

wshsmple.lib: wshsmple.obj
	 lib32 -machine:$(CPU)     \
	 -def:wshsmple.def     \
	 wshsmple.obj          \
	 -out:wshsmple.lib

# Update the dynamic link library

wshsmple.dll: wshsmple.lib wshsmple.obj wshsmple.def
	 link32 \
	 -base:0x1C000000  \
	 -dll       \
	 -entry:_CRT_INIT$(DLLENTRY)  \
	 -out:wshsmple.dll   \
	 wshsmple.exp wshsmple.obj $(conlibsdll) wsock32.lib
