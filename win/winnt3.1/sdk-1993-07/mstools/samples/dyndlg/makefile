# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: dyndlg.exe dyndlg.hlp


# Update the resource if necessary

dyndlg.rbj: dyndlg.rc dyndlg.h
    rc -r -fo dyndlg.res dyndlg.rc
    cvtres -$(CPU) dyndlg.res -o dyndlg.rbj


# Update the online help file if necessary.

dyndlg.hlp: dyndlg.hpj dyndlg.rtf
    @if exist dyndlg.PH del dyndlg.PH
    hc -n dyndlg.hpj


# Update the object file if necessary

dyndlg.obj: dyndlg.c dyndlg.h
    $(cc) $(cdebug) $(cflags) $(cvars) dyndlg.c


# Update the executable file if necessary, and if so, add the resource back in.

dyndlg.exe: dyndlg.obj dyndlg.rbj
    $(link) $(linkdebug) $(guiflags) -out:dyndlg.exe dyndlg.obj dyndlg.rbj $(guilibs)
