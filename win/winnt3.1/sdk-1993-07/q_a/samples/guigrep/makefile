# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

# This line allows NMAKE to work as well

all: ntgrep.exe guigrep.dll

# Update the resource if necessary

ntgrep.rbj: ntgrep.rc ntgrep.h
    rc -r -fo ntgrep.res ntgrep.rc
    cvtres -$(CPU) ntgrep.res -o ntgrep.rbj

guigrep.rbj: guigrep.rc guigrep.h guigrep.dlg
    rc -r -fo guigrep.res guigrep.rc
    cvtres -$(CPU) guigrep.res -o guigrep.rbj

# update .lib if necessary

guigrep.exp: guigrep.obj guigrep.def
    $(implib) -machine:$(CPU)  \
    -def:guigrep.def     \
    guigrep.obj           \
    -out:guigrep.lib     \


# Update the object file if necessary

ntgrep.obj: ntgrep.c ntgrep.h
    $(cc) $(cflags) $(cvarsdll) ntgrep.c

guigrep.obj: guigrep.c guigrep.h
    $(cc) $(cflags) $(cvarsdll) /Tp guigrep.c

# Update the executable file if necessary, and if so, add the resource back in.

ntgrep.exe: ntgrep.obj ntgrep.rbj ntgrep.def
    $(link) $(guiflags) -out:ntgrep.exe ntgrep.obj ntgrep.rbj $(guilibsdll)


guigrep.dll: guigrep.obj guigrep.def guigrep.rbj guigrep.exp
    $(link)               \
    -base:0x1C000000     \
    -dll                 \
    -entry:_DllMainCRTStartup$(DLLENTRY)  \
    -out:guigrep.dll     \
    guigrep.rbj guigrep.exp guigrep.obj $(guilibsdll)
