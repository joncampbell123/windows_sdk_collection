# Nmake macros for building Windows 32-Bit apps

!MESSAGE Building runcpl.cpl...
!MESSAGE 

.SUFFIXES: .def .lib

!include <win32.mak>

all: webrun.cpl

# Update the resource if necessary

runcpl.res: runcpl.rc  resource.h
    $(rc) $(rcflags) $(rcvars) runcpl.rc

# Update the object file if necessary

runcpl.obj: runcpl.c runcpl.h
    $(cc) $(cflags) $(cvarsdll) runcpl.c

runcpl.exp:
  $(implib) -machine:$(CPU) -nologo -Def:$(@B).Def -Out:$(@B).Lib $(@B).Obj

webrun.cpl: runcpl.obj runcpl.exp runcpl.res
  $(link)  $(dlllflags) -base:0x1C000000 -out:webrun.cpl $** $(guilibsdll) $(int64lib) 

runcpl.exp: runcpl.def runcpl.obj

clean:
  del webrun.cpl runcpl.Obj runcpl.exp runcpl.lib runcpl.res