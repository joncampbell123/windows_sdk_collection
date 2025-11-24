# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: uconvert.exe uconvert.hlp


# Update the resource if necessary
uconvert.rbj: uconvert.rc uconvert.h
    rc -r -fo uconvert.res uconvert.rc
    cvtres -$(CPU) uconvert.res -o uconvert.rbj

# Update the online help file if necessary.
uconvert.hlp: uconvert.hpj uconvert.rtf
    if exist uconvert.PH del uconvert.PH
    hc -n uconvert.hpj


# Update the object files if necessary

uconvert.obj: uconvert.c uconvert.h
    $(cc) $(cdebug) $(cflags) $(cvars) uconvert.c


install.obj: install.c install.h
    $(cc) $(cdebug) $(cflags) $(cvars) install.c

dialogs.obj: dialogs.c uconvert.h
    $(cc) $(cdebug) $(cflags) $(cvars) dialogs.c


# Update the executable file if necessary

uconvert.exe: uconvert.obj install.obj dialogs.obj uconvert.rbj
    $(link) $(linkdebug) $(guiflags) -out:uconvert.exe \
          uconvert.obj install.obj dialogs.obj uconvert.rbj $(guilibs) advapi32.lib
