# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: ttfonts.exe ttfonts.hlp

# Update the resource if necessary

ttfonts.rbj: ttfonts.rc ttfonts.h
    rc -r -fo ttfonts.res ttfonts.rc
    cvtres -$(CPU) ttfonts.res -o ttfonts.rbj

# Update the online help file if necessary.
ttfonts.hlp: ttfonts.hpj ttfonts.rtf
    @if exist ttfonts.PH del ttfonts.PH
    hc -n ttfonts.hpj

# Update the object file if necessary

ttfonts.obj: ttfonts.c ttfonts.h
    $(cc) $(cdebug) $(cflags) $(cvars) ttfonts.c

allfont.obj: allfont.c ttfonts.h
    $(cc) $(cdebug) $(cflags) $(cvars) allfont.c

dialogs.obj: dialogs.c ttfonts.h
    $(cc) $(cdebug) $(cflags) $(cvars) dialogs.c

toolbar.obj: toolbar.c ttfonts.h
    $(cc) $(cdebug) $(cflags) $(cvars) toolbar.c

display.obj: display.c ttfonts.h
    $(cc) $(cdebug) $(cflags) $(cvars) display.c

# Update the executable file if necessary, and if so, add the resource back in.

ttfonts.exe: ttfonts.obj ttfonts.rbj ttfonts.def allfont.obj dialogs.obj toolbar.obj display.obj
    $(link) $(linkdebug) $(guiflags) -out:ttfonts.exe \
          ttfonts.obj allfont.obj dialogs.obj toolbar.obj display.obj ttfonts.rbj $(guilibs)
