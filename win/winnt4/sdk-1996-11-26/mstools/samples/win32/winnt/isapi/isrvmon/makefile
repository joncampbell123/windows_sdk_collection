Proj = ISrvMon

!include <Win32.mak>

all: $(Proj).Dll Install

$(Proj).Obj: $(Proj).c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(Proj).c

$(Proj).Dll: $*.Obj $*.Exp
     $(link) $(linkdebug) $(dlllflags) -base:0x1C000000 -out:$(Proj).dll  \
      $(Proj).Obj $(Proj).Exp  $(guilibsdll)

$(Proj).Exp: $*.Obj
  $(implib) -machine:$(CPU) -nologo -Def:$(Proj).Def -Out:$(Proj).Lib \
    $(Proj).Obj 


Install:
!IF "$(WWWSCRIPTS)" != ""
!IF "$(WWWROOT)" != ""
        copy ISrvMon.dll $(WWWSCRIPTS)\SDK\ISrvMon.Dll 
        copy ..\ISAPIsmp.htm $(WWWROOT)\ISAPIsmp.Htm
        copy ISrvMon.htm $(WWWROOT)\SDK\ISrvMon.Htm
!ENDIF
!ENDIF

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exp
    del *.obj
    del *.dll
