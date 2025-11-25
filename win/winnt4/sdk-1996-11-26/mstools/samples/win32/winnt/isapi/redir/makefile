!include <win32.mak>
CC=$(cc) $(cdebug)
CFLAGS=$(cflags)
CVARS=$(cvarsdll) -DWIN32 -DNDEBUG  
LINK=$(link) $(linkdebug)
LINKOPT= $(dlllflags)
LIBS= $(guilibsdll) user32.lib 

OBJS=redirect.obj
LINKOUT=/OUT:redir.dll
DEFS=redir.def

Proj=Redir

all: redir.dll Install

.c .obj:
    $(CC) $(CFLAGS) $(CVARS) $*.c

redir.dll: $(OBJS) $(DEFS)
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
        copy Volcano.gif $(WWWROOT)\SDK\Volcano.gif

clean:
    del redir.dll
    del redirect.obj
    del redir.lib
    del redir.exp
