# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: wxform.exe

# Update the resource if necessary

wxform.rbj: wxform.rc wxform.h
    rc -r -fo wxform.res wxform.rc
    cvtres -$(CPU) wxform.res -o wxform.rbj

# Update the object file if necessary

wxform.obj: wxform.c wxform.h
    $(cc) $(cdebug) $(cflags) $(cvars) wxform.c

# Update the executable file if necessary, and if so, add the resource back in.

wxform.exe: wxform.obj wxform.rbj wxform.def
    $(link) $(linkdebug) $(guiflags) -out:wxform.exe wxform.obj wxform.rbj $(guilibs)
