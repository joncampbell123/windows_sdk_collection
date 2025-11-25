# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: cmndlg.exe

# Update the resource if necessary

cmndlg.res: cmndlg.rc cmndlg.h
    $(rc) -r -fo cmndlg.tmp cmndlg.rc
    $(cvtres) -$(CPU) cmndlg.tmp -o cmndlg.res
    del cmndlg.tmp

# Update the object file if necessary

cmndlg.obj: cmndlg.c cmndlg.h
    $(cc) $(cdebug) $(cflags) $(cvars) cmndlg.c

# Update the executable file if necessary, and if so, add the resource back in.

cmndlg.exe: cmndlg.obj cmndlg.res
    $(link) $(linkdebug) $(guiflags) -out:cmndlg.exe cmndlg.obj cmndlg.res $(guilibs)
