Proj = Lottery

!include <Win32.mak>

all: $(Proj).Dll Install

$(Proj).Obj: $(Proj).c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(Proj).c

Worker.Obj: Worker.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) Worker.c

$(Proj).Dll: $*.Obj $*.Exp Worker.Obj
     $(link) $(linkdebug) $(dlllflags) -base:0x1C000000 -out:$(Proj).dll  \
      $(Proj).Obj Worker.Obj $(Proj).Exp $(guilibsdll)

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

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exp
    del *.obj
    del *.dll
