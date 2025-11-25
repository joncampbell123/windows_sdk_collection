Proj = IS2WCGI

!include <Win32.mak>

all: $(Proj).Exe Install

$(Proj).obj: $(Proj).c
  $(cc) $(cdebug) $(cflags) $(cvars) $(Proj).c

$(Proj).Exe: $*.Obj
    $(link) $(linkdebug) $(guilflags) -out:$(Proj).exe $*.Obj \
	 $(guilibs)

Install:
!IF "$(WWWSCRIPTS)" != ""
!IF "$(WWWROOT)" != ""
         copy $(Proj).exe $(WWWSCRIPTS)\SDK\$(Proj).Exe
!ENDIF
!ENDIF

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exe

