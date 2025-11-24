# Some NT Win32 nmake macros.
# For now this resides in samples\inc.

!include <ntwin32.mak>

all: tlscall.exe tlsdll.dll

# Update the object files if necessary

tlscall.obj: tlscall.c
    $(cc) $(cdebug) $(cflags) $(cvarsdll) tlscall.c

tlsdll.obj: tlsdll.c
    $(cc) $(cdebug) $(cflags) $(cvarsdll) tlsdll.c

# Update the import library

tlsdll.lib: tlsdll.obj tlsdll.def
    $(implib) -machine:$(CPU) \
    -def:tlsdll.def	    \
    tlsdll.obj              \
    -out:tlsdll.lib


# Update the dynamic link library

tlsdll.dll: tlsdll.obj tlsdll.def
    $(link) $(linkdebug) $(lflags) \
    -base:0x1C000000  \
    -entry:_DllMainCRTStartup$(DLLENTRY)    \
    -out:tlsdll.dll   \
    tlsdll.exp tlsdll.obj $(conlibsdll)


# Update the executable file if necessary.
# If so, add the resource back in.

tlscall.exe: tlscall.obj tlsdll.lib tlscall.def
    $(link) $(linkdebug) $(conflags) -out:tlscall.exe tlscall.obj tlsdll.lib $(conlibsdll)
