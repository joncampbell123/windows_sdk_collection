# Nmake macros for building Windows 32-Bit apps

!MESSAGE Building CgiWrap.Exe...
!MESSAGE

Proj = CgiWrap

!include <win32.mak>

all: CgiWrap.Exe Install

$(Proj).Exe: $(Proj).Obj
  $(link)  $(lflags) -base:0x1C000000 -out:$@ $** $(guilibsdll)

# Update the object file if necessary

$(Proj).obj: $(Proj).c
    $(cc) $(cflags) $(cvarsdll) $(Proj).c

clean:
  del $(Proj).Exe $(Proj).Obj

Install:
!IF "$(WWWSCRIPTS)" != ""
!IF "$(WWWROOT)" != ""
        copy cgiwrap.exe $(WWWSCRIPTS)\SDK\ISrvMon.exe 
        copy ..\ISAPIsmp.htm $(WWWROOT)\ISAPIsmp.Htm
        copy cgiwrap.htm $(WWWROOT)\SDK\CgiWrap.Htm
!ENDIF
!ENDIF
