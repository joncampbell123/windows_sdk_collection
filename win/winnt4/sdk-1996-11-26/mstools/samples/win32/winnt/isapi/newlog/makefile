!include <win32.mak>

CC=$(cc) $(cdebug)
CFLAGS=$(cflags)
CVARS=$(cvarsdll) -DWIN32 -DNDEBUG  

LINK=$(link) $(linkdebug)
LINKOPT= $(dlllflags)

LIBS= $(guilibsdll) user32.lib mapi32.lib advapi32.lib
OBJS=lfilter.obj
LINKOUT=/OUT:NewLog.dll

Proj=NewLog

DEFS=lfilter.def

all: NewLog.dll Install

.c .obj:
    $(CC) $(CFLAGS) $(CVARS) $*.c

NewLog.dll: $(OBJS) $(DEFS)
    $(LINK) $(LINKOPT) /DEF:$(DEFS) $(LINKOUT) $(LIBS) $(OBJS) 


Install:
        copy $(Proj).dll $(WWWSCRIPTS)\SDK\$(Proj).Dll 
        @echo.
        @echo Add this to the registry to enable this filter 
        @echo HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\W3SVC\Parameters
        @echo Filter DLLs="$(WWWSCRIPTS)\SDK\$(Proj).Dll"
        @echo.
        copy ..\ISAPIsmp.htm $(WWWROOT)\ISAPIsmp.Htm
        copy $(Proj).htm $(WWWROOT)\SDK\$(Proj).Htm


clean:
    del NewLog.dll
    del lfilter.obj
    del NewLog.lib
    del NewLog.exp
