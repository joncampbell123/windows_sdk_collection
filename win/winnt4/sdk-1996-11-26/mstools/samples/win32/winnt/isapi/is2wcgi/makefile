Proj = IS2WCGI

!include <Win32.mak>

all: $(Proj).Dll Install TestApp

$(Proj).Obj: $(Proj).cpp
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(Proj).cpp

Keys.Obj: Keys.cpp
  $(cc) $(cdebug) $(cflags) $(cvarsdll)  Keys.cpp

Entry.Obj: Entry.cpp
  $(cc) $(cdebug) $(cflags) $(cvarsdll) Entry.cpp

$(Proj).Dll: $*.Obj $*.Exp Keys.Obj Entry.Obj
     $(link) $(linkdebug) $(dlllflags) -base:0x1C000000 -out:$(Proj).dll  \
      $(Proj).Obj  $*.Exp Keys.Obj Entry.Obj $(guilibsdll)

$(Proj).Exp: $*.Obj
  $(implib) -machine:$(CPU) -nologo -Def:$(Proj).Def -Out:$(Proj).Lib \
    $(Proj).Obj 
Install:
!IF "$(WWWSCRIPTS)" != ""
!IF "$(WWWROOT)" != ""
        copy $(Proj).dll $(WWWSCRIPTS)\SDK\$(Proj).Dll 
        copy ..\ISAPIsmp.htm $(WWWROOT)\ISAPIsmp.Htm
        copy $(Proj).htm $(WWWROOT)\SDK\$(Proj).Htm
!ENDIF
!ENDIF

TestApp: Build
Build:
    Cd TestApp
    @$(MAKE) /$(MAKEOPTS)
    cd ..

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exp
    del *.dll
