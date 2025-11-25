Proj = HomeFilt

!include <Win32.mak>

all: $(Proj).Dll Install

$(Proj).Obj: $(Proj).c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(Proj).c

$(Proj).Dll: $*.Obj $*.Exp
     $(link) $(linkdebug) $(dlllflags) -base:0x1C000000 -out:$(Proj).dll  \
      $(Proj).Obj  $(Proj).Exp $(guilibsdll)

$(Proj).Exp: $*.Obj
  $(implib) -machine:$(CPU) -nologo -Def:$(Proj).Def -Out:$(Proj).Lib \
    $(Proj).Obj 


Install:
!IF "$(WWWSCRIPTS)" != ""
!IF "$(WWWROOT)" != ""
        copy $(Proj).dll $(WWWSCRIPTS)\SDK\$(Proj).Dll 
        @echo.
        @echo Add this to the registry to enable this filter 
        @echo HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\W3SVC\Parameters
        @echo Filter DLLs="$(WWWSCRIPTS)\SDK\$(Proj).Dll"
        @echo.
        copy ..\ISAPIsmp.htm $(WWWROOT)\ISAPIsmp.Htm
        copy $(Proj).htm $(WWWROOT)\SDK\$(Proj).Htm
        copy Home.htm $(WWWROOT)\SDK\Home.Htm
!ENDIF
!ENDIF

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exp
    del *.obj
    del *.dll
