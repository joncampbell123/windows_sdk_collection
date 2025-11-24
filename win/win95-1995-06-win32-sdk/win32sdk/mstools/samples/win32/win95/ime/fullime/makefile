# Nmake macros for building Windows 32-Bit apps

APPVER=4.0

!include <win32.mak>

all: fullime.exe

LIBS=imm32.lib


# Update the resource if necessary

fullime.res: fullime.rc
    $(rc) $(rcflags) $(rcvars) fullime.rc


# Update the object file if necessary

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $<

# Update the executable file if necessary, and if so, add the resource back in.

fullime.exe:    candui.obj \
                global.obj \
                imeui.obj  \
                main.obj   \
                fullime.res
    $(link) $(linkdebug) $(guiflags) -out:fullime.exe $** $(guilibs) $(LIBS)
