# Nmake macros for building Windows 32-Bit apps

APPVER=4.0

!include <win32.mak>

all: imeapps.exe

LIBS=imm32.lib comctl32.lib


# Update the resource if necessary

imeapps.res: imeapps.rc imeapps.rcv
    $(rc) $(rcflags) $(rcvars) imeapps.rc


# Update the object file if necessary

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $<

# Update the executable file if necessary, and if so, add the resource back in.

imeapps.exe:    comp.obj    \
                data.obj    \
                imeapps.obj \
                mode.obj    \
                paint.obj   \
                setcomp.obj \
                status.obj  \
                subs.obj    \
                toolbar.obj \
                imeapps.res
    $(link) $(linkdebug) $(guiflags) -out:imeapps.exe $** $(guilibs) $(LIBS)
