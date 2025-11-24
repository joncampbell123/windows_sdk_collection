!include <ntwin32.mak>

all: sd_flppy.exe floplock.exe instsrv.exe chgflpsd.exe

# Update the object file if necessary

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.c

sd_flppy.exe: sd_flppy.obj
  $(link) $(linkdebug) $(conflags) -out:$*.exe $*.obj $(conlibs) advapi32.lib

floplock.exe: floplock.obj
    $(link) $(linkdebug) $(conflags) -out:floplock.exe floplock.obj $(conlibsmt) advapi32.lib

instsrv.exe: instsrv.obj
    $(link) $(linkdebug) $(conflags) -out:instsrv.exe instsrv.obj $(conlibsmt) advapi32.lib

chgflpsd.exe: chgflpsd.obj
    $(link) $(linkdebug) $(conflags) -out:chgflpsd.exe chgflpsd.obj $(conlibsmt)

