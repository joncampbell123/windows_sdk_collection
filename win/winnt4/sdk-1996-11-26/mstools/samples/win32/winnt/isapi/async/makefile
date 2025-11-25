!include <win32.mak>

LINKER_FLAGS=$(ldebug) $(dllflags) -entry:DllLibMain

TARGETS=fwrite.dll ftrans.dll fwasync.dll

all: $(TARGETS) Install

clean:
    erase $(TARGETS) *.obj
    for %i in (fwrite ftrans fwasync) do (erase %i.exp %i.ilk %i.lib %i.pdb)

.c.obj:
    $(cc) $(cflags) $(cvarsdll) $(cdebug) $< 


fwrite.dll: fwrite.obj openf.obj
    $(link) $(LINKER_FLAGS) -out:fwrite.dll -def:fwrite.def fwrite.obj openf.obj kernel32.lib msvcrt.lib

fwasync.dll: fwasync.obj openf.obj
    $(link) $(LINKER_FLAGS) -out:fwasync.dll -def:fwasync.def fwasync.obj openf.obj $(conlibsdll)

ftrans.dll: ftrans.obj openf.obj
    $(link) $(LINKER_FLAGS) -out:ftrans.dll -def:ftrans.def ftrans.obj openf.obj $(conlibsdll)

Install:
!IF "$(WWWSCRIPTS)" != ""
!IF "$(WWWROOT)" != ""
        copy fwasync.dll $(WWWSCRIPTS)\SDK\fwasync.Dll 
        copy ftrans.dll $(WWWSCRIPTS)\SDK\ftrans.Dll 
        copy fwrite.dll $(WWWSCRIPTS)\SDK\fwrite.Dll 
        copy ..\ISAPIsmp.htm $(WWWROOT)\ISAPIsmp.Htm
        copy fwasync.htm $(WWWROOT)\SDK\fwasync.Htm
!ENDIF
!ENDIF
