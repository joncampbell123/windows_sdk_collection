# Define NODEBUG to build DIALER without debugging information.

TARGETOS=WINNT

!include <ntwin32.mak>

#unicode = -DNODEBUG

cflags=$(cflags) -DWIN32_LEAN_AND_MEAN

all: dialer.exe

# Update the resource if necessary

dialer.res: dialer.rc resource.h
    $(rc) $(rcvars) -r dialer.rc

# Update the object files if necessary

dialer.obj: dialer.c resource.h dialer.h dialhelp.h
    $(cc) $(cdebug) $(cflags) $(cvars) $(unicode) dialer.c

# Update the executable file if necessary, and if so, add the resource back in.

dialer.exe: dialer.obj  dialer.res
    $(link) $(linkdebug)  $(guiflags) -out:dialer.exe\
    dialer.obj dialer.res\
    version.lib tapi32.lib $(guilibsmt)
