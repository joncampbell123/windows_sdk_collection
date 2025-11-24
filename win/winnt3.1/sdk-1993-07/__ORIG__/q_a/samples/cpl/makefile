# Nmake macros for building Windows 32-Bit apps
!include <NtWin32.Mak>

# target list
all: stereo.cpl

# Update the object files if necessary
stereo.obj: stereo.c stereo.h
    $(cc) $(cdebug) $(cflags) $(cvarsdll) stereo.c

# Update the resource if necessary
stereo.res: stereo.rc stereo.h
    $(rc) $(rcvars) -r -fo stereo.res stereo.rc
    $(cvtres) -$(CPU) stereo.res -o stereo.rbj

# Update the import library
stereo.lib: stereo.obj stereo.def
    $(implib)\
    -machine:$(CPU)\
    stereo.obj\
    -def:stereo.def\
    -out:stereo.lib

# Update the dynamic link library
stereo.cpl: stereo.obj stereo.lib stereo.res
    $(link) $(ldebug) -subsystem:native -machine:$(CPU)\
    -base:0x1C000000\
    -dll\
    -entry:_DllMainCRTStartup$(DLLENTRY)\
    -out:stereo.cpl\
    stereo.obj stereo.rbj stereo.exp $(guilibsdll)

