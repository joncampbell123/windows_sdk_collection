# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: simple.exe instsrv.exe client.exe

# Update the object file if necessary

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.c

simple.exe: simple.obj
    $(link) $(linkdebug) $(conflags) -out:simple.exe simple.obj $(conlibsmt) advapi32.lib

instsrv.exe: instsrv.obj
    $(link) $(linkdebug) $(conflags) -out:instsrv.exe instsrv.obj $(conlibsmt) advapi32.lib

client.exe: client.obj
    $(link) $(linkdebug) $(conflags) -out:client.exe client.obj $(conlibsmt)

