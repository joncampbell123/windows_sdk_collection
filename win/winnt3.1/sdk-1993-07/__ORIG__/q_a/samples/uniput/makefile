# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: uniput.exe uniput.hlp


# Update the resource if necessary
uniput.rbj: uniput.rc uniput.h
    rc -r -fo uniput.res uniput.rc
    cvtres -$(CPU) uniput.res -o uniput.rbj

# Update the online help file if necessary.
uniput.hlp: uniput.hpj uniput.rtf
    if exist uniput.PH del uniput.PH
    hc -n uniput.hpj


# Update the object files if necessary

uniput.obj: uniput.c uniput.h
    $(cc) $(cdebug) $(cflags) $(cvars) uniput.c

display.obj: display.c uniput.h display.h
    $(cc) $(cdebug) $(cflags) $(cvars) display.c

status.obj: status.c uniput.h
    $(cc) $(cdebug) $(cflags) $(cvars) status.c


# Update the executable file if necessary

uniput.exe: uniput.obj uniput.rbj display.obj status.obj
    $(link) $(linkdebug) $(guiflags) -out:uniput.exe \
          uniput.obj display.obj status.obj uniput.rbj $(guilibs)
