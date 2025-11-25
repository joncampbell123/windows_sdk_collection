# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

# This line allows NMAKE to work as well

all: mdi.exe debug.dll

# Update the object files if necessary

mdi.obj: mdi.c
    $(cc) $(cdebug) $(cflags) $(cvars) mdi.c

debug.obj: debug.c mdi.h mditypes.h
    $(cc) $(cdebug) $(cflags) $(cvars) debug.c

# Update the resources if necessary

mdi.rbj: mdi.rc mdi.h
    rc -r -fo mdi.res mdi.rc
    cvtres -$(CPU) mdi.res -o mdi.rbj

# Update the import library

debug.lib: debug.obj debug.def
    $(implib) -machine:$(CPU)     \
    -def:debug.def	    \
    debug.obj               \
    -out:debug.lib

# Update the dynamic link library

debug.dll: debug.obj debug.def
    $(link) $(linkdebug)          \
    -base:0x1C000000  \
    -dll	      \
    -entry:_DllMainCRTStartup$(DLLENTRY)    \
    -out:debug.dll   \
    debug.exp debug.obj $(guilibs)


# Update the executable file if necessary.
# If so, add the resource back in.

mdi.exe: mdi.obj debug.lib mdi.rbj
    $(link) $(linkdebug) $(guiflags) \
    -out:mdi.exe mdi.obj debug.lib mdi.rbj $(guilibs) shell32.lib
