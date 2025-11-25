!include <ntwin32.mak>

INCLUDE=$(INCLUDE);$(BASEDIR)\src\input\inc

all: kbd_attr.exe

kbd_attr.obj: kbd_attr.c
    $(cc) $(cflags) $(cvars) $(cdebug) kbd_attr.c

kbd_attr.exe: kbd_attr.obj
    $(link) $(linkdebug) $(conflags) -out:kbd_attr.exe kbd_attr.obj $(conlibs)
