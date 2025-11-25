# Nmake macros for building Windows 32-Bit apps

!MESSAGE Building runner.dll...
!MESSAGE

.SUFFIXES: .def .lib

!include <win32.mak>

all: runner.dll

# Update the resource if necessary

runner.res: runner.rc  # resource.h
    $(rc) $(rcflags) $(rcvars) runner.rc

# Update the object file if necessary

runner.obj: runner.c runner.h
    $(cc) $(cflags) $(cvarsdll) runner.c

# .Def.Exp:
runner.exp:
  $(implib) -machine:$(CPU) -nologo -Def:$(@B).Def -Out:$(@B).Lib $(@B).Obj

runner.Dll: runner.Obj runner.Exp runner.res
  $(link)  $(dlllflags) -base:0x1C000000 -out:$@ $** $(guilibsdll) $(int64lib) 

runner.exp: runner.Def runner.Obj

clean:
  del runner.Dll runner.Obj runner.Exp runner.Lib runner.res
