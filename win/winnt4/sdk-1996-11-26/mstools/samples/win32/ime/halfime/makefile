# Nmake macros for building Windows 32-Bit apps

APPVER=4.0

!include <win32.mak>

all: halfime.exe

LIBS=imm32.lib


# Update the resource if necessary

halfime.res: halfime.rc
    $(rc) $(rcflags) $(rcvars) halfime.rc


# Update the object file if necessary

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $<

# Update the executable file if necessary, and if so, add the resource back in.

halfime.exe:    main.obj   \
                halfime.res
    $(link) $(linkdebug) $(guiflags) -out:halfime.exe $** $(guilibs) $(LIBS)
