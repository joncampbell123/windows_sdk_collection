# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: ReadWrit.exe DataBase.dll

# Update the object files if necessary

ReadWrit.obj: ReadWrit.c
    $(cc) $(cdebug) $(cflags) $(cvars) ReadWrit.c


DataBase.obj: DataBase.c
    $(cc) $(cdebug) $(cflags) $(cvars) DataBase.c

# Update the import library

DataBase.lib: DataBase.obj DataBase.def
    $(implib) -machine:$(CPU)     \
    -def:DataBase.def DataBase.obj \
    -out:DataBase.lib

# Update the dynamic link library

DataBase.dll: DataBase.obj DataBase.def
    $(link) $(linkdebug)           \
    -base:0x1C000000  \
    -dll	      \
    -entry:_DllMainCRTStartup$(DLLENTRY)\
    -out:DataBase.dll   \
    DataBase.exp DataBase.obj $(conlibsdll)


# Update the executable file if necessary.
# If so, add the resource back in.

ReadWrit.exe: ReadWrit.obj DataBase.lib
    $(link) $(linkdebug) $(conflags) -out:ReadWrit.exe ReadWrit.obj  DataBase.lib  $(conlibs)
